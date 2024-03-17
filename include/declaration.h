#ifndef DECLARATION_H
#define DECLARATION_H

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <variant>

#include "common.h"
#include "tokentype.h"


namespace MMC{
namespace Declare{
using namespace std;
using namespace MMC::utils;

class basic_declare;

class DeclarationSpecifier;
class DirectDeclarator;
class PointerStack;

class Declarator;
class InitDeclarator;
class InitDeclaratorList;

class ParameterDeclaraton;
class ParameterTypeList;

template<class> inline constexpr bool always_false_v = false;

using token_types = variant<std::monostate, MMC::Declare::PointerStack, MMC::Declare::Declarator, MMC::Declare::ParameterTypeList>;
// helper type for the visitor #4
// template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// // explicit deduction guide (not needed as of C++20)
// template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    class basic_declare{
        virtual string dump() = 0;
    };

    /*
        declaration specifer: ko)대상에 대한 형태를 지정할 때 사용하는 요소
                              en)Elements used to specify the form of an object.

            storage class: ko) 대상이 기억 장소에 저장되는 형태
                auto, static, register, extern, typedef

            type specifier: ko) 대상의 본질적인 형식
                void, char, short ... int, unsigned, struct-or-union-specifier, enum, typedef-name

            type qualifier: ko) 대상의 특별한 성질
                const, volatile                    

    */
    class DeclarationSpecifier : public basic_declare
    {
        private:


        public:
            string storageClassSpecifier;
            vector<string> typeSpecifier;
            vector<string> typeQualifier;
            DeclarationSpecifier(string storageClassSpecifier,
                                vector<string> typeSpecifier,
                                vector<string> typeQualifier): storageClassSpecifier(storageClassSpecifier) , typeSpecifier(typeSpecifier), typeQualifier(typeQualifier){}
            DeclarationSpecifier(){};
            
            // should not use except test case
            static DeclarationSpecifier get_declarationSpecifier(const string buffer);
            static DeclarationSpecifier get_declarationSpecifier(Cstring& buffer);
/*
            string dump() override{
                cout << "{ storage Class Specifier: " << (storageClassSpecifier.empty() ? "null" : storageClassSpecifier) << ", typeSpecifier: [ ";
                for(const auto& iter : typeSpecifier)
                    cout << (iter.empty() ? "null" : iter) << ", ";
                cout << " ], typeQualifier: [ ";
                for(const auto& iter : typeSpecifier)
                    cout << (iter.empty() ? "null" : iter) << ", ";
                cout << " ] ] }";
            }
*/
            bool empty(){
                if(storageClassSpecifier.empty() &&
                    typeQualifier.empty() &&
                    typeSpecifier.empty() )
                    return true;
            
                return false;
            }
            
            string dump() override{
                string output;
                cout << (storageClassSpecifier.empty() ? "" : " " + storageClassSpecifier) << " ";
                output += ((storageClassSpecifier.empty() ? "" : " " + storageClassSpecifier));
                if(!typeQualifier.empty()){
                    for(auto& iter: typeQualifier)
                    {    cout << iter << " "; output += iter + " ";}
                }else{
                    cout << " ";
                    output += " ";
                }
                if(!typeSpecifier.empty()){
                    for(auto& iter: typeSpecifier)
                        {cout << iter << ", "; output += iter + " ";}
                }else{
                    cout << " ";
                    output += " ";
                }
                return output;
            }
    }; // class DeclarationSpecifier








    class DirectDeclarator : public basic_declare
    {
        public:

        static void dirdcl(Cstring& buffer, vector<token_types>& out);
        static void absdirdcl(Cstring& buffer, vector<token_types>& out);

        string dump() override{
            cout << "DirectDeclarator" << "\n";
            return "";
        };

    };// class DirectDeclarator






    class PointerStack : public basic_declare
    {
        public:
            string token;
            PointerStack();
            PointerStack(string token) : token(token){};
            bool empty()
            {
                if(token.empty())
                    return false;
                return true;
                
            }
            string dump() override{
                cout << token;
                return token;
            };
    };

/*
    declarator:
        pointer(opt) direct-declarator

        direct declarator: ko) 기억 장소에 직접 접근하는 선언자

        Important!!: ko) 직접 선언자와 선언자의 차이
                         만약 int num = A; 과 int *ptr = &num; 이 있을경우
                         num은 num이 A 그 자체 이기때문에 num이라는 식별자를 사용하면 변수 A에 직접 접근할 수 있다.
                         하지만 ptr의 경우 ptr을 통해 A에 접근하려면 갖고 있는 A의 "주소" 값을 통해 간접적으로 접근해야 한다.

                         여기서 '직접 선언자' 와 '선언자'의 차이가 나타난다
                         즉 선언자 ⊂ 직접 선언자 인 것이다 
                         즉 적접 선언자와 다르게 선언자의 경우 대상에 접근하는 방법이 직접적인지, 간접적인지 상황에 따라 다르다.
*/
        class Declarator : public basic_declare
    {
        public:
            string identifier;
            vector<token_types> descriptor;
            Declarator() {};
            Declarator(string identifier, vector<token_types> descriptor) : identifier(identifier), descriptor(descriptor){};

