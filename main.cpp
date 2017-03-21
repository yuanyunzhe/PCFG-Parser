#include <iostream>

#include <cstdio>

#include "grammar.hpp"
#include "sentence.hpp"
#include "generator.hpp"

using namespace std;

int main(){
	CNF emile;
	ifstream grammarIn;
	grammarIn.open("emile.cnf");
	emile.read(grammarIn);
	grammarIn.close();

	ifstream treeIn;
	ofstream treeOut, evalbOut;

	treeOut.open("sentences/sentences.save");
	evalbOut.open("sentences/sentences.gld");

	Generator generator(100, emile);
	generator.generateSentences(treeOut, evalbOut);

	evalbOut.close();
	treeOut.close();

	treeIn.open("sentences/sentences.save");
	treeOut.open("sentences/sentences2.save");
	for (int i = 0; i < 100; i++){
		
		ParseTree *a = new ParseTree;
		a->load(treeIn);
		a->save(treeOut);
		delete a;
	}
	treeIn.close();
	treeOut.close();

	treeIn.open("sentences/sentences.save");
	grammarIn.open("emile.cnf");
	Sentence sentence(treeIn, grammarIn);
	sentence.CYK();
	treeIn.close();
	grammarIn.close();
}