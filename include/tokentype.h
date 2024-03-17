#ifndef TOKENTYPE_H
#define TOKENTYPE_H

#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>



namespace MMC{
using namespace std;   
    namespace TokenType{
        class token_type{
            public:


            static bool is_storageClass(const string str) {
                    vector<string> storageClass = {"auto", "register", "static", "extern", "typedef"};
                    return find(storageClass.begin(), storageClass.end(), str) == storageClass.end() ? false : true;}

            static bool is_typeQualifier(const string str) {
                 vector<string> typeQualifier = {"const", "volatile"};
                return find(typeQualifier.begin(), typeQualifier.end(), str) == typeQualifier.end() ? false : true;}

            static bool is_typeSpecifier(const string str) {
                 vector<string> typeSpecifier = {"void", "char", "short", "int", "long", "float", "double", "signed", "unsigned", "struct", "union", "enum"};
                return find(typeSpecifier.begin(), typeSpecifier.end(), str) == typeSpecifier.end() ? false : true;}

            static bool is_pointerToken(const string str){
                return (str == "*" || is_typeQualifier(str));
            }
        }; // class token_type
    }
}

#endif