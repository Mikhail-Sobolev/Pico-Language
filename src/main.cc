
// include IO
#include <iostream>

// include FS
#include <filesystem>

// include pico functions
#include <pico/tokenize.hpp>
#include <pico/parse.hpp>
#include <pico/logger.hpp>

int main(int argc, char **argv) {

        // check if a filename was given
        if (argc != 2) {

                // if not or extra data given
                // print error and exit with failure.
                pico::log(pico::enverr, "no file name given\nusage: picoc [source_file_name]", "", 0, 0, false);
                exit(1);
        }

        // save cwd
        std::filesystem::path cur = std::filesystem::current_path();

        // set core include directory
        pico::core_inc = std::string("") + argv[1];
        pico::core_inc = pico::core_inc.substr(0, pico::core_inc.find_last_of("\\/"));
        pico::core_inc += "pico_core/";
        // set work include directory
        pico::work_inc = ".";

        // tokenize file
        pico::tokenizer tokenizer = pico::tokenizer(std::string(argv[1]));

        // parse tokens into an AST
        pico::parser parser = pico::parser(tokenizer);

        // set the current path back to what it was
        std::filesystem::current_path(cur);

        // exit with success
        return 0;
}
