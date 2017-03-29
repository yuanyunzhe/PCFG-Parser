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
	double probability, count;
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

#define MAX_NON_TERMINALS 100
#define MAX_TERMINALS 100

class TrainingRules{
public:
	int numNonTerminals, numTerminals;
	double nonTerminalProbability[MAX_NON_TERMINALS][MAX_NON_TERMINALS][MAX_NON_TERMINALS];
	double terminalProbability[MAX_NON_TERMINALS][MAX_TERMINALS];
	TrainingRules(int numNonTerminals, int numTerminals){
		this->numNonTerminals = numNonTerminals;
		this->numTerminals = numTerminals;
	}
};

#endif