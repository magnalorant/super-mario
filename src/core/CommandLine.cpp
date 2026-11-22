#include "mario/core/CommandLine.hpp"

#include <charconv>

namespace mario {

namespace {

std::string valueOf(const std::vector<std::string>& args, std::size_t& i, std::string_view option) {
    if (i + 1 >= args.size()) {
        throw CommandLineError("Missing value for " + std::string(option));
    }
    return args[++i];
}

int parseFrameLimit(const std::string& text) {
    int value = 0;
    const char* end = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(text.data(), end, value);
    if (ec != std::errc{} || ptr != end || value <= 0) {
        throw CommandLineError("Invalid frame count: " + text);
    }
    return value;
}

}  // namespace

CommandLineOptions parseCommandLine(const std::vector<std::string>& args) {
    CommandLineOptions options;
    for (std::size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (arg == "-h" || arg == "--help") {
            options.showHelp = true;
        } else if (arg == "-v" || arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "-a" || arg == "--assets") {
            options.assetDir = valueOf(args, i, arg);
        } else if (arg.rfind("--assets=", 0) == 0) {
            options.assetDir = arg.substr(std::string_view("--assets=").size());
        } else if (arg == "--frames") {
            options.frameLimit = parseFrameLimit(valueOf(args, i, arg));
        } else {
            throw CommandLineError("Unknown option: " + arg);
        }
    }
    if (options.assetDir && options.assetDir->empty()) {
        throw CommandLineError("The asset directory must not be empty");
    }
    return options;
}

std::string usage(std::string_view program) {
    std::string text = "Usage: ";
    text += program;
    text +=
        " [options]\n"
        "\n"
        "Options:\n"
        "  -a, --assets DIR   directory containing fonts/, levels/ and sprites/\n"
        "                     (default: assets next to the executable)\n"
        "  -v, --verbose      log gameplay events to standard error\n"
        "      --frames N     quit after N frames (automated smoke tests)\n"
        "  -h, --help         show this help\n"
        "\n"
        "Controls:\n"
        "  Arrows move, Up/Space jump, Left Shift shoots fireballs, P/Esc pauses.\n"
        "  Editor: 0-8 select tile, left click place, right click erase, S save.\n";
    return text;
}

}  // namespace mario
