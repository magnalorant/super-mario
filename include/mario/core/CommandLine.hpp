#pragma once

/**
 * @file CommandLine.hpp
 * @brief Parsing of the game's command line arguments.
 */

#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace mario {

/// @brief Raised for unknown options or missing option values.
class CommandLineError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/**
 * @brief Settings selected on the command line.
 */
struct CommandLineOptions {
    std::optional<std::filesystem::path> assetDir;  ///< Overrides the asset directory next to the executable.
    std::optional<int> frameLimit;  ///< Quit automatically after this many frames, used by smoke tests.
    bool verbose{false};            ///< Log gameplay events to standard error.
    bool showHelp{false};           ///< Print usage and exit.
};

/**
 * @brief Parses the arguments following the program name.
 * @param args Arguments without argv[0].
 * @throws CommandLineError on unknown options or invalid values.
 */
[[nodiscard]] CommandLineOptions parseCommandLine(const std::vector<std::string>& args);

/**
 * @brief Builds the help text.
 * @param program Name used to start the program.
 */
[[nodiscard]] std::string usage(std::string_view program);

}  // namespace mario
