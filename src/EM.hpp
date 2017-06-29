#ifndef EM_HPP
#define EM_HPP

#include <iostream>
#include <fstream>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "Grammar.hpp"
#include "Parsetree.hpp"
#include "Math.hpp"
#include "Structure.hpp"
#include "Parameter.hpp"

// #include <omp.h>
#include <regex>

using namespace std;

class EM{
public:
	TrainingGrammar grammar, standardGrammar;
	CNF cnf;
	ParseTree *parseTree;
	vector<string> words[MAX_SENTENCES], wordsTest[MAX_SENTENCES];
	LogDouble ***table;
	Previous ***record;
	int numSentencesTraining, numSentencesTest;
	LogDouble likelihood;
	ifstream *grammarIn, *sentenceTraining, *sentenceTest;
	ofstream *evalbOut;
	EM(ifstream &grammarIn, ifstream &sentenceTraining, ifstream &sentenceTest, ofstream &evalbOut){
		this->grammarIn = &grammarIn;
		this->sentenceTraining = &sentenceTraining;
		this->sentenceTest = &sentenceTest;
		this->evalbOut = &evalbOut;
	}
	void setNumSentences(int numSentencesTraining, int numSentencesTest){
		this->numSentencesTraining = numSentencesTraining;
		this->numSentencesTest = numSentencesTest;
	}

	void predictStandardData(){
		cnf.read(*grammarIn);
		for (int i = 0; i < cnf.binaryRules.size(); i++){
			BinaryRule rule = cnf.binaryRules[i];
			if (standardGrammar.indexNonTerminal.find(rule.nonTerminalParent) == standardGrammar.indexNonTerminal.end())
				standardGrammar.indexNonTerminal[rule.nonTerminalParent] = standardGrammar.indexNonTerminal.size() - 1;
			if (standardGrammar.indexNonTerminal.find(rule.nonTerminalLeft) == standardGrammar.indexNonTerminal.end())
				standardGrammar.indexNonTerminal[rule.nonTerminalLeft] = standardGrammar.indexNonTerminal.size() - 1;
			if (standardGrammar.indexNonTerminal.find(rule.nonTerminalRight) == standardGrammar.indexNonTerminal.end())
				standardGrammar.indexNonTerminal[rule.nonTerminalRight] = standardGrammar.indexNonTerminal.size() - 1;
		}
		for (int i = 0; i < cnf.unaryRules.size(); i++){
			UnaryRule rule = cnf.unaryRules[i];
			if (standardGrammar.indexNonTerminal.find(rule.nonTerminal) == standardGrammar.indexNonTerminal.end())
				standardGrammar.indexNonTerminal[rule.nonTerminal] = standardGrammar.indexNonTerminal.size() - 1;
			if (standardGrammar.indexTerminal.find(rule.terminal) == standardGrammar.indexTerminal.end())
				standardGrammar.indexTerminal[rule.terminal] = standardGrammar.indexTerminal.size() - 1;
		}
		standardGrammar.numTerminals = standardGrammar.indexTerminal.size();
		standardGrammar.numNonTerminals = standardGrammar.indexNonTerminal.size();
		for (int i = 0; i < standardGrammar.numNonTerminals; i++){
			for (int j = 0; j < standardGrammar.numNonTerminals; j++)
				for (int k = 0; k < standardGrammar.numNonTerminals; k++)
					standardGrammar.LogNonTerminalProbability[i][j][k] = LOG_ZERO;
			for (int j = 0; j < standardGrammar.numTerminals; j++)
				standardGrammar.LogTerminalProbability[i][j] = LOG_ZERO;
		}
		for (int i = 0; i < cnf.binaryRules.size(); i++){
			BinaryRule rule = cnf.binaryRules[i];
			standardGrammar.LogNonTerminalProbability[standardGrammar.indexNonTerminal[rule.nonTerminalParent]][standardGrammar.indexNonTerminal[rule.nonTerminalLeft]][standardGrammar.indexNonTerminal[rule.nonTerminalRight]] = log(rule.probability);
		}
		for (int i = 0; i < cnf.unaryRules.size(); i++){
			UnaryRule rule = cnf.unaryRules[i];
			standardGrammar.LogTerminalProbability[standardGrammar.indexNonTerminal[rule.nonTerminal]][standardGrammar.indexTerminal[rule.terminal]] = log(rule.probability);
		}
		predicting(EVALB_OUT_PREDICT_NAME, &standardGrammar);
	}

