#ifndef RULE_HPP
#define RULE_HPP

#include <vector>
#include <string>

#include "Math.hpp"

using namespace std;

class UnaryRule{
public:
	string nonTerminal, terminal;
	ld probability, gen;
	UnaryRule(){;}
	UnaryRule(string nonTerminal, string terminal, ld probability){
		this->nonTerminal = nonTerminal;
		this->terminal = terminal;
		this->probability = probability;
	}
};
class BinaryRule{
public:
	string nonTerminalParent, nonTerminalLeft, nonTerminalRight;
	ld probability, gen;
	BinaryRule(){;}
	BinaryRule(string nonTerminalParent, string nonTerminalLeft, string nonTerminalRight, ld probability){
		this->nonTerminalParent = nonTerminalParent;
		this->nonTerminalLeft = nonTerminalLeft;
		this->nonTerminalRight = nonTerminalRight;
		this->probability = probability;
	}
};

class Rules{
public:
	vector<UnaryRule> unaryRules;
	vector<BinaryRule> binaryRules;
};


#endif