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
	LogDouble table[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS], alpha[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS], beta[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS], count[MAX_NON_TERMINALS][MAX_NON_TERMINALS][MAX_NON_TERMINALS], countNon[MAX_NON_TERMINALS][MAX_TERMINALS];
	Previous record[MAX_WORDS][MAX_WORDS][MAX_NON_TERMINALS];
	int numSentences;
	LogDouble likelihood;
	EM(){;}
	void setNumSentences(int numSentences){
		this->numSentences = numSentences;
	}
	void initializeTrainingGrammar(int numNonTerminals){
		grammar.numTerminals = grammar.indexTerminal.size();
		grammar.numNonTerminals = numNonTerminals;
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
		for (int i = 0; i < 0; i++){
			cout << i << endl;
			expection();
			maximization();
			printf("      %.50Lf\n", likelihood.get());
			// cout << "   "<<likelihood.get() << endl;
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
				count[i][j][k].init();
	for (int i = 0; i < grammar.numNonTerminals; i++)
		for (int j = 0; j < grammar.numTerminals; j++)
				countNon[i][j].init();
		likelihood = 0;
		for (int i = 0; i < numSentences - 2; i++){
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
		//  cout<<"@@"<<tmp<<endl;
		//  ld xxx=1;
			for (int y = 0; y < grammar.numNonTerminals; y++)
				for (int z = 0; z < grammar.numNonTerminals; z++){
					// cout<<x<<" "<<y<<" "<<z<<"        "<<count[x][y][z].get()<<"    "<<tmp.get()<<"   "<<count[x][y][z].get() - tmp.get()<<endl;
					
					grammar.LogNonTerminalProbability[x][y][z] = count[x][y][z] - tmp;
				//	xxx=LogarithmOfAddition(xxx,grammar.nonTerminalProbability[x][y][z]);

					}
			for (int y = 0; y < grammar.numTerminals; y++){
			// 		// cout<<"    "<<grammar.terminalProbability[x][y]<<"    "<<tmp<<"   "<<grammar.terminalProbability[x][y]-tmp<<endl;
					grammar.LogTerminalProbability[x][y] = countNon[x][y] - tmp;
			// 	//	xxx=LogarithmOfAddition(xxx,grammar.terminalProbability[x][y]);
					}
					//cout<<"            "<<exp(xxx)<<"  "<<exp(tmp)<<endl;
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
							for (int z = 0; z < grammar.numNonTerminals; z++){
								beta[i][j][x] = LogarithmOfAddition(beta[i][j][x], grammar.LogNonTerminalProbability[x][y][z] + beta[i][k][y] + beta[k + 1][j][z]);
								//  cout<<i<<" "<<j<<"     "<<"("<<i<<","<<k<<") ("<<k+1<<","<<j<<")    "<<beta[i][j][x].get()<<"    "<<x<<" "<<y<<" "<<z<<" "<<"     "<<grammar.LogNonTerminalProbability[x][y][z].get() <<" "<< beta[i][k][y].get() <<" "<< beta[k + 1][j][z].get()<<endl;
				} 	
		alpha[0][words[p].size() - 1][0] = 0;
		for (int len = words[p].size() - 1; len >= 1; len--)
			for (int i = 0; i < words[p].size(); i++){
				int j = i + len - 1;

		// for (int j = words[p].size() - 1; j >= 0; j--)
		// 	for (int i = 0; i <= j; i++){
				for (int k = 0; k < i; k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++){
								alpha[i][j][z] = LogarithmOfAddition(alpha[i][j][z], grammar.LogNonTerminalProbability[x][y][z] + alpha[k][j][x] + beta[k][i - 1][y]);
			// cout<<words[p].size()<<"alpha@@@@"<<i<<" "<<j<<" "<<k<<"  "<<x<<" "<<y<<" "<<z<<" "<<alpha[i][j][y].get()<<" "<<grammar.LogNonTerminalProbability[x][y][z].get() <<" "<< alpha[k][j][x].get() << " "<< beta[k][i - 1][z].get()<<endl;
				
							}
				for (int k = j + 1; k < words[p].size(); k++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++)
								alpha[i][j][y] = LogarithmOfAddition(alpha[i][j][y], grammar.LogNonTerminalProbability[x][y][z] + alpha[i][k][x] + beta[j + 1][k][z]);
			}
		for (int i = 0; i < words[p].size() - 1; i++)
			for (int j = i + 1; j < words[p].size(); j++)
				for (int k = i; k < j; k++){
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numNonTerminals; y++)
							for (int z = 0; z < grammar.numNonTerminals; z++)
	//   cout<<i<<" "<<j<<" "<<k<<" %%%% "<<x<<" "<<y<<" " <<z<<" "<<alpha[i][j][x] << " "<<grammar.nonTerminalProbability[x][y][z] <<" "<< beta[i][k][y] <<" "<< beta[k + 1][j][z] <<" "<< beta[0][words[p].size() - 1][0]<<":"<<count[x][y][z]<<endl;
								count[x][y][z] = LogarithmOfAddition(count[x][y][z], alpha[i][j][x] + grammar.LogNonTerminalProbability[x][y][z] + beta[i][k][y] + beta[k + 1][j][z] - beta[0][words[p].size() - 1][0]);
								// if (count[x][y][z].get() > 0) cout << count[x][y][z].get()<<"!!!!!"<<endl;
								// cout<<(alpha[i][j][x] + grammar.nonTerminalProbability[x][y][z] + beta[i][k][y] + beta[k + 1][j][z] - beta[0][words[p].size() - 1][0]).get()<<"  "<<count[x][y][z].get()<<endl;
							}
				for (int i = 0; i < words[p].size() - 1; i++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numTerminals; y++)
							countNon[x][y] = LogarithmOfAddition(countNon[x][y], alpha[i][i][x] + grammar.LogTerminalProbability[x][y] + beta[i][i][x] - beta[0][words[p].size() - 1][0]);
		//likelihood = LogarithmOfAddition(likelihood, beta[0][words[p].size() - 1][0]);
						// 	for (int x = 0; x < words[p].size(); x++)
						// for (int y = x + 1; y < words[p].size(); y++)
						// 	for (int z = 0; z < grammar.numNonTerminals; z++)
						// 	cout<<x<<" "<<y<<" "<<z<<"                        ^^&&"<<alpha[x][y][z]<<" "<<beta[x][y][z]<<endl;
		// 			for (int i = 0; i < words[p].size() - 1; i++)
		// 				for (int x = 0; x < grammar.numTerminals; x++)
		// 					countNon[words[p][i]][x] = LogarithmOfAddition(countNon[words[p][i]][x], grammar.LogTerminalProbability[countNon[words[p][i]][x]][x]);
		// for (int j = 0; j < words[p].size(); j++)
		// 	for (int i = 0; i < grammar.numNonTerminals; i++)
		// 		countNon
		// 		beta[j][j][i] = grammar.LogTerminalProbability[i][grammar.indexTerminal[words[p][j]]];	


		// for (int i = 0; i < words[p].size() - 1; i++)
		// 	for (int x = 0; x < grammar.numNonTerminals; x++)
		// 		for (int y = 0; y < grammar.numTerminals; y++)
		// 			countNon[x][y] = LogarithmOfAddition(countNon[x][y], grammar.LogTerminalProbability[i][])
		likelihood = likelihood + beta[0][words[p].size() - 1][0];
					// for (int x = 0; x < grammar.numNonTerminals; x++)
					// 	for (int y = 0; y < grammar.numNonTerminals; y++)
					// 		for (int z = 0; z < grammar.numNonTerminals; z++)
							// cout<<x<<" "<<y<<" "<<z<<" "<<count[x][y][z].get()<<endl;
						
		// cout<<"                       ^^"<<beta[0][words[p].size() - 1][0].get()<<endl;
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
	// 	}
}

	void buildParseTree(int p, ParseTreeNode* node, int i, int j, int x){
		// cout<<p<<"         "<<i<<" "<<j<<" "<<x<<endl;
		// cout<<"            "<<record[i][j][x].i<<" " <<record[i][j][x].j<<" "<<record[i][j][x].k<< " "<<record[i][j][x].x<<" "<<record[i][j][x].y<<" "<<record[i][j][x].z<<endl;
		if (i == j) node->insertTerminal(words[p][i]);
		else{
			node->insertChildren("S", "S");
			buildParseTree(p, node->left, i, record[i][j][x].k, record[i][j][x].y);
			buildParseTree(p, node->right, record[i][j][x].k + 1, j, record[i][j][x].z);
		}
	}
	ParseTree* buildParseTree(int p){
		cout<<p<<"!!!!"<<endl;
		ParseTree *parseTree = new ParseTree("S");
		buildParseTree(p, parseTree->root, 0, words[p].size() - 1, 0);
		return parseTree;
	}

};

#endif