#ifndef APP_APP_H
#define APP_APP_H

#include "config/Context.hpp"
#include "misc/OutputWriter.hpp"
#include "streams/FileStream.hpp"
#include "streams/Stream.hpp"
#include "streams/WavStream.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sched.h>
#include <span>
#include <sstream>
#include <streambuf>
#include <unordered_map>
#include <vector>

struct State {
    Context &ctx;
    std::vector<StreamBox<float>> slots;

    size_t read_slot() {
        auto start = ctx.position;
        auto slot_id = read_slot_id(ctx);
        auto end = ctx.position;
        if (slot_id == 0) {
            throw ConfigError(start, end, "slot ids start from 1");
        } else if (slot_id > slots.size()) {
            throw ConfigError(start, end,
                              format("slot ", slot_id, " is empty"));
        }
        return slot_id;
    }
};

struct Command {
    virtual std::string name() const = 0;
    virtual std::string help() const = 0;
    virtual void run(State &state) = 0;
    virtual ~Command() = default;
};

struct App;

struct Plugin {
    virtual void register_at(App &app) const = 0;
    virtual ~Plugin() = default;
};

template <typename P>
static void write_wav_stream(OutputWriter &output, Stream<float> &stream,
                             P on_progress) {
    WavHeader header;
    std::memcpy(header.chunk_id, "RIFF", 4);
    header.chunk_size = 0;
    std::memcpy(header.format, "WAVE", 4);
    std::memcpy(header.subchunk_1_id, "fmt ", 4);
    header.subchunk_1_size = 16;
    header.audio_format = 1;
    header.num_channels = 1;
    header.sample_rate = 44100;
    header.byte_rate = 44100 * 2;
    header.block_align = 2;
    header.bits_per_sample = 16;

    output.write((char *)&header, sizeof(header));

    char id[] = "data";
    std::uint32_t size = 0;

    output.write(id, 4);
    output.write((char *)&size, 4);

    size_t buffer_size = 44100;
    std::vector<float> float_buffer(buffer_size);
    std::vector<int16_t> int_buffer(buffer_size);

    size_t progress = 0;

    while (1) {
        auto read = stream.read_full(float_buffer);

        for (size_t i = 0; i < read; i++) {
            int_buffer[i] = float_buffer[i];
        }

        output.write((char *)int_buffer.data(), read * 2);

        progress += read;

        // if (progress > 44100 * 5000) {
        //     throw std::runtime_error("file too big???");
        // }

        on_progress((float)progress / (float)*stream.length());
        if (read < buffer_size) {
            break;
        }
    }
}

struct App {
    template <typename P> void register_plugin(P plugin) {
        plugin.register_at(*this);
    }

    template <typename C> void register_command(C &&cmd) {
        std::unique_ptr<Command> cmd_ptr = std::make_unique<C>(std::move(cmd));
        commands.emplace(cmd.name(), std::move(cmd_ptr));
    }

    StreamBox<float>
    get_output_stream(const std::string &config,
                      const std::vector<std::string> &in_paths) {
        // prepare slots
        std::vector<StreamBox<float>> _slots;

        for (size_t i = 0; i < in_paths.size(); i++) {
            auto slot =
                box_stream<WavStream<FileStream>>(FileStream(in_paths[i]));
            _slots.push_back(std::move(slot));
        }

        auto _ctx = Context(config);

        // create execution state
        State state{.ctx = _ctx, .slots = std::move(_slots)};

        // parse config and run plugins
        skip_blank(state.ctx);

        while (*state.ctx != EOF) {
            auto cmd = read_word(state.ctx, "command");

            if (!commands.contains(cmd)) {
                throw std::runtime_error(
                    format("unknown command: \"", cmd, "\""));
            }

            skip_idents(state.ctx);

            commands[cmd]->run(state);

            skip_comment(state.ctx);

            skip_blank(state.ctx);
        }

        return std::move(state.slots[0]);
    }

    template <typename P>
    void run(const std::string &config, const std::string &out_path,
             const std::vector<std::string> &in_paths, P on_progress) {
        auto stream = get_output_stream(config, in_paths);
        FileOutputWriter writer(out_path);
        write_wav_stream(writer, stream, on_progress);
    }

    std::vector<char> run_collect(const std::string &config,
                                  const std::vector<std::string> &in_paths) {
        auto stream = get_output_stream(config, in_paths);
        VectorOutputWriter writer;
        write_wav_stream(writer, stream, [](float progress) {});
        return writer.to_vector();
    }

    std::string help(std::string &bin_name) {
        std::stringstream ss;

        ss << "usage: \n";
        ss << "  " << bin_name
           << " [-h] [-c config.txt output.wav input1.wav [input2.wav...]]\n\n";

        ss << "an audio processing program with support of multiple plugins "
              "and commands, "
              "works with WAV PCM files.\n\n";

        ss << "example of config file:\n";
        ss << "  # perform multiple operations\n";
        ss << "  mix $2 3 # mix with second track, starting from 3rd second\n";
        ss << "  mute 5 10 # mute from 5th to 10th second\n";
        ss << "  resample 200% # speed up by 2 times";

        ss << "\n\navailable commands:\n";

        bool first = true;

        for (auto &[_, cmd] : commands) {
            if (!first) {
                ss << "\n\n";
            }
            first = false;
            ss << "  " << cmd->name() << ":\n";
            ss << cmd->help();
        }

        return ss.str();
    }

  private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commands;
};

#endif