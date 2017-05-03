#ifndef EM_HPP
#define EM_HPP

#include <iostream>
#include <fstream>

#include <cmath>
#include <cstdio>
#include <cstring>

#include "Grammar.hpp"
#include "Parsetree.hpp"
#include "Math.hpp"
#include "Structure.hpp"
using namespace std;

#define MAX_WORDS 200
#define MAX_SENTENCES 500

class EM{
public:
	TrainingGrammar grammar;
	CNF cnf;
	ParseTree *parseTree;
	vector<string> words[MAX_SENTENCES];
	LogDouble ***table, ***alpha, ***beta, ***count, **countNon;
	Previous ***record;
	int numSentencesTraining, numSentencesTest;
	LogDouble likelihood;
	EM(){;}
	void setNumSentences(int numSentencesTraining, int numSentencesTest){
		this->numSentencesTraining = numSentencesTraining;
		this->numSentencesTest = numSentencesTest;
	}
	void initializeTrainingGrammar(int numNonTerminals){
		grammar.numTerminals = grammar.indexTerminal.size();
		grammar.numNonTerminals = numNonTerminals;
		malloc(&table, MAX_WORDS, MAX_WORDS, grammar.numNonTerminals);
		malloc(&alpha, MAX_WORDS, MAX_WORDS, grammar.numNonTerminals);
		malloc(&beta, MAX_WORDS, MAX_WORDS, grammar.numNonTerminals);
		malloc(&count, grammar.numNonTerminals, grammar.numNonTerminals, grammar.numNonTerminals);
		malloc(&countNon,grammar.numNonTerminals, grammar.numTerminals);
		malloc(&record, MAX_WORDS, MAX_WORDS, grammar.numNonTerminals);

		srand((unsigned)time(NULL));
		for (int i = 0; i < grammar.numNonTerminals; i++){
			ld sum = 0;
			for (int j = 0; j < grammar.numNonTerminals; j++)
					for (int k = 0; k < grammar.numNonTerminals; k++){
						grammar.nonTerminalProbability[i][j][k] = rand() / ld(RAND_MAX);
						sum += grammar.nonTerminalProbability[i][j][k];
					}
			for (int j = 0; j < grammar.numTerminals; j++){
				grammar.terminalProbability[i][j] = rand() / ld(RAND_MAX);
				sum += grammar.terminalProbability[i][j];
			}
			for (int j = 0; j < grammar.numNonTerminals; j++)
				for (int k = 0; k < grammar.numNonTerminals; k++)
					grammar.LogNonTerminalProbability[i][j][k] = log(grammar.nonTerminalProbability[i][j][k] / sum);
			for (int j = 0; j < grammar.numTerminals; j++)
				grammar.LogTerminalProbability[i][j] = log(grammar.terminalProbability[i][j] / sum);
		}
	}

	void readParseTree(ifstream &treeIn, int num){
		for (int i = 0; i < num; i++){
			words[i].clear();
			parseTree = new ParseTree;
			parseTree->load(treeIn);
			parseTree->preOrderTraversal(parseTree->root, words[i]);
			for (int j = 0; j < words[i].size(); j++)
				if (grammar.indexTerminal.find(words[i][j]) == grammar.indexTerminal.end())
					grammar.indexTerminal[words[i][j]] = grammar.indexTerminal.size();
		}
	}

	void training(int times){
		for (int i = 0; i < times; i++){
			cout << i << endl;
			expection();
			maximization();
			printf("      %.50Lf\n", likelihood.get());
			if (abs(beta[0][words[i].size() - 1][0].get()) < 1e-10) break;
		}
	}

	void predicting(ofstream &evalbOut){
		for (int i = 0; i < numSentencesTest; i++){
			CYK(i);
			parseTree = buildParseTree(i);
			parseTree->saveToEvalbFormat(evalbOut);
		}
	}

