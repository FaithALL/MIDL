#include "LexicalAnalysis.hpp"
#include "GrammarAnalysis.hpp"
#include <fstream>
#include <iostream>

using namespace std;

int main() {
    for (int i = 0; i <= 15; ++i) {
        ifstream fin("test/test" + to_string(i) + ".txt");
        ofstream fout("test/out/syntaxOut" + to_string(i) + ".txt");
        if (!fin.is_open() || !fout.is_open()) {
            cout << "failed to open file" << endl;
            return 0;
        }
        LexicalAnalysis lexicalAnalysis(fin);
        vector<Token> vec = lexicalAnalysis.scan();
        bool error = false;
        for (auto &token : vec) {
            if (token.type == TokenType::ERROR) {
                fout << "line: " << token.lineNo << " " << token.value << endl;
                error = true;
            }
        }
        if (error) continue;

        GrammarAnalysis grammarAnalysis(vec, fout);
        try {
            grammarAnalysis.parse();
        } catch (exception &e) { continue; }
        grammarAnalysis.print();
        fin.close();
        fout.close();
    }
}
