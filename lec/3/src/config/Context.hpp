#ifndef APP_CONFIG_CONTEXT_H
#define APP_CONFIG_CONTEXT_H

#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

struct Position {
    Position() : line(0), column(0), offset(0) {
    }

    size_t line;
    size_t column;
    size_t offset;

    bool operator==(const Position &) const = default;
};

struct PositionRange {
    PositionRange(Position start, Position end) : start(start), end(end) {
    }
    PositionRange(Position at) : start(at), end(at) {
    }
    Position start;
    Position end;
};

struct Context {
    Context(const std::string &data) : data(data), position() {
        if (data.length() != 0) {
            ch = data[0];
        } else {
            ch = EOF;
        }
    }

    char operator*() const noexcept {
        return ch;
    }

    Context &operator++(int) {
        position.offset++;

        if (position.offset == data.length()) {
            ch = EOF;
        } else {
            ch = data[position.offset];
            if (ch == '\n') {
                position.line++;
                position.column = 0;
            } else {
                position.column++;
            }
        }

        return *this;
    }

    Position position;

  private:
    const std::string &data;
    int ch;
};

struct ConfigException : public std::exception {
    virtual char *what() {
        return (char *)"error while reading config";
    }

    virtual PositionRange where() const = 0;
    virtual std::string describe() const = 0;

    virtual ~ConfigException(){};

    bool fatal = false;
};

struct ConfigError : public ConfigException {
    ConfigError(PositionRange range, std::string msg)
        : ConfigException(), range(range), msg(msg) {
    }

    ConfigError(Position at, std::string msg)
        : ConfigException(), range(at), msg(msg) {
    }

    ConfigError(Position start, Position end, std::string msg)
        : ConfigException(), range(start, end), msg(msg) {
    }

    PositionRange where() const override {
        return range;
    }

    std::string describe() const override {
        return msg;
    }

  private:
    PositionRange range;
    std::string msg;
};

static bool is_ident(char c) {
    return c == ' ' || c == '\t';
}

static bool is_break(char c) {
    return is_ident(c) || c == '\n';
}

static bool is_eol(char c) {
    return c == '\n' || c == EOF;
}

static void skip_breaks(Context &ctx) {
    while (is_break(*ctx) && *ctx != EOF) {
        ctx++;
    }
}

static void skip_idents(Context &ctx) {
    while (is_ident(*ctx) && *ctx != EOF) {
        ctx++;
    }
}

static void skip_word(Context &ctx, const std::string &word) {
    Position saved = ctx.position;
    for (size_t i = 0; i < word.length(); i++) {
        if (*ctx != word[i]) {
            std::stringstream ss;
            ss << "expected \"" << word << "\"";
            throw ConfigError(saved, ss.str());
        }
        ctx++;
    }
}

static void skip_eof(Context &ctx) {
    if (*ctx != EOF) {
        throw ConfigError(ctx.position, "expected EOF");
    }
}

static void skip_until_newline(Context &ctx) {
    while (*ctx != '\n' && *ctx != EOF) {
        ctx++;
    }
    if (*ctx == '\n') {
        ctx++;
    }
}

static void skip_expect_eol(Context &ctx) {
    skip_idents(ctx);
    if (!is_eol(*ctx)) {
        throw ConfigError(ctx.position, "expected end of line");
    }
    if (*ctx == '\n') {
        ctx++;
    }
}

template <typename T> static T read_unsigned(Context &ctx) {
    T result = 0;
    bool present = false;

    while (*ctx >= '0' && *ctx <= '9') {
        result = result * 10 + (*ctx - '0');
        ctx++;
        present = true;
    }

    if (!present) {
        throw ConfigError(ctx.position, "expected number");
    }

    return result;
}

static size_t read_slot_id(Context &ctx) {
    auto at = ctx.position;
    try {
        skip_word(ctx, "$");
        return read_unsigned<size_t>(ctx);
    } catch (ConfigException &e) {
        throw ConfigError(at, "expected correct slot id (like $100)");
    }
}

static std::string read_word(Context &ctx, const std::string &what) {
    auto at = ctx.position;

    std::string word;

    while (!is_break(*ctx) && *ctx != EOF) {
        word.push_back(*ctx);
        ctx++;
    }

    if (word.length() == 0) {
        std::stringstream msg;
        msg << "expected " << what;
        throw ConfigError(at, msg.str());
    }

    return word;
}

// pretty-print config error
static void pretty_print_error(const std::string &filename,
                               const std::string &str,
                               const ConfigException &err) {
    auto pos = err.where();
    // find line start
    size_t start = pos.start.offset;
    while (start != -1 && str[start] != '\n') {
        start--;
    }
    start++;

    // find line end
    size_t end = pos.end.offset;
    while (end != str.length() && str[end] != '\n') {
        end++;
    }

    std::cerr << "--> " << filename << ":" << std::endl;

    auto linenr = std::to_string(pos.start.line + 1);

    std::cerr << std::string(linenr.length(), ' ') << " | " << std::endl;
    std::cerr << linenr << " | " << std::string_view(str).substr(start, end)
              << std::endl;

    std::cerr << std::string(linenr.length(), ' ') << " | "
              << std::string(pos.start.offset - start, ' ')
              << std::string(
                     std::max(pos.end.offset - pos.start.offset, (size_t)1),
                     '^')
              << std::endl;

    std::cerr << "error while reading config: " << err.describe();
    std::cerr << " at line " << pos.start.line + 1 << ", column "
              << pos.start.column + 1 << std::endl;
}

template <typename F> static auto make_fatal(F f) {
    try {
        return f();
    } catch (ConfigException &e) {
        e.fatal = true;
        throw;
    }
}

static void skip_comment(Context &ctx) {
    auto at = ctx.position;
    skip_idents(ctx);
    if (*ctx == '#') {
        skip_until_newline(ctx);
    } else if (*ctx == '\n') {
        ctx++;
    } else if (*ctx != EOF) {
        throw ConfigError(at, "expected end of line");
    }
}

static void skip_blank(Context &ctx) {
    while (true) {
        try {
            if (!is_break(*ctx) && *ctx != '#') {
                return;
            }

            skip_breaks(ctx);

            // skip comments
            if (*ctx == '#') {
                skip_until_newline(ctx);
            }
        } catch (ConfigError &e) {
            return;
        }
    }
}

#endif