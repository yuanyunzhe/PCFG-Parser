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
	CNF cnf;
	vector<ParseTree> parseTrees;
	void generate(ParseTreeNode *node){
		if (cnf.isTerminal[node->symbol]){
			TerminalRule rule = cnf.randomizeTerminalRule(node->symbol);
			node->insertWord(rule.word);
		}
		else{
			NonTerminalRule rule = cnf.randomizeNonTerminalRule(node->symbol);
			node->insertChildren(rule.nonTerminalLeft, rule.nonTerminalRight);
			generate(node->left);
			generate(node->right);
		}
	}
public:
	Generator(int numSentences, CNF cnf){
		this->numSentences = numSentences;
		this->cnf = cnf;
	}

	void generateSentences(ofstream &treeOut, ofstream &evalbOut){
		srand((unsigned)time(NULL));
		parseTrees.clear();
		for (int i = 1; i <= numSentences; i++){
			ParseTree *parseTree;
			parseTree = new ParseTree("S");
			generate(parseTree->root);
			parseTree->reassignIndex();
			parseTree->save(treeOut);		
			parseTree->saveToEvalbFormat(evalbOut);
			parseTrees.push_back(*parseTree);
		}
	}

};

#endif