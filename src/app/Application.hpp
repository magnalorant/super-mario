#pragma once

/**
 * @file Application.hpp
 * @brief Wires the SDL frontend to the game core and runs the main loop.
 */

#include <filesystem>

#include "mario/core/CommandLine.hpp"

namespace mario::app {

/**
 * @brief Composition root of the executable.
 *
 * Creates the SDL resources, the SDL renderer adapter and the Game, injects the renderer into
 * the render calls and the event logger into the dispatcher, then runs the frame loop.
 */
class Application {
public:
    /// @param options Parsed command line.
    explicit Application(CommandLineOptions options);

    /**
     * @brief Runs the game until the player quits or the frame limit is reached.
     * @return Process exit code.
     * @throws SdlError if SDL resources cannot be created.
     */
    int run();

private:
    [[nodiscard]] std::filesystem::path resolveAssetDir() const;

    CommandLineOptions options_;
};

}  // namespace mario::app
