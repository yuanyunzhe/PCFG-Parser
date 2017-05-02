#include <iostream>

#include <cstdio>

#include "src/Grammar.hpp"
#include "src/EM.hpp"
#include "src/Generator.hpp"

using namespace std;

int main(int argc, char const *argv[]){
	if ((string)argv[1] == "-g"){
		CNF emile;
		ifstream grammarIn;
		grammarIn.open("grammar/emile_init.cnf");
		emile.read(grammarIn);
		grammarIn.close();

		ifstream treeIn;
		ofstream treeOut, evalbOut;

		treeOut.open("sentences/sentences.save");
		evalbOut.open("sentences/sentences.gld");

		Generator generator(20, emile);
		generator.generateSentences(treeOut, evalbOut);

		evalbOut.close();
		treeOut.close();
	}
	else if ((string)argv[1] == "-t"){
		// CNF emile;
		// ifstream grammarIn;
		// grammarIn.open("grammar/emile.cnf");
		// emile.read(grammarIn);
		// grammarIn.close();
				ifstream treeIn;
		// ofstream treeOut, evalbOut;
		treeIn.open("sentences/sentences.save");
		// grammarIn.open("grammar/emile.cnf");

		// ofstream lalala;
		// lalala.open("sentences/lalala.save");
		// for (int i = 0; i < 50; i++){
		// 	cout << "The " << i << "-th sentence:" << endl;
		// 	Sentence sentence(treeIn, emile);
		// 	cout << "Length: " << sentence.words.size() << endl;
		// 	sentence.emTraining(lalala);
		// 	cout << endl;
		// }
		ofstream evalbOut;
		evalbOut.open("sentences/sentences_out.gld");
		EM em;
		cout<<"###"<<endl;
		em.setNumSentences(20);
		em.readParseTree(treeIn);
		em.initializeTrainingGrammar(5);
		em.training();
		em.predicting(evalbOut);
		treeIn.close();
		evalbOut.close();
		// grammarIn.close();
	}


	// treeIn.open("sentences/sentences.save");
	// treeOut.open("sentences/sentences2.save");
	// for (int i = 0; i < 100; i++){
		
	// 	ParseTree *a = new ParseTree;
	// 	a->load(treeIn);
	// 	a->save(treeOut);
	// 	delete a;
	// }
	// treeIn.close();
	// treeOut.close();
	return 0;
}