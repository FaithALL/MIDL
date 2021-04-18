#include "LexicalAnalysis.hpp"
#include <iostream>
#include <unordered_map>
#include <iomanip>
#include <fstream>

using namespace std;

//将类型映射到string，用于输出
const unordered_map<TokenType, string> type{
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

//输出Token序列
void print(ostream& fout, vector<Token> &vec) {
    const int width = 20;
    fout << left;
    fout << setw(width) << "line" << setw(width) << "TokenType" << setw(width) << "TokenValue" << endl;
    for (const auto &token: vec) {
        if (token.type == TokenType::ERROR) cout << "\033[31m";
        fout << setw(width) << token.lineNo << setw(width) << type.at(token.type) << setw(width) << token.value << endl;
        fout << "\033[0m";
    }
}

int main() {
    ifstream fin("test/test.txt");
    if (!fin.is_open()) {
        cout << "failed to open file" << endl;
        return 0;
    }
    LexicalAnalysis lexicalAnalysis(fin);
    vector<Token> vec = lexicalAnalysis.scan();
    print(cout, vec);
    return 0;
}