	void initializeTrainingGrammar(int numNonTerminals){
		grammar.numTerminals = grammar.indexTerminal.size();
		grammar.numNonTerminals = numNonTerminals;
		srand((unsigned)time(NULL));
		for (int i = 0; i < grammar.numNonTerminals; i++){
			ld sum = 0;
			for (int j = 0; j < grammar.numNonTerminals; j++)
					for (int k = 0; k < grammar.numNonTerminals; k++){
						grammar.nonTerminalProbability[i][j][k] = rand();
						sum += grammar.nonTerminalProbability[i][j][k];
					}
			for (int j = 0; j < grammar.numTerminals; j++){
				grammar.terminalProbability[i][j] = rand();
				sum += grammar.terminalProbability[i][j];
			}
			for (int j = 0; j < grammar.numNonTerminals; j++)
				for (int k = 0; k < grammar.numNonTerminals; k++)
					grammar.LogNonTerminalProbability[i][j][k] = log(grammar.nonTerminalProbability[i][j][k] / sum);
			for (int j = 0; j < grammar.numTerminals; j++)
				grammar.LogTerminalProbability[i][j] = log(grammar.terminalProbability[i][j] / sum);
		}
	}

	void readParseTree(int num, string s){
		string word;
		if (s == "training"){
			// cout<<num<<endl;
			for (int i = 0; i < num; i++){
				words[i].clear();
				while (*sentenceTraining >> word){
					if (word == ".") break;
					words[i].push_back(word);
				}
				for (int j = 0; j < words[i].size(); j++)
					if (grammar.indexTerminal.find(words[i][j]) == grammar.indexTerminal.end())
						grammar.indexTerminal[words[i][j]] = grammar.indexTerminal.size() - 1;
			}
		}
		else{
			for (int i = 0; i < num; i++){
				wordsTest[i].clear();
				while (*sentenceTest >> word){
					if (word == ".") break; 
					wordsTest[i].push_back(word);
				}
				for (int j = 0; j < wordsTest[i].size(); j++)
					if (grammar.indexTerminal.find(wordsTest[i][j]) == grammar.indexTerminal.end())
						grammar.indexTerminal[wordsTest[i][j]] = grammar.indexTerminal.size() - 1;
			}
		}
	}

