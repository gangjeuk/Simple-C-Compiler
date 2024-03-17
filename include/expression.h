#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <stack>
#include <type_traits>
#include <variant>
#include "common.h"
#include "declaration.h"



namespace MMC{
namespace Expression{
using namespace std;
using namespace MMC::utils;
using namespace MMC::Declare;

class UnaryExpression;
class BinaryExpression;
class AssignmentExpression;
class PrimaryExpression;
class ConditionalExpression;
class ExpressionInfo;
class PostfixExpression;
class CastOperator;


using binary_exp_types = variant<std::monostate, BinaryExpression, string, UnaryExpression>;

using assign_exp_types = variant<std::monostate, UnaryExpression, string, AssignmentExpression, ConditionalExpression>;

using primary_exp_types = variant<std::monostate, ExpressionInfo, string>;

using postfix_exp_types = variant<std::monostate, PrimaryExpression,BinaryExpression, string>;

using unary_exp_types = variant<std::monostate, UnaryExpression, PostfixExpression, CastOperator, string>;

using false_exp_types = variant<std::monostate, ConditionalExpression>;




/*
    assignment-expression:
        conditional-expression
        unary-expression assignment-operator assignment-expression

    ---> 마지막으로 정리
            할당식은 단항식, 할당 연산자, 조건식의 리스트
    assignment-expressioin:
        unaryExpr assignOp unaryExpr assginOp condExpr
        unaryExpr assignOp unaryExpr assginOp unaryExpr assignOp condExpr
    

*/
    class AssignmentExpression
    {
        public:
            vector<assign_exp_types> exprTokenList;
            AssignmentExpression(){};
            AssignmentExpression(vector<assign_exp_types> exprTokenList) : exprTokenList(exprTokenList) {};
            static AssignmentExpression getAssignmentExpression(Cstring& buffer);
            bool empty();
            string dump();
            vector<string> get_token();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;
    };

    /*
        수식을 분석하는 getExpressionInfo
    */
    class ExpressionInfo
    {
        public:
            vector<AssignmentExpression> assignmentExprList;
            ExpressionInfo(){};
            ExpressionInfo(vector<AssignmentExpression> assignExprList) : assignmentExprList(assignExprList){};
            static ExpressionInfo getExpressionInfo(Cstring& buffer);
            bool empty() {
                for(auto& iter: assignmentExprList)
                    if(!(iter.empty()))
                        return false;
                return true;
            }
            string dump(){
                string  output;
                for(auto& iter: assignmentExprList)
                    output += iter.dump();
                return output;
            }
            vector<string> get_token();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;

    };
    /*
        binary-expression:
            cast-expression
            binary-expression  binary-operator  cast-expression  

        cast-expression 가 unary-expression 로 대체 가능 하다

        binary-expression:
            unary-expression
            binary-expression binary-operator unary-expression

        ex)
        10      +       2        *      345      +      66;
        ^       ^       ^        ^       ^       ^       ^
        |       |       |        |       |       |       |
     unary    string  unary   string  unary   string   unary 


    */

    class BinaryExpression
    {
        public:
            vector<binary_exp_types> binaryExpression;
            BinaryExpression(){};
            BinaryExpression(vector<binary_exp_types> binaryExpression) : binaryExpression(binaryExpression) {};
            static BinaryExpression getBinaryExpression(Cstring& buffer);
            vector<string> get_token();
            bool empty();
            string dump();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;

    };
    
    /*
        캐스트 연산자
        
    */
    class CastOperator
    {
            public:

            
            DeclarationSpecifier declspec;
            Declarator abs_decl;
            CastOperator(){};
            CastOperator(DeclarationSpecifier declspec,
                         Declarator declarator) : declspec(declspec), abs_decl(declarator){};
            static CastOperator getCastOperator(Cstring& buffer);
            bool empty() {
                if (declspec.empty() && abs_decl.empty())
                    return true;
                return false;
            }
            string dump(){
                string output;
                output += declspec.dump();
                output += abs_decl.dump();
                return output;
            }
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict){};

    };

    // 그냥 연산자 얻는 용도의 클래스
    class GetOperator{

    public:
        static unary_exp_types getPrefixOperator(Cstring& buffer);
    

    };// class GetOperator

    /*
        기본 수식 
        ex)
            num             // D1) identifier
            10              // D2) constant
            "Hello world1"  // D3) string
            (1 + 2)         // D4) (expression)

            접미 수식, 모두 단항식으로 본다

            arr[idx]        // d2) postfix-expression [expression]
            sum()           // d3) postfix-expression ( )
            data.value      // d4) postfix-expression . identifer
            pdata->value    // d5) postfix-expression -> identifier
            num++           // d6) postfix-expression ++ 

    */

