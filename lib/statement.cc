#include "statement.h"

using namespace MMC::Statement;

StatementInfo StatementInfo::getStatementInfo(Cstring &buffer)
{
    buffer.trim();
    long unsigned int prev_idx = buffer.idx;
    string token = buffer.get_ctoken(); // Look forward
    buffer.idx = prev_idx;

    StatementInfo stmt_info;

    if (token == "{")
    {
        CompoundStatementInfo compound_stmt = CompoundStatementInfo::getCompoundStatementInfo(buffer);
        stmt_info.obj = make_shared<stmt_info_types>(compound_stmt);
    }
    else if (token == "}")
    {
        stmt_info = StatementInfo();
    }
    else if (token == "if" || token == "switch")
    {
        SelectionStatementInfo select_stmt_info = SelectionStatementInfo::getSelectionStatementInfo(buffer);
        stmt_info.obj = make_shared<stmt_info_types>(select_stmt_info);
    }
    else if (token == "while" || token == "do" || token == "for")
    {
        IterationStatementInfo iteration_stmt_info = IterationStatementInfo::getIterationStatementInfo(buffer);
        stmt_info.obj = make_shared<stmt_info_types>(iteration_stmt_info);
    }
    else if (token == "goto" || token == "continue" || token == "break" || token == "return")
    {
        JumpStatementInfo jump_stmt_info = JumpStatementInfo::getStateJumpStatementInfomentInfo(buffer);
        stmt_info.obj = make_shared<stmt_info_types>(jump_stmt_info);
    }
    else if (token == "case" || token == "default")
    {
        LabeledStatementInfo label_stmt_info = LabeledStatementInfo::getLabeledStatementInfo(buffer);
        stmt_info.obj = make_shared<stmt_info_types>(label_stmt_info);
    }
    else
    {
        buffer.get_ctoken();
        if (buffer.get_ctoken() == ":")
        {
            buffer.idx = prev_idx;
            LabeledStatementInfo label_stmt_info = LabeledStatementInfo::getLabeledStatementInfo(buffer);
            stmt_info.obj = make_shared<stmt_info_types>(label_stmt_info);
        }
        else
        {
            buffer.idx = prev_idx;
            ExpressionStatementInfo expr_stmt_info = ExpressionStatementInfo::getExpressionStatementInfo(buffer);
            stmt_info.obj = make_shared<stmt_info_types>(expr_stmt_info);
        }
    }

    return stmt_info;
}

ExpressionStatementInfo ExpressionStatementInfo::getExpressionStatementInfo(Cstring &buffer)
{
    Expression::ExpressionInfo expr_info = Expression::ExpressionInfo::getExpressionInfo(buffer);

    buffer.trim();

    if (buffer.peekc() != ';')
        throw compiler_error("cannnot find ';'");

    buffer.getc();

    ExpressionStatementInfo expr_stmt_info = ExpressionStatementInfo(expr_info);
    return expr_stmt_info;
}

CompoundStatementInfo CompoundStatementInfo::getCompoundStatementInfo(Cstring &buffer)
{
    vector<Declare::DeclarationInfo> decl_list;
    vector<StatementInfo> stmt_list;

    buffer.trim();
    if (buffer.peekc() != '{')
        throw compiler_error("cannnot find start of compound statement");
    buffer.getc();

    while (buffer.is_empty() == false)
    {
        Declare::DeclarationInfo decl_info = Declare::DeclarationInfo::getDeclarationInfo(buffer);
        if (decl_info.empty())
            break;
        decl_list.push_back(decl_info);
    }

    while (buffer.is_empty() == false)
    {
        StatementInfo stmt_info = StatementInfo::getStatementInfo(buffer);
        if (stmt_info.empty())
            break;
        stmt_list.push_back(stmt_info);
    }

    buffer.trim();
    if (buffer.peekc() != '}')
        throw compiler_error("cannot find end of compound statement");
    buffer.getc();

    CompoundStatementInfo compound_stmt = CompoundStatementInfo(decl_list, stmt_list);
    return compound_stmt;
}

