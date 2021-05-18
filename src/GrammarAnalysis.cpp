/**
 * MIDL的语法分析程序，递归下降
 * using c++17
 * compile with g++ 9.3.0
 */

#include "GrammarAnalysis.hpp"
#include "json.hpp"

using namespace std;
using nlohmann::json;


//递归下降的入口
void GrammarAnalysis::parse() {
    try {
        getToken();
        root = struct_type();
        if (curToken.type != TokenType::EEOF) throw runtime_error("not end of file");
    } catch (std::exception &ep) {
        fout << ep.what() << endl;
        throw runtime_error("syntax error");
    }
}

//打印树结构
void GrammarAnalysis::print() {
    json js;
    unordered_map<TreeNodeType, string> map = {
            {TreeNodeType::DEFAULT,    "DEFAULT"},
            {TreeNodeType::STRUCT,     "STRUCT"},
            {TreeNodeType::ID,         "ID"},
            {TreeNodeType::MEMBER,     "MEMBER"},
            {TreeNodeType::DECLARATOR, "DECLARATOR"},
            {TreeNodeType::TYPE_SPEC,  "TYPE_SPEC"},
            {TreeNodeType::OR_EXPR,    "OR_EXPR"},
            {TreeNodeType::XOR_EXPR,   "XOR_EXPR"},
            {TreeNodeType::AND_EXPR,   "AND_EXPR"},
            {TreeNodeType::SHIFT_EXPR, "SHIFT_EXPR"},
            {TreeNodeType::ADD_EXPR,   "ADD_EXPR"},
            {TreeNodeType::MULT_EXPR,  "MULT_EXPR"},
            {TreeNodeType::UNARY_EXPR, "UNARY_EXPR"}
    };
    inorder(root, js, map);
    fout << js.dump(4) << endl;
}

//中序遍历打印树
void GrammarAnalysis::inorder(TreeNode *node, json &js, const unordered_map<TreeNodeType, string> &map) {
    if (node) {
        js["value"] = map.at(node->type) + ": " + node->value.value;
        if (node->left) inorder(node->left, js["left"], map);
        if (node->right) inorder(node->right, js["right"], map);
        if (node->next) inorder(node->next, js["next"], map);
    }
}

//产生式struct_type
TreeNode *GrammarAnalysis::struct_type() {
    auto node = new TreeNode(TreeNodeType::STRUCT);

    match(TokenType::STRUCT);
    if (curToken.type == TokenType::ID) node->left = new TreeNode(TreeNodeType::ID, curToken);
    match(TokenType::ID);

    match(TokenType::LBRACES);
    node->right = member_list();
    match(TokenType::RBRACES);
    return node;
}

//产生式member_list
TreeNode *GrammarAnalysis::member_list() {
    if (!member_list_help()) return nullptr;
    auto node = new TreeNode(TreeNodeType::MEMBER);
    node->left = type_spec();
    node->right = declarators();
    match(TokenType::SEMICOLON);
    node->next = member_list();
    return node;
}

//辅助判断memer_list的第一个终结符
bool GrammarAnalysis::member_list_help() const {
    switch (curToken.type) {
        case TokenType::CHAR:
        case TokenType::BOOLEAN:
        case TokenType::FLOAT:
        case TokenType::DOUBLE:
        case TokenType::LONG:
        case TokenType::SHORT:
        case TokenType::INT16:
        case TokenType::INT32:
        case TokenType::INT64:
        case TokenType::INT8:
        case TokenType::UNSIGNED:
        case TokenType::UINT16:
        case TokenType::UINT32:
        case TokenType::UINT64:
        case TokenType::UINT8:
        case TokenType::STRUCT:
            return true;
        default:
            return false;
    }
}

//产生式type_spec
TreeNode *GrammarAnalysis::type_spec() {
    TreeNode *node = nullptr;
    switch (curToken.type) {
        case TokenType::CHAR:
        case TokenType::BOOLEAN:
        case TokenType::FLOAT:
        case TokenType::DOUBLE:
        case TokenType::SHORT:
        case TokenType::INT16:
        case TokenType::INT32:
        case TokenType::INT64:
        case TokenType::INT8:
        case TokenType::UINT16:
        case TokenType::UINT32:
        case TokenType::UINT64:
        case TokenType::UINT8:
            node = new TreeNode(curToken);
            getToken();
            break;
        case TokenType::LONG:
            node = new TreeNode(curToken);
            getToken();
            if (curToken.type == TokenType::DOUBLE || curToken.type == TokenType::LONG) {
                node->next = new TreeNode(curToken);
                getToken();
            }
            break;
        case TokenType::UNSIGNED:
            node = new TreeNode(curToken);
            getToken();
            if (curToken.type == TokenType::SHORT) node->next = new TreeNode(curToken), getToken();
            else if (curToken.type == TokenType::LONG) {
                node->next = new TreeNode(curToken);
                getToken();
                if (curToken.type == TokenType::LONG) node->next->next = new TreeNode(curToken), getToken();
            } else {
                throw runtime_error("line: " + to_string(curToken.lineNo) + " incomplete unsigned");
            }
            break;
        case TokenType::STRUCT:
            node = struct_type();
            break;
        default:
            break;
    }
    if (node) node->type = TreeNodeType::TYPE_SPEC;
    return node;
}

