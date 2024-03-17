#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
namespace MMC{
namespace utils{
using namespace std;

class Exception
{
    public:
        std::string msg;
        Exception(const std::string& msg) : msg(msg) {}
        virtual ~Exception() {}

        virtual std::string what() const {return (msg);}
};

class invalid_value : public Exception
{
    public:
        invalid_value(const std::string& msg) : Exception(msg) {}
        virtual ~invalid_value() {}

    virtual std::string what() const override { return (msg);}
};

class compiler_error : public Exception
{
    public:
        compiler_error(const std::string& msg) : Exception(msg) {}
        virtual ~compiler_error() {}
};

class declarationSpecifier_error : public Exception
{
    public:
        declarationSpecifier_error(const std::string& msg) : Exception(msg) {}
        virtual ~declarationSpecifier_error() {}
};

class declarator_error : public Exception
{
    public:
        declarator_error(const std::string& msg) : Exception(msg) {}
        virtual ~declarator_error() {}
};

class declare_obligation : public Exception
{
    public:
        declare_obligation(const std::string& msg) : Exception(msg) {}
        virtual ~declare_obligation(){}
};


class TokenEater{
    public:
        TokenEater(){};

        bool is_digit(char c) { return isdigit(c);};
        bool is_lower(char c) { return islower(c);};
        bool is_upper(char c) { return isupper(c);};
        bool is_alpha(char c) { return isalpha(c);};
        bool is_alnum(char c) { return isalnum(c);};
        bool is_space(char c) { return isspace(c);};
        bool is_namch(char c) { return (isalnum(c) || (c == '_'));};
        bool is_first_namch(char c) { return (isalpha(c) || (c == '_'));};

        ~TokenEater(){};
};

    class Cstring{
        private:
            string str;
            TokenEater et;

        public:
            mutable long unsigned int idx;

            char getc();
            char peekc();
            bool ungetc();
            bool is_empty();
            bool trim();
            string get_number();
            string get_identifier();
            string get_token();
            string get_ctoken(); // C 토큰 획득
            string get_extra(); // 특수 토큰

            Cstring(const string str) : str(str), idx(0){};

    };

class IsOperator{
private:
map<string, unsigned int> BinaryOperator{{"*", 1}, {"/", 1}, {"%", 1},
                                            {"+", 2}, {"-", 2}, 
                                            {"<<",3}, {">>",3},
                                            {"<", 4}, {">", 4}, {">=", 4}, {"<=",4},
                                            {"==",5}, {"!=",5},
                                            {"&", 6},
                                            {"^", 7},
                                            {"|", 8},
                                            {"&&",9},
                                            {"||",10}};
const vector<string> PrefixOperator{"sizeof", "+", "-", "*", "&", "~", "!", "++", "--"}; // ++ -- not allowed
const vector<string> AssignmentOperator{"=", "+=", "-=", "*=", "/=", "%=", "^=", "|=", "<<=", ">>="};

public:

bool is_binaryOperator(string token){
    return (!( BinaryOperator.find(token) == BinaryOperator.end()));
}
bool is_prefixOperator(string token){
    if(token == "++" || token == "--")
    {
        cout << "not allowed operator ++, --";
        exit(EXIT_FAILURE);
    }
    return (!(find(PrefixOperator.begin(), PrefixOperator.end(), token) == PrefixOperator.end()));
}
bool is_assign_op(string token){
    return (!(find(AssignmentOperator.begin(), AssignmentOperator.end(), token) == AssignmentOperator.end()));
}

unsigned int get_binary_level(string token){
    return BinaryOperator[token];
}

};



} // namespace utils
} // namespace MMC

#endif 