SelectionStatementInfo SelectionStatementInfo::getSelectionStatementInfo(Cstring &buffer)
{
    SelectionStatementInfo select_stmt_info;
    string token = buffer.get_ctoken();
    string selectionType;

    if (token == "if")
    {
        selectionType = token;

        if (buffer.get_ctoken() != "(")
            throw compiler_error("cannnot find start of conditional statement: (");

        Expression::ExpressionInfo condExpr = Expression::ExpressionInfo::getExpressionInfo(buffer);
        if (condExpr.empty())
            throw compiler_error("cannnot find expression");

        if (buffer.get_ctoken() != ")")
            throw compiler_error("cannnot find start of conditional statement: )");

        StatementInfo trueStmt = StatementInfo::getStatementInfo(buffer);
        if (trueStmt.empty())
            throw compiler_error("cannot find true statement");

        long unsigned int prev_idx = buffer.idx;
        string token = buffer.get_ctoken();

        StatementInfo falseStmt;
        if (token == "else")
        {
            falseStmt = StatementInfo::getStatementInfo(buffer);
            if (falseStmt.empty())
                throw compiler_error("cannot find false statement");
        }
        else
        {
            buffer.idx = prev_idx;
        }

        select_stmt_info = SelectionStatementInfo(selectionType, condExpr, trueStmt, falseStmt);
    }
    else if (token == "switch")
    {
        selectionType = token;

        if (buffer.get_ctoken() != "(")
            throw compiler_error("cannnot find start of conditional expression");

        Expression::ExpressionInfo condExpr = Expression::ExpressionInfo::getExpressionInfo(buffer);
        if (condExpr.empty())
            throw compiler_error("cannot find expression");

        if (buffer.get_ctoken() != ")")
            throw compiler_error("cannnot find end of conditional expression");

        StatementInfo trueStmt = StatementInfo::getStatementInfo(buffer);
        if (trueStmt.empty())
            throw compiler_error("cannot find true case statement");
        select_stmt_info = SelectionStatementInfo(selectionType, condExpr, trueStmt, StatementInfo());
    }
    else
    {
        throw compiler_error("invalid selction statement token");
    }

    return select_stmt_info;
}

IterationStatementInfo IterationStatementInfo::getIterationStatementInfo(Cstring &buffer)
{
    IterationStatementInfo iter_stmt;
    string token = buffer.get_ctoken();
    string iterationType;

    if (token == "while")
    {
        iterationType = token;

        if (buffer.get_ctoken() != "(")
            throw compiler_error("cannnot find start of conditional statement: (");

        Expression::ExpressionInfo condExpr = Expression::ExpressionInfo::getExpressionInfo(buffer);
        if (condExpr.empty())
            throw compiler_error("cannnot find expression");

        if (buffer.get_ctoken() != ")")
            throw compiler_error("cannnot find start of conditional statement: )");

        StatementInfo trueStmt = StatementInfo::getStatementInfo(buffer);
        if (trueStmt.empty())
            throw compiler_error("cannot find true statement");

        iter_stmt = IterationStatementInfo(iterationType, condExpr, trueStmt, Expression::ExpressionInfo(), Expression::ExpressionInfo());
    }
    else if (token == "do")
    {
        iterationType = token;

        StatementInfo trueStmt = StatementInfo::getStatementInfo(buffer);
        if (trueStmt.empty())
            throw compiler_error("cannnot find true case statement");

        if (buffer.get_ctoken() != "while")
            throw compiler_error("cannnot find keyword while");

        if (buffer.get_ctoken() != "(")
            throw compiler_error("cannnot find start of conditional statement: (");

        Expression::ExpressionInfo condExpr = Expression::ExpressionInfo::getExpressionInfo(buffer);
        if (condExpr.empty())
            throw compiler_error("cannot find expression");

        if (buffer.get_ctoken() != ")")
            throw compiler_error("cannnot find end of conditional expression");

        if (buffer.get_ctoken() != ";")
            throw compiler_error("cannnot find end of do-while expression");

        iter_stmt = IterationStatementInfo(iterationType, condExpr, trueStmt, Expression::ExpressionInfo(), Expression::ExpressionInfo());
    }
    else if (token == "for")
    {
        iterationType = token;

        if (buffer.get_ctoken() != "(")
            throw compiler_error("cannnot find start of conditional statement: (");

        Expression::ExpressionInfo init_exp = Expression::ExpressionInfo::getExpressionInfo(buffer);
        if (buffer.get_ctoken() != ";")
            throw compiler_error("cannnot find end of do-while expression");

        Expression::ExpressionInfo cond_exp = Expression::ExpressionInfo::getExpressionInfo(buffer);
        if (buffer.get_ctoken() != ";")
            throw compiler_error("cannnot find end of do-while expression");

        Expression::ExpressionInfo iter_exp = Expression::ExpressionInfo::getExpressionInfo(buffer);
        if (buffer.get_ctoken() != ")")
            throw compiler_error("cannnot find end of do-while expression");

        StatementInfo trueStmt = StatementInfo::getStatementInfo(buffer);
        if (trueStmt.empty())
            throw compiler_error("cannnot find true case statement");

        iter_stmt = IterationStatementInfo(iterationType, cond_exp, trueStmt, init_exp, iter_exp);
    }
    else
    {
        throw compiler_error("invalid iteration statement token");
    }
    return iter_stmt;
}

