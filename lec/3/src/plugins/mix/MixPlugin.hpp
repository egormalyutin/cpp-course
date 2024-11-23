#ifndef APP_MIX_PLUGIN_H
#define APP_MIX_PLUGIN_H

#include "../../App.hpp"
#include "MixStream.hpp"

struct MixCommand : public Command {
    std::string name() const override {
        return "mix";
    }

    std::string help() const override {
        return ("      mix $<slot> \n"
                "    or\n"
                "      mix $<slot> <insert at>\n"
                "    combines two tracks (the main one and the selected one) "
                "into one.\n"
                "    if <insert at> is provided, will start mixing at the "
                "selected time.");
    }

    void run(State &state) override {
        auto mix_with = state.read_slot();

        skip_idents(state.ctx);

        size_t insert_at = 0;
        if (!is_eol(*state.ctx)) {
            insert_at = read_unsigned<size_t>(state.ctx);
        }

        auto a = state.slots[0];
        auto b = state.slots[mix_with - 1];
        auto m = box_stream<MixStream<StreamBox<float>, StreamBox<float>>>(
            std::move(a), std::move(b), insert_at * 44100);
        state.slots[0] = m;
    }
};

struct MixPlugin : public Plugin {
    void register_at(App &app) const override {
        app.register_command(std::move(MixCommand()));
    }
};

#endif