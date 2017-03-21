#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <cstdlib>

using namespace std;

class TerminalRule{
public:
	string terminal, word;
	double probability;
	TerminalRule(){;}
	TerminalRule(string terminal, string word, double probability){
		this->terminal = terminal;
		this->word = word;
		this->probability = probability;
	}
};
class NonTerminalRule{
public:
	string nonTerminalParent, nonTerminalLeft, nonTerminalRight;
	double probability;
	NonTerminalRule(){;}
	NonTerminalRule(string nonTerminalParent, string nonTerminalLeft, string nonTerminalRight, double probability){
		this->nonTerminalParent = nonTerminalParent;
		this->nonTerminalLeft = nonTerminalLeft;
		this->nonTerminalRight = nonTerminalRight;
		this->probability = probability;
	}
};

class CNF{
public:
	vector<TerminalRule> terminalRules;
	vector<NonTerminalRule> nonTerminalRules;
	map<string, bool> isTerminal;
	multimap<string, TerminalRule> indexTerminal;
	multimap<string, NonTerminalRule>indexNonTerminalParent, indexNonTerminalLeft, indexNonTerminalRight;
	map<string, TerminalRule> indexWord;
	multimap<pair<string, string>, NonTerminalRule> indexNonTerminalChildren;

	void addTerminal(string terminal, string word, double probability){
		terminalRules.push_back(TerminalRule(terminal, word, probability));
		isTerminal.insert(pair<string, bool>(terminal, true));
		indexWord.insert(pair<string, TerminalRule>(word, terminalRules.back()));
	}
	void addNonTerminal(string nonTerminalParent, string nonTerminalLeft, string nonTerminalRight, double probability){
		nonTerminalRules.push_back(NonTerminalRule(nonTerminalParent, nonTerminalLeft, nonTerminalRight, probability));
		isTerminal.insert(pair<string, bool>(nonTerminalParent, false));
		indexNonTerminalParent.insert(pair<string, NonTerminalRule>(nonTerminalParent, nonTerminalRules.back()));
		indexNonTerminalLeft.insert(pair<string, NonTerminalRule>(nonTerminalLeft, nonTerminalRules.back()));
		indexNonTerminalRight.insert(pair<string, NonTerminalRule>(nonTerminalRight, nonTerminalRules.back()));
		indexNonTerminalChildren.insert(pair<pair<string, string>, NonTerminalRule>(pair<string, string>(nonTerminalLeft, nonTerminalRight), nonTerminalRules.back()));
	}

	TerminalRule randomizeTerminalRule(string terminal){
		double randomProbability = rand() / double(RAND_MAX);
		vector<TerminalRule>::iterator ruleIterator;
		TerminalRule rule;
		for (ruleIterator = terminalRules.begin(); ruleIterator != terminalRules.end(); ruleIterator++){
			rule = *ruleIterator;
			if (rule.terminal == terminal){
				randomProbability -= rule.probability;
				if (randomProbability <= 0) break;
			}
		}
		return rule;
	}
	NonTerminalRule randomizeNonTerminalRule(string nonTerminal){
		double randomProbability = rand() / double(RAND_MAX);
		vector<NonTerminalRule>::iterator ruleIterator;
		NonTerminalRule rule;
		for (ruleIterator = nonTerminalRules.begin(); ruleIterator != nonTerminalRules.end(); ruleIterator++){
			rule = *ruleIterator;
			if (rule.nonTerminalParent == nonTerminal){
				randomProbability -= rule.probability;
				if (randomProbability <= 0) break;
			}
		}
		return rule;
	}
	
	void read(ifstream &grammarIn){
		while (!grammarIn.eof()){
			string s1, s2, s3;
			double p;
			grammarIn >> s1 >> p >> s2 >> s3;
			if (s2 == ".") addTerminal(s1, s3, p);
			else addNonTerminal(s1, s2, s3, p);
		}
	}
};

#endif