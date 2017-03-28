#ifndef SENTENCE_HPP
#define SENTENCE_HPP

#include <iostream>
#include <fstream>

#include <cmath>
#include <cstdio>
#include <cstring>

#include "grammar.hpp"
#include "parsetree.hpp"
using namespace std;

const int MAXN = 100;

class Table{
public:
	double probability;
	int preK;
	string preNonTerminalA, preNonTerminalB;
};

// class IO{
// public:
// 	double alpha, beta;
// };	

class Sentence{
public:
	CNF cnf;
	ParseTree *parseTree;
	vector<string> words;
	map<string, Table> table[MAXN][MAXN];
	map<string, double> alpha[MAXN][MAXN], beta[MAXN][MAXN];

	Sentence(ifstream &treeIn, CNF &cnf){
		parseTree = new ParseTree;
		parseTree->load(treeIn);
		words.clear();
		parseTree->preOrderTraversal(parseTree->root, words);
		this->cnf = cnf;
		this->cnf.randomizeProbability();
		this->cnf.createMap();
	}
	
	void emTraining(){
		calculateInsideOutside();
		expection();
		maximization();
	}
	
	void expection(){
		
		vector<BinaryRule>::iterator iter;
		for (iter = cnf.binaryRules.begin(); iter != cnf.binaryRules.end(); iter++){
			double tmp = 0;
			for (int i = 0; i < words.size() - 1; i++)
				for (int j = i + 1; j < words.size(); j++)
					for (int k = i; k < j; k++)
						tmp += alpha[i][j][iter->nonTerminalParent] * iter->probability * beta[i][k][iter->nonTerminalLeft] * beta[k + 1][j][iter->nonTerminalRight] / beta[0][words.size() - 1]["S"];
			iter->count = tmp;
		}
	}
	void maximization(){
		vector<BinaryRule>::iterator iter;
		for (iter = cnf.binaryRules.begin(); iter != cnf.binaryRules.end(); iter++)
			iter->probability = iter->count;
		cnf.generalizeProbability();
	}

