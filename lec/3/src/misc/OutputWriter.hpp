#ifndef APP_OUTPUT_WRITER_H
#define APP_OUTPUT_WRITER_H

#include <cstring>
#include <fstream>
#include <iostream>
#include <span>
#include <string>
#include <vector>

struct OutputWriter {
    virtual void write(std::span<char> out) {
        write(out.data(), out.size());
    }

    virtual void write(char *data, size_t len) {
        write(std::span(data, len));
    }

    virtual ~OutputWriter() = default;
};

struct FileOutputWriter : public OutputWriter {
    FileOutputWriter(const std::string &filename) : stream() {
        stream.exceptions(std::ofstream::failbit);
        stream.open(filename);
    }

    virtual void write(char *data, size_t len) override {
        stream.write(data, len);
    }

  private:
    std::ofstream stream;
};

struct VectorOutputWriter : public OutputWriter {
    virtual void write(char *data, size_t len) override {
        auto size = vector.size();
        vector.resize(size + len);
        std::memcpy(vector.data() + size, data, len);
    }

    std::vector<char> to_vector() {
        return std::move(vector);
    }

  private:
    std::vector<char> vector;
};

#endif