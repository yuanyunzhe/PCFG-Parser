#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <cstdlib>

#include "rule.hpp"

using namespace std;

class CNF{
public:
	vector<UnaryRule> unaryRules;
	vector<BinaryRule> binaryRules;

	map<string, bool> isUnary;

	map<string, UnaryRule> indexTerminal;
	//multimap<string, UnaryRule> indexUnary;
	multimap<string, BinaryRule>indexNonTerminalParent, indexNonTerminalLeft, indexNonTerminalRight;
	multimap<pair<string, string>, BinaryRule> indexNonTerminalChildren;

	void addUnary(string nonTerminal, string terminal, double probability){
		unaryRules.push_back(UnaryRule(nonTerminal, terminal, probability));
		isUnary.insert(pair<string, bool>(nonTerminal, true));
		indexTerminal.insert(pair<string, UnaryRule>(terminal, unaryRules.back()));
	}
	void addBinary(string nonTerminalParent, string nonTerminalLeft, string nonTerminalRight, double probability){
		binaryRules.push_back(BinaryRule(nonTerminalParent, nonTerminalLeft, nonTerminalRight, probability));
		isUnary.insert(pair<string, bool>(nonTerminalParent, false));
		indexNonTerminalParent.insert(pair<string, BinaryRule>(nonTerminalParent, binaryRules.back()));
		indexNonTerminalLeft.insert(pair<string, BinaryRule>(nonTerminalLeft, binaryRules.back()));
		indexNonTerminalRight.insert(pair<string, BinaryRule>(nonTerminalRight, binaryRules.back()));
		indexNonTerminalChildren.insert(pair<pair<string, string>, BinaryRule>(pair<string, string>(nonTerminalLeft, nonTerminalRight), binaryRules.back()));
	}

	UnaryRule randomizeUnaryRule(string nonTerminal){
		double randomProbability = rand() / double(RAND_MAX);
		vector<UnaryRule>::iterator iter;
		UnaryRule rule;
		for (iter = unaryRules.begin(); iter != unaryRules.end(); iter++)
			if (iter->nonTerminal == nonTerminal){
				randomProbability -= iter->probability;
				if (randomProbability <= 0) break;
			}
		return *iter;
	}
	BinaryRule randomizeBinaryRule(string nonTerminal){
		double randomProbability = rand() / double(RAND_MAX);
		vector<BinaryRule>::iterator iter;
		BinaryRule rule;
		for (iter = binaryRules.begin(); iter != binaryRules.end(); iter++)
			if (iter->nonTerminalParent == nonTerminal){
				randomProbability -= iter->probability;
				if (randomProbability <= 0) break;
			}
		return *iter;
	}
	
	void read(ifstream &grammarIn){
		while (!grammarIn.eof()){
			string s1, s2, s3;
			double p;
			grammarIn >> s1 >> p >> s2 >> s3;
			if (s2 == ".") addUnary(s1, s3, p);
			else addBinary(s1, s2, s3, p);
		}
	}
};

#endif