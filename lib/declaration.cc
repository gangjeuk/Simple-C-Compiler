#include "declaration.h"

using namespace std;
using namespace MMC::TokenType;
using namespace MMC::Declare;

Declarator Declarator::getDeclarator(Cstring& buffer)
{
    long unsigned int orgi_idx = buffer.idx;

    Declarator decl;

    vector<token_types> token_list;

    dcl(buffer, token_list);


    if(token_list.empty()){
        buffer.idx = orgi_idx;
        return Declarator();
    }

    

    string identifier = std::get<PointerStack>(token_list[0]).token;
    vector<token_types> descriptor;
    for(auto& iter: token_list)
        descriptor.push_back(iter);
    descriptor.erase(descriptor.begin());

    decl.identifier = identifier;
    decl.descriptor = descriptor;

    return decl;

}

// 선언자를 분석 토큰으로 나눔
// input: buffer, output: token_list
// unique_ptr을 return 하나 AbstractDeclarator와 다르게 
// 무조건 string을 담은 uniqut_ptr<PointerStack>을 return함
void Declarator::dcl(Cstring& buffer, vector<token_types>& out)
{

    vector<PointerStack> ptr_stack;

    while (buffer.is_empty() == false){
        long unsigned int prev_idx = buffer.idx;

        string token = buffer.get_ctoken();

        if (token_type::is_pointerToken(token) == false){ // 포인터 토큰이 아니면 탈출
            buffer.idx = prev_idx;
            break;
        }

        ptr_stack.push_back({token});

        buffer.trim();
    }

    // 포인터 획득 끝나면 
    // 직접 선언자 분석

    DirectDeclarator::dirdcl(buffer, out);

    while(ptr_stack.size() > 0){
        PointerStack tmp = ptr_stack.back();
        ptr_stack.pop_back();
        out.emplace_back(tmp);
    }

}

// 추산 선언자 분석 결과 리턴
// 실패스 예외 throw
void Declarator::absdcl(Cstring& buffer, vector<token_types>& out)
{
    vector<string> pointerStack;

    while (buffer.is_empty() == false) {
        long unsigned int prev_idx = buffer.idx;

        string token = buffer.get_ctoken();
        if (token_type::is_pointerToken(token) == false) {
            buffer.idx = prev_idx;
            break;
        }

        pointerStack.push_back(token);

        buffer.trim();
    }

    DirectDeclarator::absdirdcl(buffer, out);

    while(pointerStack.size() > 0){
        PointerStack tmp(pointerStack.back());
        pointerStack.pop_back();
        out.push_back(tmp);
    }

    return;
}

Declarator Declarator::getAbstractDeclarator(Cstring& buffer)
{



    Declarator decl;

    vector<token_types> token_list;


    //가장 첫 원소를 식별자로 간주함으로, 첫 원소를 null로 하고 함수를 부른다
    token_types mono;
    token_list.push_back(mono);

    absdcl(buffer, token_list);

    // 추상 선언자는 식별자가 없음으로 
    // 처음 요소는 없어야 한다

    vector<token_types> descriptor;
    for(auto& iter: token_list)
        descriptor.emplace_back(iter);
    descriptor.erase(descriptor.begin());

    decl.descriptor = descriptor;

    return decl;

}

string Declarator::dump(){
    string iden = identifier.empty() ? " " : identifier;

    for(auto& iter: this->descriptor){
        iden += std::visit(Debuger(), iter);
    }
    return iden;
}


InitDeclaratorList InitDeclaratorList::getInitDeclaratorList(Cstring& buffer)
{
    InitDeclaratorList init_decl_list;

    while(buffer.is_empty() == false){
        InitDeclarator init_decl = InitDeclarator::getInitDeclarator(buffer);

        init_decl_list.init_decl.push_back(init_decl);

        buffer.trim();

        if (buffer.peekc() != ',')
            break;

        buffer.getc();
    }

    return init_decl_list;
}

