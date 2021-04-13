/**
 * MIDL的语法分析程序
 * using c++17
 * compile with g++ 9.3.0
 */

#include "LexicalAnalysis.hpp"
#include <unordered_map>
#include <algorithm>

using namespace std;

//整个程序的DFA
vector<Token> LexicalAnalysis::scan() {
    curState = STATE::START;
    while (curState != STATE::FINISH) {
        switch (curState) {
            case STATE::START:
                handleStart();
                break;
            case STATE::InID:
                handleInID();
                break;
            case STATE::InSTR:
                handleInStr();
                break;
            case STATE::InINT:
                handleInInt();
                break;
            case STATE::InGRE:
                handleGreOrLes(true);
                break;
            case STATE::InLES:
                handleGreOrLes(false);
                break;
            default:
                break;
        }
    }
    return tokens;
}

//以START状态为起点的状态转换函数
void LexicalAnalysis::handleStart() {
    if (auto curChar = readChar()) {
        startHelp(curChar.value());
    } else {
        curState = STATE::FINISH;
    }
}

//handleStart的辅助函数，使curState指向START识别curChar后的下一个状态
void LexicalAnalysis::startHelp(char curChar) {
    const unordered_map<char, TokenType> hash_map{
            {'{', TokenType::LBRACES},
            {'}', TokenType::RBRACES},
            {';', TokenType::SEMICOLON},
            {'[', TokenType::LBRACKETS},
            {']', TokenType::RBRACKETS},
            {'*', TokenType::TIMES},
            {'+', TokenType::PLUS},
            {'-', TokenType::MINUS},
            {'~', TokenType::TILDE},
            {'/', TokenType::SLASH},
            {'%', TokenType::PERCENT},
            {'&', TokenType::AMPERSAND},
            {'^', TokenType::CARET},
            {'|', TokenType::VERTICAL},
            {',', TokenType::COMMA},
    };

    token.value = string(1, curChar);
    token.lineNo = curLineNo;
    if (isspace(curChar)) {
        curState = STATE::START;
    } else if (isalpha(curChar)) {
        token.type = TokenType::ID;
        curState = STATE::InID;
    } else if (isdigit(curChar)) {
        token.type = TokenType::INTEGER;
        curState = STATE::InINT;
    } else {
        switch (curChar) {
            case '"':
                token.type = TokenType::STRING;
                curState = STATE::InSTR;
                break;
            case '>':
                token.type = TokenType::GREATER;
                curState = STATE::InGRE;
                break;
            case '<':
                token.type = TokenType::LESS;
                curState = STATE::InLES;
                break;
            default:
                if (hash_map.find(curChar) != hash_map.end()) {
                    token.type = hash_map.at(curChar);
                    tokens.push_back(token);
                    curState = STATE::START;
                } else {
                    token.type = TokenType::ERROR;
                    token.value = "unrecognized character: " + token.value;
                    tokens.push_back(token);
                    curState = STATE::START;
                }
                break;
        }
    }
}

//以InID状态为起点的状态转换函数
void LexicalAnalysis::handleInID() {
    if (auto curChar = readChar()) {
        if (curChar.value() == '_') {
            token.value.push_back(curChar.value());
            curChar = readChar();
            if (!curChar) {
                token.type = TokenType::ERROR;
                token.lineNo = curLineNo;
                idHelp();
                curState = STATE::FINISH;
            } else if (curChar.value() == '_') {
                token.value.push_back(curChar.value());
                token.type = TokenType::ERROR;
                token.lineNo = curLineNo;
                //延迟处理错误，等到无法识别id时处理
                curState = STATE::InID;
            } else if (isalnum(curChar.value())) {
                token.value.push_back(curChar.value());
                curState = STATE::InID;
            } else {
                token.type = TokenType::ERROR;
                token.lineNo = curLineNo;
                idHelp();
                //状态转移
                startHelp(curChar.value());
            }
        } else if (isalnum(curChar.value())) {
            token.value.push_back(curChar.value());
            curState = STATE::InID;
        } else {
            //other1情况
            idHelp();
            //状态转移
            startHelp(curChar.value());
        }
    } else {
        idHelp();
        curState = STATE::FINISH;
    }
}

