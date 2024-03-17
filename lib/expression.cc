#include "expression.h"

using namespace MMC::Expression;

ExpressionInfo ExpressionInfo::getExpressionInfo(Cstring &buffer)
{
    long unsigned int orig_idx = buffer.idx;
    ExpressionInfo exprInfo;

    vector<AssignmentExpression> assignExprList;

    while (buffer.is_empty() == false)
    {
        AssignmentExpression assignExprInfo;
        assignExprInfo = AssignmentExpression::getAssignmentExpression(buffer);

        if (assignExprInfo.empty()) // 할당식이 비었으면
            break;                  // 탈출

        assignExprList.push_back(assignExprInfo);

        buffer.trim();

        if (buffer.peekc() != ',')
            break;

        buffer.getc();
    }

    if (assignExprList.size() == 0)
    {
        buffer.idx = orig_idx;
        return ExpressionInfo(); // 빈 컨테이너 반환
    }

    exprInfo = ExpressionInfo(assignExprList);

    return exprInfo;
}

CastOperator CastOperator::getCastOperator(Cstring &buffer)
{
    long unsigned int orig_idx = buffer.idx;

    try
    {
        // 선언 지정자 획득
        DeclarationSpecifier declspec = Declare::DeclarationSpecifier::get_declarationSpecifier(buffer);

        if (declspec.empty())
            return CastOperator();
        // TODO 원래 throw 였는데 내가 바꿈
        // throw MMC::utils::compiler_error("cannot find declaration specifiers");
        // 캐스트 연산자에는 선언 지정자가 있을 수 없습니다
        else if (declspec.storageClassSpecifier.empty() == false)
            throw compiler_error("storage class specifier found int cast operator");

        Declarator abs_decl = Declarator::getAbstractDeclarator(buffer);
        // 추상 선언자를 획득한다
        if (!abs_decl.identifier.empty())
            throw compiler_error("identifier found in case operator");

        CastOperator castOperator = CastOperator(declspec, abs_decl);

        return castOperator;
    }
    catch (Exception &e)
    {
        e.what();
        if (dynamic_cast<compiler_error *>(&e))
        {
            buffer.idx = orig_idx;
            return CastOperator();
        }
        throw e;
    }
}

unary_exp_types GetOperator::getPrefixOperator(Cstring &buffer)
{
    long unsigned int orig_idx = buffer.idx;
    IsOperator is_op;
    string token = buffer.get_ctoken();

    if (is_op.is_prefixOperator(token))
    {
        return token;
    }
    else if (token == "(")
    { // 캐스트 연산 시작
        CastOperator cast_op = CastOperator::getCastOperator(buffer);
        if (cast_op.empty())
        {
            buffer.idx = orig_idx;
            return std::monostate();
        }

        if (buffer.get_ctoken() == ")")
            return cast_op;
    }

    buffer.idx = orig_idx;
    return string();
}

// PostfixExpression을 PrimaryExpression으로 부터 생성하고
// 실패하면 예외를 throw한다
/*
postfix-expression:
    primary-expression                                  (O)
    postfix-expression [ expression ]                   (X)
    postifx-expression ( argument-expression-listopt )  (O)
    postfix-expression . identifier                     (X)
    postfix-expression -> identifier                    (X)
    postfix-expression ++                               banned
    postifx-expression --                               banned
*/
PostfixExpression PostfixExpression::getPostfixExpression(Cstring &buffer)
{
    long unsigned int orig_idx = buffer.idx;
    vector<postfix_exp_types> exprTokenList;

    try
    {

        PrimaryExpression pri_expr = PrimaryExpression::getPrimaryExpression(buffer);
        if (pri_expr.empty())
            throw compiler_error("cannot find primary expression");

        exprTokenList.push_back(pri_expr);

        long unsigned int prev_idx = buffer.idx;
        // 함수 호출의 경우
        // ex) func(a,b);
        if (buffer.peekc() == '(')
        {
            exprTokenList.push_back("(");
            prev_idx = buffer.idx;
            while (!pri_expr.empty())
            {
                pri_expr = PrimaryExpression::getPrimaryExpression(buffer);
                exprTokenList.push_back(pri_expr);
                prev_idx = buffer.idx;
                if (buffer.get_ctoken() != ",")
                {
                    buffer.idx = prev_idx;
                    break;
                }
            }
            exprTokenList.push_back(")");
        }
        // 배열 호출의 경우
        else if (buffer.peekc() == '[')
        {
            buffer.getc();
            exprTokenList.push_back("[");

            while (!pri_expr.empty())
            {
                // pri_expr의 한계
                // a[2][i * 2] = 6같은 식을 쓸수 없다
                pri_expr = PrimaryExpression::getPrimaryExpression(buffer);
                exprTokenList.push_back(pri_expr);
                if (buffer.getc() != ']')
                    throw compiler_error("no right paren");
                prev_idx = buffer.idx;
                if (buffer.get_ctoken() != "[")
                {
                    buffer.idx = prev_idx;
                    break;
                }
                exprTokenList.push_back("[");
            }
            exprTokenList.push_back("]");
        }
        else
        {
            buffer.idx = prev_idx;
        }
        return PostfixExpression(exprTokenList);
    }
    catch (Exception &e)
    {
        e.what();
        buffer.idx = orig_idx;
        return PostfixExpression();
    }
}
PrimaryExpression PrimaryExpression::getPrimaryExpression(Cstring &buffer)
{
    long unsigned int orig_idx = buffer.idx;
    TokenEater et;
    try
    {
        PrimaryExpression pri_expr;
        string token = buffer.get_ctoken();
        char ch = token.front();

        // 여는 괄호면 직접 선언자 (매개변수 형식 리스트) 정의 이다
        if (token == "(")
        {
            ExpressionInfo exprInfo = ExpressionInfo::getExpressionInfo(buffer); // 수식 획득 후
            if (buffer.get_ctoken() != ")")                                      // 닫는 소괄호가 없으면
                throw compiler_error("cannot fine close bracket");               // 예외 처리

            pri_expr = PrimaryExpression(exprInfo);
        }
        // 배열의 경우 ex)  a[5] = 10;
        else if (token == "[")
        {
            ExpressionInfo exprInfo = ExpressionInfo::getExpressionInfo(buffer);
            if (buffer.get_ctoken() != "]")
                throw compiler_error("cannot find right pharenthesis");

            pri_expr = PrimaryExpression(exprInfo);
        }
        else if (et.is_first_namch(ch) || et.is_digit(ch) || ch == '\'' || ch == '\"')
        {
            pri_expr = PrimaryExpression(token);
        }
        else
        {
            throw compiler_error("invalid token found in primary expression");
        }
        return pri_expr;
    }
    catch (Exception &e)
    {
        if (dynamic_cast<compiler_error *>(&e))
        {
            buffer.idx = orig_idx;
            return PrimaryExpression();
        }
        throw e;
    }
}