//产生式declarators
TreeNode *GrammarAnalysis::declarators() {
    auto node = declarator();
    auto tmp = node;
    while (curToken.type == TokenType::COMMA) {
        getToken();
        tmp->next = declarator();
        tmp = tmp->next;
    }
    return node;
}

//产生式declarator
TreeNode *GrammarAnalysis::declarator() {
    auto tmpToken = curToken;
    match(TokenType::ID);
    auto node = new TreeNode(TreeNodeType::DECLARATOR);
    node->left = new TreeNode(TreeNodeType::ID, tmpToken);
    if (curToken.type == TokenType::LBRACKETS) node->right = exp_list();
    return node;
}

//产生式exp_list
TreeNode *GrammarAnalysis::exp_list() {
    match(TokenType::LBRACKETS);
    auto node = or_expr();
    auto tmp = node;
    while (curToken.type == TokenType::COMMA) {
        getToken();
        tmp->next = or_expr();
        tmp = tmp->next;
    }
    match(TokenType::RBRACKETS);
    return node;
}

//产生式or_expr
TreeNode *GrammarAnalysis::or_expr() {
    auto node = xor_expr();
    while (curToken.type == TokenType::VERTICAL) {
        auto cur = new TreeNode(TreeNodeType::OR_EXPR, curToken);
        getToken();
        cur->left = node;
        cur->right = xor_expr();
        node = cur;
    }
    return node;
}

//产生式xor_expr
TreeNode *GrammarAnalysis::xor_expr() {
    auto node = and_expr();
    while (curToken.type == TokenType::CARET) {
        auto cur = new TreeNode(TreeNodeType::XOR_EXPR, curToken);
        getToken();
        cur->left = node;
        cur->right = and_expr();
        node = cur;
    }
    return node;
}

//产生式and_expr
TreeNode *GrammarAnalysis::and_expr() {
    auto node = shift_expr();
    while (curToken.type == TokenType::AMPERSAND) {
        auto cur = new TreeNode(TreeNodeType::AND_EXPR, curToken);
        getToken();
        cur->left = node;
        cur->right = shift_expr();
        node = cur;
    }
    return node;
}

//产生式shift_expr
TreeNode *GrammarAnalysis::shift_expr() {
    auto node = add_expr();
    while (curToken.type == TokenType::GREATER || curToken.type == TokenType::LESS) {
        auto cur = new TreeNode(TreeNodeType::SHIFT_EXPR, curToken);
        getToken();
        cur->left = node;
        cur->right = add_expr();
        node = cur;
    }
    return node;
}

//产生式add_expr
TreeNode *GrammarAnalysis::add_expr() {
    auto node = mult_expr();
    while (curToken.type == TokenType::PLUS || curToken.type == TokenType::MINUS) {
        auto cur = new TreeNode(TreeNodeType::ADD_EXPR, curToken);
        getToken();
        cur->left = node;
        cur->right = mult_expr();
        node = cur;
    }
    return node;
}

//产生式mult_expr
TreeNode *GrammarAnalysis::mult_expr() {
    auto node = unary_expr();
    while (curToken.type == TokenType::TIMES || curToken.type == TokenType::SLASH ||
           curToken.type == TokenType::PERCENT) {
        auto cur = new TreeNode(TreeNodeType::MULT_EXPR, curToken);
        getToken();
        cur->left = node;
        cur->right = unary_expr();
        node = cur;
    }
    return node;
}

//产生式unary_expr
TreeNode *GrammarAnalysis::unary_expr() {
    auto node = new TreeNode(TreeNodeType::UNARY_EXPR);
    switch (curToken.type) {
        case TokenType::MINUS:
        case TokenType::PLUS:
        case TokenType::TILDE:
            node->left = new TreeNode(curToken);
            getToken();
            break;
        default:
            break;
    }
    switch (curToken.type) {
        case TokenType::INTEGER:
        case TokenType::STRING:
        case TokenType::BOOL:
            node->right = new TreeNode(curToken);
            getToken();
            break;
        default:
            throw runtime_error("line: " + to_string(curToken.lineNo) + " unary_expr error");
    }
    return node;
}