            static Declarator getDeclarator(Cstring& buffer);
            static void dcl(Cstring& buffer, vector<token_types>& out);        // 선언자 분석

            static Declarator getAbstractDeclarator(Cstring& buffer); // 추상 선언자
            static void absdcl(Cstring& buffer, vector<token_types>& out);

            bool empty(){
                if(identifier.empty() && descriptor.empty())
                    return true;
                return false;
            }
           
            string dump() override;
                    


                //    std::visit(overloaded {
                //        [](std::monostate arg) {cout << " ";},
                //        [](PointerStack arg) {arg.dump();},
                //        [](Declarator arg) {arg.dump();},
                //        [](DeclarationSpecifier arg) {arg.dump();},
                //        [](ParameterTypeList arg) {arg.dump();}
                //    }, iter);
                // cout << " ]";
                //}
            
            /*  
            void DeclarationSpecifier::dump(){
                if(!this->storageClassSpecifier.empty())
                    cout << "storage Class Specifier: " << this->storageClassSpecifier << "\n";
                if(!this->typeQualifier.empty()){
                    cout << "type Qualifier: ";
                    for (const auto& iter : this->typeQualifier)
                        cout << iter << " ";
                    cout << "\n";
                }
                if(!this->typeSpecifier.empty()){
                    cout << "type Specifier: ";
                    for (const auto& iter : this->typeSpecifier)
                    cout << iter << " ";
                    cout << "\n";
                }
                cout << "\n";
            }

*/
    }; // class Declarator

    class InitDeclarator : public basic_declare
    {
        public:
            MMC::Declare::Declarator dcl;
            string initializer;

            InitDeclarator() {};
            InitDeclarator(Declarator dcl, string initializer) : dcl(dcl), initializer(initializer) {};
            InitDeclarator(Declarator dcl) : dcl(dcl) {};

        static InitDeclarator getInitDeclarator(Cstring& buffer);
        string dump() override{
            string output;
            output += "[ ";
            cout << "[ "; output += dcl.dump(); cout << " ]" ; output += " ]"; output += initializer; cout << initializer ;  

            return output;
        };

    };
    
    class InitDeclaratorList : public basic_declare
    {
        public:
            vector<InitDeclarator> init_decl;
            
        static InitDeclaratorList getInitDeclaratorList(Cstring& buffer);
        bool empty(){
            if(init_decl.empty())
                return true;
            return false;
        }
        string dump() override{
            string output;
            for(auto& iter: this->init_decl){
                output += iter.dump();
            }
            return output;
        };

    }; // class InitDeclaratorList

    class DeclarationInfo : public basic_declare
    {
        public:
            DeclarationSpecifier declspec;
            InitDeclaratorList  init_decl_list;
            DeclarationInfo() {};
            DeclarationInfo(DeclarationSpecifier declspec, InitDeclaratorList init_decl_list) : declspec(declspec), init_decl_list(init_decl_list){};
            static DeclarationInfo getDeclarationInfo(Cstring& buffer);

            bool empty() {
                if(declspec.empty() && init_decl_list.empty())
                    return true;
                return false;
            }
            string dump() override{
                string output;
                output += "[";
                cout << "[ "; output += declspec.dump(); cout << " "; output += " "; output += init_decl_list.dump(); cout << " ]";
                output += "]";
                return output;
            };
    };
    class ParameterDeclaraton : public basic_declare
    {
        public:
            DeclarationSpecifier declspec;
            Declarator declarator;

            ParameterDeclaraton() {};
            ParameterDeclaraton(DeclarationSpecifier declspec, Declarator decl_list) : declspec(declspec), declarator(decl_list) {};

            static ParameterDeclaraton getParameterDeclaration(Cstring& buffer);

            bool empty(){
                if(declspec.empty() && declarator.empty())
                    return true;
                return false;
            }
            string dump() override{
                string output;
                output += declarator.dump();
                output += declspec.dump();
                return output;
        };
            
    };
    class ParameterTypeList : public basic_declare
    {
        public:
            vector<ParameterDeclaraton> param_decl;

            static ParameterTypeList getParameterTypeList(Cstring& buffer);
            bool empty(){
                if(param_decl.empty())
                    return true;
                return false;
            }
            string dump() override{
                string output;
                output += "(";
                cout << "( ";
                for(auto& iter: param_decl)
                    output += iter.dump(); 
                cout << " )";
                output += ")";
                return output;
            };
    };

    struct Debuger{
        string operator()(const std::monostate& mono) { cout << ""; return "";}
        string operator()(const std::string& str) { cout << str; return str;}
        string operator()(PointerStack& ass_exp) { ass_exp.dump(); return ass_exp.dump();}
        string operator()(Declarator& bi_exp) { bi_exp.dump(); return bi_exp.dump();}
        string operator()(ParameterTypeList& unary_exp) { unary_exp.dump(); return unary_exp.dump();}
    }; // Debuger

} // namespace Declarator
} // namespace MMC

#endif