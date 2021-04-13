/**
 * MIDL的语法分析程序
 * using c++17
 * compile with g++ 9.3.0
 */

#ifndef LEXICALANALYSIS_HPP
#define LEXICALANALYSIS_HPP

#include <vector>
#include <istream>
#include <optional>

enum class TokenType {
    //16个关键字
    STRUCT,
    FLOAT,
    BOOLEAN,
    SHORT,
    LONG,
    DOUBLE,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    CHAR,
    UNSIGNED,

    //17个专用符号
    LBRACES,            //  {
    RBRACES,            //  }
    SEMICOLON,          //  ;
    LBRACKETS,          //  [
    RBRACKETS,          //  ]
    TIMES,              //  *
    PLUS,               //  +
    MINUS,              //  -
    TILDE,              //  ~
    SLASH,              //  /
    PERCENT,            //  %
    GREATER,            //  >>
    LESS,               //  <<
    AMPERSAND,          //  &
    CARET,              //  ^
    VERTICAL,           //  |
    COMMA,              //  ,

    //4种类型
    ID,                 //标识符
    STRING,             //字符串
    INTEGER,            //整数
    BOOL,               //布尔值

    //自定义错误
    ERROR
};

struct Token {
    Token() = default;

    //构造函数
    Token(int line, std::string_view tokenValue, TokenType tokenType) {
        lineNo = line;
        value = tokenValue;
        type = tokenType;
    }

    //每个Token的类型
    TokenType type = TokenType::ID;

    //每个Token的值
    std::string value;

    //行号
    int lineNo = -1;
};

//词法分析程序
class LexicalAnalysis {
public:
    explicit LexicalAnalysis(std::istream &in) : in(in) {}

    std::vector<Token> scan();

private:
    //输入
    std::istream &in;
    //状态集
    enum class STATE {
        START,
        InID,
        InSTR,
        InINT,
        InGRE,
        InLES,
        FINISH
    };
    //当前状态
    STATE curState = STATE::START;
    //当前token
    Token token;
    //tokens
    std::vector<Token> tokens;
    //当前行
    int curLineNo = 0;

    //读取一个字符，并设置curLineNo，如果读到EOF，返回std::nullopt
    std::optional<char> readChar() {
        static std::string str;
        static int pos = 0;
        if (pos >= str.size()) {
            ++curLineNo;
            pos = 0;
            if (!getline(in, str)) return std::nullopt;
        }
        return str[pos++];
    }

    //以START状态为起点的状态转换函数
    void handleStart();

    //handleStart的辅助函数，使curState指向START识别curChar后的下一个状态
    void startHelp(char curChar);

    //以InID状态为起点的状态转换函数
    void handleInID();

    //handleInID的辅助函数，当id遇到other的时候，识别id为BOOL类型、关键字还是普通id
    void idHelp();

    //以InStr状态为起点的状态转换函数
    void handleInStr();

    //以InInt状态为起点的状态转换函数
    void handleInInt();

    //InGre和InLES的状态转换函数
    //t为true时处理大于情况(GREATER)，t为false处理小于情况(LESS)
    void handleGreOrLes(bool t);
};


#endif //LEXICALANALYSIS_HPP
