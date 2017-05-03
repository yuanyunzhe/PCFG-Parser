main : ./src/Rule.hpp ./src/Grammar.hpp ./src/Parsetree.hpp ./src/EM.hpp ./src/Generator.hpp ./src/Math.hpp ./src/Structure.hpp main.cpp
	g++ main.cpp -o main

clean :
	rm ./src/Rule.hpp.gch ./src/Grammar.hpp.gch ./src/Parsetree.hpp.gch ./src/EM.hpp.gch ./src/Generator.hpp.gch ./src/Math.hpp.gch ./src/Structure.hpp.gch