#ifndef RULE_HPP
#define RULE_HPP

#include <vector>
#include <string>

using namespace std;

class UnaryRule{
public:
	string nonTerminal, terminal;
	double probability;
	UnaryRule(){;}
	UnaryRule(string nonTerminal, string terminal, double probability){
		this->nonTerminal = nonTerminal;
		this->terminal = terminal;
		this->probability = probability;
	}
};
class BinaryRule{
public:
	string nonTerminalParent, nonTerminalLeft, nonTerminalRight;
	double probability;
	BinaryRule(){;}
	BinaryRule(string nonTerminalParent, string nonTerminalLeft, string nonTerminalRight, double probability){
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