ConditionalExpression ConditionalExpression::getConditionalExpression(Cstring &buffer)
{
    long unsigned int orig_idx = buffer.idx;
    ConditionalExpression condExpr;
    try
    {
        BinaryExpression binary_exp = BinaryExpression::getBinaryExpression(buffer);

        if (binary_exp.empty())
            throw compiler_error("cannnot find binary expression is conditional");

        buffer.trim();

        if (buffer.peekc() == '?')
        {
            buffer.getc();

            ExpressionInfo trueExpr = ExpressionInfo::getExpressionInfo(buffer);

            if (buffer.get_ctoken() != ":")
                throw compiler_error("cannnot find colon of conditional expression");

            ConditionalExpression falseExpr = ConditionalExpression::getConditionalExpression(buffer);

            if (trueExpr.empty() || falseExpr.empty())
                throw compiler_error("empty expression found in condiitonal expression");

            condExpr = ConditionalExpression(binary_exp, trueExpr, vector<false_exp_types>{falseExpr});
        }
        else
        {
            condExpr.condExpr = binary_exp;
        }

        return condExpr;
    }
    catch (Exception &e)
    {
        if (compiler_error *err = dynamic_cast<compiler_error *>(&e))
        { // check Exception is compiler_error
            err->what();
            buffer.idx = orig_idx;
            return ConditionalExpression();
        }
        e.what();
        throw e;
    }
}

UnaryExpression UnaryExpression::getUnaryExpression(Cstring &buffer)
{
    long unsigned int orig_idx = buffer.idx;
    UnaryExpression unary_exp;

    while (buffer.is_empty() == false)
    {
        long unsigned int prev_idx = buffer.idx;
        unary_exp_types op = GetOperator::getPrefixOperator(buffer);
        if (std::visit(EmptyCheck(), op))
        {
            buffer.idx = prev_idx;

            PostfixExpression post_exp = PostfixExpression::getPostfixExpression(buffer);
            if (post_exp.empty())
            {
                buffer.idx = orig_idx;
                return UnaryExpression();
            }

            unary_exp.exprTokenList.push_back(post_exp);
            break;
        }
        else if (std::holds_alternative<string>(op) && std::get<string>(op) == "sizeof")
        {
            CastOperator cast_op = CastOperator::getCastOperator(buffer);
            if (cast_op.empty() == false)
            { // 획득 성공시
                unary_exp.exprTokenList.push_back(op);
                unary_exp.exprTokenList.push_back(cast_op);
                break;
            }

            unary_exp.exprTokenList.push_back(op);
        }
        else
        {
            unary_exp.exprTokenList.push_back(op);
        }
    }

    return unary_exp;
}

BinaryExpression BinaryExpression::getBinaryExpression(Cstring &buffer)
{
    long unsigned int orig_idx = buffer.idx;
    BinaryExpression binary_exp;
    IsOperator is_op;
    try
    {

        while (buffer.is_empty() == false)
        {
            long unsigned int prev_idx = buffer.idx;

            UnaryExpression unary_exp = UnaryExpression::getUnaryExpression(buffer);

            if (unary_exp.empty())
            {
                buffer.idx = orig_idx;
                break;
            }

            binary_exp.binaryExpression.push_back(unary_exp);

            prev_idx = buffer.idx;
            string token = buffer.get_ctoken();

            if (is_op.is_binaryOperator(token) == false)
            {
                buffer.idx = prev_idx;
                break;
            }

            binary_exp.binaryExpression.push_back(token);
        }

        if (binary_exp.binaryExpression.empty())
            throw compiler_error("cannnot find binary expression");

        // 이항 연산자 계산
        // 스택 머신
        stack<string> op_stack;
        vector<binary_exp_types> post_exp;

        for (auto &iter : binary_exp.binaryExpression)
        {

            binary_exp_types token = iter;

            // 단항식이라면 후위 표기 토큰 리스트에
            if (std::holds_alternative<UnaryExpression>(token))
            {
                post_exp.push_back(token);
            }
            else
            { // 단항식이 아니라면 연산자이다

                while (op_stack.size() > 0)
                {
                    string prev_op = op_stack.top();

                    long unsigned int op_level = is_op.get_binary_level(std::get<string>(token));

                    if (op_level < is_op.get_binary_level(prev_op))
                        break;

                    string top = op_stack.top();
                    op_stack.pop();
                    post_exp.push_back(top);
                }

                op_stack.push(std::get<string>(token));
            }
        }

        while (!op_stack.empty())
        {
            string tmp = op_stack.top();
            op_stack.pop();
            post_exp.push_back(tmp);
        }

        binary_exp.binaryExpression = post_exp;

        return binary_exp;
    }
    catch (Exception &e)
    {
        if (compiler_error *err = dynamic_cast<compiler_error *>(&e))
        {
            err->what();
            buffer.idx = orig_idx;
            return BinaryExpression();
        }
        e.what();
        throw e;
    }
}

AssignmentExpression AssignmentExpression::getAssignmentExpression(Cstring &buffer)
{
    long unsigned int orig_idx = buffer.idx;
    AssignmentExpression assignExpr;
    IsOperator opEater;

    vector<assign_exp_types> exprTokenList;

    while (buffer.is_empty() == false)
    { //  단항식, 할당 연산자
        long unsigned int prev_idx = buffer.idx;
        UnaryExpression unaryExpr = UnaryExpression::getUnaryExpression(buffer);
        string assignOp = buffer.get_ctoken();
        if (!unaryExpr.empty() && opEater.is_assign_op(assignOp)) // 단항식과 할당 연산자 획득 성공 시
        {
            exprTokenList.push_back(unaryExpr);
            exprTokenList.push_back(assignOp);
        }
        else
        { // 실패한 경우 조건식을 획득하고 종료
            buffer.idx = prev_idx;
            ConditionalExpression condExpr = ConditionalExpression::getConditionalExpression(buffer);
            if (condExpr.empty())
            {
                buffer.idx = orig_idx;
                return AssignmentExpression();
            }
            exprTokenList.push_back(condExpr);
            break;
        }
    }

    assignExpr = AssignmentExpression(exprTokenList);
    return assignExpr;
}

void ExpressionInfo::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
    // codeseg += "; " + this->dump();

    for (auto &iter : assignmentExprList)
    {
        iter.compile(dataseg, codeseg, identifierDict);
    }
}

