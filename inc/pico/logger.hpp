
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <cstdint>
#include <fstream>

namespace pico {
        typedef enum {
                tokerr=0,
                prserr,
                prswarn,
                prsnote,
                generr,
                genwarn,
                gennote,
                enverr
        } llevel;

        void log(pico::llevel lev, std::string msg, std::string filename, size_t l, size_t c, bool putln);
}

#endif
