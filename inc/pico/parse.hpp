
#ifndef PICO_PARSE_HPP
#define PICO_PARSE_HPP

// for tokenizer, token enum, and token type
#include <pico/tokenize.hpp>

// for error logging
#include <pico/logger.hpp>

// standard libraries
#include <string>
#include <vector>
#include <map>

namespace pico {

        typedef enum {
                ntype_binop,
                ntype_var,
                ntype_const,
                ntype_syscall,
                ntype_export,
                ntype_import,
                ntype_int_lit,
                ntype_str_lit,
                ntype_flt_lit,
                ntype_func_call,
                ntype_proc_call,
                ntype_if,
                ntype_while,
                ntype_for,
                ntype_asm
        } node_type;

        typedef struct node {
                pico::node_type type;
        } node;

        class parser {
        public:
                parser(pico::tokenizer tokens);

                pico::node outast;
        private:
                pico::
        };

}

#endif
