#ifndef APP_REGISTER_H
#define APP_REGISTER_H

#include "App.hpp"

#include "plugins/mix/MixPlugin.hpp"
#include "plugins/mute/MutePlugin.hpp"
#include "plugins/resample/ResamplePlugin.hpp"
#include "plugins/vocoder/VocoderPlugin.hpp"

static App create_app() {
    App app;
    app.register_plugin(MixPlugin());
    app.register_plugin(MutePlugin());
    app.register_plugin(ResamplePlugin());
    app.register_plugin(VocoderPlugin());
    return app;
}

#endif