void AssignmentExpression::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
    codeseg.erase(codeseg.end() - 1);
    codeseg += "    # " + this->dump() + "\n";
    string left;
    string left_reg;
    string op;
    string right;
    string right_reg;
    string constant_right;
    TokenEater et;

    char c = 1;
    if (std::visit(getToken(), exprTokenList.back()).size() == 1)
        c = std::visit(getToken(), exprTokenList.back()).front()[0];
    // 최적화
    // 대입식에 상수
    if (et.is_digit(c) && this->exprTokenList.size() == 3)
    {
        constant_right = std::visit(getToken(), exprTokenList.back()).front();
    }
    else if (exprTokenList.size() == 1)
    {
        std::visit(Compile{dataseg, codeseg, identifierDict}, exprTokenList.back());
    }
    else
    {
        // 마지막 원소부터 컴파일 진행
        std::visit(Compile{dataseg, codeseg, identifierDict}, exprTokenList.back());
        if (!std::visit(EmptyCheck(), exprTokenList.back()))
        {
            right = std::visit(getToken(), exprTokenList.back()).front();
            right_reg = std::get<0>(identifierDict[right]);
        }
    }

    for (auto iter = exprTokenList.rbegin() + 1; iter != exprTokenList.rend();)
    {

        op = std::visit(getToken(), *iter).front();
        iter++;

        std::visit(Compile{dataseg, codeseg, identifierDict}, *iter);
        left = std::visit(getToken(), *iter).front();
        left_reg = std::get<0>(identifierDict[left]);
        iter++;
        if (op == "=")
        {
            if (right.empty() && constant_right.empty())
            {
                if (identifierDict.find("right_result") == identifierDict.end())
                    throw compiler_error("no right value");
                right_reg = std::get<string>(identifierDict["right_result"]);

                // = 가 연속 하지않으면 ex) a = b = 3;
                if (iter != exprTokenList.rend())
                {
                    op = std::visit(getToken(), *iter).front();
                    if (op != "=")
                        identifierDict.erase("right_result");
                }
                else if (iter == exprTokenList.rend())
                    identifierDict.erase("right_result");

                if (et.is_digit(right_reg[0]))
                    codeseg += "addi " + left_reg + ", $zero," + right_reg + "\n";
                else
                {
                    // 메모리 접근의 경우
                    if (right_reg[0] == '(')
                    {
                        codeseg += "lw " + left_reg + ", 0" + right_reg + "\nnop\n";
                        identifierDict.erase("right_result");
                    }
                    else
                        codeseg += "add " + left_reg + ", $zero," + right_reg + "\n";
                }

                identifierDict.erase("right_result");
            }
            else if (constant_right.empty())
            {
                // 메모리 접근의 경우
                if (left_reg[0] == '(')
                {
                    codeseg += "sw " + right_reg + ", 0" + left_reg + "\nnop\n";
                    identifierDict.erase("right_result");
                }
                else if (right_reg[0] == '(')
                {
                    codeseg += "lw " + left_reg + ", 0" + right_reg + "\nnop\n";
                    identifierDict.erase("right_result");
                }
                else
                    codeseg += "add " + left_reg + ", $zero, " + right_reg + "\n";
            }
            else
            {
                // 메모리 접근의 경우
                if (left_reg[0] == '(')
                {
                    codeseg += "li $s3," + constant_right + "\n";
                    codeseg += "sw $s3," + left_reg + "\n";
                    identifierDict.erase("right_result");
                }
                else
                    codeseg += "addi " + left_reg + ", $zero, " + constant_right + "\n";
            }
        }
    }
}

void ConditionalExpression::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
    // codeseg += "; " + this->dump();
    //  conditionalExpression은 ExpressionInfo
    condExpr.compile(dataseg, codeseg, identifierDict);

    // 삼항 연산자의 경우
    // TODO
    if (!trueExpr.empty())
    {
        throw compiler_error("not implemented yet!!!");
    }
}

