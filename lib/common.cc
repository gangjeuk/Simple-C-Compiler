#include "common.h"

using namespace std;

char MMC::utils::Cstring::getc()
{
    if (this->is_empty())
        return false;
    return this->str[this->idx++];
}
char MMC::utils::Cstring::peekc()
{
    if (this->is_empty())
        return false;
    return this->str[this->idx];
}
bool MMC::utils::Cstring::ungetc()
{ // undo getc()
    if (this->idx > 0)
    {
        --this->idx;
        return true;
    }
    return false;
}
bool MMC::utils::Cstring::is_empty()
{
    return (this->idx >= this->str.length());
}
bool MMC::utils::Cstring::trim()
{
    while (this->is_empty() == false)
    {
        if (et.is_space(this->peekc()) == false)
            break;
        this->getc();
    }
    return true;
}

string MMC::utils::Cstring::get_number()
{
    this->trim();
    if (this->is_empty())
        return string("");
    else if (et.is_digit(this->peekc()) == false)
        return string("");

    string int_str;

    if (this->peekc() == '0') // mean number is hex
    {
        this->getc();

        if (this->is_empty())
            return to_string('0');

        if (this->peekc() == 'x')
        {
            this->getc();

            while (this->is_empty() == false)
            {

                if (string("0123456789abcdefABCDEF").find(this->peekc()) == string::npos) // if no hex character
                    break;                                                                // break
                int_str += this->getc();
            }
            return "0x" + int_str;
        }
        else
        { // if not undo getc()
            this->ungetc();
        }
    }

    // decimal
    while (this->is_empty() == false)
    {
        if (et.is_digit(this->peekc()) == false)
            break;
        int_str += this->getc();
    }
    return int_str;
}

string MMC::utils::Cstring::get_identifier()
{
    this->trim();
    if (this->is_empty())
        return string("");
    else if (et.is_first_namch(this->peekc()) == false)
        return string("");
    string identifier;
    while (this->is_empty() == false)
    {
        if (et.is_namch(this->peekc()) == false)
            break;
        identifier += this->getc();
    }
    return identifier;
}

string MMC::utils::Cstring::get_token()
{
    this->trim();
    char c = this->peekc();
    string result;

    if (et.is_digit(c))
        result = this->get_number();
    else if (et.is_first_namch(c))
        result = this->get_identifier();
    else
    {
        if (c == '\"' || c == '\'')
        {                             // start of "string" or 'c'
            result.clear();           // reset
            char quot = this->getc(); // save quot

            while (this->is_empty() == false) // while something on buffer
            {
                char sch = this->peekc();

                if (sch == '\\')
                {                 // if escape
                    this->getc(); // get next char
                    char next = this->getc();
                    char ech; // for escape sequence

                    switch (next)
                    {
                    case 'n':
                        ech = '\n';
                        break;
                    case 'r':
                        ech = '\r';
                        break;
                    case 't':
                        ech = '\t';
                        break;
                    case '0':
                        ech = '\0';
                        break;
                    case '\\':
                        ech = '\\';
                        break;
                    default:
                        throw utils::invalid_value("invalid escape sequence");
                    }
                    result += to_string(ech); // add escape sequence
                }
                else if (sch == quot)
                { // if quot finish string aquire
                    this->getc();
                    break;
                }
                else
                { // rest string to result
                    result += to_string(this->getc());
                }
            }
            result = quot + result + quot;
        }
        else if (c == '[')
        { // if start of memory
            result.clear();
            this->getc(); // pass '['
            while (this->is_empty() == false)
            {
                if (this->peekc() == ']')
                    break;
                result += this->getc(); // get char ---> 1234 ex: char c[1234];
            }
            this->getc();                // '[' already used. pass ']' and
            result = '[' + result + ']'; // add [] to string
        }
        else
        {
            result = this->getc();
        }
    }

    return (result.empty() ? string("") : result);
}

string MMC::utils::Cstring::get_ctoken()
{
    this->trim();
    char c = this->peekc();
    string result;

    if (et.is_digit(c))
        result = this->get_number();
    else if (et.is_first_namch(c))
        result = this->get_identifier();
    else if (c == '\"' || c == '\'')
    {                             // start of "string" or 'c'
        result.clear();           // reset
        char quot = this->getc(); // save quot

        while (this->is_empty() == false) // while something on buffer
        {
            char sch = this->peekc();

            if (sch == '\\')
            {                 // if escape
                this->getc(); // get next char
                char next = this->getc();
                char ech; // for escape sequence

                switch (next)
                {
                case 'n':
                    ech = '\n';
                    break;
                case 'r':
                    ech = '\r';
                    break;
                case 't':
                    ech = '\t';
                    break;
                case '0':
                    ech = '\0';
                    break;
                case '\\':
                    ech = '\\';
                    break;
                default:
                    throw utils::invalid_value("invalid escape sequence");
                }
                result += to_string(ech); // add escape sequence
            }
            else if (sch == quot)
            { // if quot finish string aquire
                this->getc();
                break;
            }
            else
            { // rest string to result
                result += to_string(this->getc());
            }
        }
        result = quot + result + quot;
    }
    else
    {
        result = this->get_extra();
    }

    return (result.empty() ? string("") : result);
}

string MMC::utils::Cstring::get_extra()
{
    if (this->str.empty())
        string();

    char c = this->getc();
    char nextc = this->peekc();
    string op;
    op += c;

    switch (c)
    {
    case '+':
        if (string("+=").find(nextc) != string::npos)
            op += this->getc();
        break;
    case '-':
        if (string("-=>").find(nextc) != string::npos) // [ - -- -= -> ]
            op += this->getc();
        break;
    case '*': // [ * *= */ ]
        if (string("=/").find(nextc) != string::npos)
            op += nextc;
        break;
    case '/': // [ / /= // /* ]
        if (string("=/*").find(nextc) != string::npos)
            op += this->getc();
        break;
    case '.': // 가변 인자를 표현하는 "..." 토큰인지 검사합니다.
        if (this->str.substr(this->idx, 2) == "..")
        {
            this->idx += 2;
            op = string("...");
        }
        break;
    case '<': // [ < << <= <<= ]
        if (nextc == '<')
        { // << 또는 <<=
            op += this->getc();
            nextc = this->peekc();
            if (nextc == '=')
                op += this->getc();
        }
        else if (string("<=").find(nextc) != string::npos)
            op += this->getc();
        break;
    case '>': // [ > >> >= >>= ]
        if (nextc == '>')
        { // >> 또는 >>=
            op += this->getc();
            nextc = this->peekc();
            if (nextc == '=')
                op += this->getc();
        }
        else if (string(">=").find(nextc) != string::npos)
            op += this->getc();
        break;
    case '&': // [ & &= && ]
        if (string("=&").find(nextc) != string::npos)
            op += nextc;
        break;
    case '|': // [ | |= || ]
        if (string("=|").find(nextc) != string::npos)
            op += nextc;
        break;
    case '%': // [ % %= ]
    case '=': // [ = == ]
    case '!': // [ ! != ]
    case '^': // [ ^ ^= ]
        if (nextc == '=')
            op += this->getc();
        break;
    case '~':
        break;
    }

    return op;
}
