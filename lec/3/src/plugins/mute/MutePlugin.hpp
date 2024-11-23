#ifndef APP_MUTE_PLUGIN_H
#define APP_MUTE_PLUGIN_H

#include "../../App.hpp"
#include "MuteStream.hpp"

struct MuteCommand : public Command {
    std::string name() const override {
        return "mute";
    }

    std::string help() const override {
        return ("    mute <start> <end> \n"
                "    mutes the main track at selected time range.");
    }

    void run(State &state) override {
        auto start = read_unsigned<size_t>(state.ctx);

        skip_idents(state.ctx);

        auto end = read_unsigned<size_t>(state.ctx);

        auto s = state.slots[0];
        auto m = box_stream<MuteStream<StreamBox<float>>>(
            std::move(s), start * 44100, end * 44100);
        state.slots[0] = m;
    }
};

struct MutePlugin : public Plugin {
    void register_at(App &app) const override {
        app.register_command(std::move(MuteCommand()));
    }
};

#endif