#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <cstdlib>
#include <cstring>
#include <cmath>

#include "Rule.hpp"
#include "Math.hpp"
#include "Structure.hpp"

using namespace std;

class CNF{
public:
	vector<UnaryRule> unaryRules;
	vector<BinaryRule> binaryRules;

	map<string, ld> isUnary, isBinary;
	
	void addUnary(string nonTerminal, string terminal, ld probability){
		unaryRules.push_back(UnaryRule(nonTerminal, terminal, probability));
		if (isUnary.find(nonTerminal) != isUnary.end()) isUnary[nonTerminal] = probability;
		else isUnary[nonTerminal] += probability;
	}
	void addBinary(string nonTerminalParent, string nonTerminalLeft, string nonTerminalRight, ld probability){
		binaryRules.push_back(BinaryRule(nonTerminalParent, nonTerminalLeft, nonTerminalRight, probability));
		if (isBinary.find(nonTerminalParent) != isBinary.end()) isBinary[nonTerminalParent] = probability;
		else isBinary[nonTerminalParent] += probability;
	}

	UnaryRule randomizeUnaryRule(string nonTerminal){
		ld randomProbability = rand() / ld(RAND_MAX);
		vector<UnaryRule>::iterator iter;
		UnaryRule rule;
		for (iter = unaryRules.begin(); iter != unaryRules.end(); iter++)
			if (iter->nonTerminal == nonTerminal){
				randomProbability -= iter->gen;
				if (randomProbability <= 0.0000001) break;
			}
		return *iter;
	}
	BinaryRule randomizeBinaryRule(string nonTerminal){
		ld randomProbability = rand() / ld(RAND_MAX);
		vector<BinaryRule>::iterator iter;
		BinaryRule rule;
		for (iter = binaryRules.begin(); iter != binaryRules.end(); iter++)
			if (iter->nonTerminalParent == nonTerminal){
				randomProbability -= iter->gen;
				if (randomProbability <= 0.0000001)
					break;
			}
		return *iter;
	}

	void normalizeProbability(){
		vector<UnaryRule>::iterator iterUnary;
		vector<BinaryRule>::iterator iterBinary;
		map<string, ld> normalization, normalizationUnary, normalizationBinary;
		for (iterUnary = unaryRules.begin(); iterUnary != unaryRules.end(); iterUnary++){
			normalizationUnary[iterUnary->nonTerminal] += iterUnary->probability;
			normalization[iterUnary->nonTerminal] += iterUnary->probability;
		}
		for (iterBinary = binaryRules.begin(); iterBinary != binaryRules.end(); iterBinary++){
			normalizationBinary[iterBinary->nonTerminalParent] += iterBinary->probability;
			normalization[iterBinary->nonTerminalParent] += iterBinary->probability;
		}
		for (iterUnary = unaryRules.begin(); iterUnary != unaryRules.end(); iterUnary++){
			iterUnary->gen = iterUnary->probability / normalizationUnary[iterUnary->nonTerminal];
			iterUnary->probability /= normalization[iterUnary->nonTerminal];
		}
		for (iterBinary = binaryRules.begin(); iterBinary != binaryRules.end(); iterBinary++){
			iterBinary->gen = iterBinary->probability / normalizationBinary[iterBinary->nonTerminalParent];
			iterBinary->probability /= normalization[iterBinary->nonTerminalParent];
		}
	}

	void read(ifstream &grammarIn){
		while (!grammarIn.eof()){
			string s1, s2, s3;
			ld p;
			grammarIn >> s1 >> p >> s2 >> s3;
			if (s2 == ".") addUnary(s1, s3, p);
			else addBinary(s1, s2, s3, p);
		}
		normalizeProbability();
	}
};

#define MAX_NON_TERMINALS 500
#define MAX_TERMINALS 300

class TrainingGrammar{
public:
	int numNonTerminals, numTerminals;
	map<string, int> indexTerminal, indexNonTerminal;
	ld ***nonTerminalProbability, **terminalProbability;
	LogDouble ***LogNonTerminalProbability, **LogTerminalProbability;
	
	TrainingGrammar(int numNonTerminals, int numTerminals){
		this->numNonTerminals = numNonTerminals;
		this->numTerminals = numTerminals;
		malloc(&nonTerminalProbability, MAX_NON_TERMINALS, MAX_NON_TERMINALS, MAX_NON_TERMINALS);
		malloc(&terminalProbability, MAX_NON_TERMINALS, MAX_TERMINALS);
		malloc(&LogNonTerminalProbability, MAX_NON_TERMINALS, MAX_NON_TERMINALS, MAX_NON_TERMINALS);
		malloc(&LogTerminalProbability, MAX_NON_TERMINALS, MAX_TERMINALS);
	}
	TrainingGrammar(){
		malloc(&nonTerminalProbability, MAX_NON_TERMINALS, MAX_NON_TERMINALS, MAX_NON_TERMINALS);
		malloc(&terminalProbability, MAX_NON_TERMINALS, MAX_TERMINALS);
		malloc(&LogNonTerminalProbability, MAX_NON_TERMINALS, MAX_NON_TERMINALS, MAX_NON_TERMINALS);
		malloc(&LogTerminalProbability, MAX_NON_TERMINALS, MAX_TERMINALS);
	}
};

#endif