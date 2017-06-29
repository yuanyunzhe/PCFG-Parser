#ifndef PARSETREE_HPP
#define PARSETREE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <map>

using namespace std;

class ParseTreeNode{
public:
	ParseTreeNode *parent, *left, *right;
	bool isLeft;
	int index;
	string nonTerminal, terminal;
	ParseTreeNode(){;}
	ParseTreeNode(string nonTerminal){
		this->nonTerminal = nonTerminal;
		this->terminal = "-";
		this->left = NULL;
		this->right = NULL;
	}
	ParseTreeNode(string nonTerminal, string terminal){
		this->nonTerminal = nonTerminal;
		this->terminal = terminal;
		this->left = NULL;
		this->right = NULL;
	}

	void insertChildren(string nonTerminalLeft, string nonTerminalRight){
		ParseTreeNode *newNode;
		newNode = new ParseTreeNode(nonTerminalLeft);
		this->left = newNode;
		newNode->parent = this;
		newNode->isLeft = true;
		newNode = new ParseTreeNode(nonTerminalRight);
		this->right = newNode;
		newNode->parent = this;
		newNode->isLeft = false;
	}
	void insertTerminal(string terminal){
		this->terminal = terminal;
	}
};
class ParseTree{
public:
	ParseTreeNode *root;
	ParseTree(){;}
	ParseTree(string nonTerminal){
		root = new ParseTreeNode(nonTerminal);
		root->parent = root;
		root->isLeft = true;
	}

	void preOrderTraversal(ParseTreeNode *node, vector<string> &words){
		if (node->left == NULL) words.push_back(node->terminal);
		else{
			preOrderTraversal(node->left, words);
			preOrderTraversal(node->right, words);
		}
	}

	void inOrderTraversal(ParseTreeNode *node, vector<string> &words){
		if (node->left == NULL){
			words.push_back("(");
			words.push_back(node->nonTerminal);
			words.push_back(node->terminal);
			words.push_back(")");
		}
		else{
			words.push_back("(");
			words.push_back(node->nonTerminal);
			inOrderTraversal(node->left, words);
			inOrderTraversal(node->right, words);
			words.push_back(")");
		}
	}

	void inOrderTraversalLeaves(ParseTreeNode *node, vector<string> &words){
		if (node->left == NULL) words.push_back(node->terminal);
		else{
			inOrderTraversalLeaves(node->left, words);
			inOrderTraversalLeaves(node->right, words);
		}
	}

	vector<string> getTreeAndWords(){
		vector<string> words;
		inOrderTraversal(root, words);
		return words;
	}

	vector<string> getWords(){
		vector<string> words;
		inOrderTraversalLeaves(root, words);
		return words;
	}

	void saveToEvalbFormat(ofstream &evalbOut){
		vector<string> words = getTreeAndWords();
		for (int j = 0; j < words.size() - 1; j++){
			evalbOut << words[j];
			if (((words[j] == ")") && (words[j + 1] != ")")) || 
				((words[j] != "(") && (words[j] != ")") && (words[j + 1] != "(") && (words[j + 1] != ")")) ||
				((words[j] != "(") && (words[j + 1] == "("))) evalbOut << " ";
		}
		evalbOut << words.back() << endl;
	}

	void saveToSentence(ofstream &sentenceOut){
		vector<string> words = getWords();
		for (int j = 0; j < words.size(); j++)
				sentenceOut << words[j] << " ";
		sentenceOut << "." << endl;
	}

	// void save(ofstream &treeOut, ParseTreeNode *node){
	// 	treeOut << node->index << " " << node->nonTerminal << " " << node->terminal << " " << node->parent->index << " " << node->isLeft << endl;
	// 	if (node->left != NULL){
	// 		save(treeOut, node->left);
	// 		save(treeOut, node->right);
	// 	}
	// }
	// void save(ofstream &treeOut){
	// 		save(treeOut, this->root);
	// 	treeOut << "-1 - - 0 0" << endl;
	// }
	
	// void load(ifstream &treeIn){
	// 	int index, parentIndex;
	// 	string nonTerminal, terminal;
	// 	bool isLeft;
	// 	map<int, ParseTreeNode*> indexNode;
	// 	while (treeIn >> index >> nonTerminal >> terminal >> parentIndex >> isLeft){
	// 		if (index == -1) break;
	// 		ParseTreeNode *newNode = new ParseTreeNode(nonTerminal, terminal);
	// 		newNode->index = index;
	// 		newNode->isLeft = isLeft;
	// 		indexNode.insert(pair<int, ParseTreeNode*>(index, newNode));
	// 		if (index == parentIndex){
	// 			this->root = newNode;
	// 			newNode->parent = newNode;
	// 		}
	// 		else if (indexNode.find(parentIndex) != indexNode.end()){
	// 			newNode->parent = indexNode[parentIndex];
	// 			if (newNode->isLeft) indexNode[parentIndex]->left = newNode;
	// 			else indexNode[parentIndex]->right = newNode;
	// 		}
	// 	}
	// }

	// void reassignIndex(ParseTreeNode *node, int &index){
	// 	node->index = index;
	// 	if (node->left != NULL){
	// 		index++;
	// 		reassignIndex(node->left, index);
	// 		index++;
	// 		reassignIndex(node->right, index);
	// 	}
	// }
	// void reassignIndex(){
	// 	int index = 0;
	// 	reassignIndex(this->root, index);
	// }
};

#endif