JumpStatementInfo JumpStatementInfo::getStateJumpStatementInfomentInfo(Cstring &buffer)
{
    JumpStatementInfo jump_stmt;
    string token = buffer.get_ctoken();

    if (token == "goto")
    {
        string jumpType = token;
        string identifier = buffer.get_ctoken();
        if (identifier.empty())
            throw compiler_error("cannnot find identifier after goto");

        if (buffer.get_ctoken() != ";")
            throw compiler_error("cannot find start of conditional expression");

        jump_stmt = JumpStatementInfo(jumpType, identifier);
    }
    else if (token == "continue" || token == "break")
    {
        string jumpType = token;

        if (buffer.get_ctoken() != ";")
            throw compiler_error("cannot find start of conditional expression");

        jump_stmt = JumpStatementInfo(jumpType, string());
    }
    else if (token == "return")
    {
        string jumpType = token;

        Expression::ExpressionInfo expression = Expression::ExpressionInfo::getExpressionInfo(buffer);

        if (buffer.get_ctoken() != ";")
            throw compiler_error("cannot find start of conditional expression");
        jump_stmt = JumpStatementInfo(jumpType, expression);
    }

    return jump_stmt;
}

LabeledStatementInfo LabeledStatementInfo::getLabeledStatementInfo(Cstring &buffer)
{
    LabeledStatementInfo label_stmt;
    string token = buffer.get_ctoken();

    if (token == "case")
    {
        string labelType = token;

        Expression::ExpressionInfo value = Expression::ExpressionInfo::getExpressionInfo(buffer);
        if (value.empty())
            throw compiler_error("cannot find expression after case");

        if (buffer.get_ctoken() != ":")
            throw compiler_error("cannot find start of conditional expression");

        StatementInfo statement = StatementInfo::getStatementInfo(buffer);

        label_stmt = LabeledStatementInfo(labelType, value, statement);
    }
    else if (token == "default")
    {
        string labelType = token;

        if (buffer.get_ctoken() != ":")
            throw compiler_error("cannot find start of conditional expression");

        StatementInfo statement = StatementInfo::getStatementInfo(buffer);

        label_stmt = LabeledStatementInfo(labelType, std::monostate(), statement);
    }
    else
    {
        string labelName = token;

        if (buffer.get_ctoken() != ":")
            throw compiler_error("cannot find start of conditional expression");

        StatementInfo statement = StatementInfo::getStatementInfo(buffer);

        label_stmt = LabeledStatementInfo(string(), labelName, statement);
    }
    return label_stmt;
}

