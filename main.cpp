#include <iostream>

#include <cstdio>

#include "src/Grammar.hpp"
#include "src/EM.hpp"
#include "src/Generator.hpp"
#include "src/Parameter.hpp"

using namespace std;

int main(int argc, char const *argv[]){
	if ((string)argv[1] == "-g"){
		CNF emile;
		ifstream grammarIn;
		grammarIn.open("grammar/emile_init.cnf");
		emile.read(grammarIn);
		grammarIn.close();

		ofstream treeOutTraining, evalbOutTraining, treeOutTest, evalbOutTest;

		treeOutTraining.open(TRAINING_NAME);
		evalbOutTraining.open(EVALB_OUT_TRAINING_NAME);
		treeOutTest.open(TEST_NAME);
		evalbOutTest.open(EVALB_OUT_TEST_NAME);

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
		treeInTraining.open(TRAINING_NAME);
		treeInTest.open(TEST_NAME);
		
		EM em(treeInTraining, treeInTest, evalbOut);
		em.setNumSentences(NUM_TRAINING, NUM_TEST);
		em.readParseTree(NUM_TRAINING, "training");
		em.initializeTrainingGrammar(NUM_NONTERMINALS);
		em.training(MAX_TRAINING_TIMES);
		em.readParseTree(NUM_TEST, "test");
		em.predicting();
		treeInTraining.close();
		
	}
	return 0;
}