void BinaryExpression::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
    codeseg.erase(codeseg.end() - 1);
    codeseg += "    # " + this->dump() + "\n";
    // 이항식의 1번 정의
    if (binaryExpression.size() == 1)
    {
        std::visit(Compile{dataseg, codeseg, identifierDict}, binaryExpression.front());
    }
    else
    { // 이항식 2번 정의

        string jump_label;
        bool is_if_epression = false;
        stack<binary_exp_types> operandStack;
        if (identifierDict.find("goto") != identifierDict.end())
            jump_label = std::get<0>((*identifierDict.find("goto")).second);
        else if (identifierDict.find("ifgoto") != identifierDict.end())
        {
            jump_label = std::get<0>((*identifierDict.find("ifgoto")).second);
            is_if_epression = true;
        }
        // 함수 인자가 있는 경우
        if (identifierDict.find("$a") != identifierDict.end())
        {
            // PrimaryExpression tmp(string("$a"));
            // PostfixExpression tmp_wrap; tmp_wrap.exprTokenList.push_back(tmp);
            // UnaryExpression   tmp_wrap_wrap; tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
            // binaryExpression.insert(binaryExpression.begin(), tmp_wrap_wrap);
            // binaryExpression.push_back("=");
        }
        while (binaryExpression.empty() == false)
        {
            auto iter = binaryExpression.front();
            string token = std::visit(getToken(), iter).front();

            // 단항식의 경우
            if (std::holds_alternative<UnaryExpression>(iter))
            {
                operandStack.push(iter);
            }
            else
            { // 그 외에 경우 연산자로 간주
                binary_exp_types right = operandStack.top();
                string right_token;
                if (!std::visit(EmptyCheck(), right))
                    right_token = std::visit(getToken(), right).front();
                bool right_is_digit = false;
                operandStack.pop();
                binary_exp_types left = operandStack.top();
                string left_token;
                if (!std::visit(EmptyCheck(), left))
                    left_token = std::visit(getToken(), left).front();
                bool left_is_digit = false;
                operandStack.pop();

                TokenEater et;
                bool swaped = false;

                if (et.is_digit(left_token[0]) || (left_token[0] == '-' && et.is_digit(left_token[1])))
                    left_is_digit = true;
                else
                    left_is_digit = false;
                if (et.is_digit(right_token[0]) || (right_token[0] == '-' && et.is_digit(right_token[1])))
                    right_is_digit = true;
                else
                    right_is_digit = false;

                // 우변 식을 획득했다면 컴파일
                if (!std::holds_alternative<std::monostate>(right))
                {
                    std::visit(Compile{dataseg, codeseg, identifierDict}, right);
                    // // 우변 식은 $s1
                    if (identifierDict.find("right_result") != identifierDict.end())
                    {
                        PrimaryExpression tmp(right_token);
                        PostfixExpression tmp_wrap;
                        tmp_wrap.exprTokenList.push_back(tmp);
                        UnaryExpression tmp_wrap_wrap;
                        tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                        right = tmp_wrap_wrap;
                        right_token = std::get<string>(identifierDict["right_result"]);
                        identifierDict.erase("right_result");
                        if (right_token[0] == '(')
                        {
                            right_token = "$s0";
                            PrimaryExpression tmp(left_token);
                            PostfixExpression tmp_wrap;
                            tmp_wrap.exprTokenList.push_back(tmp);
                            UnaryExpression tmp_wrap_wrap;
                            tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                            right = tmp_wrap_wrap;
                            right_is_digit = false;
                            codeseg += "lw $s0,   0($s4)\nnop\n"; // TODO not sure
                        }
                        else
                        {
                            if (et.is_digit(right_token[0]) || (right_token[0] == '-' && et.is_digit(right_token[1])))
                                right_is_digit = true;
                            else
                                right_is_digit = false;
                        }
                    }
                }

                // 좌변 식을 컴파일
                //여기서 부터
                // 일단 right_result에서 값을 얻지만 혹시 레지스터 값이 같을 경우
                // 다른 레지스터에 넣을수 있도록 바꾼다
                if (!std::holds_alternative<std::monostate>(left))
                {
                    std::visit(Compile{dataseg, codeseg, identifierDict}, left);
                    // // 좌변 식은 $s2
                    if (identifierDict.find("right_result") != identifierDict.end())
                    {
                        PrimaryExpression tmp(left_token);
                        PostfixExpression tmp_wrap;
                        tmp_wrap.exprTokenList.push_back(tmp);
                        UnaryExpression tmp_wrap_wrap;
                        tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                        left = tmp_wrap_wrap;
                        left_token = std::get<string>(identifierDict["right_result"]);
                        identifierDict.erase("right_result");
                        if (left_token[0] == '(')
                        {
                            left_token = "$s4";
                            PrimaryExpression tmp(left_token);
                            PostfixExpression tmp_wrap;
                            tmp_wrap.exprTokenList.push_back(tmp);
                            UnaryExpression tmp_wrap_wrap;
                            tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                            left = tmp_wrap_wrap;
                            left_is_digit = false;
                            codeseg += "lw $s4, 0($s0)\nnop\n";
                        }
                        else
                        {
                            if (et.is_digit(left_token[0]) || (left_token[0] == '-' && et.is_digit(left_token[1])))
                                left_is_digit = true;
                            else
                                left_is_digit = false;
                        }
                    }
                }

                // 우변, 좌변이 모두 상수일 경우
                // 컴파일 시간에 계산
                if (right_is_digit && left_is_digit)
                {
                    long result;
                    if (token == "+")
                    {
                        result = stol(left_token) + stol(right_token);
                    }
                    else if (token == "-")
                    {
                        result = stol(left_token) - stol(right_token);
                    }
                    else if (token == "*")
                    {
                        result = stol(left_token) * stol(right_token);
                    }
                    else if (token == "/")
                    {
                        result = stol(left_token) / stol(right_token);
                    }
                    binaryExpression.erase(binaryExpression.begin());
                    PrimaryExpression tmp(to_string(result));
                    PostfixExpression tmp_wrap;
                    tmp_wrap.exprTokenList.push_back(tmp);
                    UnaryExpression tmp_wrap_wrap;
                    tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                    operandStack.push(tmp_wrap_wrap);
                    continue;
                }

                if (et.is_digit(left_token[0]) || (left_token[0] == '-' && et.is_digit(left_token[1])))
                    left_is_digit = true;
                else
                    left_is_digit = false;
                if (et.is_digit(right_token[0]) || (right_token[0] == '-' && et.is_digit(right_token[1])))
                    right_is_digit = true;
                else
                    right_is_digit = false;

                if (left_is_digit)
                {
                    swap(right, left);
                    swap(right_token, left_token);
                    swap(right_is_digit, left_is_digit);
                    swaped = true;
                    if (identifierDict.find(left_token) != identifierDict.end())
                        left_token = std::get<string>(identifierDict[left_token]);
                }
                else if (right_is_digit)
                {
                    if (identifierDict.find(left_token) != identifierDict.end())
                        left_token = std::get<string>(identifierDict[left_token]);
                }
                else
                {

                    if (identifierDict.find(right_token) != identifierDict.end())
                        right_token = std::get<string>(identifierDict[right_token]);
                    if (identifierDict.find(left_token) != identifierDict.end())
                        left_token = std::get<string>(identifierDict[left_token]);
                }

                if (et.is_digit(left_token[0]) || (left_token[0] == '-' && et.is_digit(left_token[1])))
                    left_is_digit = true;
                else
                    left_is_digit = false;
                if (et.is_digit(right_token[0]) || (right_token[0] == '-' && et.is_digit(right_token[1])))
                    right_is_digit = true;
                else
                    right_is_digit = false;

                // 연산자에 따라 처리
                // "right_result"는 a = b + c 의 값의 b + c
                // a의 right가 b+c의 마지막 결과 즉 left이다
                if (is_if_epression)
                {
                    if (token == ">")
                    {
                        token = "<=";
                    }
                    else if (token == "<")
                    {
                        token = ">=";
                    }
                    else if (token == ">=")
                    {
                        token = "<";
                    }
                    else if (token == "<=")
                    {
                        token = ">";
                    }
                    else if (token == "==")
                    {
                        token = "!=";
                    }
                    else if (token == "!=")
                    {
                        token = "==";
                    }
                }

                if (token == "+")
                {
                    codeseg += (("add" + string(right_is_digit == true ? "i $s1, " : " $s1, ")) + left_token + ", " + right_token + "\n");
                    left_token = "$s1";
                    PrimaryExpression tmp(left_token);
                    PostfixExpression tmp_wrap;
                    tmp_wrap.exprTokenList.push_back(tmp);
                    UnaryExpression tmp_wrap_wrap;
                    tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                    left = tmp_wrap_wrap;
                    if (binaryExpression.size() == 1)
                    {
                        if (left_token[0] != '$')
                            identifierDict["right_result"] = right_is_digit == true ? make_tuple(left_token, -1, -1, vector<int>()) : identifierDict[std::visit(getToken(), left).front()];
                        else if (left_token[0] == '$')
                            identifierDict["right_result"] = make_tuple(left_token, -1, -1, vector<int>());
                    }
                }
                else if (token == "-")
                {
                    codeseg += (("sub" + string(right_is_digit == true ? "u $s1, " : " $s1, ")) + left_token + ", " + right_token + "\n");
                    left_token = "$s1";
                    PrimaryExpression tmp(left_token);
                    PostfixExpression tmp_wrap;
                    tmp_wrap.exprTokenList.push_back(tmp);
                    UnaryExpression tmp_wrap_wrap;
                    tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                    left = tmp_wrap_wrap;
                    if (binaryExpression.size() == 1)
                    {
                        if (left_token[0] != '$')
                            identifierDict["right_result"] = right_is_digit == true ? make_tuple(left_token, -1, -1, vector<int>()) : identifierDict[std::visit(getToken(), left).front()];
                        else if (left_token[0] == '$')
                            identifierDict["right_result"] = make_tuple(left_token, -1, -1, vector<int>());
                    }
                }
                else if (token == "*")
                {
                    if (right_is_digit)
                    {
                        codeseg += "ori $s3, $zero, " + right_token + "\nnop\n";
                        right_token = "$s3";
                        codeseg += "mult " + left_token + ", " + right_token + "\nnop\n";
                        if (identifierDict.find("($s2_used)") != identifierDict.end())
                        {
                            left_token = "$s0";
                            identifierDict.erase("($s2_used)");
                        }
                        else
                        {
                            left_token = "$s2";
                            identifierDict["($s2_used)"] = make_tuple("", -1, -1, vector<int>());
                        }
                        codeseg += "mflo " + left_token + "\n";
                        PrimaryExpression tmp(left_token);
                        PostfixExpression tmp_wrap;
                        tmp_wrap.exprTokenList.push_back(tmp);
                        UnaryExpression tmp_wrap_wrap;
                        tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                        left = tmp_wrap_wrap;
                        if (binaryExpression.size() == 1)
                        {
                            if (left_token[0] != '$')
                                identifierDict["right_result"] = right_is_digit == true ? make_tuple(left_token, -1, -1, vector<int>()) : identifierDict[std::visit(getToken(), left).front()];
                            else if (left_token[0] == '$')
                                identifierDict["right_result"] = make_tuple(left_token, -1, -1, vector<int>());
                        }
                    }
                    else
                    {
                        codeseg += "mult " + left_token + ", " + right_token + "\nnop\n";
                        if (identifierDict.find("($s2_used)") != identifierDict.end())
                        {
                            left_token = "$s0";
                            identifierDict.erase("($s2_used)");
                        }
                        else
                        {
                            left_token = "$s2";
                            identifierDict["($s2_used)"] = make_tuple("", -1, -1, vector<int>());
                        }
                        codeseg += "mflo " + left_token + "\nnop\n";
                        PrimaryExpression tmp(left_token);
                        PostfixExpression tmp_wrap;
                        tmp_wrap.exprTokenList.push_back(tmp);
                        UnaryExpression tmp_wrap_wrap;
                        tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                        left = tmp_wrap_wrap;
                        // 애라 코드 실험적 TODO
                        if (binaryExpression.size() == 1)
                        {
                            if (left_token[0] != '$')
                                identifierDict["right_result"] = right_is_digit == true ? make_tuple(left_token, -1, -1, vector<int>()) : identifierDict[std::visit(getToken(), left).front()];
                            else if (left_token[0] == '$')
                                identifierDict["right_result"] = make_tuple(left_token, -1, -1, vector<int>());
                        }
                    }
                }
                else if (token == "/")
                {
                    // 캐스팅 처리는 어떻게 하는지 아직 모름
                    if (right_is_digit)
                    {
                        codeseg += "ori $s3, $zero, " + right_token + "\nnop\n";
                        right_token = "$s3";
                        codeseg += "divu " + left_token + ", " + right_token + "\nnop\n";
                        if (identifierDict.find("$s1") != identifierDict.end())
                            left_token = "$s4";
                        else
                            left_token = "$s1";
                        codeseg += "mflo " + left_token + "\n";
                        PrimaryExpression tmp(left_token);
                        PostfixExpression tmp_wrap;
                        tmp_wrap.exprTokenList.push_back(tmp);
                        UnaryExpression tmp_wrap_wrap;
                        tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                        left = tmp_wrap_wrap;
                        if (binaryExpression.size() == 1)
                        {
                            if (left_token[0] != '$')
                                identifierDict["right_result"] = right_is_digit == true ? make_tuple(left_token, -1, -1, vector<int>()) : identifierDict[std::visit(getToken(), left).front()];
                            else if (left_token[0] == '$')
                                identifierDict["right_result"] = make_tuple(left_token, -1, -1, vector<int>());
                        }
                    }
                    else
                    {
                        codeseg += "div " + left_token + ", " + right_token + "\nnop\n";
                        if (identifierDict.find("$s1") != identifierDict.end())
                            left_token = "$s4";
                        else
                            left_token = "$s1";
                        codeseg += "mflo " + left_token + "\n";
                        PrimaryExpression tmp(left_token);
                        PostfixExpression tmp_wrap;
                        tmp_wrap.exprTokenList.push_back(tmp);
                        UnaryExpression tmp_wrap_wrap;
                        tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                        left = tmp_wrap_wrap;
                        if (binaryExpression.size() == 1)
                        {
                            if (left_token[0] != '$')
                                identifierDict["right_result"] = right_is_digit == true ? make_tuple(left_token, -1, -1, vector<int>()) : identifierDict[std::visit(getToken(), left).front()];
                            else if (left_token[0] == '$')
                                identifierDict["right_result"] = make_tuple(left_token, -1, -1, vector<int>());
                        }
                    }
                }
                else if (token == "%")
                {
                    if (right_is_digit)
                    {
                        codeseg += "ori $s3, $zero, " + right_token + "\n";
                        right_token = "$s3";
                        codeseg += "divu " + left_token + ", " + right_token + "\n";
                        codeseg += "mfhi " + left_token + "\n";
                        codeseg += "or $s1," + left_token + ", $zero\n";
                        left_token = "$s1";
                        PrimaryExpression tmp(left_token);
                        PostfixExpression tmp_wrap;
                        tmp_wrap.exprTokenList.push_back(tmp);
                        UnaryExpression tmp_wrap_wrap;
                        tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                        left = tmp_wrap_wrap;
                        if (binaryExpression.size() == 1)
                        {
                            if (left_token[0] != '$')
                                identifierDict["right_result"] = right_is_digit == true ? make_tuple(left_token, -1, -1, vector<int>()) : identifierDict[std::visit(getToken(), left).front()];
                            else if (left_token[0] == '$')
                                identifierDict["right_result"] = make_tuple(left_token, -1, -1, vector<int>());
                        }
                    }
                    else
                    {
                        codeseg += "div " + left_token + ", " + right_token + "\n";
                        codeseg += "mfhi " + left_token + "\n";
                        codeseg += "or $s1," + left_token + ", $zero\n";
                        left_token = "$s1";
                        PrimaryExpression tmp(left_token);
                        PostfixExpression tmp_wrap;
                        tmp_wrap.exprTokenList.push_back(tmp);
                        UnaryExpression tmp_wrap_wrap;
                        tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
                        left = tmp_wrap_wrap;
                        if (binaryExpression.size() == 1)
                        {
                            if (left_token[0] != '$')
                                identifierDict["right_result"] = right_is_digit == true ? make_tuple(left_token, -1, -1, vector<int>()) : identifierDict[std::visit(getToken(), left).front()];
                            else if (left_token[0] == '$')
                                identifierDict["right_result"] = make_tuple(left_token, -1, -1, vector<int>());
                        }
                    }
                }
                else if (token == ">=")
                {
                    if (right_is_digit)
                    {
                        codeseg += "ori $s3, $zero, " + right_token + "\n";
                        right_token = "$s3";
                    }
                    if (swaped)
                    {
                        swap(right, left);
                        swap(right_token, left_token);
                        swap(right_is_digit, left_is_digit);
                    }
                    codeseg += "slt $s0, " + left_token + ", " + right_token + "\nnop\n";
                    codeseg += "beq $s0, $zero, " + jump_label + "\n nop \n";
                }
                else if (token == "<=")
                {
                    if (right_is_digit)
                    {
                        codeseg += "ori $s3, $zero, " + right_token + "\n";
                        right_token = "$s3";
                    }
                    if (swaped)
                    {
                        swap(right, left);
                        swap(right_token, left_token);
                        swap(right_is_digit, left_is_digit);
                    }
                    codeseg += "slt $s0, " + right_token + ", " + left_token + "\nnop\n";
                    codeseg += "beq $s0, $zero, " + jump_label + "\n nop \n";
                }
                else if (token == "<")
                {
                    if (right_is_digit)
                    {
                        codeseg += "ori $s3, $zero, " + right_token + "\n";
                        right_token = "$s3";
                    }
                    if (swaped)
                    {
                        swap(right, left);
                        swap(right_token, left_token);
                        swap(right_is_digit, left_is_digit);
                    }
                    codeseg += "slt $s0, " + left_token + ", " + right_token + "\nnop\n";
                    codeseg += "bne $s0, $zero, " + jump_label + "\n nop \n";
                }
                else if (token == ">")
                {
                    if (right_is_digit)
                    {
                        codeseg += "ori $s3, $zero, " + right_token + "\n";
                        right_token = "$s3";
                    }
                    if (swaped)
                    {
                        swap(right, left);
                        swap(right_token, left_token);
                        swap(right_is_digit, left_is_digit);
                    }
                    codeseg += "slt $s0, " + right_token + ", " + left_token + "\nnop\n";
                    codeseg += "bne $s0, $zero, " + jump_label + "\n nop \n";
                }
                else if (token == "==")
                {
                    if (right_is_digit)
                    {
                        codeseg += "ori $s0, $zero, " + right_token + "\n";
                        codeseg += "bne $s0, " + left_token + ", " + jump_label + "\n nop \n";
                    }
                    else
                    {
                        codeseg += "beq " + right_token + ", " + left_token + ", " + jump_label + "\n nop \n";
                    }
                }
                else if (token == "!=")
                {
                    if (right_is_digit)
                    {
                        codeseg += "ori $s0, $zero, " + right_token + "\n";
                        codeseg += "bne $s0, " + left_token + ", " + jump_label + "\n nop \n";
                    }
                    else
                    {
                        codeseg += "bne " + right_token + ", " + left_token + ", " + jump_label + "\n nop \n";
                    }
                }
                else if (token == "=") // 함수 인자의 경우
                {
                    if (right_is_digit)
                        codeseg += "ori " + left_token + ",$zero, " + right_token + "\n";
                    else
                        codeseg += "or " + left_token + ", $zero, " + right_token + "\n";
                }
                else
                {
                    throw compiler_error("not implemented yet");
                }

                // 항의 갯수에 따른 처리
                // ex) sum = a + b 와 sum = a + b + c를 다르게처리
                // sum = a + b + c의 경우
                // tmp = b + c
                // sum = a + tmp 와 같이 처리
                if (binaryExpression.size() != 1)
                {
                    operandStack.push(left);
                }

                if (right_is_digit == true)
                {
                    // codeseg += "addi" + left_token + ", $zero, " + right_token + "\n";
                }
                // else
                // codeseg += "addi" + left_token + ", " + right_token + ", 0 \n";
                // 반복문 형식을 유지하되 계산한 피연산자를
                // 중복으로 계산하지 않도로 null 넣음
                // operandStack.push(std::monostate());
            }
            binaryExpression.erase(binaryExpression.begin());
        }
        if (operandStack.size() == 1)
        {
            string tmp = std::visit(getToken(), operandStack.top()).front();
            if (isdigit(tmp[0]) || (tmp[0] == '-' && isdigit(tmp[1])))
                identifierDict["right_result"] = make_tuple(std::visit(getToken(), operandStack.top()).front(), -1, -1, vector<int>());
        }
    }
}

