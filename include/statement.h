#ifndef STATEMENT_H
#define STATEMENT_H
#include <vector>
#include <string>
#include <variant>
#include <memory>
#include "common.h"
#include "expression.h"
#include "declaration.h"

namespace MMC{
namespace Statement{
using namespace std;
using namespace MMC::utils;
/*
    문장
    statement:
        labeled-statement       // 레이블 ex) case, default

        expression-statement    // 수식문 ex) expression(opt);

        compound-statement      // 복합문 ex) {declaration-list(opt)  statement-list(opt)}

        selection-statement     // 선택문 ex) if (expression) statement
                                              if (expression) statement else statement
                                              switch (expression) statement

        iteration-statement     // 반복문 ex) while (expression) statement
                                              do statement while (expression); 
                                              for (expression(opt); expression(opt); expression(opt)) statement

        jump-statement          // 점프   ex) goto identifier ;
                                              continue ;
                                              break ;
                                              return expression(opt) ;

*/

class ExpressionStatementInfo;
class CompoundStatementInfo;
class SelectionStatementInfo;
class IterationStatementInfo;
class JumpStatementInfo;
class LabeledStatementInfo;
class FlowBlock;

using label_stmt_types = variant<std::monostate, Expression::ExpressionInfo, std::string>;

using stmt_info_types = variant<std::monostate, CompoundStatementInfo, SelectionStatementInfo, IterationStatementInfo, JumpStatementInfo, LabeledStatementInfo, ExpressionStatementInfo>;

using jump_stmt_types = label_stmt_types;


    class ExpressionStatementInfo{
        public:
            Expression::ExpressionInfo expr_info;
            ExpressionStatementInfo(Expression::ExpressionInfo expr_info) : expr_info(expr_info){};
            ExpressionStatementInfo(){};
            bool empty(){
                if(expr_info.empty())
                    return true;
                return false;
            }
            static ExpressionStatementInfo getExpressionStatementInfo(Cstring& buffer);
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;
            void regAlloc(vector<FlowBlock>& in);
    }; // class Expression Statement
    class JumpStatementInfo{
        public:
            string jumpType;
            jump_stmt_types operand;

            JumpStatementInfo(){};
            JumpStatementInfo(string jumpType, jump_stmt_types operand) : jumpType(jumpType), operand(operand) {};
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;
            void regAlloc(vector<FlowBlock>& in); 
                       bool empty();
            void dump();
            static JumpStatementInfo getStateJumpStatementInfomentInfo(Cstring& buffer);
    }; // class JumpStatementInfo
    class StatementInfo{
        private:

        public:
            shared_ptr<stmt_info_types> obj;    // TODO: danger. use vector instead of pointer
            StatementInfo(){};
            static StatementInfo getStatementInfo(Cstring& buffer);
            void dump();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;
                       bool empty();
    }; // class StatementInfo


    class CompoundStatementInfo{
        public:
            vector<Declare::DeclarationInfo> decl_list;
            vector<StatementInfo> stmt_list;
            CompoundStatementInfo(){};
            CompoundStatementInfo(vector<Declare::DeclarationInfo> decl_list, vector<StatementInfo> stmt_list) : decl_list(decl_list), stmt_list(stmt_list){};
            bool empty();
            void dump();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;
            void regAlloc(vector<FlowBlock>& in); 
                       static CompoundStatementInfo getCompoundStatementInfo(Cstring& buffer);
    }; // class CompoundStatement

    class SelectionStatementInfo{
        public:
            string selectionType;
            Expression::ExpressionInfo condition;
            StatementInfo stmt;
            StatementInfo else_stmt;

            SelectionStatementInfo(){};
            SelectionStatementInfo(string selectionType,Expression::ExpressionInfo condition,StatementInfo stmt,StatementInfo else_stmt)  
                                   : selectionType(selectionType), condition(condition), stmt(stmt), else_stmt(else_stmt) {};
            bool empty();
            void dump();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;
            void regAlloc(vector<FlowBlock>& in);
                        static SelectionStatementInfo getSelectionStatementInfo(Cstring& buffer);
    }; // class SelectionStatementInfo

    class IterationStatementInfo{
        public:
            string iterationType;
            Expression::ExpressionInfo condition;
            StatementInfo statement;
            Expression::ExpressionInfo initialier;
            Expression::ExpressionInfo iterator;

            IterationStatementInfo(){};
            IterationStatementInfo(string iterationType,Expression::ExpressionInfo condition,StatementInfo statement,Expression::ExpressionInfo initialier,Expression::ExpressionInfo iterator)
                                    : iterationType(iterationType), condition(condition), statement(statement), initialier(initialier), iterator(iterator) {};
            bool empty();
            void dump();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;
            void regAlloc(vector<FlowBlock>& in);
                        static IterationStatementInfo getIterationStatementInfo(Cstring& buffer);
    }; // class IterationStatementInfo


    class LabeledStatementInfo{
        public:
            string labelType;
            label_stmt_types value;
            StatementInfo statement;