void FlowBlock::search_use()
{
    string token;
    TokenEater et;
    if (std::holds_alternative<CompoundStatementInfo>(m_block))
    {
    }
    else if (std::holds_alternative<SelectionStatementInfo>(m_block))
    {
        auto block = std::get<SelectionStatementInfo>(m_block);
        for (auto &iter : block.condition.assignmentExprList)
        {
            for (auto &list : iter.exprTokenList)
            {
                vector<string> tmp;
                tmp = std::visit(MMC::Expression::getToken(), list);
                use.insert(use.end(), tmp.begin(), tmp.end());
            }
        }
    }
    else if (std::holds_alternative<IterationStatementInfo>(m_block))
    {
        auto self = std::get<IterationStatementInfo>(m_block);
    }
    else if (std::holds_alternative<JumpStatementInfo>(m_block))
    {
    }
    else if (std::holds_alternative<LabeledStatementInfo>(m_block))
    {
    }
    else if (std::holds_alternative<ExpressionStatementInfo>(m_block))
    {
        for (auto &iter : std::get<ExpressionStatementInfo>(m_block).expr_info.assignmentExprList)
        {
            for (auto &list : iter.exprTokenList)
            {
                vector<string> tmp;
                tmp = std::visit(MMC::Expression::getToken(), list);
                use.insert(use.end(), tmp.begin(), tmp.end());
            }
        }
    }
    else
    {
    }

    use = this->unique(use, use);

    auto new_end = std::remove_if(use.begin(), use.end(), [&](const string &str)
                                  { return !et.is_first_namch(str[0]); });

    use.erase(new_end, use.end());
}

void FlowBlock::add_inblock_tokens(FlowBlock flow_block)
{
    tokens = this->unique(use, flow_block.out);
}

void FlowBlock::cal_out(const vector<FlowBlock> &full_block)
{
    for (auto &iter : succ)
    {
        vector<string> tmp;
        tmp = this->unique(use, full_block[iter].out);
        out.insert(out.end(), tmp.begin(), tmp.end());
    }
}

/*------------------------- emtpy --------------------------*/

bool JumpStatementInfo::empty()
{
    if (jumpType.empty())
        return true;

    if (std::visit(EmptyCheck(), operand) == false)
        return false;
    return true;
}

bool StatementInfo::empty()
{
    if (obj == nullptr)
        return true;
    if (std::visit(EmptyCheck(), (*obj)) == false)
        return false;
    return true;
}

bool CompoundStatementInfo::empty()
{
    for (auto &iter : decl_list)
        if (!iter.empty())
            return false;
    for (auto &iter : stmt_list)
        if (!iter.empty())
            return false;

    return true;
}

bool SelectionStatementInfo::empty()
{
    if (!(selectionType.empty() && condition.empty() && stmt.empty() && else_stmt.empty()))
        return false;

    return true;
}

bool IterationStatementInfo::empty()
{
    if (!(iterationType.empty() && condition.empty() && initialier.empty() && iterator.empty() && statement.empty()))
        return false;
    return true;
}

bool LabeledStatementInfo::empty()
{
    if (!(labelType.empty() && statement.empty()))
        return false;
    return true;
}

/*------------------------dump -----------------*/

void JumpStatementInfo::dump()
{
    cout << jumpType;
    std::visit(Debuger(), operand);
}

void StatementInfo::dump()
{
    if (this->empty() == false)
        std::visit(Debuger(), (*obj));
}

void CompoundStatementInfo::dump()
{

    cout << "[ ";
    for (auto &iter : decl_list)
    {
        iter.dump();
    }
    cout << " | ";
    for (auto &iter : stmt_list)
    {
        iter.dump();
    }
    cout << " ]";
}

void SelectionStatementInfo::dump()
{

    cout << selectionType;
    condition.dump();
    stmt.dump();
    else_stmt.dump();
}

void IterationStatementInfo::dump()
{

    cout << iterationType;
    condition.dump();
    statement.dump();
    initialier.dump();
    iterator.dump();
}

void LabeledStatementInfo::dump()
{
    cout << labelType;
    std::visit(Debuger(), value);
    statement.dump();
}

/* --------------------------------------------- compile -------------------------------- */
void ExpressionStatementInfo::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
    expr_info.compile(dataseg, codeseg, identifierDict);
}

void JumpStatementInfo::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
}

void StatementInfo::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
}

void CompoundStatementInfo::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
}

void SelectionStatementInfo::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
}

