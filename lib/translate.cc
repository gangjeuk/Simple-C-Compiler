#include "translate.h"

using namespace std;
using namespace MMC::Units;
using namespace MMC::TokenType;
using namespace MMC::Declare;
using namespace MMC::Statement;

/*
int arr[100] = {0, 1};      <---------  translate-unit::external-declaration::declaration
int func(int n);            <---------  translate-unit::external-declaration::declaratioin

int main(void){}            <---------  translate-unit::external-declaration::function-definition

int func(int n){}           <---------  translate-unit::external-declaration::function-definition

*/

void Translator::translateUnit(Cstring &buffer)
{
    buffer.trim();
    while (buffer.is_empty() == false)
    { // until translateUnit left
        Translator::externalDeclaration(buffer);
        buffer.trim();
    }
}

// 선언 분석 및 예외 분석
void Translator::declaration(Cstring &buffer)
{
    DeclarationSpecifier decl_spcf;
    InitDeclaratorList init_decl_list;
    try
    {
        decl_spcf = DeclarationSpecifier::get_declarationSpecifier(buffer);
        init_decl_list = InitDeclaratorList::getInitDeclaratorList(buffer);
    }
    catch (MMC::utils::Exception &e)
    {
        cout << "declaration catched Exception"
             << "\n";
        e.what();
    }

    for (const auto &iter : init_decl_list.init_decl)
    {
        string initializer = iter.dcl.identifier;
        // 변수 선언의 경우
        // ex) static int a, b;
        if (init_decl_list.init_decl.front().dcl.descriptor.empty())
        {
            cout << initializer << " : ";
            Declare::Declarator decl = iter.dcl;
            decl.dump();
            cout << "->";
            decl_spcf.dump();

            cout << "\n";
        }
        // 함수 선언의 경우
        // int func(int a, int b);
        else
        {
            cout << initializer << " : ";
            Declare::Declarator decl = iter.dcl;
            decl.dump();
            cout << "->";
            decl_spcf.dump();

            cout << "\n";
        }
    }

    // pass semicolon
    buffer.get_ctoken();

    return;
}

bool Translator::is_funcdef(Cstring &buffer)
{
    long unsigned int orig_idx = buffer.idx;

    DeclarationSpecifier decl_spec = DeclarationSpecifier::get_declarationSpecifier(buffer);
    // 선언 지정자 획들 실패시에 함수 정의
    if (decl_spec.empty())
    {
        buffer.idx = orig_idx;
        return true;
    }

    Declarator declarator = Declarator::getDeclarator(buffer);
    if (declarator.empty())
    {
        buffer.idx = orig_idx;
        return false;
    }

    DeclarationInfo decl_info = DeclarationInfo::getDeclarationInfo(buffer);

    // in case of int func(int a, int b) {}
    if (!decl_info.empty())
    {
        buffer.idx = orig_idx;
        return true;
    }

    buffer.trim();

    // 토큰이 여는 중괄호이면 함수 정의
    // in case of int func() {}
    if (buffer.peekc() == '{')
    {
        buffer.idx = orig_idx;
        return true;
    }

    buffer.idx = orig_idx;
    return false;
}

//
void Translator::externalDeclaration(Cstring &buffer)
{
    if (Translator::is_funcdef(buffer))
        Translator::functionDefinition(buffer);
    else
        Translator::declaration(buffer);
}

