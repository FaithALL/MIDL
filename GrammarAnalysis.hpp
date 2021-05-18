/**
 * MIDL的语法分析程序
 * using c++17
 * compile with g++ 9.3.0
 */

#ifndef GRAMMARANALYSIS_HPP
#define GRAMMARANALYSIS_HPP

#include "LexicalAnalysis.hpp"
#include "json.hpp"
#include <utility>
#include <vector>
#include <memory>

using nlohmann::json;

//将类型映射到string，用于输出
const std::unordered_map<TokenType, std::string> type{
        //16个关键字
        {TokenType::STRUCT,    "STRUCT"},
        {TokenType::FLOAT,     "FLOAT"},
        {TokenType::BOOLEAN,   "BOOLEAN"},
        {TokenType::SHORT,     "SHORT"},
        {TokenType::LONG,      "LONG"},
        {TokenType::DOUBLE,    "DOUBLE"},
        {TokenType::INT8,      "INT8"},
        {TokenType::INT16,     "INT16"},
        {TokenType::INT32,     "INT32"},
        {TokenType::INT64,     "INT64"},
        {TokenType::UINT8,     "UINT8"},
        {TokenType::UINT16,    "UINT16"},
        {TokenType::UINT32,    "UINT32"},
        {TokenType::UINT64,    "UINT64"},
        {TokenType::CHAR,      "CHAR"},
        {TokenType::UNSIGNED,  "UNSIGNED"},
        {TokenType::LBRACES,   "LBRACES"},
        {TokenType::RBRACES,   "RBRACES"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::LBRACKETS, "LBRACKETS"},
        {TokenType::RBRACKETS, "RBRACKETS"},
        {TokenType::TIMES,     "TIMES"},
        {TokenType::PLUS,      "PLUS"},
        {TokenType::MINUS,     "MINUS"},
        {TokenType::TILDE,     "TILDE"},
        {TokenType::SLASH,     "SLASH"},
        {TokenType::PERCENT,   "PERCENT"},
        {TokenType::GREATER,   "GREATER"},
        {TokenType::LESS,      "LESS"},
        {TokenType::AMPERSAND, "AMPERSAND"},
        {TokenType::CARET,     "CARET"},
        {TokenType::VERTICAL,  "VERTICAL"},
        {TokenType::COMMA,     "COMMA"},
        {TokenType::ID,        "ID"},
        {TokenType::STRING,    "STRING"},
        {TokenType::INTEGER,   "INTEGER"},
        {TokenType::BOOL,      "BOOL"},
        {TokenType::ERROR,     "ERROR"}
};

//树结点类型
enum class TreeNodeType {
    DEFAULT,
    STRUCT,
    ID,
    MEMBER,
    DECLARATOR,
    TYPE_SPEC,
    OR_EXPR,
    XOR_EXPR,
    AND_EXPR,
    SHIFT_EXPR,
    ADD_EXPR,
    MULT_EXPR,
    UNARY_EXPR
};

struct TreeNode;

//树结点声明
struct TreeNode {
    TreeNode *next = nullptr;
    TreeNode *left = nullptr;
    TreeNode *right = nullptr;
    //树结点类型
    TreeNodeType type = TreeNodeType::DEFAULT;
    //树结点存的token
    Token value;

    TreeNode() = default;

    explicit TreeNode(Token token) : value(std::move(token)) {}

    explicit TreeNode(TreeNodeType treeNodeType) : type(treeNodeType) {}

    TreeNode(TreeNodeType treeNodeType, Token token) : type(treeNodeType), value(std::move(token)) {}
};


class GrammarAnalysis {
public:
    explicit GrammarAnalysis(std::vector<Token> vec, std::ostream &fout)
            : vec(std::move(vec)), fout(fout) {
        Token token;
        token.type = TokenType::EEOF;
        this->vec.push_back(token);
    }

    //解析出抽象语法树
    void parse();

    //打印抽象语法树
    void print();

    ~GrammarAnalysis() { destroy(root); }

private:
    //树根结点
    TreeNode *root = nullptr;

    Token curToken;
    std::vector<Token> vec;
    int pos = 0;
    std::ostream &fout;

    //遍历语法树
    void inorder(TreeNode *node, json &js, const std::unordered_map<TreeNodeType, std::string> &map);

    //销毁语法分析树
    void destroy(TreeNode *node) {
        if (node) {
            destroy(node->next);
            destroy(node->left);
            destroy(node->right);
            delete node;
            node = nullptr;
        }
    }

    //获取一个token
    void getToken() {
        if (pos < vec.size()) {
            curToken = vec[pos++];
        } else {
            throw std::runtime_error("reach to EOF! expected more token");
        }
    }

    //匹配一个终结符token
    void match(TokenType tokenType) {
        if (curToken.type == tokenType) {
            getToken();
        } else {
            throw std::runtime_error(
                    "line: " + std::to_string(curToken.lineNo) + " want " + type.at(tokenType) + " but match " +
                    type.at(curToken.type) + "( " + curToken.value + " )");
        }
    }

    TreeNode *struct_type();

    TreeNode *member_list();

    bool member_list_help() const;

    TreeNode *type_spec();

    TreeNode *declarators();

    TreeNode *declarator();

    TreeNode *exp_list();

    TreeNode *or_expr();

    TreeNode *xor_expr();

    TreeNode *and_expr();

    TreeNode *shift_expr();

    TreeNode *add_expr();

    TreeNode *mult_expr();

    TreeNode *unary_expr();
};


#endif //GRAMMARANALYSIS_HPP