            LabeledStatementInfo(){};
            LabeledStatementInfo(string labelType, label_stmt_types value, StatementInfo statement) : labelType(labelType), value(value), statement(statement){};
            bool empty();
            void dump();
            void compile(string& dataseg, string& codeseg, map<string, tuple<string, int,  int, vector<int>>>& identifierDict) ;
            void regAlloc(vector<FlowBlock>& in);
                        static LabeledStatementInfo getLabeledStatementInfo(Cstring& buffer);

    }; // class LabeledStatementInfo


class FlowBlock{
    using stmt_info_types = MMC::Statement::stmt_info_types;


    void search_use();
    
    void add_inblock_tokens(FlowBlock flow_block);    // tokens = use U out[n-1]
    
    void cal_out(const vector<FlowBlock>& full_block);

    vector<string> unique(vector<string> a1, vector<string> a2){
        a1.insert(a1.end(), a2.begin(), a2.end());
        sort(a1.begin(), a1.end());
        a1.erase(std::unique(a1.begin(), a1.end()), a1.end());
        return a1;
    }
public:
    stmt_info_types m_block;
    string label;
    unsigned long block_idx;
    int jump;
    vector<int> succ;
    vector<string> use;
    vector<string> tokens;
    vector<string> out;
    FlowBlock() {jump = -1; block_idx = -1;};
    FlowBlock(stmt_info_types stmt) : m_block(stmt){ search_use(); jump = -1; block_idx = -1;};
    vector<string> getOut(){ return out; }

}; // class FlowBlock
    struct Debuger{
        void operator()(std::monostate& stat) { cout << ""; }
        void operator()(std::string& str) { cout << str; }
        void operator()(ExpressionStatementInfo& stat) { cout << "[ "; stat.expr_info.dump(); cout << " ]"; }
        void operator()(JumpStatementInfo& stat) { cout << " " ; stat.dump(); cout << " "; }
        void operator()(StatementInfo& stat) { stat.dump(); cout << " "; }
        void operator()(CompoundStatementInfo& stat) { cout << "  "; stat.dump(); cout << " "; }
        void operator()(SelectionStatementInfo& stat) { cout << "  "; stat.dump(); cout << " "; }
        void operator()(IterationStatementInfo& stat) { cout << "  "; stat.dump(); cout << " "; }
        void operator()(LabeledStatementInfo& stat) { cout << " "; stat.dump(); cout << " "; }
        void operator()(Expression::ExpressionInfo& exp_info){cout << " "; exp_info.dump(); cout << " "; }

    }; // Debuger


    struct EmptyCheck
    {       

        bool operator()(std::monostate& mono) {return  true;}
        bool operator()(std::string& str) {if(str.empty()) return  true; return  false;}
        bool operator()(ExpressionStatementInfo& stmt) {if(stmt.empty()) return  true; return  false;}
        bool operator()(JumpStatementInfo& stmt){if(stmt.empty()) return  true; return  false;}
        bool operator()(CompoundStatementInfo& stmt){if(stmt.empty()) return  true; return  false;}
        bool operator()(SelectionStatementInfo& stmt){if(stmt.empty()) return  true; return  false;}
        bool operator()(IterationStatementInfo& stmt){if(stmt.empty()) return  true; return  false;}
        bool operator()(LabeledStatementInfo& stmt){if(stmt.empty()) return  true; return  false;}
        bool operator()(Expression::ExpressionInfo& exp_info){if(exp_info.empty()) return  true; return  false;}

    };
    struct Compile{
    void operator()(const std::monostate& mono) {  }
    void operator()(const std::string& str) { }
    void operator()(ExpressionStatementInfo& exp_info) {  exp_info.expr_info.compile(dataseg, codeseg, identifierDict); }
    void operator()(CompoundStatementInfo& pri_exp) {   pri_exp.compile(dataseg, codeseg, identifierDict); }
    void operator()(SelectionStatementInfo& ass_exp) {  ass_exp.compile(dataseg, codeseg, identifierDict); }
    void operator()(IterationStatementInfo& bi_exp) {  bi_exp.compile(dataseg, codeseg, identifierDict); }
    void operator()(JumpStatementInfo& cast_op) {  cast_op.compile(dataseg, codeseg, identifierDict); }
    void operator()(LabeledStatementInfo& postfix_exp) {  postfix_exp.compile(dataseg, codeseg, identifierDict); }

    string& dataseg;
    string& codeseg;
    map<string, tuple<string, int,  int, vector<int>>>& identifierDict;
}; // Compile


    struct RegAlloc{
    void operator()(const std::monostate& mono) {  }
    void operator()(const std::string& str) { }
    void operator()(ExpressionStatementInfo& exp_info) {  exp_info.regAlloc(in); }
    void operator()(CompoundStatementInfo& pri_exp) {   pri_exp.regAlloc(in); }
    void operator()(SelectionStatementInfo& ass_exp) {  ass_exp.regAlloc(in); }
    void operator()(IterationStatementInfo& bi_exp) {  bi_exp.regAlloc(in); }
    void operator()(JumpStatementInfo& cast_op) {  cast_op.regAlloc(in); }
    void operator()(LabeledStatementInfo& postfix_exp) {  postfix_exp.regAlloc(in); }

    vector<FlowBlock>& in;
    
}; // Compile
}
}



#endif