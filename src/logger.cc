
// include selfs file
#include <pico/logger.hpp>

// for cerr
#include <iostream>

// define function
void pico::log(llevel lev, std::string msg, std::string file, size_t l, size_t c, bool putln) {
        
        // what level?
        switch (lev) {
                // tokenizer error
        case pico::tokerr:
                std::cerr << "picoc: \033[1;31mtokenizer error:\033[0m " << file << "(" << l << "," << c << ")" << ": " << msg << "\n";
                break;
        case pico::prserr:
                std::cerr << "picoc: \033[1;31mtokenizer error:\033[0m " << file << "(" << l << "," << c << ")" << ": " << msg << "\n";
                break;
        case pico::prswarn:
                std::cerr << "picoc: \033[1;33mparser warning:\033[0m " << msg << "\n";
                break;
        case pico::prsnote:
                std::cerr << "picoc: \033[1;34mparser note:\033[0m " << msg << "\n";
                break;
        default:
                std::cerr << "picoc: " << msg << "\n";

        }

        if (putln) {
                std::ifstream a(file, std::ifstream::in);
                if (a.bad() || a.fail()) {
                        std::cerr << "<failed to read file line>\n";
                        return (void)0;
                }
                char c='\0';
                int lcount=0;
                while (lcount != l) {
                        a.get(c);
                        if (c == '\n') lcount++;
                }

                std::string line;
                std::getline(a, line);
                std::cerr << line;
        }
}

