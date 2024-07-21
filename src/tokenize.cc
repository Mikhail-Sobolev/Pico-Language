
// include the header
#include <pico/tokenize.hpp>

// include the logger
#include <pico/logger.hpp>

#include <sstream>
#include <iostream>
#include <iterator>
#include <cstring>
#include <filesystem>

std::string pico::work_inc = "";
std::string pico::core_inc = "";

// convert token to string
std::string pico::token::tostr() {
        std::stringstream strstr;
        strstr << "(" <<  magic_enum::enum_name(this->type) << " \"" << this->str << "\" (start \"" << this->sta.fname << "\" " << this->sta.lin << " " << this->sta.col << ") "
               << "(end \"" << this->end.fname << "\" " << this->end.lin << " " << this->end.col << "))";
        return strstr.str();
}

// move token 
void pico::token::copy_to(pico::token *tok) {
        std::memcpy(tok, this, sizeof(pico::token));
}


/// CLASS *(pico::tokenizer) \\\

// initializer


pico::tokenizer::tokenizer(std::string name) {
        
        // create file
        std::ifstream a(name, std::ifstream::in);

        // test for file not found error
        if (a.bad() || a.fail()) {
                pico::log(pico::tokerr, "failed to open file", name, 1, 1, false);
                exit(0);
        }

        std::cout << name.substr(0, name.find_last_of("\\/")) << "\n";

        // set current directory
        std::filesystem::current_path(std::filesystem::path(name.substr(0, name.find_last_of("\\/"))));

        // push file and thereby save it
        this->files.emplace(&a);
        this->names.emplace(name.substr(name.find_last_of("\\/")+1, name.size()));
        this->lines.emplace(1);
        this->columns.emplace(1);
        this->tokenize();
}

// destructor
pico::tokenizer::~tokenizer() {
        
        // loop through file
        for (int x=0; x<this->files.size(); x++) {
                
                // pop everything
                this->files.pop();
                this->names.pop();
                this->lines.pop();
                this->columns.pop();
        }
}

// is file EOF?
bool pico::tokenizer::eof() {
        return this->files.top()->eof() || this->files.top()->bad() || this->files.top()->fail();
}

char pico::tokenizer::nextchar() {
        
        // test and return EOF
        if (this->eof()) return PICO_EOF;
        
        size_t a=0;

        // otherwise return next character
        char c; this->files.top()->get(c);
        if (c == -1) { return PICO_EOF; }

        // increament lines and/or columns
        if (c == '\n')  {
                // if newline, increament line
                a = this->lines.top()+1;
                this->lines.pop();
                this->lines.emplace(a);

                // and set column to 1
                this->columns.pop();
                this->columns.emplace(1);
        } else {
                // otherwise increament column
                a = this->columns.top()+1;
                this->columns.pop();
                this->columns.emplace(a);
        }
        return ((char)c);
}

#define SET_POINT(p) p.fname = this->names.top(); p.lin = this->lines.top(); p.col = this->columns.top()

