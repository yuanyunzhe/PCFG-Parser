main : ./src/grammar.hpp ./src/parsetree.hpp ./src/sentence.hpp ./src/generator.hpp main.cpp
	g++ main.cpp -o main

clean :
	rm ./src/grammar.hpp.gch ./src/parsetree.hpp.gch ./src/sentence.hpp.gch ./src/generator.hpp.gch