	void training(int times){
		
		for (int time = 1; time <= times; time++){
			LogDouble ***count, **countNon;
			malloc(&count, grammar.numNonTerminals, grammar.numNonTerminals, grammar.numNonTerminals);
			malloc(&countNon,grammar.numNonTerminals, grammar.numTerminals);
			for (int i = 0; i < grammar.numNonTerminals; i++)
				for (int j = 0; j < grammar.numNonTerminals; j++)
					for (int k = 0; k < grammar.numNonTerminals; k++)
						count[i][j][k].init();
			for (int i = 0; i < grammar.numNonTerminals; i++)
				for (int j = 0; j < grammar.numTerminals; j++)
						countNon[i][j].init();
			likelihood = 0;

			// #pragma omp parallel for
			for (int sen = 0; sen < numSentencesTraining; sen++){
							// cout<<"       "<<sen<<endl;
				LogDouble ***alpha, ***beta;
				malloc(&alpha, MAX_WORDS, MAX_WORDS, grammar.numNonTerminals);
				malloc(&beta, MAX_WORDS, MAX_WORDS, grammar.numNonTerminals);

				for (int i = 0; i < words[sen].size(); i++)
					for (int j = 0; j < words[sen].size(); j++)
						for (int k = 0; k < grammar.numNonTerminals; k++){
							alpha[i][j][k] = LOG_ZERO;
							beta[i][j][k] = LOG_ZERO;
						}
				for (int j = 0; j < words[sen].size(); j++)
					for (int i = 0; i < grammar.numNonTerminals; i++)
						beta[j][j][i] = grammar.LogTerminalProbability[i][grammar.indexTerminal[words[sen][j]]];
				for (int j = 0; j < words[sen].size(); j++)
					for (int i = j - 1; i >= 0; i--)
						for (int k = i; k <= j - 1; k++)
							for (int x = 0; x < grammar.numNonTerminals; x++)
								for (int y = 0; y < grammar.numNonTerminals; y++)
									for (int z = 0; z < grammar.numNonTerminals; z++)
										beta[i][j][x] = LogarithmOfAddition(beta[i][j][x], grammar.LogNonTerminalProbability[x][y][z] + beta[i][k][y] + beta[k + 1][j][z]);	
				alpha[0][words[sen].size() - 1][0] = 0;
				// for (int len = words[p].size() - 1; len >= 1; len--)
				// 	for (int i = 0; i < words[p].size(); i++){
				// 		int j = i + len - 1;
				for (int j = words[sen].size() - 1; j >= 0; j--)
					for (int i = 0; i <= j; i++){
						if ((i == 0) && (j == words[sen].size() - 1)) continue;
						for (int k = 0; k < i; k++)
							for (int x = 0; x < grammar.numNonTerminals; x++)
								for (int y = 0; y < grammar.numNonTerminals; y++)
									for (int z = 0; z < grammar.numNonTerminals; z++)
										alpha[i][j][z] = LogarithmOfAddition(alpha[i][j][z], grammar.LogNonTerminalProbability[x][y][z] + alpha[k][j][x] + beta[k][i - 1][y]);
						for (int k = j + 1; k < words[sen].size(); k++)
							for (int x = 0; x < grammar.numNonTerminals; x++)
								for (int y = 0; y < grammar.numNonTerminals; y++)
									for (int z = 0; z < grammar.numNonTerminals; z++)
										alpha[i][j][y] = LogarithmOfAddition(alpha[i][j][y], grammar.LogNonTerminalProbability[x][y][z] + alpha[i][k][x] + beta[j + 1][k][z]);
					}
				for (int i = 0; i < words[sen].size() - 1; i++)
					for (int j = i + 1; j < words[sen].size(); j++)
						for (int k = i; k < j; k++)
							for (int x = 0; x < grammar.numNonTerminals; x++)
								for (int y = 0; y < grammar.numNonTerminals; y++)
									for (int z = 0; z < grammar.numNonTerminals; z++)
										count[x][y][z] = LogarithmOfAddition(count[x][y][z], alpha[i][j][x] + grammar.LogNonTerminalProbability[x][y][z] + beta[i][k][y] + beta[k + 1][j][z] - beta[0][words[sen].size() - 1][0]);
				for (int i = 0; i < words[sen].size(); i++)
					for (int x = 0; x < grammar.numNonTerminals; x++)
						for (int y = 0; y < grammar.numTerminals; y++)
							countNon[x][y] = LogarithmOfAddition(countNon[x][y], alpha[i][i][x] + grammar.LogTerminalProbability[x][y] + beta[i][i][x] - beta[0][words[sen].size() - 1][0]);
				likelihood = likelihood + beta[0][words[sen].size() - 1][0];
				// printf("%.10Lf\t", beta[0][words[p].size() - 1][0].get());
				free(alpha, MAX_WORDS, MAX_WORDS, grammar.numNonTerminals);
				free(beta, MAX_WORDS, MAX_WORDS, grammar.numNonTerminals);
			}

			for (int x = 0; x < grammar.numNonTerminals; x++){
				LogDouble tmp1, tmp2;
				for (int y = 0; y < grammar.numNonTerminals; y++)
					for (int z = 0; z < grammar.numNonTerminals; z++)
						tmp1 = LogarithmOfAddition(tmp1, count[x][y][z]);
				for (int y = 0; y < grammar.numTerminals; y++)
						tmp2 = LogarithmOfAddition(tmp2, countNon[x][y]);
				for (int y = 0; y < grammar.numNonTerminals; y++)
					for (int z = 0; z < grammar.numNonTerminals; z++)
						grammar.LogNonTerminalProbability[x][y][z] = count[x][y][z] - tmp1;
				for (int y = 0; y < grammar.numTerminals; y++)
					grammar.LogTerminalProbability[x][y] = countNon[x][y] - tmp2;
			}
			// printf("The %d-th training: the logarithm of the likelihood is %.50Lf\n", time, likelihood.get());
			printf("%d %.50Lf\n", time, likelihood.get());
			// if (abs(beta[0][words[i].size() - 1][0].get()) < 1e-10) break;
			// readParseTree(NUM_TEST, "test");
			predicting(EVALB_OUT_TEST_NAME, &grammar);
			// readParseTree(NUM_TRAINING, "training");
			// delete count, countNon;
			// cout<<"lalala"<<endl;
// system("tree");
			system("include/EVALB/evalb -p sentences/io.prm sentences/standard.gld sentences/test.gld > fscore");
			ifstream fscore;
			fscore.open("./fscore");
			string s;
			while (getline(fscore, s)){
				int p = s.find("Bracketing Recall         =");
				if (p < s.size()){
					cout << s.substr(p + 27, 255) << endl;
					break;
				}
			}
			fscore.close();
		}
	}

