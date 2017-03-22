#ifndef SENTENCE_HPP
#define SENTENCE_HPP

#include <iostream>
#include <fstream>

#include <cmath>
#include <cstring>

#include "grammar.hpp"
#include "parsetree.hpp"
using namespace std;

const int MAXN = 100;

class Sentence{
public:
	CNF cnf;
	ParseTree *parseTree;
	vector<string> words;
	map<string, double> table[MAXN][MAXN];

	Sentence(ifstream &treeIn, ifstream &grammarIn){
		parseTree = new ParseTree;
		parseTree->load(treeIn);
		words.clear();
		parseTree->preOrderTraversal(parseTree->root, words);
		cnf.read(grammarIn);
	}
	
	void expection(){;}
	void maximization(){;}

	void calculateInside(){;}
	void calculateOutside(){;}
	void CYK(){
		for (int i = 0; i < MAXN; i++)
			for (int j = 0; j < MAXN; j++)
				table[i][j].clear();
		//memset(table, 0, sizeof(table));
		for (int j = 0; j < words.size(); j++){
			table[j][j][cnf.indexTerminal[words[j]].nonTerminal] = cnf.indexTerminal[words[j]].probability;
			for (int i = j - 1; i >= 0; i--)
				for (int k = i; k <= j - 1; k++){
					map<string, double>::iterator iterA, iterB;
					for (iterA = table[i][k].begin(); iterA != table[i][k].end(); iterA++)
						for (iterB = table[k + 1][j].begin(); iterB != table[k + 1][j].end(); iterB++){
							string nonTerminalA = iterA->first, nonTerminalB = iterB->first;
							multimap<pair<string, string>, BinaryRule>::iterator iterC;
							pair<multimap<pair<string, string>, BinaryRule>::iterator, multimap<pair<string, string>, BinaryRule>::iterator> ret = cnf.indexNonTerminalChildren.equal_range(pair<string, string>(nonTerminalA, nonTerminalB));
							for (iterC = ret.first; iterC != ret.second; iterC++){
								double tmp = iterC->second.probability * iterA->second * iterB->second;
								if (table[i][j].find(iterC->second.nonTerminalParent) == table[i][j].end())
									table[i][j][iterC->second.nonTerminalParent] = tmp;
								else if (tmp > table[i][j][iterC->second.nonTerminalParent])
									table[i][j][iterC->second.nonTerminalParent] = tmp;
							}
						}
				}
		}
		for (int i = 0; i < MAXN; i++)
			for (int j = 0; j < MAXN; j++){
				map<string, double>::iterator iter;
				for (iter = table[i][j].begin(); iter != table[i][j].end(); iter++)
					cout << i << " " << j << " " << iter->first << " " << iter->second << endl;
			}
		cout << table[0][words.size()-1]["S"] << endl;
	}
};

#endif