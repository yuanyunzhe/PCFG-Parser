#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <cstdlib>
#include <cmath>

#include "Rule.hpp"

using namespace std;

class CNF{
public:
	vector<UnaryRule> unaryRules;
	vector<BinaryRule> binaryRules;

	map<string, bool> isUnary;
	
	void addUnary(string nonTerminal, string terminal, double probability){
		unaryRules.push_back(UnaryRule(nonTerminal, terminal, probability));
		isUnary.insert(pair<string, bool>(unaryRules.back().nonTerminal, true));
	}
	void addBinary(string nonTerminalParent, string nonTerminalLeft, string nonTerminalRight, double probability){
		binaryRules.push_back(BinaryRule(nonTerminalParent, nonTerminalLeft, nonTerminalRight, probability));
		isUnary.insert(pair<string, bool>(binaryRules.back().nonTerminalParent, false));
	}

	UnaryRule randomizeUnaryRule(string nonTerminal){
		double randomProbability = rand() / double(RAND_MAX);
		vector<UnaryRule>::iterator iter;
		UnaryRule rule;
		for (iter = unaryRules.begin(); iter != unaryRules.end(); iter++)
			if (iter->nonTerminal == nonTerminal){
				randomProbability -= iter->probability;
				if (randomProbability <= 0.0000001) break;
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
				if (randomProbability <= 0.0000001)
					break;
			}
		return *iter;
	}

	void generalizeProbability(){
		vector<UnaryRule>::iterator iterUnary;
		vector<BinaryRule>::iterator iterBinary;
		map<string, double> generalizationUnary, generalizationBinary;
		for (iterUnary = unaryRules.begin(); iterUnary != unaryRules.end(); iterUnary++)
			generalizationUnary[iterUnary->nonTerminal] += iterUnary->probability;
		for (iterBinary = binaryRules.begin(); iterBinary != binaryRules.end(); iterBinary++)
			generalizationBinary[iterBinary->nonTerminalParent] += iterBinary->probability;
		for (iterUnary = unaryRules.begin(); iterUnary != unaryRules.end(); iterUnary++)
			iterUnary->probability /= generalizationUnary[iterUnary->nonTerminal];
		for (iterBinary = binaryRules.begin(); iterBinary != binaryRules.end(); iterBinary++)
			iterBinary->probability /= generalizationBinary[iterBinary->nonTerminalParent];
	}

	void read(ifstream &grammarIn){
		while (!grammarIn.eof()){
			string s1, s2, s3;
			double p;
			grammarIn >> s1 >> p >> s2 >> s3;
			if (s2 == ".") addUnary(s1, s3, p);
			else addBinary(s1, s2, s3, p);
		}
		generalizeProbability();
	}
};

#define MAX_NON_TERMINALS 100
#define MAX_TERMINALS 100

class TrainingGrammar{
public:
	int numNonTerminals, numTerminals;
	double nonTerminalProbability[MAX_NON_TERMINALS][MAX_NON_TERMINALS][MAX_NON_TERMINALS];
	double terminalProbability[MAX_NON_TERMINALS][MAX_TERMINALS];
	TrainingGrammar(int numNonTerminals, int numTerminals){
		this->numNonTerminals = numNonTerminals;
		this->numTerminals = numTerminals;
	}
	map<string, int> indexTerminal;
};

#endif