//handleInId的辅助函数，识别id为BOOL类型、关键字还是普通id
void LexicalAnalysis::idHelp() {
    const unordered_map<string, TokenType> keyWord{
            {"STRUCT",   TokenType::STRUCT},
            {"FLOAT",    TokenType::FLOAT},
            {"BOOLEAN",  TokenType::BOOLEAN},
            {"SHORT",    TokenType::SHORT},
            {"LONG",     TokenType::LONG},
            {"DOUBLE",   TokenType::DOUBLE},
            {"INT8",     TokenType::INT8},
            {"INT16",    TokenType::INT16},
            {"INT32",    TokenType::INT32},
            {"INT64",    TokenType::INT64},
            {"UINT8",    TokenType::UINT8},
            {"UINT16",   TokenType::UINT16},
            {"UINT32",   TokenType::UINT32},
            {"UINT64",   TokenType::UINT64},
            {"CHAR",     TokenType::CHAR},
            {"UNSIGNED", TokenType::UNSIGNED}
    };

    if (token.type == TokenType::ERROR) {
        token.value = "invalid id: " + token.value;
    } else {
        if (token.value == "TRUE" || token.value == "FALSE") token.type = TokenType::BOOL;
        else {
            string tmp;
            auto lambda = [](char c) { return toupper(c); };
            std::transform(token.value.begin(), token.value.end(), std::back_inserter(tmp), lambda);
            if (keyWord.find(tmp) != keyWord.end()) {
                token.type = keyWord.at(tmp);
            }
        }
    }

    tokens.push_back(token);
}

//以InStr状态为起点的状态转换函数，可处理转义情况
void LexicalAnalysis::handleInStr() {
    if (auto curChar = readChar()) {
        token.value.push_back(curChar.value());
        if (curChar.value() == '"') {
            if (token.type == TokenType::ERROR) token.value = "unrecognized escape character: " + token.value;
            tokens.push_back(token);
            curState = STATE::START;
        } else if (curChar.value() == '\\') {
            //转义字符处理
            curChar = readChar();
            if (curChar) {
                const string str = "btnfr*\"\\";
                token.value.push_back(curChar.value());
                //\后无期待的转移字符，设置ERROR类型
                if (str.find(curChar.value()) == std::string::npos) {
                    token.type = TokenType::ERROR;
                    token.lineNo = curLineNo;
                    curState = STATE::InSTR;
                } else {
                    curState = STATE::InSTR;
                }
            } else {
                //将处理交给遇到EOF的情况
                curState = STATE::InSTR;
            }
        } else {
            //一般情况
            curState = STATE::InSTR;
        }
    } else {
        token.type = TokenType::ERROR;
        token.value = "incomplete string: " + token.value;
        tokens.push_back(token);
        curState = STATE::FINISH;
    }
}

//以InInt状态为起点的状态转换函数
void LexicalAnalysis::handleInInt() {
    //最后处理，判别数字是否合法
    auto intHelp = [this] {
        if (token.value.size() > 1 && token.value[0] == '0') {
            token.type = TokenType::ERROR;
            token.value = "invalid integer: " + token.value;
        }
        tokens.push_back(token);
    };
    if (auto curChar = readChar()) {
        if (isdigit(curChar.value())) {
            token.value.push_back(curChar.value());
            curState = STATE::InINT;
        } else if (curChar.value() == 'l' || curChar.value() == 'L') {
            token.value.push_back(curChar.value());
            intHelp();
            curState = STATE::START;
        } else {
            intHelp();
            startHelp(curChar.value());
        }
    } else {
        intHelp();
        curState = STATE::FINISH;
    }
}

//InGre和InLES的状态转换函数
//t为true时处理大于情况(GREATER)，t为false处理小于情况(LESS)
void LexicalAnalysis::handleGreOrLes(bool t) {
    if (auto curChar = readChar()) {
        if ((t && curChar.value() == '>') || (!t && curChar.value() == '<')) {
            token.value.push_back(curChar.value());
            tokens.push_back(token);
            curState = STATE::START;
        } else {
            token.type = TokenType::ERROR;
            token.value = (t ? "do you mean >>" : "do you mean <<");
            token.lineNo = curLineNo;
            tokens.push_back(token);
            startHelp(curChar.value());
        }
    } else {
        token.type = TokenType::ERROR;
        token.value = (t ? "do you mean >>" : "do you mean <<");
        token.lineNo = curLineNo;
        tokens.push_back(token);
        curState = STATE::FINISH;
    }
}