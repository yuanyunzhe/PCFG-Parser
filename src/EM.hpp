#ifndef SENTENCE_HPP
#define SENTENCE_HPP

#include <iostream>
#include <fstream>

#include <cmath>
#include <cstdio>
#include <cstring>

#include "Grammar.hpp"
#include "Parsetree.hpp"
#include "Math.hpp"
using namespace std;

#define MAX_WORDS 50
#define MAX_SENTENCES 50


class Previous{
public:
	int i, j, k, x, y, z;
};
class EM{
public:
	TrainingGrammar grammar;
	CNF cnf;
	ParseTree *parseTree;
	vector<string> words[MAX_SENTENCES];
	double table[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS], alpha[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS], beta[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS], count[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS];
	Previous record[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS];
	int numSentences;
	double likelihood;
	EM(){;}
	void setNumSentences(int numSentences){
		this->numSentences = numSentences;
	}
	void initializeTrainingGrammar(int numNonTerminals){
		grammar.numTerminals = grammar.indexTerminal.size();
		grammar.numNonTerminals = numNonTerminals;
		srand((unsigned)time(NULL));
		for (int i = 0; i < numNonTerminals; i++){
			double sum = 0;
			for (int j = 0; j < numNonTerminals; j++)
					for (int k = 0; k < numNonTerminals; k++){
						grammar.nonTerminalProbability[i][j][k] = rand() / double(RAND_MAX);
						sum += grammar.nonTerminalProbability[i][j][k];
					}
			for (int j = 0; j < grammar.numNonTerminals; j++)
				for (int k = 0; k < grammar.numNonTerminals; k++)
					grammar.nonTerminalProbability[i][j][k] = log(grammar.nonTerminalProbability[i][j][k] / sum);
		}
		for (int i = 0; i < grammar.numNonTerminals; i++){
			double sum = 0;
			for (int j = 0; j < grammar.numTerminals; j++){
				grammar.terminalProbability[i][j] = rand() / double(RAND_MAX);
				sum += grammar.terminalProbability[i][j];
			}
			for (int j = 0; j < grammar.numTerminals; j++)
				grammar.terminalProbability[i][j] = log(grammar.terminalProbability[i][j] / sum);
		}	
	}

	void readParseTree(ifstream &treeIn){
		for (int i = 0; i < numSentences; i++){
			parseTree = new ParseTree;
			parseTree->load(treeIn);
			parseTree->preOrderTraversal(parseTree->root, words[i]);
			for (int j = 0; j < words[i].size(); j++)
				if (grammar.indexTerminal.find(words[i][j]) == grammar.indexTerminal.end())
					grammar.indexTerminal[words[i][j]] = grammar.indexTerminal.size();
		}
	}

	void training(){
		cout << grammar.numNonTerminals << " "<<grammar.numTerminals << endl;
		for (int i = 0; i < 10; i++){
			cout << i << endl;
			expection();
			maximization();
		}
	}

	void predicting(ofstream &evalbOut){
		for (int i = 0; i < numSentences; i++){
			CYK(i);
			parseTree = buildParseTree(i);
			parseTree->saveToEvalbFormat(evalbOut);
		}
	}

	void expection(){
	for (int i = 0; i < grammar.numNonTerminals; i++)
		for (int j = 0; j < grammar.numNonTerminals; j++)
			for (int k = 0; k < grammar.numNonTerminals; k++)
				count[i][j][k] = 1;
		likelihood = 1;
		for (int i = 0; i < numSentences; i++){
			calculateInsideOutside(i);
			cout << "   "<<likelihood << endl;
		}
	}
	void maximization(){
		for (int x = 0; x < grammar.numNonTerminals; x++){
			double tmp = 0;
			for (int y = 0; y < grammar.numNonTerminals; y++)
				for (int z = 0; z < grammar.numNonTerminals; z++)
					tmp += count[x][y][z];
			for (int y = 0; y < grammar.numNonTerminals; y++)
				for (int z = 0; z < grammar.numNonTerminals; z++)
					count[x][y][z] /= tmp;
		}
	}