    class PrimaryExpression
    {
        public:
            primary_exp_types obj;    // Expression or token<string>
            PrimaryExpression(){};
            PrimaryExpression(string str) : obj(str){};
            PrimaryExpression(ExpressionInfo exprInfo) : obj(exprInfo) {};
            bool objIsString(){
                try{
                    std::get<string>(obj);
                    return true;
                }catch(std::bad_variant_access& e)
                {
                    return false;
                }
            };
            bool empty();
            static PrimaryExpression getPrimaryExpression(Cstring& buffer);
            string dump();
            vector<string> get_token();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;

    }; // class PrimaryExpression

    /*
        postfix-expression:
            primary-expression
            postfix-expression [expression]
            postfix-expression (argument-expression-list(opt))
            postfix-expression . identifier

        ex)
        arr[idx]        // d2) postfix-expression [expression]
        sum()           // d3) postfix-expression ( )
        data.value      // d4) postfix-expression . identifer
        pdata->value    // d5) postfix-expression -> identifier
        num++           // d6) postfix-expression ++ 

    */
        class PostfixExpression
    {
        public:
            vector<postfix_exp_types> exprTokenList;
            PostfixExpression(){};
            PostfixExpression(vector<postfix_exp_types> exprTokenList) : exprTokenList(exprTokenList) {};
            static PostfixExpression getPostfixExpression(Cstring& buffer);
            bool empty();
            string dump();
            vector<string> get_token();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;

    };
    class ConditionalExpression
    {
        public:
            BinaryExpression                    condExpr;
            ExpressionInfo                      trueExpr;
            vector<false_exp_types>             falseExpr;

            ConditionalExpression(){};
            ConditionalExpression(BinaryExpression condExpr, ExpressionInfo trueExpr, vector<false_exp_types> falseExpr):
                                    condExpr(condExpr), trueExpr(trueExpr), falseExpr(falseExpr){};
            static ConditionalExpression getConditionalExpression(Cstring& buffer);
            bool empty();
            string dump();
            vector<string> get_token();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;
    };
    /*
        단항식 (unary expression)
        접미 수식, 증감 연산자, 캐스트 수식, sizeof 수식

        unary-expression:
            postfix-expression
            ++ unary-expression
            -- unary-expression
            unary-operator cast-expression
            sizeof unary-expression
            sizeof ( type-name )

        unary-operator: 단항 연산자
            & * + - ~ !

        ex)
            sum = +a -b;
            *ptr = *p1 * *p2;
            ptr = &num;

        캐스트 수식을 대체할 수 있다.
        cast-expression:
            cast-operator-list(opt)  unary-expression
    */
    class UnaryExpression
    {
        public:
            vector<unary_exp_types> exprTokenList; 
            UnaryExpression(){};
            UnaryExpression(vector<unary_exp_types> exprTokenList) : exprTokenList(exprTokenList) {};
            static UnaryExpression getUnaryExpression(Cstring& buffer);
            bool empty();
            string dump();
            vector<string> get_token();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;
    };

    // struct Debuger{
    //     void operator()(const std::monostate& mono) { cout << ""; }
    //     void operator()(ExpressionInfo& exp_info) { cout << "[Expr "; exp_info.dump(); cout << " ]"; }
    //     void operator()(PrimaryExpression& pri_exp) { cout << "[PriExpr "; pri_exp.dump(); cout << " ]"; }
    //     void operator()(AssignmentExpression& ass_exp) { cout << "[AssignExpr "; ass_exp.dump(); cout << " ]"; }
    //     void operator()(BinaryExpression& bi_exp) { cout << "[BinaryExpr "; bi_exp.dump(); cout << " ]"; }
    //     void operator()(CastOperator& cast_op) { cout << "[CastExpr "; cast_op.dump(); cout << " ]"; }
    //     void operator()(PostfixExpression& postfix_exp) { cout << "[PostExpr "; postfix_exp.dump(); cout << " ]"; }
    //     void operator()(ConditionalExpression& cond_exp) { cout << "[CondExpr "; cond_exp.dump(); cout << " ]"; }
    //     void operator()(UnaryExpression& unary_exp) { cout << "[UnaryExpr "; unary_exp.dump(); cout << " ]"; }
    //     void operator()(const std::string& str) { cout << str; }