void Translator::functionDefinition(Cstring &buffer)
{

    DeclarationSpecifier decl_spec = DeclarationSpecifier::get_declarationSpecifier(buffer);

    Declarator declarator = Declarator::getDeclarator(buffer);

    // 선언 리스트 획득
    ParameterTypeList func_para = std::get<ParameterTypeList>(declarator.descriptor.front());

    CompoundStatementInfo compound = CompoundStatementInfo::getCompoundStatementInfo(buffer);

    string func_name = declarator.identifier;
    string func_return_type = decl_spec.typeSpecifier.front();

    cout << func_name << "\n";
    codeseg += func_name + ":\n";
    codeseg += "addi    $sp, $sp, -32\n";
    codeseg += "sw $ra, 20($sp)\n";
    for (size_t i = 0; i < func_para.param_decl.size(); i++)
    {
        string identifier = func_para.param_decl[i].declarator.identifier;
        codeseg += "sw $a" + to_string(i) + ", " + to_string(4 * i) + "($sp)\n";
    }
    codeseg += "addiu   $fp, $sp, 28\n";

    identifierDict[func_name] = make_tuple("$v1", -1, -1, vector<int>());

    // 함수 인자 부터 처리
    for (size_t i = 0; i < func_para.param_decl.size(); i++)
    {
        string identifier = func_para.param_decl[i].declarator.identifier;
        string reg_name = "$a" + to_string(i);
        int obj_size = 4;
        identifierDict[identifier] = make_tuple(reg_name, 1, 0, vector<int>(obj_size));
    }

    vector<StatementInfo> stmt_list = compound.stmt_list;
    vector<DeclarationInfo> decl_list = compound.decl_list;
    StatementInfo stmt_return;

    // 일단 여기서 스코프 처리
    // 최적화 알고리즘

    reg_usage = {{"$t0", false}, {"$t1", false}, {"$t2", false}, {"$t3", false}, {"$t4", false}, {"$t5", false}, {"$t6", false}, {"$t7", false}, {"$t8", false}, {"$t9", false}, {"$s7", false}, {"$s6", false}, {"$s5", false}};
    in.clear();
    in = vector<FlowBlock>(1);
    in[0].block_idx = 0;
    identifierDict.clear();

    // 제일 처음의 in 에 함수 인자를 넣어준다
    for (size_t i = 0; i < func_para.param_decl.size(); i++)
    {
        string identifier = func_para.param_decl[i].declarator.identifier;
        identifierDict[identifier] = make_tuple("$a" + to_string(i), -1, -1, vector<int>());
        in[0].tokens.push_back(identifier);
        in[0].use.push_back(identifier);
    }

    // 배열 처리
    for (auto iter : decl_list)
    {
        //                                           0 조작 위험 int a[2], n[3] 같은식 불가 TODO
        string ident = iter.init_decl_list.init_decl[0].dcl.identifier;
        cout << "test" << ident;
        long unsigned int arr_size = 1;
        vector<int> size_vec;
        if (iter.init_decl_list.init_decl[0].dcl.descriptor.size() != 0)
        {
            for (auto descp : iter.init_decl_list.init_decl[0].dcl.descriptor)
            {
                arr_size *= stoi(std::get<PointerStack>(descp).token);
                size_vec.push_back(stoi(std::get<PointerStack>(descp).token));
            }
            dataseg += (ident + ": .word ");
            for (long unsigned int arr_idx = 0; arr_idx < arr_size; arr_idx++)
                dataseg += to_string(0) + ", ";
            dataseg += "\n";
            size_vec.erase(size_vec.begin());
            size_vec.push_back(1);
        }
        identifierDict["(" + ident + ")"] = make_tuple(ident, -1, -1, size_vec);
    }

    // 레지스터 생존 정보 분석을 위한 식                    |
    // in[n] = use[n] U (out[n] - def[n])                  v    in[n]
    // out[n] = U in[s]                             ----------------
    //        s ∈ succ[n]                          |               |
    //                                              |      n        |
    //                                              |_______________|
    //                                                     |  out[n]
    //                                                     v

    for (size_t i = 0; i < stmt_list.size(); i++)
    {

        stmt_info_types statement = (*stmt_list[i].obj);

        if (std::holds_alternative<IterationStatementInfo>(statement))
            std::visit(RegAlloc{in}, statement);
        else if (std::holds_alternative<ExpressionStatementInfo>(statement))
            std::visit(RegAlloc{in}, statement);
        else if (std::holds_alternative<JumpStatementInfo>(statement))
            std::visit(RegAlloc{in}, statement);
        else if (std::holds_alternative<SelectionStatementInfo>(statement))
            std::visit(RegAlloc{in}, statement);
    }

    // 각 블록의 생성 후
    // 블록별 사용사는 레지스터 지정
    // 블록 별로(블록은 무조건 expression이다) compile 함수 콜
    vector<string> using_reg;
    for (auto iter = in.begin() + 1; iter < in.end(); iter += 1)
    {
        // 사용 하는 레지스터 업데이트
        update_iden_dict((*iter));
        int for_check_idx = iter->block_idx;
        cout << for_check_idx << "\n";
        auto block_type = iter->m_block;
        auto block_label = iter->label;

        if (std::holds_alternative<ExpressionStatementInfo>(block_type))
        {
            std::get<ExpressionStatementInfo>(block_type).compile(dataseg, codeseg, identifierDict);
        }
        else if (std::holds_alternative<JumpStatementInfo>(block_type))
        {
            auto jumpType = std::get<JumpStatementInfo>(block_type).jumpType;
            auto operand = std::get<JumpStatementInfo>(block_type).operand;
            if (jumpType == "goto")
            {
                string jump_label = std::get<string>(operand);
                codeseg += "j " + jump_label + "\n nop \n";
            }
            else if (jumpType == "continue" || jumpType == "break")
            {
            }
            else if (jumpType == "return")
            {
                if (func_return_type == "void")
                    throw compiler_error("return non void value");
                if (isdigit(std::visit(Expression::getToken(), operand).front()[0]))
                {
                    codeseg += "ori $v1, $zero," + std::get<string>(operand) + "\n";
                    if (iter != in.end() - 1)
                        codeseg += "addi $sp, $sp, 32\njr  $ra\n";
                    continue;
                }
                string return_token = std::visit(Expression::getToken(), operand).front();
                string use_reg = std::get<0>(identifierDict[return_token]);

                codeseg += "or $v1," + use_reg + ", $zero \n";

                if (iter != in.end() - 1)
                    codeseg += "addi $sp, $sp, 32\njr  $ra\n";
            }
        }
        // 라벨 블록 처리
        if (block_label.empty() == false)
        {
            codeseg += block_label;
            codeseg += ":\n";
        }
    }

    // finish procesor
    for (size_t i = 0; i < func_para.param_decl.size(); i++)
    {
        string identifier = func_para.param_decl[i].declarator.identifier;
        codeseg += "lw $a" + to_string(i) + ", " + to_string(4 * i) + "($sp)\n";
    }
    codeseg += "lw $ra, 20($sp)\n";
    codeseg += "lw $fp, 16($sp)\n";
    codeseg += "addi $sp, $sp, 32\n";
    // TODO return 문 완성
    codeseg += "jr  $ra\n nop \n";
}

