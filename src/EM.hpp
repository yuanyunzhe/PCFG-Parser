#ifndef SENTENCE_HPP
#define SENTENCE_HPP

#include <iostream>
#include <fstream>

#include <cmath>
#include <cstdio>
#include <cstring>

#include "Grammar.hpp"
#include "Parsetree.hpp"
using namespace std;

const int MAX_WORDS = 100;
#define MAX_SENTENCES 1000

class Previous{
public:
	int i, j, x;
};

class EM{
public:
	TrainingGrammar grammar;
	CNF cnf;
	ParseTree *parseTree;
	vector<string> words[MAX_SENTENCES];
	double table[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS], alpha[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS], beta[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS], count[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS];
	Previous pre[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS];
	int numSentences, numNonTerminals;
	double likelihood;
	EM(){;}
	void setNumSentences(int numSentences){
		this->numSentences = numSentences;
	}
	void initializeTrainingGrammar(int numNonTerminals){
		int numTerminals = grammar.indexTerminal.size();
		grammar.numNonTerminals = numNonTerminals;
		grammar.numTerminals = numTerminals;
		srand((unsigned)time(NULL));
		for (int i = 0; i < numNonTerminals; i++){
			double sum = 0;
			for (int j = 0; j < numNonTerminals; j++)
				if (i != j)
					for (int k = 0; k < numNonTerminals; k++)
						if (j != k){
							grammar.nonTerminalProbability[i][j][k] = rand() / double(RAND_MAX);
							sum += grammar.nonTerminalProbability[i][j][k];
						}
			for (int j = 0; j < numNonTerminals; j++)
				for (int k = 0; k < numNonTerminals; k++)
					grammar.nonTerminalProbability[i][j][k] /= sum;
		}
		for (int i = 0; i < numNonTerminals; i++){
			double sum = 0;
			for (int j = 0; j < numTerminals; j++){
				grammar.terminalProbability[i][j] = rand() / double(RAND_MAX);
				sum += grammar.terminalProbability[i][j];
			}
			for (int j = 0; j < numNonTerminals; j++)
				grammar.terminalProbability[i][j] /= sum;
		}	
	}

	void readParseTree(ifstream &treeIn){
		for (int i = 0; i < numSentences; i++){
			parseTree = new ParseTree;
			parseTree->load(treeIn);
			parseTree->preOrderTraversal(parseTree->root, words[i]);
			for (int j = 0; j < words[i].size(); j++)
				if (grammar.indexTerminal.find(words[i][j]) != grammar.indexTerminal.end())
					grammar.indexTerminal[words[i][j]] = grammar.indexTerminal.size();
		}
	}
	void readGrammar(ifstream &grammarIn){
		cnf.read(grammarIn);
	}

	void training(){
		for (int i = 0; i < 10; i++){
			cout << i << " ";
			expection();
			maximization();
		}
	//	CYK(lalala);
	}

	void expection(){
		memset(count, 0, sizeof(count));
		likelihood = 1;
		for (int j = 0; j < numSentences; j++)
			calculateInsideOutside(j);		
	}
	void maximization(){
		for (int x = 0; x < numNonTerminals; x++){
			double tmp = 0;
			for (int y = 0; y < numNonTerminals; y++)
				for (int z = 0; z < numNonTerminals; z++)
					tmp += count[x][y][z];
			for (int y = 0; y < numNonTerminals; y++)
				for (int z = 0; z < numNonTerminals; z++)
					count[x][y][z] /= tmp;
		}
	}

	void calculateInsideOutside(int p){
		memset(alpha, 0, sizeof(alpha));
		memset(beta, 0, sizeof(beta));
		for (int j = 0; j < words[p].size(); j++){
			for (int i = 0; i < numNonTerminals; i++)
				beta[j][j][i] = grammar.terminalProbability[i][grammar.indexTerminal[words[p][j]]];
			for (int i = j - 1; i >= 0; i--)
				for (int k = i; k <= j - 1; k++)
					for (int x = 0; x < numNonTerminals; x++)
						for (int y = 0; y < numNonTerminals; y++)
							for (int z = 0; z < numNonTerminals; z++)
								beta[i][j][x] += grammar.nonTerminalProbability[x][y][z] * beta[i][k][x] * beta[k + 1][j][x];
		}
		alpha[0][words[p].size() - 1][0] = 1;
		for (int j = 0; j < words[p].size(); j++)
			for (int i = 0; i <= j; i++){
				for (int k = 0; k < i; k++)
					for (int x = 0; x < numNonTerminals; x++)
						for (int y = 0; y < numNonTerminals; y++)
							for (int z = 0; z < numNonTerminals; z++)
								alpha[i][j][y] += grammar.nonTerminalProbability[x][y][z] * alpha[i][k][x] * alpha[k + 1][j][z];
				for (int k = j + 1; k < words[p].size(); k++)
					for (int x = 0; x < numNonTerminals; x++)
						for (int y = 0; y < numNonTerminals; y++)
							for (int z = 0; z < numNonTerminals; z++)
								alpha[i][j][z] += grammar.nonTerminalProbability[x][y][z] * alpha[k][j][x] * beta[k][i - 1][y];
			}

		for (int i = 0; i < words[p].size() - 1; i++)
			for (int j = i + 1; j < words[p].size(); j++)
				for (int k = i; k < j; k++)
					for (int x = 0; x < numNonTerminals; x++)
						for (int y = 0; y < numNonTerminals; y++)
							for (int z = 0; z < numNonTerminals; z++)
								count[x][y][z] += alpha[i][j][x] * grammar.nonTerminalProbability[x][y][z] * beta[i][k][y] * beta[k + 1][j][z] / beta[0][words[p].size()][0];
		likelihood *= beta[0][words[p].size() - 1][0];
	}
	void CYK(int p){
		memset(table, 0, sizeof(table));
		for (int j = 0; j < words[p].size(); j++){
			for (int i = 0; i < numNonTerminals; i++)
				table[j][j][i] = grammar.terminalProbability[i][grammar.indexTerminal[words[p][j]]];
			for (int i = j - 1; i >= 0; i--)
				for (int k = i; k <= j - 1; k++)
					for (int x = 0; x < numNonTerminals; x++)
						for (int y = 0; y < numNonTerminals; y++)
							for (int z = 0; z < numNonTerminals; z++){
								double tmp = grammar.nonTerminalProbability[x][y][z] * table[i][k][x] * table[k + 1][j][x];
								if (tmp > table[i][j][x]){
									table[i][j][x] = tmp;
									pre[i][j][x].i = i;
									pre[i][j][x].j = j;
									pre[i][j][x].x = x;
								}
						}
		}
	}

	// void buildParseTree(ParseTreeNode* node, int i, int j, string nonTerminal){
	// 	if (i == j) node->insertTerminal(words[i]);
	// 	else{
	// 		node->insertChildren(table[i][j][nonTerminal].preNonTerminalA, table[i][j][nonTerminal].preNonTerminalB);
	// 		buildParseTree(node->left, i, table[i][j][nonTerminal].preK, table[i][j][nonTerminal].preNonTerminalA);
	// 		buildParseTree(node->right, table[i][j][nonTerminal].preK + 1, j, table[i][j][nonTerminal].preNonTerminalB);
	// 	}
	// }
	// ParseTree* buildParseTree(int length){
	// 	ParseTree* parseTree = new ParseTree("S");
	// 	buildParseTree(parseTree->root, 0, length - 1, "S");
	// 	return parseTree;
	// }
};

#endif