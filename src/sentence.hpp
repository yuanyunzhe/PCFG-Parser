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

class Table{
public:
	double probability;
	string symbol;
	Table(){probability = 0; symbol = "";}
};

class Sentence{
public:
	ParseTree *parseTree;
	CNF cnf;
	vector<string> words;
	Table table[MAXN][MAXN];
//	vector<vector<map<string, double> > > theta, alpha, beta;

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
		memset(table, 0, sizeof(table));

		for (int j = 0; j < words.size(); j++){
					cout<<"!!!"<<endl;
					cout<<cnf.indexWord[words[j]].word<<endl;
			table[j][j].symbol = cnf.indexWord[words[j]].word;
			table[j][j].probability = cnf.indexWord[words[j]].probability;
			for (int i = j - 1; i >= 0; i--)
				for (int k = i; k <= j - 1; k++){
					multimap<pair<string, string>, NonTerminalRule>::iterator iter;
					pair<string, string> nonTerminalChildren = pair<string, string>(table[i][k].symbol, table[k + 1][j].symbol);
					for (iter = cnf.indexNonTerminalChildren.begin(); iter != cnf.indexNonTerminalChildren.end(); iter++)
						if (table[i][j].probability < iter->second.probability * table[i][k].probability * table[k + 1][j].probability){
							table[i][j].probability = iter->second.probability * table[i][k].probability * table[k + 1][j].probability;
							table[i][j].symbol = iter->second.nonTerminalParent;
							cout<<i<<" "<<j<<" "<<table[i][j].probability<<endl;
						}
				}
		}
		cout << table[words.size()-1][words.size()-1].symbol << endl;
	}
};

#endif