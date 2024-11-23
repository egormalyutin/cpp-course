#ifndef CSV_PARSER_HPP
#define CSV_PARSER_HPP

#include <iostream>
#include <sstream>
#include <strstream>

namespace CSVParser {

// Position in file
struct Position {
    Position() : line(0), line_offset(0), offset(0){};

    std::size_t line;
    std::size_t line_offset;
    std::size_t offset;
};

template <typename Ch, typename Tr>
auto &operator<<(std::basic_ostream<Ch, Tr> &os, Position const &pos) {
    os << "line " << pos.line + 1 << ", column "
       << pos.offset - pos.line_offset + 1;
    return os;
}

// Base exception class
struct Exception : public std::runtime_error {
    Exception(Position at)
        : std::runtime_error("error when reading csv"), at(at){};

    Position at;

    virtual std::string describe() const = 0;
    virtual ~Exception(){};
};

// Internal context
class Context {
  public:
    Context(std::istream &stream, char delimiter, char quote)
        : stream(stream), position(), ch(0), delimiter(delimiter),
          quote(quote) {
    }

    int operator++(int) {
        ch = stream.get();
        position.offset++;
        if (ch == '\n') {
            position.line++;
            position.line_offset = position.offset;
        }
        return ch;
    }

    int operator*() {
        return ch;
    }

    bool is_indent() {
        return ch == ' ' || ch == '\t';
    }

    bool is_eol() {
        return ch == '\n' || ch == EOF;
    }

    bool is_delimiter() {
        return ch == delimiter;
    }

    bool is_part_end() {
        return is_eol() || is_delimiter();
    }

    void skip_indents() {
        while (is_indent()) {
            (*this)++;
        }
    }

    void skip_blank() {
        while (is_indent() || ch == '\n') {
            (*this)++;
        }
    }

    Position position;

    char delimiter;
    char quote;

  private:
    std::istream &stream;
    int ch;
};

// All read_part implementations
// 1. should assume that indents are trimmed at start
// 2. must consume all remaining indents after part
template <typename T> void read_part(Context &ctx, T &part);

// Expected correct number exception
enum NumberType { SHORT, INT, LONG, LONG_LONG, FLOAT, DOUBLE };
static std::string number_types[] = {"short",     "int",   "long",
                                     "long long", "float", "double"};

struct NumberException : public Exception {
    NumberException(Position at, NumberType type, bool is_signed)
        : Exception(at), type(type), is_signed(is_signed) {
    }

    std::string describe() const override {
        std::stringstream out;
        out << "expected " << (is_signed ? "" : "unsigned ")
            << number_types[type] << " at " << at;
        return out.str();
    };

    NumberType type;
    bool is_signed;
};

// Read any number
template <typename T>
void read_number(Context &ctx, T &part, NumberType type, bool is_signed) {
    auto at = ctx.position;
    part = 0;

    T sign = 1;
    if (is_signed && *ctx == '-') {
        ctx++;
        sign = -1;
    }

    bool correct = false;

    while (true) {
        if (ctx.is_indent()) {
        } else if (*ctx >= '0' && *ctx <= '9') {
            part = part * 10 + (*ctx - '0');
            correct = 1;
        } else if (ctx.is_part_end()) {
            break;
        } else {
            throw NumberException(at, type, is_signed);
        }
        ctx++;
    }

    if (!correct) {
        throw NumberException(at, type, is_signed);
    }

    part *= sign;
}

// Implement read_part for all numbers
static void read_part(Context &ctx, unsigned short &part) {
    read_number(ctx, part, SHORT, false);
}

static void read_part(Context &ctx, unsigned int &part) {
    read_number(ctx, part, INT, false);
}

static void read_part(Context &ctx, unsigned long &part) {
    read_number(ctx, part, LONG, false);
}

static void read_part(Context &ctx, unsigned long long &part) {
    read_number(ctx, part, LONG_LONG, false);
}

static void read_part(Context &ctx, short &part) {
    read_number(ctx, part, SHORT, true);
}

static void read_part(Context &ctx, int &part) {
    read_number(ctx, part, INT, true);
}

static void read_part(Context &ctx, long &part) {
    read_number(ctx, part, LONG, true);
}

static void read_part(Context &ctx, long long &part) {
    read_number(ctx, part, CSVParser::LONG_LONG, true);
}

// Read float-like types
template <typename T>
static void read_float(Context &ctx, T &part, NumberType type) {
    Position at = ctx.position;

    char buf[512];
    std::size_t i = 0;

    while (i < 512) {
        if (ctx.is_indent()) {
            continue;
        } else if (ctx.is_part_end()) {
            break;
        } else {
            buf[i] = *ctx;
            i++;
            ctx++;
        }
    }

    std::istrstream stream(buf, i);
    if (!(stream >> part)) {
        throw NumberException(at, type, true);
    }
}

static void read_part(Context &ctx, float &part) {
    read_float(ctx, part, FLOAT);
}

static void read_part(Context &ctx, double &part) {
    read_float(ctx, part, DOUBLE);
}

// Read string
struct ExpectedStringEndException : public Exception {
    ExpectedStringEndException(Position at) : Exception(at) {
    }