void UnaryExpression::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
    for (auto &iter : exprTokenList)
    {
        if (std::holds_alternative<PostfixExpression>(iter))
        {
            std::visit(Compile{dataseg, codeseg, identifierDict}, iter);
            break;
        }
    }
}

void PostfixExpression::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
    codeseg.erase(codeseg.end() - 1);
    codeseg += "    # " + this->dump() + "\n";
    // size == 1 일 경우 그냥 진행
    // func() 의 경우
    // func 과 함께 (, ), 가 들어있음으로 size 가 3이 된다
    if (exprTokenList.size() == 1)
    {
        if (std::holds_alternative<PrimaryExpression>(exprTokenList.front()))
        {
            std::visit(Compile{dataseg, codeseg, identifierDict}, exprTokenList.front());
        }
        return;
    }

    int loop = 0;
    // else 일 경우 배열 call ex) a[10] = 1;
    if (std::visit(getToken(), exprTokenList[1]).front() == "[")
    {
        // array 형식
        // a[4][i][j] 의 경우
        // a [ 4 [ i [ j] 로 저장 된다
        // 괄호 사이의 식은 무조건 단항식이여야하고
        // 단항식만 계산 가능하다
        // CODE 더러움
        string ident = std::visit(getToken(), exprTokenList.front()).front();
        vector<int> size_list = std::get<vector<int>>(identifierDict["(" + ident + ")"]);
        BinaryExpression bi_exp;
        PrimaryExpression tmp;
        PostfixExpression tmp_wrap;
        UnaryExpression tmp_wrap_wrap;
        if (size_list.size() == 0)
        {
            size_list.push_back(1);
        }
        for (auto iter = exprTokenList.end() - 2; iter >= exprTokenList.begin() + 2; iter--)
        {
            string token = std::visit(getToken(), (*iter)).front();
            tmp = PrimaryExpression();
            tmp_wrap = PostfixExpression();
            tmp_wrap_wrap = UnaryExpression();

            tmp.obj = token;
            tmp_wrap.exprTokenList.push_back(tmp);
            tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
            bi_exp.binaryExpression.push_back(tmp_wrap_wrap);

            token = to_string(size_list[loop]);
            tmp = PrimaryExpression();
            tmp_wrap = PostfixExpression();
            tmp_wrap_wrap = UnaryExpression();
            tmp.obj = token;
            tmp_wrap.exprTokenList.push_back(tmp);
            tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
            bi_exp.binaryExpression.push_back(tmp_wrap_wrap);

            bi_exp.binaryExpression.push_back("*");

            iter--;
            loop += 1;
            if (loop % 2 == 0)
            {
                bi_exp.binaryExpression.push_back("+");
            }
        }
        tmp = PrimaryExpression();
        tmp_wrap = PostfixExpression();
        tmp_wrap_wrap = UnaryExpression();
        tmp.obj = "4";
        tmp_wrap.exprTokenList.push_back(tmp);
        tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);

        bi_exp.binaryExpression.push_back(tmp_wrap_wrap);
        bi_exp.binaryExpression.push_back("*");
        bi_exp.compile(dataseg, codeseg, identifierDict);
        string addr_reg;
        string result_reg;
        if (identifierDict.find("right_result") != identifierDict.end())
        {
            result_reg = std::get<string>(identifierDict["right_result"]);
            identifierDict.erase("right_result");
        }

        if (identifierDict.find(std::visit(getToken(), exprTokenList.front()).front()) != identifierDict.end())
        {
            addr_reg = std::visit(getToken(), exprTokenList.front()).front();
        }
        codeseg += "la $s3, " + addr_reg + "\nnop\n";
        if (isdigit(result_reg[0]))
        {
            codeseg += "addi $s4, $s3," + result_reg + "\n";
        }
        else
        {
            codeseg += "add $s4," + result_reg + ", $s3\n";
        }
        exprTokenList.front() = "right_result";
        if (identifierDict.find("right_result") != identifierDict.end())
        {
            if (std::get<string>(identifierDict["right_result"]) == "($s4)")
                identifierDict["right_result"] = make_tuple("($s2)", -1, -1, vector<int>());
            else
                identifierDict["right_result"] = make_tuple("($s4)", -1, -1, vector<int>());
        }
        else
        {
            identifierDict["right_result"] = make_tuple("($s4)", -1, -1, vector<int>());
        }

        return;
    }

    // else 일 경우 function call
    TokenEater et;
    string func_name = std::visit(getToken(), exprTokenList.front()).front();

    if (func_name == "__print_int")
    {
        codeseg += "li $v0, print_int\n";
        auto assign_args = std::get<ExpressionInfo>(std::get<PrimaryExpression>(exprTokenList[2]).obj).assignmentExprList;
        string arg_name = std::visit(getToken(), assign_args.front().exprTokenList.front()).front();
        string reg_name = std::get<string>(identifierDict[arg_name]);
        codeseg += "ori $a0, " + reg_name + ", 0\n";
        codeseg += "syscall\nnop\n";
        return;
    }
    if (func_name == "__print_clr")
    {
        codeseg += "li $v0, print_string\n";
        codeseg += "la $a0, CR_MESSAGE\n";
        codeseg += "syscall\nnop\n";
        return;
    }
    if (func_name == "__print_tab")
    {
        codeseg += "li $v0, print_string\n";
        codeseg += "la $a0, TAB_MESSAGE\n";
        codeseg += "syscall\nnop\n";
        return;
    }

    if (identifierDict.find(func_name) != identifierDict.end())
        identifierDict[func_name] = make_tuple("$v1", -1, -1, vector<int>());

    // exprTokenList.front() == func_name
    // exprTokenlist.begin() + 1 == '('
    // exprTokenList.back() == ')'
    // 이다
    loop = 0;

    // save regis
    codeseg += "addi    $sp, $sp, -44\n";
    codeseg += "sw $ra, 40($sp)\n";
    codeseg += "sw $fp, 36($sp)\n";
    for (auto iter : identifierDict)
    {
        string reg_name = std::get<string>(iter.second);
        if (!reg_name.compare(0, 2, "$v") || iter.first == "right_result" || iter.first == "ifgoto" || !iter.first.compare(0, 1, "("))
            continue;
        codeseg += "sw " + reg_name + ", " + to_string(loop * 4) + "($sp)\n";
        loop += 1;
    }
    loop = 0;

    // 함수 인자들
    if (exprTokenList.size() == 4)
    {
        auto assign_args = std::get<ExpressionInfo>(std::get<PrimaryExpression>(exprTokenList[2]).obj).assignmentExprList;
        for (auto iter = assign_args.begin(); iter != assign_args.end(); iter++)
        {
            string reg_name = "$a" + to_string(loop++);
            identifierDict["func_arg"] = make_tuple(reg_name, -1, -1, vector<int>());
            PrimaryExpression tmp;
            PostfixExpression tmp_wrap;
            UnaryExpression tmp_wrap_wrap;
            tmp.obj = "func_arg";
            tmp_wrap.exprTokenList.push_back(tmp);
            tmp_wrap_wrap.exprTokenList.push_back(tmp_wrap);
            auto &iter_binary_exps = std::get<ConditionalExpression>((*iter).exprTokenList[0]).condExpr.binaryExpression;
            iter_binary_exps.insert(iter_binary_exps.begin(), tmp_wrap_wrap);
            iter_binary_exps.push_back("=");
            (*iter).compile(dataseg, codeseg, identifierDict);
            identifierDict.erase("func_arg");
            // codeseg += "addi " + reg_name + ", $s0, 0\n";
        }
    }

    cout << codeseg << "\n";
    // 계산 후
    // func 을 call 한다
    codeseg += "jal " + func_name + "\nnop\n";
    loop = 0;
    for (auto iter : identifierDict)
    {
        string reg_name = std::get<string>(iter.second);
        if (!reg_name.compare(0, 2, "$v") || iter.first == "right_result" || iter.first == "ifgoto" || !iter.first.compare(0, 1, "("))
            continue;
        codeseg += "lw " + reg_name + ", " + to_string(loop * 4) + "($sp)\n";
        loop += 1;
    }
    codeseg += "lw $ra, 40($sp)\n";
    codeseg += "lw $fp, 36($sp)\nnop\n";
    codeseg += "addi $sp, $sp, 44\n";
    // codeseg += "nop \nor $s0, $v1, $zero\n";
}

