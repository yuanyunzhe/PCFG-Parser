#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cstdlib>
#include <ctime>

#include "grammar.hpp"
#include "parsetree.hpp"

using namespace std;

class Generator{
private:
	int numSentences;
	void generate(CNF &cnf, ParseTreeNode *node){
		if (cnf.isUnary[node->nonTerminal]){
			UnaryRule rule = cnf.randomizeUnaryRule(node->nonTerminal);
			node->insertTerminal(rule.terminal);
		}
		else{
			BinaryRule rule = cnf.randomizeBinaryRule(node->nonTerminal);
			node->insertChildren(rule.nonTerminalLeft, rule.nonTerminalRight);
			generate(cnf, node->left);
			generate(cnf, node->right);
		}
	}
public:
	Generator(int numSentences){
		this->numSentences = numSentences;
	}

	void generateSentences(CNF &cnf, ofstream &treeOut, ofstream &evalbOut){
		srand((unsigned)time(NULL));
		for (int i = 1; i <= numSentences; i++){
			ParseTree *parseTree = new ParseTree("S");
			generate(cnf, parseTree->root);
			parseTree->reassignIndex();
			parseTree->save(treeOut);		
			parseTree->saveToEvalbFormat(evalbOut);
		}
	}
};

#endif