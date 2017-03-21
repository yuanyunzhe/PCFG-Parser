main : grammar.hpp parsetree.hpp sentence.hpp generator.hpp main.cpp
	g++ main.cpp -o main

clean :
	rm grammar.hpp.gch parsetree.hpp.gch sentence.hpp.gch generator.hpp.gch