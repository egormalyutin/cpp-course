#include "App.hpp"
#include "config/Context.hpp"
#include "misc/MelFilterBank.hpp"
#include "register.hpp"
#include "streams/FileStream.hpp"
#include "streams/Stream.hpp"
#include "streams/WavStream.hpp"
#include <exception>
#include <system_error>

#define ESC "\033["

struct AppArgs {
    std::string config_path;
    std::string config;
    std::string out_path;
    std::vector<std::string> in_paths;
};

AppArgs parse_args(App &app, std::span<char *> argv) {
    std::string config_path;
    std::string out_path;
    std::vector<std::string> in_paths;

    size_t arg = 1;
    while (arg < argv.size()) {
        if (!strcmp(argv[arg], "-h") || !strcmp(argv[arg], "--help")) {
            std::string bin_name = argv[0];
            std::cout << app.help(bin_name) << std::endl;
            exit(0);
        } else if (!strcmp(argv[arg], "-c")) {
            arg++;
            if (config_path != "") {
                throw std::runtime_error("config path was specified twice");
            } else if (arg == argv.size()) {
                throw std::runtime_error(
                    "expected path to config file after -c");
            }
            config_path = argv[arg];
        } else if (out_path == "") {
            out_path = argv[arg];
        } else {
            in_paths.emplace_back(argv[arg]);
        }

        arg++;
    }

    if (out_path == "") {
        throw std::runtime_error("output file was not specified");
    } else if (config_path == "") {
        throw std::runtime_error("config file was not specified");
    }

    // read config
    std::fstream cfg_file;
    cfg_file.exceptions(std::ios::failbit);
    cfg_file.open(config_path);
    std::stringstream cfg_buffer;
    cfg_buffer << cfg_file.rdbuf();
    auto cfg = cfg_buffer.str();

    return AppArgs{
        .config_path = config_path,
        .config = cfg,
        .out_path = out_path,
        .in_paths = in_paths,
    };
}

int main(int argc, char **argv) {
    auto app = create_app();

    AppArgs args;

    try {
        args = parse_args(app, std::span(argv, argc));
    } catch (std::runtime_error &e) {
        std::cerr << "invalid params: " << e.what() << std::endl;
        return 1;
    }

    // hide cursor
    std::cout << ESC "?25l";

    try {
        app.run(args.config, args.out_path, args.in_paths, [](float progress) {
            std::cout << ESC "2K\r" << progress * 100. << "%";
            std::cout.flush();
        });
    } catch (ConfigException &e) {
        std::cout << ESC "?25h" ESC "2K\r";
        pretty_print_error(args.config_path, args.config, e);
        return 1;
    } catch (std::runtime_error &e) {
        std::cout << ESC "?25h" ESC "2K\r";
        std::cerr << "runtime error: " << e.what() << std::endl;
        return 1;
    }
    std::cout << ESC "?25h" ESC "2K\rcomplete!\n";
}