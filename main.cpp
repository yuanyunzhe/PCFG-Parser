#include <iostream>

#include <cstdio>

#include "src/Grammar.hpp"
#include "src/EM.hpp"
#include "src/Generator.hpp"

#define NUM_TRAINING 500
#define NUM_TEST 100
#define NUM_NONTERMINALS 10
#define MAX_TRAINING_TIMES 100

using namespace std;

int main(int argc, char const *argv[]){
	if ((string)argv[1] == "-g"){
		CNF emile;
		ifstream grammarIn;
		grammarIn.open("grammar/emile_init.cnf");
		emile.read(grammarIn);
		grammarIn.close();

		ofstream treeOutTraining, evalbOutTraining, treeOutTest, evalbOutTest;

		treeOutTraining.open("sentences/sentences_training.save");
		evalbOutTraining.open("sentences/sentences_training.gld");
		treeOutTest.open("sentences/sentences_test.save");
		evalbOutTest.open("sentences/sentences_test.gld");

		Generator generatorTraining(NUM_TRAINING, emile), generatorTest(NUM_TEST, emile);
		generatorTraining.generateSentences(treeOutTraining, evalbOutTraining);
		generatorTest.generateSentences(treeOutTest, evalbOutTest);

		treeOutTraining.close();
		evalbOutTraining.close();
		treeOutTest.close();
		evalbOutTest.close();
		
	}
	else if ((string)argv[1] == "-t"){
		ifstream treeInTraining, treeInTest;
		ofstream evalbOut;
		treeInTraining.open("sentences/sentences_training.save");
		treeInTest.open("sentences/sentences_test.save");
		evalbOut.open("sentences/sentences_out.gld");
		EM em;
		em.setNumSentences(NUM_TRAINING, NUM_TEST);
		em.readParseTree(treeInTraining, NUM_TRAINING);
		em.initializeTrainingGrammar(NUM_NONTERMINALS);
		em.training(MAX_TRAINING_TIMES);
		em.readParseTree(treeInTest, NUM_TEST);
		em.predicting(evalbOut);
		treeInTraining.close();
		evalbOut.close();
	}
	return 0;
}