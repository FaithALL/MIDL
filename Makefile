cc = g++
MIDL: LexicalAnalysis.o GrammarAnalysis.o
	$(cc) -std=c++17 test2.cpp LexicalAnalysis.o GrammarAnalysis.o -o MIDL
LexicalAnalysis.o: LexicalAnalysis.cpp LexicalAnalysis.hpp
	$(cc) -std=c++17 -c LexicalAnalysis.cpp -o LexicalAnalysis.o
GrammarAnalysis.o: GrammarAnalysis.cpp GrammarAnalysis.hpp
	$(cc) -std=c++17 -c GrammarAnalysis.cpp -o GrammarAnalysis.o
clean:
	rm *.o