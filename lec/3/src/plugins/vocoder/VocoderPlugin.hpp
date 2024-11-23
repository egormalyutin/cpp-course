#ifndef APP_VOCODER_PLUGIN_H
#define APP_VOCODER_PLUGIN_H

#include "../../App.hpp"
#include "VocoderStream.hpp"

struct VocoderCommand : public Command {
    std::string name() const override {
        return "vocoder";
    }

    std::string help() const override {
        return ("    vocoder $<slot id>\n"
                "    makes your speech sound like a song");
    }

    void run(State &state) override {
        auto mix_with = state.read_slot();

        skip_idents(state.ctx);

        auto a = state.slots[0];
        auto b = state.slots[mix_with - 1];
        auto m = box_stream<VocoderStream<StreamBox<float>, StreamBox<float>>>(
            std::move(a), std::move(b), 2048, 1024);
        state.slots[0] = m;
    }
};

struct VocoderPlugin : public Plugin {
    void register_at(App &app) const override {
        app.register_command(std::move(VocoderCommand()));
    }
};

#endif