	void calculateInsideOutside(int p){
		for (int i = 0; i < grammar.numNonTerminals; i++)
			for (int j = 0; j < grammar.numNonTerminals; j++)
				for (int k = 0; k < grammar.numNonTerminals; k++){
					alpha[i][j][k] = 1;
					beta[i][j][k] = 1;
				}
		for (int j = 0; j < words[p].size(); j++){
			for (int i = 0; i < grammar.numNonTerminals; i++)
				beta[j][j][i] = grammar.terminalProbability[i][grammar.indexTerminal[words[p][j]]];
			for (int i = j - 1; i >= 0; i--)
				for (int k = i; k <= j - 1; k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++){
								beta[i][j][x] = LogarithmOfAddition(beta[i][j][x], grammar.nonTerminalProbability[x][y][z] + beta[i][k][y] + beta[k + 1][j][z]);
							}
		}
		alpha[0][words[p].size() - 1][0] = 0;
		for (int j = 0; j < words[p].size(); j++)
			for (int i = 0; i <= j; i++){
				for (int k = 0; k < i; k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++)
								alpha[i][j][y] = LogarithmOfAddition(alpha[i][j][y], grammar.nonTerminalProbability[x][y][z] + alpha[i][k][x] + beta[k + 1][j][z]);
				for (int k = j + 1; k < words[p].size(); k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++)
								alpha[i][j][z] = LogarithmOfAddition(alpha[i][j][z], grammar.nonTerminalProbability[x][y][z] + alpha[k][j][x] + beta[k][i - 1][y]);
			}
		for (int i = 0; i < words[p].size() - 1; i++)
			for (int j = i + 1; j < words[p].size(); j++)
				for (int k = i; k < j; k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++)
								count[x][y][z] = LogarithmOfAddition(count[x][y][z], alpha[i][j][x] + grammar.nonTerminalProbability[x][y][z] + beta[i][k][y] + beta[k + 1][j][z] - beta[0][words[p].size()][0]);
		likelihood += beta[0][words[p].size() - 1][0];
	}
	void CYK(int p){
	for (int i = 0; i < grammar.numNonTerminals; i++)
		for (int j = 0; j < grammar.numNonTerminals; j++)
			for (int k = 0; k < grammar.numNonTerminals; k++)
				table[i][j][k] = -10000000;
		for (int j = 0; j < words[p].size(); j++){
			for (int i = 0; i < grammar.numNonTerminals; i++)
				table[j][j][i] = grammar.terminalProbability[i][grammar.indexTerminal[words[p][j]]];
			for (int i = j - 1; i >= 0; i--)
				for (int k = i; k <= j - 1; k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++){
								double tmp = grammar.nonTerminalProbability[x][y][z] + table[i][k][y] + table[k + 1][j][z];
								if (tmp > table[i][j][x]){
									table[i][j][x] = tmp;
									record[i][j][x].i = i;
									record[i][j][x].j = j;
									record[i][j][x].k = k;
									record[i][j][x].x = x;
									record[i][j][x].y = y;
									record[i][j][x].z = z;
								}
						}
		}
}

	void buildParseTree(int p, ParseTreeNode* node, int i, int j, int x){
		if (i == j) node->insertTerminal(words[p][i]);
		else{
			node->insertChildren("S", "S");
			buildParseTree(p, node->left, i, record[i][j][x].k, record[i][j][x].y);
			buildParseTree(p, node->right, record[i][j][x].k + 1, j, record[i][j][x].z);
		}
	}
	ParseTree* buildParseTree(int p){
		ParseTree *parseTree = new ParseTree("S");
		buildParseTree(p, parseTree->root, 0, words[p].size() - 1, 0);
		return parseTree;
	}

};

#endif