InitDeclarator InitDeclarator::getInitDeclarator(Cstring& buffer)
{
    InitDeclarator init_decl;
    Declarator declarator;
    string initializer;

    declarator = Declarator::getDeclarator(buffer);

    buffer.trim();

    if (buffer.peekc() == '='){
        buffer.getc();

        // TODO 저자가 작성안해서 int a = 10; 사용 불가
        // 내가 일단 임이로 넣는다
        while(buffer.is_empty() == false && buffer.peekc() != ';')
        {
            initializer += buffer.get_ctoken();
        }
        // ; 등장 까지 모든걸 다 넣는다 
        // 넣은 후에 compiler타임에 분석
        if(buffer.peekc() != ';') // 분석 끝났을때 ; 없으면 오류다
        {
            cout << " no ; found: " << buffer.idx;
            exit(EXIT_FAILURE);
        }

        init_decl = InitDeclarator(declarator, initializer);
    }
    else{

        init_decl = InitDeclarator(declarator);
    }

    return  init_decl;
}
/*
 직접 선언자 분석
 괄호 (선언자) 분석 실패시 예외 throw
 
 ex) int func (int a, int b)
               ^           ^
             from          end
               -------------
               
*/
void DirectDeclarator::dirdcl(Cstring& buffer, vector<token_types>& out)
{
    TokenEater tokenEater;
    buffer.trim();

    if (buffer.peekc() == '(') {
        // ( declarator )
        buffer.getc();
        
        Declarator::dcl(buffer, out);

        if (buffer.peekc() == ')') {
            // 선언자 분석 끝났는데 ) 있으면 책임을 넘긴다
            return;
        }

        buffer.getc();
    }
    else if (tokenEater.is_first_namch(buffer.peekc())) {
        string identifier = buffer.get_ctoken();
        PointerStack tmp(identifier);
        out.push_back(tmp);
    }
    buffer.trim();

    // 배열과 함수에 대해 처리합니다.
    while (buffer.peekc() == '(' || buffer.peekc() == '[') {
        if (buffer.peekc() == '(') { // 함수 기호의 시작이라면
            buffer.getc(); // 함수의 시작으로서 분석한, 여는 소괄호는 생략합니다.

            ParameterTypeList param_type_list = ParameterTypeList::getParameterTypeList(buffer);

            out.emplace_back(param_type_list);

            buffer.trim(); // 다음 토큰의 시작 지점까지 버퍼 포인터를 옮깁니다.
            if (buffer.peekc() != ')') // 다음 토큰이 닫는 소괄호가 아니라면
                break; // 토큰의 해석 책임을 dirdcl을 호출한 함수로 넘깁니다.

            buffer.getc(); // 해석한 닫는 소괄호를 지나갑니다.
        }
        else if(buffer.peekc() == '['){ 
            buffer.getc();
            buffer.trim();

            string size = buffer.get_ctoken();
            out.push_back(size);
            buffer.trim();
            if (buffer.peekc() != ']')
                break;

            buffer.getc();            
        }
        else{
            throw compiler_error("wrong ch");
        }
    }
    return;
}


// 직점 선언자
// 괄호 처리 실패시 throw
// cast 처리를 위한 식별자인 경우 throw
void DirectDeclarator::absdirdcl(Cstring& buffer, vector<token_types>& out){
    TokenEater tokenEater;

    buffer.trim();

    if (buffer.peekc() == '(') { // 괄호 발견시
        buffer.getc();

        Declarator::absdcl(buffer, out);

        if (buffer.peekc() != ')') {
            return;
        }

        buffer.getc();
    }else if(tokenEater.is_first_namch(buffer.peekc())) { // 식별자 시작이면
        buffer.get_ctoken();   // 식별자 무시
        // 혹시 식별자가 발견된 경우 표시
        // 표시 코드 작성 어려움 TODO
        // 이단 is_identifier 넣고 getCastOperator 함수에서 이거 보고 판단
        token_types mono = Declarator();
        std::get<Declarator>(mono).identifier = "is_identifier";
        out[0] = mono;
        buffer.trim();
    }

    // 배열과 함수 처리
    while (buffer.peekc() == '(' || buffer.peekc() == '[') {
        if (buffer.peekc() == '(') {
            buffer.getc();

            ParameterTypeList param_type_list = ParameterTypeList::getParameterTypeList(buffer);

            out.emplace_back(param_type_list);

            buffer.trim();
            if (buffer.peekc() != ')')
                break;

            buffer.getc();
        }
        else if(buffer.peekc() == '['){
            buffer.getc();
            buffer.trim();

            string size = buffer.get_ctoken();
            out.push_back(size);
            buffer.trim();
            if (buffer.peekc() != ']')
                break;

            buffer.getc();       
        }
        else{
            throw compiler_error("wrong ch");
        }
    }

    return;
}