	void expection(){
	for (int i = 0; i < grammar.numNonTerminals; i++)
		for (int j = 0; j < grammar.numNonTerminals; j++)
			for (int k = 0; k < grammar.numNonTerminals; k++)
				count[i][j][k].init();
	for (int i = 0; i < grammar.numNonTerminals; i++)
		for (int j = 0; j < grammar.numTerminals; j++)
				countNon[i][j].init();
		likelihood = 0;
		for (int i = 0; i < numSentencesTraining - 2; i++){
			calculateInsideOutside(i);
			
		}
	}
	void maximization(){
		for (int x = 0; x < grammar.numNonTerminals; x++){
			LogDouble tmp;
			for (int y = 0; y < grammar.numNonTerminals; y++)
				for (int z = 0; z < grammar.numNonTerminals; z++)
					tmp = LogarithmOfAddition(tmp, count[x][y][z]);
			for (int y = 0; y < grammar.numTerminals; y++)
					tmp = LogarithmOfAddition(tmp, countNon[x][y]);
			for (int y = 0; y < grammar.numNonTerminals; y++)
				for (int z = 0; z < grammar.numNonTerminals; z++)
					grammar.LogNonTerminalProbability[x][y][z] = count[x][y][z] - tmp;
			for (int y = 0; y < grammar.numTerminals; y++)
				grammar.LogTerminalProbability[x][y] = countNon[x][y] - tmp;
		}
	}

	void calculateInsideOutside(int p){
		for (int i = 0; i < words[p].size(); i++)
			for (int j = 0; j < words[p].size(); j++)
				for (int k = 0; k < grammar.numNonTerminals; k++){
					alpha[i][j][k] = LOG_ZERO;
					beta[i][j][k] = LOG_ZERO;
				}
		for (int j = 0; j < words[p].size(); j++)
			for (int i = 0; i < grammar.numNonTerminals; i++)
				beta[j][j][i] = grammar.LogTerminalProbability[i][grammar.indexTerminal[words[p][j]]];
		for (int j = 0; j < words[p].size(); j++)
			for (int i = j - 1; i >= 0; i--)
				for (int k = i; k <= j - 1; k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++)
								beta[i][j][x] = LogarithmOfAddition(beta[i][j][x], grammar.LogNonTerminalProbability[x][y][z] + beta[i][k][y] + beta[k + 1][j][z]);	
		alpha[0][words[p].size() - 1][0] = 0;
		for (int len = words[p].size() - 1; len >= 1; len--)
			for (int i = 0; i < words[p].size(); i++){
				int j = i + len - 1;
				for (int k = 0; k < i; k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++)
								alpha[i][j][z] = LogarithmOfAddition(alpha[i][j][z], grammar.LogNonTerminalProbability[x][y][z] + alpha[k][j][x] + beta[k][i - 1][y]);
				for (int k = j + 1; k < words[p].size(); k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++)
								alpha[i][j][y] = LogarithmOfAddition(alpha[i][j][y], grammar.LogNonTerminalProbability[x][y][z] + alpha[i][k][x] + beta[j + 1][k][z]);
			}
		for (int i = 0; i < words[p].size() - 1; i++)
			for (int j = i + 1; j < words[p].size(); j++)
				for (int k = i; k < j; k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++)
								count[x][y][z] = LogarithmOfAddition(count[x][y][z], alpha[i][j][x] + grammar.LogNonTerminalProbability[x][y][z] + beta[i][k][y] + beta[k + 1][j][z] - beta[0][words[p].size() - 1][0]);
		for (int i = 0; i < words[p].size() - 1; i++)
			for (int x = 0; x < grammar.numNonTerminals; x++)
				for (int y = 0; y < grammar.numTerminals; y++)
					countNon[x][y] = LogarithmOfAddition(countNon[x][y], alpha[i][i][x] + grammar.LogTerminalProbability[x][y] + beta[i][i][x] - beta[0][words[p].size() - 1][0]);
		likelihood = likelihood + beta[0][words[p].size() - 1][0];
	}
	void CYK(int p){
		for (int i = 0; i < words[p].size(); i++)
			for (int j = 0; j < words[p].size(); j++)
				for (int k = 0; k < grammar.numNonTerminals; k++)
					table[i][j][k] = LOG_ZERO;
		for (int j = 0; j < words[p].size(); j++)
			for (int i = 0; i < grammar.numNonTerminals; i++)
				table[j][j][i] = grammar.terminalProbability[i][grammar.indexTerminal[words[p][j]]];
		for (int j = 0; j < words[p].size(); j++)
			for (int i = j - 1; i >= 0; i--)
				for (int k = i; k <= j - 1; k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++){
								LogDouble tmp = grammar.LogNonTerminalProbability[x][y][z] + table[i][k][y] + table[k + 1][j][z];
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