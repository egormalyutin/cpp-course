#ifndef APP_FILE_STREAM_H
#define APP_FILE_STREAM_H

#include "../streams/Stream.hpp"
#include <fstream>
#include <iostream>
#include <optional>
#include <span>

struct FileStream : public Stream<char> {
    FileStream(const std::string &path)
        : path(path), pos(0), fstream(std::nullopt), ended(false) {
    }

    FileStream(const FileStream &stream)
        : path(stream.path), pos(stream.pos), fstream(std::nullopt),
          ended(stream.ended) {
    }

    FileStream &operator=(const FileStream &stream) {
        path = stream.path;
        pos = stream.pos;
        fstream.reset();
        return *this;
    }

    std::optional<size_t> read(std::span<char> buffer) override {
        if (ended) {
            return std::nullopt;
        }

        if (!fstream.has_value()) {
            open();
        }

        if (!fstream->read(buffer.data(), buffer.size())) {
            if (fstream->eof()) {
                ended = true;
            } else {
                throw std::system_error(errno, std::generic_category());
            }
        }

        size_t read = fstream->gcount();
        pos += read;

        return std::make_optional(read);
    }

    StreamBox<char> clone() const override {
        return box_stream<FileStream>(*this);
    }

    std::string path;
    std::optional<std::ifstream> fstream;
    bool ended;
    size_t pos;

  private:
    void open() {
        fstream.emplace();
        fstream->open(path, std::ios::binary);
        if (pos != 0) {
            fstream->seekg(pos);
        }
    }
};

#endif