// 매개 변수 획득
// 실패시 예외 throw
// 함수는 getParameterTypeList()에서 불려지고 예외처리도 여기서 일어남
ParameterDeclaraton ParameterDeclaraton::getParameterDeclaration(Cstring& buffer)
{
    DeclarationSpecifier decl_spec;
    Declarator declarator;
    TokenEater et;

    decl_spec = DeclarationSpecifier::get_declarationSpecifier(buffer);
    
    buffer.trim();
    long unsigned int prev_idx = buffer.idx;
    char token = buffer.getc();
    // 수정 되었다
    // int func(int a, int b) 를 func: int, int -> int 로 받던 것을
    //                           func: int a, int b -> int 와 같이 식별자도 얻을 수 있게
    //                             token == '*' 은 포인터 인수 처리르 위해
    if(et.is_first_namch(token) || token == '*')
    {
        buffer.idx = prev_idx;
        declarator = Declarator::getDeclarator(buffer);
    }else{
        buffer.idx = prev_idx;
        declarator = Declarator::getAbstractDeclarator(buffer);
    }

    // TODO 여기 서 이상한것 같아서 내가 수정
    if(decl_spec.empty())
        return ParameterDeclaraton();

    ParameterDeclaraton param_decl = ParameterDeclaraton(decl_spec, declarator);
    param_decl.declspec = decl_spec;
    param_decl.declarator = declarator;
    return param_decl;
}

DeclarationInfo DeclarationInfo::getDeclarationInfo(Cstring& buffer)
{
    long unsigned int orig_idx = buffer.idx;
    DeclarationSpecifier decl_spec;

    decl_spec= DeclarationSpecifier::get_declarationSpecifier(buffer);
    

    if(decl_spec.empty()){
           buffer.idx = orig_idx;
           return DeclarationInfo();
       }
    
    InitDeclaratorList init_decl_list = InitDeclaratorList::getInitDeclaratorList(buffer);

    buffer.trim();

    // 다음 토큰이 세미콜론이 아니면 선언 획득 실패로 간주
    if (buffer.peekc() != ';') {
        buffer.idx = orig_idx;
        return DeclarationInfo();
    }

    buffer.getc();

    DeclarationInfo decl_info = DeclarationInfo(decl_spec, init_decl_list);

    return decl_info;
    
}


ParameterTypeList ParameterTypeList::getParameterTypeList(Cstring& buffer)
{
    ParameterTypeList param_type_list;


    while (buffer.is_empty() == false){
        long unsigned int prev_idx = buffer.idx;
        ParameterDeclaraton param_decl = ParameterDeclaraton::getParameterDeclaration(buffer);

        if (param_decl.empty()) {
            buffer.idx = prev_idx;
            string token = buffer.get_ctoken();
            if (token != "...")
                buffer.idx = prev_idx;
        
            break;
        }

        param_type_list.param_decl.push_back(param_decl);

        buffer.trim();

        if (buffer.peekc() != ',')
            break;

        buffer.getc();
    }


    return param_type_list;
}
// 선언 지정자 탐색 
// 없을시 예외 throw 발생
DeclarationSpecifier DeclarationSpecifier::get_declarationSpecifier(Cstring& buffer){
    long unsigned int orig_idx = buffer.idx;

    try{

        long unsigned int prev_idx = buffer.idx;

        string sc_spec;
        vector<string> type_spec;
        vector<string> type_qlif;

        while(buffer.is_empty() == false) // until buffer is empty
        {
            prev_idx = buffer.idx;
            string token = buffer.get_ctoken();
            if (token_type::is_storageClass(token)){
                if(!sc_spec.empty()){       // 기억 형태자는 2개 이상 지정 불가
                    throw declarationSpecifier_error("Storage Class Specifier cannot be defined twice");
                }
                sc_spec = token;
            }
            else if (token_type::is_typeQualifier(token)){
                type_qlif.push_back(token);
            }
            else if (token_type::is_typeSpecifier(token)){
                type_spec.push_back(token);
            }
            else {  // 모두 아닌 경우 선언 지정자가 아니므로 획든하면 안 된다.
                buffer.idx = prev_idx;
                break;
            }
        }

        if( sc_spec.empty() && type_spec.empty() && type_qlif.empty() )
            throw declarationSpecifier_error("undeclared identifier");
        
        DeclarationSpecifier de_sp(sc_spec, type_spec, type_qlif);

        return de_sp;
    } catch (Exception& dc_sp_error) {
        buffer.idx = orig_idx;
        dc_sp_error.what();
        return DeclarationSpecifier();
    }
}

DeclarationSpecifier DeclarationSpecifier::get_declarationSpecifier(const string buffer)
{
    cout << "alert this function should not be used except debugging \n";
    Cstring new_buf(buffer);
    return get_declarationSpecifier(new_buf);
}



// namespace Declarator