void PrimaryExpression::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
    codeseg.erase(codeseg.end() - 1);
    codeseg += "    # " + this->dump() + "\n";

    // 기본 수식 4번 정의
    if (std::holds_alternative<ExpressionInfo>(obj))
    {
        std::visit(Compile{dataseg, codeseg, identifierDict}, obj);
        return;
    }

    // 첫 글자를 기준으로 ---> 식벌자, 상수, 문자열 판정
    // TODO 확실하지 않음
    string token = std::get<string>(obj);
    string use_reg;
    if (identifierDict.find(token) != identifierDict.end())
        use_reg = std::get<0>(identifierDict[token]);

    char ch = token.front();
    TokenEater et;

    if (et.is_first_namch(ch)) // 식별자라면 ----> 위에서 처리
    {
        // if (identifierDict.find("$a") != identifierDict.end())
        // {
        //     for(auto it: identifierDict) // 이미 매개 변수가 들어있을 경우
        //         if(it != *identifierDict.find("$a") && std::get<string>(it.second).find("$a") != string::npos)
        //             return;
        //     string arg_reg = std::get<string>(identifierDict["$a"]);
        //     codeseg += "or " + arg_reg + ", $zero," + use_reg +"\n";
        // }
    }
    else if (et.is_digit(ch) || ch == '-')
    { // 상수라면 기록 ---> 위에서 처리 하도록 최적화
      // if (identifierDict.find("$a") != identifierDict.end() )
      // {
      //     for(auto it: identifierDict) // 이미 매개 변수가 들어있을 경우
      //         if(it != *identifierDict.find("$a") && std::get<string>(it.second).find("$a") != string::npos)
      //             return;
      //     string arg_reg = std::get<string>(identifierDict["$a"]);
      //     codeseg += "ori " + arg_reg + ", $zero," + token +"\n";
      // }
    }
    else if (ch == '\'' || ch == '\"')
    { // TODO 아직 남음, 문자열
        throw compiler_error("not implmented yet");
    }
    else if (ch == '$')
    {
        cout << "func arg\n";
    }
    else
    { // 그 외의 경우는 오류
        throw compiler_error("invalid primary expression value");
    }
}