    // }; // Debuger

    struct Debuger{
        string operator()(const std::monostate& mono) { cout << ""; return "";}
        string operator()(ExpressionInfo& exp_info) { cout << " "; exp_info.dump(); cout << " ,"; return exp_info.dump();}
        string operator()(PrimaryExpression& pri_exp) { cout << " "; pri_exp.dump(); cout << " "; return pri_exp.dump();}
        string operator()(AssignmentExpression& ass_exp) { cout << " "; ass_exp.dump(); cout << " ,"; return ass_exp.dump();}
        string operator()(BinaryExpression& bi_exp) { cout << " "; bi_exp.dump(); cout << " "; return bi_exp.dump();}
        string operator()(CastOperator& cast_op) { cout << " "; cast_op.dump(); cout << " "; return cast_op.dump();}
        string operator()(PostfixExpression& postfix_exp) { cout << " "; postfix_exp.dump(); cout << " "; return postfix_exp.dump();}
        string operator()(ConditionalExpression& cond_exp) { cout << " "; cond_exp.dump(); cout << " "; return cond_exp.dump();}
        string operator()(UnaryExpression& unary_exp) { cout << " "; unary_exp.dump(); cout << " "; return unary_exp.dump();}
        string operator()(const std::string& str) { cout << str; return str;}

    }; // Debuger
    struct EmptyCheck{
        bool operator()(const std::monostate& mono) { return true; }
        bool operator()(const std::string& str) { if(str.empty()) return true; return false; }
        bool operator()(ExpressionInfo& exp_info) { return exp_info.empty(); }
        bool operator()(PrimaryExpression& pri_exp) {  return pri_exp.empty(); }
        bool operator()(AssignmentExpression& ass_exp) { return ass_exp.empty(); }
        bool operator()(BinaryExpression& bi_exp) { return bi_exp.empty(); }
        bool operator()(CastOperator& cast_op) { return cast_op.empty(); }
        bool operator()(PostfixExpression& postfix_exp) { return postfix_exp.empty(); }
        bool operator()(ConditionalExpression& cond_exp) { return cond_exp.empty(); }
        bool operator()(UnaryExpression& unary_exp) { return unary_exp.empty(); }
    }; // EmptyCheck
    struct Compile{
    void operator()(const std::monostate& mono) {  }
    void operator()(const std::string& str) { }
    void operator()(ExpressionInfo& exp_info) {  exp_info.compile(dataseg, codeseg, identifierDict); }
    void operator()(PrimaryExpression& pri_exp) {   pri_exp.compile(dataseg, codeseg, identifierDict); }
    void operator()(AssignmentExpression& ass_exp) {  ass_exp.compile(dataseg, codeseg, identifierDict); }
    void operator()(BinaryExpression& bi_exp) {  bi_exp.compile(dataseg, codeseg, identifierDict); }
    void operator()(CastOperator& cast_op) {  cast_op.compile(dataseg, codeseg, identifierDict); }
    void operator()(PostfixExpression& postfix_exp) {  postfix_exp.compile(dataseg, codeseg, identifierDict); }
    void operator()(ConditionalExpression& cond_exp) {  cond_exp.compile(dataseg, codeseg, identifierDict); }
    void operator()(UnaryExpression& unary_exp) {  unary_exp.compile(dataseg, codeseg, identifierDict); }

    string& dataseg;
    string& codeseg;
    map<string, tuple<string, int,  int, vector<int>>>& identifierDict;
    
}; // Compile

struct getToken{
    vector<string> operator()(const std::monostate& mono) { return vector<string>(); }
    vector<string> operator()(const std::string& str) { return vector<string>{str}; }
    vector<string> operator()(BinaryExpression& bi_exp) {  return bi_exp.get_token(); }
    vector<string> operator()(UnaryExpression& unary_exp) {  return unary_exp.get_token(); }
    vector<string> operator()(AssignmentExpression& ass_exp) {  return ass_exp.get_token(); }
    vector<string> operator()(PrimaryExpression& pri_exp) {   return pri_exp.get_token();}
    vector<string> operator()(ConditionalExpression& cond_exp) { return cond_exp.get_token(); }
    vector<string> operator()(PostfixExpression& postfix_exp) { return  postfix_exp.get_token(); }
    vector<string> operator()(CastOperator& postfix_exp) { return  vector<string>(); }
    vector<string> operator()(ExpressionInfo& exp) { return  exp.get_token(); }

}; // getToken


} // namespace Expression
} // namespace MMC

#endif