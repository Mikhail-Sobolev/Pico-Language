
// pragma
#ifndef PICO_TOKENIZE_HPP
#define PICO_TOKENIZE_HPP

#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <stack>
#include <magic_enum.hpp>

#define PICO_EOF 0xff

namespace pico {  

        #ifndef TOKENIZE_CC
        // strings for core and work include directories
        extern std::string core_inc, work_inc;
        #endif

        // define point (a description of a specific characters
        // location in a file)
        typedef struct point {
                std::size_t lin, col;   // (line, column) => for error logging; (loc) => the physical location in the file
                                        // assuming it to have been written flat in memory.
                std::string fname;      // name of file the point is in. 
        };

        typedef enum : size_t {

                // litterals
                tok_intlit=0,
                tok_strlit,
                tok_fltlit,
                
                // keywords
                tok_return,
                tok_proc,
                tok_func,
                tok_syscall,
                tok_var,
                tok_struct,
                tok_type,
                tok_if,
                tok_elif,
                tok_else,
                tok_for,
                tok_while,
                tok_const,
                tok_export,
                tok_import,
                tok_importc,
                tok_asm,

                // characters
                tok_semi,
                tok_coma,
                tok_rcurl,
                tok_lcurl,
                tok_rparm,
                tok_lparm,
                tok_rbox,
                tok_lbox,
                tok_col,
                tok_eq,
                tok_dot,
                tok_bslash,

                // non-comparative double characters
                tok_dcol,       // double colon
                tok_ddot,       // double dot
                tok_larr,       // left thin arrow (<-)
                tok_rarr,       // right thin arrow (->)
                tok_aeq,        // add equals (+=)
                tok_seq,        // subtract equals (-=)
                tok_meq,        // multiply equals (*=)
                tok_deq,        // divide equals (/=)

                // arithemtic symbols
                tok_plus,
                tok_minus,
                tok_asterisk,
                tok_fslash,

                // comparatives
                tok_ceq,
                tok_neq,
                tok_leq,
                tok_geq,
                tok_less,
                tok_grt,

                // miscs
                tok_name,
                tok_none,

                // file info
                tok_eof,
                tok_eol,
        } toktype;

        // define token
        typedef struct token {

                std::string tostr();
                void copy_to(pico::token *a);

                pico::point sta, end;   // start and end points.
                pico::toktype type;     // type of token.
                std::string str;        // string representation of the token.
        } token;    

        // define tokenizer
        class tokenizer {
        public:
                // init and distroy functions
                tokenizer(std::string name);
                ~tokenizer();

                // get the next token
                pico::token nexttok();
                bool eof();

        private:

                char nextchar();
                void tokenize();

                size_t                     tokcount; // token system
                std::vector<pico::token>   tokens;

                std::stack<std::ifstream*> files; // stack of open files
                std::stack<std::string>    names; // stack of file names
                std::stack<size_t>         lines, columns; // stack of lines and columns
                                                           
                std::vector<std::string>   included_files; // list of files already included by the compiler
                                                           // used to not re-enclude any files
        };
}
#endif