    std::string describe() const override {
        std::stringstream out;
        out << "expected string end at " << at;
        return out.str();
    };
};

static void read_part(Context &ctx, std::string &part) {
    part = std::string();

    bool quoted = *ctx == ctx.quote;

    if (quoted) {
        ctx++;
        while (*ctx != ctx.quote) {
            if (*ctx == EOF) {
                throw ExpectedStringEndException(ctx.position);
            } else if (*ctx == '\\') {
                ctx++;
                if (*ctx == EOF) {
                    throw ExpectedStringEndException(ctx.position);
                }
                part.push_back(*ctx);
            } else {
                part.push_back(*ctx);
            }
            ctx++;
        }
        ctx.skip_indents();
        ctx++;
    } else {
        std::size_t length = 0;
        while (true) {
            if (ctx.is_part_end()) {
                break;
            } else if (!ctx.is_indent()) {
                length = part.length() + 1;
            }
            part.push_back(*ctx);
            ctx++;
        }

        // trim
        part.resize(length);
    }
}

// Read tuple
struct ExpectedDelimiterException : public Exception {
    ExpectedDelimiterException(Position at) : Exception(at) {
    }

    std::string describe() const override {
        std::stringstream out;
        out << "expected delimiter at " << at;
        return out.str();
    };
};

template <typename Tuple, std::size_t N> struct TupleReader {
    static void read(Context &ctx, Tuple &t) {
        TupleReader<Tuple, N - 1>::read(ctx, t);

        if (*ctx != ',') {
            throw ExpectedDelimiterException(ctx.position);
        }
        ctx++;

        ctx.skip_indents();

        read_part(ctx, std::get<N>(t));
    }
};

template <typename Tuple> struct TupleReader<Tuple, 0> {
    static void read(Context &ctx, Tuple &t) {
        read_part(ctx, std::get<0>(t));
    }
};

template <typename... Args>
void read_part(Context &ctx, std::tuple<Args...> &part) {
    TupleReader<typeof(part), sizeof...(Args) - 1>::read(ctx, part);
}

struct ExpectedNewlineException : public Exception {
    ExpectedNewlineException(Position at) : Exception(at) {
    }

    std::string describe() const override {
        std::stringstream out;
        out << "expected newline at " << at;
        return out.str();
    };
};

enum ParserState { Initialized, Reading, Ended };

template <typename... Row> class Iterator;

template <typename... Row> class Parser {
  public:
    Parser(std::istream &stream, char delimiter = ',', char quote = '"')
        : ctx(stream, delimiter, quote), state(Initialized) {
    }

    bool read_line(std::tuple<Row...> &t) {
        skip_blank();

        if (state != Reading) {
            return false;
        }

        read_part(ctx, t);

        return true;
    }

    void skip_blank() {
        if (state == Initialized) {
            ctx++;
        } else if (!ctx.is_eol()) {
            throw ExpectedNewlineException(ctx.position);
        }

        ctx.skip_blank();

        if (*ctx == EOF) {
            state = Ended;
            return;
        }

        state = Reading;
    }

    Iterator<Row...> begin() {
        return Iterator<Row...>(*this);
    }

    Iterator<Row...> end() {
        return Iterator<Row...>();
    }

  private:
    Context ctx;
    ParserState state;
};

template <typename... Row> class Iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::tuple<Row...>;
    using pointer = value_type *;
    using reference = value_type &;

    Iterator() : parser(nullptr), end_marker(true), value(), ended(true){};
    Iterator(Parser<Row...> &parser)
        : parser(&parser), end_marker(false), value() {
        ended = !parser.read_line(value);
    };

    value_type operator*() {
        return value;
    }

    void operator++() {
        ended = !parser->read_line(value);
    }

    bool operator==(Iterator &other) {
        return (end_marker && other.ended) || (ended && other.end_marker);
    }

    bool operator!=(Iterator &other) {
        return !(*this == other);
    }

  private:
    Parser<Row...> *parser;
    bool end_marker;
    bool ended;
    value_type value;
};

}; // namespace CSVParser

#endif