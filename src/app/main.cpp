#include <SDL.h>

#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "Application.hpp"
#include "mario/core/CommandLine.hpp"

int main(int argc, char* argv[]) {
    const std::string program = argc > 0 ? argv[0] : "super_mario";
    try {
        const mario::CommandLineOptions options =
            mario::parseCommandLine(std::vector<std::string>(argv + (argc > 0 ? 1 : 0), argv + argc));
        if (options.showHelp) {
            std::cout << mario::usage(program);
            return 0;
        }
        return mario::app::Application(options).run();
    } catch (const mario::CommandLineError& error) {
        std::cerr << error.what() << "\n\n" << mario::usage(program);
        return 2;
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << '\n';
        return 1;
    }
}
