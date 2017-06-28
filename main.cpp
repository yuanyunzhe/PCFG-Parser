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
		grammarIn.open(GRAMMAR_NAME);
		emile.read(grammarIn);
		grammarIn.close();

		ofstream sentenceTraining, sentenceTest;

		sentenceTraining.open(TRAINING_NAME);
		// evalbOutTraining.open(EVALB_OUT_TRAINING_NAME);
		sentenceTest.open(TEST_NAME);
		// evalbOutTest.open(EVALB_OUT_TEST_NAME);

		Generator generatorTraining(NUM_TRAINING, emile), generatorTest(NUM_TEST, emile);
		generatorTraining.generateSentences(sentenceTraining, 100);
		generatorTest.generateSentences(sentenceTest, 1000);

		sentenceTraining.close();
		// evalbOutTraining.close();
		sentenceTest.close();
		// evalbOutTest.close();
		
	}
	else if ((string)argv[1] == "-t"){
		ifstream grammar, sentenceTraining, sentenceTest;
		ofstream evalbOut;
		grammar.open(GRAMMAR_NAME);
		sentenceTraining.open(TRAINING_NAME);
		sentenceTest.open(TEST_NAME);
		
		EM em(grammar, sentenceTraining, sentenceTest, evalbOut);
		em.setNumSentences(NUM_TRAINING, NUM_TEST);
		em.readParseTree(NUM_TRAINING, "training");
		em.readParseTree(NUM_TEST, "test");
		em.initializeTrainingGrammar(NUM_NONTERMINALS);
		
		em.predictStandardData();

		em.training(MAX_TRAINING_TIMES);

		// em.predicting();

		grammar.close();
		sentenceTraining.close();
		sentenceTest.close();
		
	}
	return 0;
}