	void calculateInsideOutside(){
		for (int i = 0; i < MAXN; i++)
			for (int j = 0; j < MAXN; j++){
				alpha[i][j].clear();
				beta[i][j].clear();
			}
		for (int j = 0; j < words.size(); j++){
			beta[j][j][cnf.indexTerminal[words[j]]->nonTerminal] = cnf.indexTerminal[words[j]]->probability;
			for (int i = j - 1; i >= 0; i--)
				for (int k = i; k <= j - 1; k++){
					map<string, double>::iterator iterA, iterB;
					for (iterA = beta[i][k].begin(); iterA != beta[i][k].end(); iterA++)
						for (iterB = beta[k + 1][j].begin(); iterB != beta[k + 1][j].end(); iterB++){
							string nonTerminalA = iterA->first, nonTerminalB = iterB->first;
							multimap<pair<string, string>, BinaryRule*>::iterator iterC;
							pair<multimap<pair<string, string>, BinaryRule*>::iterator, multimap<pair<string, string>, BinaryRule*>::iterator> ret = cnf.indexNonTerminalChildren.equal_range(pair<string, string>(nonTerminalA, nonTerminalB));
							for (iterC = ret.first; iterC != ret.second; iterC++){
								double tmp = iterC->second->probability * iterA->second * iterB->second;
								beta[i][j][iterC->second->nonTerminalParent] += tmp;
							}
						}
				}
		}
		vector<BinaryRule>::iterator iter;
		for (iter = cnf.binaryRules.begin(); iter != cnf.binaryRules.end(); iter++)
			if (iter->nonTerminalParent == "S") alpha[0][words.size() - 1][iter->nonTerminalParent] = 1;
			//else alpha[0][words.size() - 1][iter->nonTerminalParent] = 0;
		for (int j = 0; j < words.size(); j++){
			for (int i = 0; i <= j; i++){
				if (i > 0)
					for (int k = 0; k < i; k++){
						map<string, double>::iterator iterA, iterB;
						for (iterA = alpha[k][j].begin(); iterA != alpha[k][j].end(); iterA++){
					//		cout<<"~~~~~~~~~~~"<<k<< " "<<j<<"   "<<iterA->first<<endl;
							for (iterB = beta[k][i - 1].begin(); iterB != beta[k][i - 1].end(); iterB++){
								string nonTerminalA = iterA->first, nonTerminalB = iterB->first;
								multimap<pair<string, string>, BinaryRule*>::iterator iterC;
								pair<multimap<pair<string, string>, BinaryRule*>::iterator, multimap<pair<string, string>, BinaryRule*>::iterator> ret = cnf.indexNonTerminalParentAndLeft.equal_range(pair<string, string>(nonTerminalA, nonTerminalB));
								for (iterC = ret.first; iterC != ret.second; iterC++){
									double tmp = iterC->second->probability * iterA->second * iterB->second;	
									alpha[i][j][iterC->second->nonTerminalRight] += tmp;
						//		cout<<"!!!!"<<k<<" "<<j<<" "<<iterC->second->probability<<" "<<iterA->first<<" "<<iterB->first<<" "<<iterA->second<<" "<<iterB->second<<" "<<words.size()-1<<"   "<<tmp<<endl;
								}
							}}
					}
				if (j < words.size() - 1)
					for (int k = j + 1; k < words.size(); k++){
						map<string, double>::iterator iterA, iterB;
						for (iterA = alpha[i][k].begin(); iterA != alpha[i][k].end(); iterA++)
							for (iterB = beta[j + 1][k].begin(); iterB != beta[j + 1][k].end(); iterB++){
								string nonTerminalA = iterA->first, nonTerminalB = iterB->first;
								multimap<pair<string, string>, BinaryRule*>::iterator iterC;
								pair<multimap<pair<string, string>, BinaryRule*>::iterator, multimap<pair<string, string>, BinaryRule*>::iterator> ret = cnf.indexNonTerminalParentAndRight.equal_range(pair<string, string>(nonTerminalA, nonTerminalB));
								for (iterC = ret.first; iterC != ret.second; iterC++){
									double tmp = iterC->second->probability * iterA->second * iterB->second;	
									alpha[i][j][iterC->second->nonTerminalLeft] += tmp;
								}
							}
					}
			}
		}
		// for (int i = 0; i < MAXN; i++)
		// 	for (int j = 0; j < MAXN; j++){
		// 		map<string, double>::iterator iter;
		// 		for (iter = alpha[i][j].begin(); iter != alpha[i][j].end(); iter++)
		// 			cout << i << " " << j << " " << iter->first << " " << iter->second << endl;
			// }
	}
	void CYK(ofstream &lalala){		
		for (int i = 0; i < MAXN; i++)
			for (int j = 0; j < MAXN; j++)
				table[i][j].clear();
		for (int j = 0; j < words.size(); j++){
			table[j][j][cnf.indexTerminal[words[j]]->nonTerminal].probability = cnf.indexTerminal[words[j]]->probability;
			for (int i = j - 1; i >= 0; i--)
				for (int k = i; k <= j - 1; k++){
					map<string, Table>::iterator iterA, iterB;
					for (iterA = table[i][k].begin(); iterA != table[i][k].end(); iterA++)
						for (iterB = table[k + 1][j].begin(); iterB != table[k + 1][j].end(); iterB++){
							string nonTerminalA = iterA->first, nonTerminalB = iterB->first;
							multimap<pair<string, string>, BinaryRule*>::iterator iterC;
							pair<multimap<pair<string, string>, BinaryRule*>::iterator, multimap<pair<string, string>, BinaryRule*>::iterator> ret = cnf.indexNonTerminalChildren.equal_range(pair<string, string>(nonTerminalA, nonTerminalB));
							for (iterC = ret.first; iterC != ret.second; iterC++){
								double tmp = iterC->second->probability * iterA->second.probability * iterB->second.probability;
								if ((table[i][j].find(iterC->second->nonTerminalParent) == table[i][j].end()) || (tmp > table[i][j][iterC->second->nonTerminalParent].probability)){
									table[i][j][iterC->second->nonTerminalParent].probability = tmp;
									table[i][j][iterC->second->nonTerminalParent].preK = k;
									table[i][j][iterC->second->nonTerminalParent].preNonTerminalA = iterA->first;
									table[i][j][iterC->second->nonTerminalParent].preNonTerminalB = iterB->first;
								}
							}
						}
				}
		}
		// for (int i = 0; i < MAXN; i++)
		// 	for (int j = 0; j < MAXN; j++){
		// 		map<string, Table>::iterator iter;
		// 		for (iter = table[i][j].begin(); iter != table[i][j].end(); iter++)
		// 			cout << i << " " << j << " " << iter->first << " " << iter->second.probability << endl;
		// 	}
		// cout << table[0][words.size()-1]["S"].probability << endl;
		//cout<<cnf.binaryRules[0].probability<<endl;
		ParseTree* parseTree = buildParseTree(words.size());
		parseTree->reassignIndex();
		parseTree->saveToEvalbFormat(lalala);
	}

	void buildParseTree(ParseTreeNode* node, int i, int j, string nonTerminal){
		if (i == j) node->insertTerminal(words[i]);
		else{
			node->insertChildren(table[i][j][nonTerminal].preNonTerminalA, table[i][j][nonTerminal].preNonTerminalB);
			buildParseTree(node->left, i, table[i][j][nonTerminal].preK, table[i][j][nonTerminal].preNonTerminalA);
			buildParseTree(node->right, table[i][j][nonTerminal].preK + 1, j, table[i][j][nonTerminal].preNonTerminalB);
		}
	}
	ParseTree* buildParseTree(int length){
		ParseTree* parseTree = new ParseTree("S");
		buildParseTree(parseTree->root, 0, length - 1, "S");
		return parseTree;
	}
};

#endif