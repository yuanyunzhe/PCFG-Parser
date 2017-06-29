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
	int numSentences, numWords;
	CNF cnf;
	void generate(ParseTreeNode *node){
		// cout<<numWords<<endl;
		// if (numWords > MAX_WORDS_IN_SENTENCE) return;
		if (cnf.isUnary[node->nonTerminal]){
			UnaryRule rule = cnf.randomizeUnaryRule(node->nonTerminal);
			node->insertTerminal(rule.terminal);
			numWords++;
		}
		else{
			BinaryRule rule = cnf.randomizeBinaryRule(node->nonTerminal);
			node->insertChildren(rule.nonTerminalLeft, rule.nonTerminalRight);
			generate(node->left);
			generate(node->right);
		}
	}
public:
	Generator(int numSentences, CNF &cnf){
		this->numSentences = numSentences;
		this->cnf = cnf;
	}

	void generateSentences(ofstream &sentenceOut, int seed){
		srand((unsigned)time(NULL) + seed);
		int i = 0;
		while (i < numSentences){
			numWords = 0;
			ParseTree *parseTree = new ParseTree("S");
			generate(parseTree->root);
			// parseTree->reassignIndex();
			if (numWords <= MAX_WORDS_IN_SENTENCE){
				// parseTree->save(treeOut);
				// parseTree->saveToEvalbFormat(evalbOut);
				parseTree->saveToSentence(sentenceOut);
				i++;
			}
		}
	}
};

#endif