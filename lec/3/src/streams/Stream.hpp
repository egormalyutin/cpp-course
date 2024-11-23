#ifndef APP_STREAM_H
#define APP_STREAM_H

#include "../misc/util.hpp"

#include <memory>
#include <optional>
#include <span>

template <typename T> struct StreamBox;

template <typename T> struct Stream {
    virtual std::optional<size_t> read(std::span<T>) = 0;

    virtual std::optional<T> read_single() {
        T data;
        while (true) {
            auto r = read(std::span(&data, 1));
            if (r.has_value() && *r == 1) {
                return std::make_optional(data);
            } else if (!r.has_value()) {
                return std::nullopt;
            }
        }
    }

    virtual size_t read_full(std::span<T> buf) {
        auto off = 0;
        while (off < buf.size()) {
            auto r = read(buf.subspan(off, buf.size() - off));
            if (!r.has_value()) {
                break;
            }
            off += *r;
        }
        return off;
    }

    virtual size_t skip(size_t n) {
        T buf[10];
        size_t off = 0;
        while (off < n) {
            auto r = read(
                std::span(buf, std::min(sizeof(buf) / sizeof(T), n - off)));
            if (!r.has_value()) {
                break;
            }
            off += *r;
        }
        return off;
    };

    virtual std::optional<size_t> length() const {
        return std::nullopt;
    };

    virtual StreamBox<T> clone() const = 0;

    virtual ~Stream() = default;
};

template <typename S, typename T>
concept IsStream = std::is_base_of<Stream<T>, S>::value;

template <typename S>
using StreamType = typename ArgType<0, decltype(&S::read)>::element_type;

template <typename T> struct StreamBox : public Stream<T> {
    StreamBox() : u() {
    }

    StreamBox(std::unique_ptr<Stream<T>> u) : u(std::move(u)) {
    }

    StreamBox(const StreamBox<T> &box) {
        u = std::move(box.clone().u);
    }

    Stream<T> &operator=(StreamBox<T> &box) {
        u = std::move(box.clone().u);
        return *this;
    }

    StreamBox(StreamBox<T> &&box) {
        u = std::move(box.u);
    }

    Stream<T> &operator=(StreamBox<T> &&box) {
        u = std::move(box.u);
        return *this;
    }

    std::optional<size_t> read(std::span<T> buffer) override {
        return u->read(buffer);
    }

    std::optional<T> read_single() override {
        return u->read_single();
    }

    size_t read_full(std::span<T> buffer) override {
        return u->read_full(buffer);
    }

    size_t skip(size_t n) override {
        return u->skip(n);
    }

    std::optional<size_t> length() const override {
        return u->length();
    }

    StreamBox<T> clone() const override {
        return u->clone();
    }

  private:
    std::unique_ptr<Stream<T>> u;
};

template <typename S, typename... Args>
StreamBox<StreamType<S>> box_stream(Args &&...args) {
    std::unique_ptr<Stream<StreamType<S>>> u =
        std::make_unique<S>(std::forward<Args>(args)...);
    return StreamBox(std::move(u));
}

#endif