/*------------------ dump functions ---------------------*/

string AssignmentExpression::dump()
{
    string output;
    for (auto &iter : exprTokenList)
    {
        output += std::visit(Debuger(), iter);
    }
    return output;
}

string BinaryExpression::dump()
{
    string output;
    for (auto &iter : binaryExpression)
    {
        output += std::visit(Debuger(), iter);
    }
    return output;
}

string PrimaryExpression::dump()
{
    string output;
    output += std::visit(Debuger(), (obj));
    return output;
}

string PostfixExpression::dump()
{
    string output;
    for (auto &iter : exprTokenList)
    {
        output += std::visit(Debuger(), iter);
    }
    return output;
}

string ConditionalExpression::dump()
{
    string output;
    output += condExpr.dump();
    output += trueExpr.dump();
    for (auto &iter : falseExpr)
    {
        output += std::visit(Debuger(), iter);
    }
    return output;
}

string UnaryExpression::dump()
{
    string output;
    for (auto &iter : exprTokenList)
    {
        output += std::visit(Debuger(), iter);
    }
    return output;
}

/*-----------------------------------------------------*/

/*----------------------empty --------------------------*/

bool AssignmentExpression::empty()
{
    for (auto &iter : exprTokenList)
    {
        if ((!std::visit(EmptyCheck(), iter)))
            return false;
    }
    return true;
}