// finaly the actual tokenization!
void pico::tokenizer::tokenize() {
        
        pico::token tok;
        std::string buf = "";
        char c;

        while (!this->eof()) {

                // reset token to none
                SET_POINT(tok.sta);
                tok.type = pico::tok_none;

                // flags
                bool alp_str=false,dig_str=false,str_str=false,inc_str=false,firstloop=true,inc_gnext=false;

                // comment flags
                bool single_line_comments=false, multi_line_comments=false;

                while (!this->eof()) {

                        c = this->nextchar();
                
                        if (single_line_comments) {
                                if (c == '\n') {
                                        single_line_comments = false;
                                }
                                continue;
                        }

                        if (multi_line_comments) {
                                if (buf == "!" && c == ')') {
                                        multi_line_comments = false;
                                        c = this->nextchar();
                                }
                                buf = c;
                                continue;
                        }

                        
                        // handle comments
                        if (buf == "!" && c == '!') {
                                single_line_comments = true;
                                buf = "";
                                continue;
                        }

                        if (buf == "(" && c == '!') {
                                multi_line_comments = true;
                                buf = "";
                                continue;
                        }

                        // check if including file
                        if (inc_str) {
                                // check for inc get next
                                if (inc_gnext) {
                                        buf += c; 
                                        continue;
                                }
                                // check for end of inc_str
                                if (c == ';') { 
                                        inc_str = false;
                                        std::string tpath = "";
                                        std::vector<std::string> path = {std::string("")};
                                        for (int x=0;x<buf.size();x++) {
                                                if (buf[x] == '\\') {
                                                        inc_gnext = true;
                                                        continue;
                                                }
                                                if (inc_gnext) {
                                                        path[path.size()-1] = path[path.size()-1] + buf[x];
                                                        continue;
                                                }
                                                if (buf[x] == '.') {
                                                        path.push_back("");
                                                }  else {
                                                        path[path.size()-1] = path[path.size()-1] + buf[x];
                                                }
                                        }

                                        // check for 
                                        if (path.size() == 0) {
                                                pico::log(pico::tokerr, "tried to include from empty line", this->names.top(), this->lines.top(), this->columns.top(), true);
                                                exit(1);
                                        } 
                                        if (path[0] == "core") {
                                                tpath += pico::core_inc;
                                        } else if (path[0] == "work") {
                                                tpath += pico::work_inc;
                                        } else {
                                                pico::log(pico::tokerr, "unknown entry to include directory", this->names.top(), this->lines.top(), this->columns.top(), true);
                                                exit(1);
                                        }

                                        for (int x = 1; x <path.size(); x++) {
                                                tpath += "/";
                                                tpath += path[x];
                                        }

                                        tpath += ".pico";

                                        std::filesystem::path opath = std::filesystem::current_path();
                                        std::filesystem::current_path(std::filesystem::path(tpath.substr(0, tpath.find_last_of("\\/"))));
       
                                        std::cout << opath.string() << "\n";

                                        std::ifstream file(tpath.substr(tpath.find_last_of("\\/")+1, tpath.size()));

                                        if (file.bad() || file.fail()) {
                                                pico::log(pico::tokerr, "failed to open file", tpath.substr(tpath.find_last_of("\\/")+1, tpath.size()), 1, 1, false);
                                                exit(1);
                                        }

                                        this->files.emplace(&file);
                                        this->names.emplace(tpath.substr(tpath.find_last_of("\\/")+1, tpath.size()));
                                        this->lines.emplace(1);
                                        this->columns.emplace(1);
                                       
                                        this->tokenize();
                                        this->tokens.pop_back();

                                        std::filesystem::current_path(opath);

                                        this->files.pop();
                                        this->names.pop();
                                        this->lines.pop();
                                        this->columns.pop();
                                        tok.type = pico::tok_none;
                                        buf = " ";
                                        c = this->nextchar();
                                        firstloop = false;
                                        continue;
                                } else if (c == ' ' || c == '\t') {
                                        continue;
                                } else if (c == '\\') {
                                        inc_gnext = true;
                                }
                                buf += c;
                                continue;
                        }

                        // test last char
                        
                        // check if it is a digit and if this is the end of the digit
                        if (dig_str && !std::isdigit(c)) {
                                break;
                        } 
                        // check if it is a name/keyword and if it is the end of said name/keyword
                        else if (alp_str && !(std::isalpha(c) || std::isdigit(c) || c=='_')) {

				// check for keywords (note that if a word is a keyword at this stage it does not mean it will end as a keyword, ie prc_c vs prc)
                                if (buf == "prc")               tok.type = pico::tok_proc;
                                else if (buf == "fnc")          tok.type = pico::tok_func;
                                else if (buf == "return")       tok.type = pico::tok_return;
                                else if (buf == "syscall")      tok.type = pico::tok_syscall;
                                else if (buf == "const")        tok.type = pico::tok_const;
                                else if (buf == "var")          tok.type = pico::tok_var;
                                else if (buf == "struct")       tok.type = pico::tok_struct;
                                else if (buf == "type")         tok.type = pico::tok_type;
                                else if (buf == "if")           tok.type = pico::tok_if;
                                else if (buf == "elif")         tok.type = pico::tok_elif;
                                else if (buf == "else")         tok.type = pico::tok_else;
                                else if (buf == "for")          tok.type = pico::tok_for;
                                else if (buf == "while")        tok.type = pico::tok_while;
                                else if (buf == "importc")      tok.type = pico::tok_importc;
                                else if (buf == "asm")          tok.type = pico::tok_asm;
                                else if (buf == "require") {
                                        inc_str = true;
                                        buf = "";
                                        continue;
                                }
                                break;
                        } 
                        // check if it is a string
                        else if (str_str) {
                                if (c == '"') {
                                        str_str = false;
                                        buf += c;
                                        c = this->nextchar();
                                        break;
                                }
                        }
                        
                        // check for newline
                        else if (buf == "\n") {
                                tok.type = pico::tok_eol;
                                buf = "\\n";    // for better logging
                                break;
                        } 
                        // check for curly backets
                        else if (buf == "{") {
                                tok.type = pico::tok_lcurl;
                                break;
                        } 
                        else if (buf == "}") {
                                tok.type = pico::tok_rcurl;
                                break;
                        } 
                        // check for comma
                        else if (buf == ",") {
                                tok.type = pico::tok_coma;
                                break;
                        }
                        // check for semi-colon
                        else if (buf == ";") {
                                tok.type = pico::tok_semi;
                                break;
                        }
                        // check for params
                        else if (buf == "(") {
                                tok.type = pico::tok_lparm;
                                break;
                        }
                        else if (buf == ")") {
                                tok.type = pico::tok_rparm;
                                break;
                        }
                        // check for box brackets
                        else if (buf == "[") {
                                tok.type = pico::tok_lbox;
                                break;
                        }
                        else if (buf == "]") {
                                tok.type = pico::tok_rbox;
                                break;
                        }
                        // check for back slash
                        else if (buf == "\\") {
                                tok.type = pico::tok_bslash;
                                break;
                        } 

                        // check for dot
                        else if (buf == ".") {
                                tok.type = pico::tok_dot;
                                if (c == '.') {
                                        tok.type = pico::tok_ddot;
                                        c = this->nextchar();
                                }
                                break;
                        }
                        // check for plus
                        else if (buf == "+") {
                                tok.type = pico::tok_plus;
                                if (c == '=') {
                                        tok.type = pico::tok_aeq;
                                        c = this->nextchar();
                                }
                                break;
                        }
                        // check for minus
                        else if (buf == "-") {
                                tok.type = pico::tok_minus;
                                if (c == '>')           tok.type = pico::tok_rarr;
                                else if (c == '=')      tok.type = pico::tok_seq;
                                else                    break;
                                c = this->nextchar();
                                break;
                        }
                        // check for asterisk
                        else if (buf == "*") {
                                tok.type = pico::tok_asterisk;
                                if (c == '=') {
                                        tok.type = pico::tok_meq;
                                        c = this->nextchar();
                                }
                                break;
                        } 
                        // check for front slash
                        else if (buf == "/") {
                                tok.type = pico::tok_fslash;
                                if (c == '=') {
                                        tok.type = pico::tok_deq;
                                        c = this->nextchar();
                                }
                                break;
                        }
                        // check for colon
                        else if (buf == ":") {
                                tok.type = pico::tok_col;
                                if (c == ':') {
                                        tok.type = pico::tok_dcol;
                                        c = this->nextchar();
                                }
                                break;
                        }
                        // check for equals
                        else if (buf == "=") {
                                tok.type = pico::tok_eq;
                                if (c == '=') {
                                        tok.type = pico::tok_deq;
                                        c = this->nextchar();
                                }
                                break;
                        }
                        // check for exclamation mark and equals
                        else if (buf == "!" && c == '=') {
                                tok.type = pico::tok_neq;
                                c = this->nextchar();
                                break;
                        }
                        // check for less
                        else if (buf == "<") {
                                tok.type = pico::tok_less;
                                if (c == '=')           tok.type = pico::tok_leq;
                                else if (c == '-')      tok.type = pico::tok_larr;
                                else                    break;
                                c = this->nextchar();
                                break;
                        }
                        // check for more 
                        else if (buf == ">") {
                                tok.type = pico::tok_grt;
                                if (c == '=') {
                                        tok.type = pico::tok_geq;
                                        c = this->nextchar();
                                }
                                break;
                        }

                        // check for next character in word/keyword
                        else if (std::isalpha(c) || c=='_' || ((!firstloop) && std::isdigit(c))) {
                                alp_str = true;
                                tok.type = pico::tok_name;

                                if (firstloop) {
                                        if (!(std::isalpha(buf[0]) || buf[0]=='_')) buf = "";
                                }
                        } 
                        // check for next character in digit
                        else if (std::isdigit(c)) {
                                dig_str = true;
                                tok.type = pico::tok_intlit;
                                if (firstloop) {
                                        if (!(std::isdigit(buf[0]))) buf = "";
                                }
                        } 
                        // check for string litteral
                        else if (buf == "\"") {
                                str_str = true;
                                tok.type = pico::tok_strlit;
                        }

                        // check for single letter
                        else if (std::isalpha(buf[0]) || buf[0]=='_') {
                                tok.type = pico::tok_name;
                                break;
                        }
                        // check for single number
                        else if (std::isdigit(buf[0])) {
                                tok.type = pico::tok_intlit;
                                break;
                        }

                        // un-used characters i.e. ' ', '\t'... will be ignored
                        else {
                                break;
                        }
                        buf += c;
                        firstloop = false;
                }
                
                if (tok.type != pico::tok_none) {
                        // set token string and end
                        tok.str = buf;
                        SET_POINT(tok.end);
        
                        // append to token array
                        this->tokens.push_back(tok);
                }

                buf = c;
        }
        this->tokcount = 0;
        tok.str = "";
        SET_POINT(tok.end);
        SET_POINT(tok.sta);
        tok.type = pico::tok_eof;
        this->tokens.push_back(tok);
}

pico::token pico::tokenizer::nexttok() {
        if (this->tokcount >= this->tokens.size()) {
                pico::log(pico::tokerr, "asked for token past EOF token", std::string("!!"), 1, 1, false);
                exit(1);
        }
        pico::token tok = this->tokens[this->tokcount];
        this->tokcount++;
        return tok;
}