string Translator::dump()
{
    string tmp;
    tmp += "INITIAL_GP = 0x10008000 \nINITIAL_SP = 0x7ffffffc\n# system call service number\nprint_int = 1 \nprint_string = 4 \nstop_service = 99\n	# system call service number";

    tmp += "\n.text\n" + codeseg + "\n" + string(".data\n") + dataseg + "\nCR_MESSAGE:\n  .asciiz \"\\n\" \nTAB_MESSAGE:\n  .asciiz \"\\t\"";

    cout << tmp;
    return tmp;
}

void Translator::update_iden_dict(MMC::Statement::FlowBlock fb)
{

    if (fb.label.empty() == false && fb.jump == -1)
        return;

    changed = true;

    vector<string> use = fb.use;
    // succ 가 존재하면 넣는다
    for (auto &it : fb.succ)
    {
        FlowBlock succ_block = in[it];
        update_iden_dict(succ_block);
    }
    if (fb.block_idx != 0 && changed == true)
    {
        update_iden_dict(in[fb.block_idx - 1]);
    }
    for (auto it : use)
    {
        // $a 는 패스
        string iden = it;
        if (identifierDict.find(iden) == identifierDict.end())
        {
            auto reg_name = std::get<0>(identifierDict[iden]);
            if (!reg_name.compare(0, 2, "$a") || !reg_name.compare(0, 2, "$v") || !reg_name.compare(0, 2, "$__") || !reg_name.compare(0, 1, "("))
            {
                continue;
            }
            // identifierDict 정리후 그냥 다 넣기
            reg_name = get_small_reg();
            reg_usage[reg_name] = true;
            identifierDict[it] = make_tuple(reg_name, 1, 0, vector<int>{4});
            changed = true;
        }
    }

    if (identifierDict.find("goto") != identifierDict.end()) // 전에 점프문이 있었으면 삭제
    {
        identifierDict.erase(identifierDict.find("goto"));
        changed = true;
    }
    if (identifierDict.find("ifgoto") != identifierDict.end()) // 전에 점프문이 있었으면 삭제
    {
        identifierDict.erase(identifierDict.find("ifgoto"));
        changed = true;
    }
    // 점프 문일 경우 dict에 goto로 넣음
    if (fb.jump != -1) // jump == -1 는 점프 없다
    {
        string jump_label = in[fb.jump].label;
        if ((std::holds_alternative<JumpStatementInfo>(in[fb.jump].m_block)) && std::get<JumpStatementInfo>(in[fb.jump].m_block).jumpType == "ifgoto")
        {
            identifierDict["ifgoto"] = make_tuple(jump_label, -1, -1, vector<int>());
            changed = true;
        }
        else
        {
            identifierDict["goto"] = make_tuple(jump_label, -1, -1, vector<int>());
            changed = true;
        }
    }
}

string Translator::get_small_reg()
{
    for (auto &iter : reg_usage)
    {
        if (iter.second == false)
            return iter.first;
    }

    throw compiler_error("no more register");
};

// namespace units

/*
    declaration: ex) int a; // no memory allocation
    definition:  ex) int a = 2; // memory allocation occured

    declaration:
        declration-specifier init-declarator-list(opt);

    ex)
        int num;    // {decl-spec: int, init-decl-list: "num"}
        const int hellowww: // {decl-spec: const int, init-decl-list: "hellowww = 10"}
        static unsigned int a, b; // {decl-spec: static unsigned int, init-decl-list: a, b}


    ex) int *ptr = 0, num;
        int    *ptr    =    0   ,    num     ;
         ^       ^     ^    ^   ^     ^      ^
         |       |     |    |   |     |      |
     decl-spec   ----init-decl-list---      semicolon
                           ^
                           |
                *ptr   =   0     ,   num
                 ^     ^   ^     ^    ^
                 |     |   |     |    |
                 -ini-..-list-  comma  declarator
                       ^
                       |
                *ptr      =     0
                  ^       ^     ^
                  |       |     |
            declarator  equal   initializer

*/

// namespace Specifier