bool BinaryExpression::empty()
{
    for (auto &iter : binaryExpression)
    {
        if ((!std::visit(EmptyCheck(), iter)))
            return false;
    }
    return true;
}

bool PrimaryExpression::empty()
{
    return std::visit(EmptyCheck(), (obj));
}

bool PostfixExpression::empty()
{
    for (auto &iter : exprTokenList)
    {
        if ((!std::visit(EmptyCheck(), iter)))
            return false;
    }
    return true;
}

bool ConditionalExpression::empty()
{
    if (!(condExpr.empty() && trueExpr.empty()))
        return false;

    for (auto &iter : falseExpr)
    {
        if ((!std::visit(EmptyCheck(), iter)))
            return false;
    }
    return true;
}

bool UnaryExpression::empty()
{
    for (auto &iter : exprTokenList)
    {
        if ((!std::visit(EmptyCheck(), iter)))
            return false;
    }
    return true;
}

/* ------------------------ ETC --------------------------- */
vector<string> BinaryExpression::get_token()
{
    vector<string> rtn;
    for (auto &iter : binaryExpression)
    {
        vector<string> tmp = std::visit(getToken(), iter);
        rtn.insert(rtn.end(), tmp.begin(), tmp.end());
    }
    return rtn;
}

vector<string> AssignmentExpression::get_token()
{
    vector<string> rtn;
    for (auto &iter : exprTokenList)
    {
        vector<string> tmp = std::visit(getToken(), iter);
        rtn.insert(rtn.end(), tmp.begin(), tmp.end());
    }
    return rtn;
}

vector<string> UnaryExpression::get_token()
{
    vector<string> rtn;
    for (auto &iter : exprTokenList)
    {
        vector<string> tmp = std::visit(getToken(), iter);
        rtn.insert(rtn.end(), tmp.begin(), tmp.end());
    }
    return rtn;
}

vector<string> PrimaryExpression::get_token()
{
    vector<string> rtn;
    rtn = std::visit(getToken(), obj);
    return rtn;
}
vector<string> PostfixExpression::get_token()
{
    vector<string> rtn;
    for (auto &iter : exprTokenList)
    {
        vector<string> tmp = std::visit(getToken(), iter);
        rtn.insert(rtn.end(), tmp.begin(), tmp.end());
    }
    return rtn;
}

vector<string> ConditionalExpression::get_token()
{
    vector<string> tmp;
    tmp = condExpr.get_token();

    return tmp;
}

vector<string> ExpressionInfo::get_token()
{
    vector<string> rtn;

    vector<string> tmp;
    for (auto &iter : assignmentExprList)
    {
        tmp = iter.get_token();
        rtn.insert(rtn.end(), tmp.begin(), tmp.end());
    }

    return rtn;
}
