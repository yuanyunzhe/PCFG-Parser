#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cstdlib>
#include <ctime>

#include "Grammar.hpp"
#include "Parsetree.hpp"
#include "Parameter.hpp"

using namespace std;

class Generator{
private:
	int numSentences, numWords, numRules;
	CNF cnf;
	void generate(ParseTreeNode *node){
		// cout<<numWords<<"    "<<numRules<<endl;
		if (numRules > MAX_WORDS_IN_SENTENCE) return;
		bool isUnary = (cnf.isUnary.find(node->nonTerminal) != cnf.isUnary.end());
		bool isBinary = (cnf.isBinary.find(node->nonTerminal) != cnf.isBinary.end());
		// cout<<isUnary<<isBinary<<endl;
		if ((isUnary) && (!isBinary)) generateUnary(node);
		else if ((!isUnary) && (isBinary)) generateBinary(node);
		else{
			// srand((unsigned)time(NULL) + numRules);
			ld r = rand() / ld(RAND_MAX), p = cnf.isUnary[node->nonTerminal] / (cnf.isUnary[node->nonTerminal] + cnf.isBinary[node->nonTerminal]);
			// cout<<"@@@@@@"<<cnf.isUnary[node->nonTerminal]<<" "<<cnf.isBinary[node->nonTerminal]<<" "<<node->nonTerminal<<endl;
			// cout<<"   "<<p<<" "<<r<<endl;
			if (p < r) generateUnary(node);
			else generateBinary(node);
		}
	}

	void generateUnary(ParseTreeNode *node){
		UnaryRule rule = cnf.randomizeUnaryRule(node->nonTerminal);
		node->insertTerminal(rule.terminal);
		numWords++;
	}

	void generateBinary(ParseTreeNode *node){
		BinaryRule rule = cnf.randomizeBinaryRule(node->nonTerminal);
		node->insertChildren(rule.nonTerminalLeft, rule.nonTerminalRight);
		numRules++;
		generate(node->left);
		generate(node->right);
	}
public:
	Generator(int numSentences, CNF &cnf){
		this->numSentences = numSentences;
		this->cnf = cnf;
	}

	void generateSentences(ofstream &sentenceOut, int seed){
		// srand((unsigned)time(NULL) + seed);
		int i = 0;
		while (i < numSentences){
			numWords = 0;
			numRules = 0;
			ParseTree *parseTree = new ParseTree("S");
			generate(parseTree->root);
			// parseTree->reassignIndex();
			if ((numWords <= MAX_WORDS_IN_SENTENCE) && (numRules <= MAX_WORDS_IN_SENTENCE)){
				// parseTree->save(treeOut);
				// parseTree->saveToEvalbFormat(evalbOut);
				parseTree->saveToSentence(sentenceOut);
				i++;
			}
		}
	}
};

#endif