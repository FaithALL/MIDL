CC = g++
SRC = ../src

../MIDL: LexicalAnalysis.o GrammarAnalysis.o
	$(CC) -std=c++17 ../test2.cpp LexicalAnalysis.o GrammarAnalysis.o -o ../MIDL

LexicalAnalysis.o: $(SRC)/LexicalAnalysis.cpp $(SRC)/LexicalAnalysis.hpp
	$(CC) -std=c++17 -c $(SRC)/LexicalAnalysis.cpp -o LexicalAnalysis.o

GrammarAnalysis.o: $(SRC)/GrammarAnalysis.cpp $(SRC)/GrammarAnalysis.hpp
	$(CC) -std=c++17 -c $(SRC)/GrammarAnalysis.cpp -o GrammarAnalysis.o

clean:
	rm *.o