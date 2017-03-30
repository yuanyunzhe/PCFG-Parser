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

using namespace std;

class Generator{
private:
	int numSentences;
	CNF cnf;
	void generate(ParseTreeNode *node){
		if (cnf.isUnary[node->nonTerminal]){
			UnaryRule rule = cnf.randomizeUnaryRule(node->nonTerminal);
			node->insertTerminal(rule.terminal);
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

	void generateSentences(ofstream &treeOut, ofstream &evalbOut){
		srand((unsigned)time(NULL));
		for (int i = 1; i <= numSentences; i++){
			ParseTree *parseTree = new ParseTree("S");
			generate(parseTree->root);
			parseTree->reassignIndex();
			parseTree->save(treeOut);		
			parseTree->saveToEvalbFormat(evalbOut);
		}
	}
};

#endif