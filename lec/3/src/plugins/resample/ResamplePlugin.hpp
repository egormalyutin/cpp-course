#ifndef APP_RESAMPLE_PLUGIN_H
#define APP_RESAMPLE_PLUGIN_H

#include "../../App.hpp"
#include "ResampleStream.hpp"

struct ResampleCommand : public Command {
    std::string name() const override {
        return "resample";
    }

    std::string help() const override {
        return (
            "    resample <speed>% \n"
            "    speeds up or slows down the main track by specified factor.");
    }

    void run(State &state) override {
        auto factor = read_unsigned<size_t>(state.ctx);
        skip_idents(state.ctx);
        skip_word(state.ctx, "%");

        auto s = state.slots[0];
        auto m = box_stream<ResampleStream<StreamBox<float>>>(
            std::move(s), (float)factor / 100.);
        state.slots[0] = m;
    }
};

struct ResamplePlugin : public Plugin {
    void register_at(App &app) const override {
        app.register_command(std::move(ResampleCommand()));
    }
};

#endif