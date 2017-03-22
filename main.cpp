#include <iostream>

#include <cstdio>

#include "src/grammar.hpp"
// #include "src/sentence.hpp"
#include "src/generator.hpp"

using namespace std;

int main(){
	CNF emile;
	ifstream grammarIn;
	grammarIn.open("grammar/emile.cnf");
	emile.read(grammarIn);
	grammarIn.close();

	ifstream treeIn;
	ofstream treeOut, evalbOut;

	treeOut.open("sentences/sentences.save");
	evalbOut.open("sentences/sentences.gld");

	Generator generator(100);
	generator.generateSentences(emile, treeOut, evalbOut);

	evalbOut.close();
	treeOut.close();

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

	// treeIn.open("sentences/sentences.save");
	// grammarIn.open("grammar/emile.cnf");
	// Sentence sentence(treeIn, grammarIn);
	// sentence.CYK();
	// treeIn.close();
	// grammarIn.close();
}