	void predicting(string s, TrainingGrammar *myGrammar){
		evalbOut->open(s);
		for (int i = 0; i < numSentencesTest; i++){
			malloc(&table, MAX_WORDS, MAX_WORDS, myGrammar->numNonTerminals);
			malloc(&record, MAX_WORDS, MAX_WORDS, myGrammar->numNonTerminals);
			CYK(i, myGrammar);
			parseTree = buildParseTree(i);
			parseTree->saveToEvalbFormat(*evalbOut);
			delete table, record;
		}
		evalbOut->close();
	}

	void CYK(int p, TrainingGrammar *grammar){
		for (int i = 0; i < wordsTest[p].size(); i++)
			for (int j = 0; j < wordsTest[p].size(); j++)
				for (int k = 0; k < grammar->numNonTerminals; k++)
					table[i][j][k] = LOG_ZERO;

		for (int j = 0; j < wordsTest[p].size(); j++)
			for (int i = 0; i < grammar->numNonTerminals; i++){
				table[j][j][i] = grammar->LogTerminalProbability[i][grammar->indexTerminal[wordsTest[p][j]]];
				// cout<<"   "<<i<<" "<<j<<" "<<table[j][j][i].get()<<endl;
			}
		for (int j = 0; j < wordsTest[p].size(); j++)
			for (int i = j - 1; i >= 0; i--)
				for (int k = i; k <= j - 1; k++)
					for (int x = 0; x < grammar->numNonTerminals; x++)
						for (int y = 0; y < grammar->numNonTerminals; y++)
							for (int z = 0; z < grammar->numNonTerminals; z++){
								LogDouble tmp = grammar->LogNonTerminalProbability[x][y][z] + table[i][k][y] + table[k + 1][j][z];
								// if ((i==0)&&(j==wordsTest[p].size()-1)&&(x==0))
								// cout<<i<<" "<<j<<" "<<k<<" "<<x<<" "<<y<<" "<<z<<" "<<"lalala"<<grammar->LogNonTerminalProbability[x][y][z].get()<<" "<<table[i][k][y].get() <<" "<< table[k + 1][j][z].get()<<"                     "<<tmp.get()<<endl;
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
		// cout<<i<<" "<<j<<" "<<x<<"           "<<wordsTest[p].size()<<"   "<<standardGrammar.numNonTerminals<<endl;
		// cout<<"         "<<record[i][j][x].i<<" "<<record[i][j][x].j<<" "<<record[i][j][x].k<<" "<<record[i][j][x].x<<" "<<record[i][j][x].y<<" "<<record[i][j][x].z<<" "<<endl;
		if (i == j) node->insertTerminal(wordsTest[p][i]);
		else{
			node->insertChildren("S", "S");
			buildParseTree(p, node->left, i, record[i][j][x].k, record[i][j][x].y);
			buildParseTree(p, node->right, record[i][j][x].k + 1, j, record[i][j][x].z);
		}
	}
	ParseTree* buildParseTree(int p){
		// cout<<p<<endl;
		ParseTree *parseTree = new ParseTree("S");
		buildParseTree(p, parseTree->root, 0, wordsTest[p].size() - 1, 0);
		return parseTree;
	}
};

#endif