void IterationStatementInfo::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
    if (iterationType == "do")
    {
    }
    else if (iterationType == "while")
    {
    }
    else if (iterationType == "for")
    {
    }
}

void LabeledStatementInfo::compile(string &dataseg, string &codeseg, map<string, tuple<string, int, int, vector<int>>> &identifierDict)
{
}

/* --------------------------------------------- regalloc -------------------------------- */
void ExpressionStatementInfo::regAlloc(vector<FlowBlock> &in)
{
    FlowBlock block(*this);
    block.block_idx = in.size();
    in.push_back(block);
}

void JumpStatementInfo::regAlloc(vector<FlowBlock> &in)
{
    string token;
    if (!std::visit(MMC::Expression::getToken(), this->operand).empty())
        token = std::visit(MMC::Expression::getToken(), this->operand).front();
    FlowBlock block;

    if (jumpType == "goto")
    {
        throw compiler_error("not implemented yet");
    }
    else if (jumpType == "continue" || jumpType == "break")
    {
        throw compiler_error("not implemented yet");
    }
    else if (jumpType == "return")
    {
        if (isdigit(std::visit(Expression::getToken(), this->operand).front()[0]))
        {
            operand = std::visit(Expression::getToken(), this->operand).front();
            block.m_block = *this;
            block.block_idx = in.size();
            in.push_back(block);
            return;
        }
        if (std::visit(Expression::getToken(), this->operand).empty() == false)
        {
            string reg_use = std::visit(Expression::getToken(), this->operand).front();
            block.use.push_back(reg_use);
        }
        block.m_block = *this;
        block.block_idx = in.size();
        in.push_back(block);
    }
}

void CompoundStatementInfo::regAlloc(vector<FlowBlock> &in)
{
    for (auto &iter : this->stmt_list)
    {
        if (iter.obj == nullptr)
            continue;
        std::visit(RegAlloc{in}, (*iter.obj));
    }
}

void SelectionStatementInfo::regAlloc(vector<FlowBlock> &in)
{
    FlowBlock condition_block(this->condition);
    FlowBlock jump_false_block;
    jump_false_block.label = "$__L" + to_string(rand());

    if (selectionType == "if")
    {

        int cond_block_idx;

        // insert conditioin block
        condition_block.block_idx = in.size();
        cond_block_idx = condition_block.block_idx;

        in.push_back(condition_block);

        if (!stmt.empty())
        {
            std::visit(RegAlloc{in}, (*this->stmt.obj));
        }

        jump_false_block.block_idx = in.size();
        jump_false_block.succ.push_back(cond_block_idx);

        in.push_back(jump_false_block);
        in[cond_block_idx].jump = jump_false_block.block_idx;

        JumpStatementInfo tmp;
        tmp.jumpType = "ifgoto";
        tmp.operand = jump_false_block.label;
        in[jump_false_block.block_idx].m_block = tmp;
        if (!else_stmt.empty())
        {
            std::visit(RegAlloc{in}, (*this->else_stmt.obj));
        }
    }
    else if (selectionType == "switch")
    {
        throw compiler_error("not implemented yet");
    }
}

void IterationStatementInfo::regAlloc(vector<FlowBlock> &in)
{

    FlowBlock jump_block;
    jump_block.label = "$__L" + to_string(rand());
    jump_block.block_idx = in.size();
    int orig_back_idx = in.size();
    in.push_back(jump_block);

    if (this->statement.empty() == false)
        std::visit(RegAlloc{in}, (*this->statement.obj));

    FlowBlock iter_block(this->iterator);
    FlowBlock cond_block(this->condition);
    FlowBlock init_block(this->initialier);

    if (iterationType == "do")
    {
        cond_block.block_idx = in.size();
        int prev_back_idx = in.size();
        in.emplace_back(cond_block);

        in[prev_back_idx].jump = orig_back_idx;
        in[orig_back_idx].succ.emplace_back(prev_back_idx);
    }
    else if (iterationType == "while")
    {
    }
    else if (iterationType == "for")
    {
    }
}

void LabeledStatementInfo::regAlloc(vector<FlowBlock> &in)
{
}