main : ./src/Rule.hpp ./src/Grammar.hpp ./src/Parsetree.hpp ./src/Sentence.hpp ./src/Generator.hpp main.cpp
	g++ main.cpp -o main

clean :
	rm ./src/Rule.hpp.gch ./src/Grammar.hpp.gch ./src/Parsetree.hpp.gch ./src/Sentence.hpp.gch ./src/Generator.hpp.gch