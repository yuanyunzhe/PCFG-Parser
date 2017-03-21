#ifndef PARSETREE_HPP
#define PARSETREE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <map>

#include "grammar.hpp"
using namespace std;

class ParseTreeNode{
public:
	ParseTreeNode *parent, *left, *right;
	bool isLeft;
	int index;
	string symbol, word;
	ParseTreeNode(){;}
	ParseTreeNode(string symbol){
		this->symbol = symbol;
		this->word = "-";
		this->left = NULL;
		this->right = NULL;
	}
	ParseTreeNode(string symbol, string word){
		this->symbol = symbol;
		this->word = word;
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
	void insertWord(string word){
		this->word = word;
	}
};
class ParseTree{
public:
	ParseTreeNode *root;
	ParseTree(){;}
	ParseTree(string symbol){
		root = new ParseTreeNode(symbol);
		root->parent = root;
		root->isLeft = true;
	}

	void preOrderTraversal(ParseTreeNode *node, vector<string> &words){
		if (node->left == NULL) words.push_back(node->word);
		else{
			preOrderTraversal(node->left, words);
			preOrderTraversal(node->right, words);
		}
	}

	void inOrderTraversal(ParseTreeNode *node, vector<string> &words){
		if (node->left == NULL) words.push_back(node->word);
		else{
			words.push_back("(");
			inOrderTraversal(node->left, words);
			inOrderTraversal(node->right, words);
			words.push_back(")");
		}
	}
	vector<string> getWords(){
		vector<string> words;
		inOrderTraversal(root, words);
		return words;
	}
	void saveToEvalbFormat(ofstream &evalbOut){
		vector<string> words;
		words = getWords();
		for (int j = 0; j < words.size() - 1; j++){
			evalbOut << words[j];
			if (((words[j] == ")") && (words[j + 1] == "(")) || 
				((words[j] != "(") && (words[j] != ")") && (words[j + 1] != "(") && (words[j + 1] != ")")) ||
				((words[j] != "(") && (words[j + 1] == "("))) evalbOut << " ";
		}
		evalbOut << words.back() << endl;
	}

	void save(ofstream &treeOut, ParseTreeNode *node){
		treeOut << node->index << " " << node->symbol << " " << node->word << " " << node->parent->index << " " << node->isLeft << endl;
		if (node->left != NULL){
			save(treeOut, node->left);
			save(treeOut, node->right);
		}
	}
	void save(ofstream &treeOut){
			save(treeOut, this->root);
		treeOut << "-1 - - 0 0" << endl;
	}
	
	void load(ifstream &treeIn){
		int index, parentIndex;
		string symbol, word;
		bool isLeft;
		map<int, ParseTreeNode*> indexNode;
		while (treeIn >> index >> symbol >> word >> parentIndex >> isLeft){
			if (index == -1) break;
			ParseTreeNode *newNode = new ParseTreeNode(symbol, word);
			newNode->index = index;
			newNode->isLeft = isLeft;
			indexNode.insert(pair<int, ParseTreeNode*>(index, newNode));
			if (index == parentIndex){
				this->root = newNode;
				newNode->parent = newNode;
			}
			else if (indexNode.find(parentIndex) != indexNode.end()){
				newNode->parent = indexNode[parentIndex];
				if (newNode->isLeft) indexNode[parentIndex]->left = newNode;
				else indexNode[parentIndex]->right = newNode;
			}
		}
	}

	void reassignIndex(ParseTreeNode *node, int &index){
		node->index = index;
		if (node->left != NULL){
			index++;
			reassignIndex(node->left, index);
			index++;
			reassignIndex(node->right, index);
		}
	}
	void reassignIndex(){
		int index = 0;
		reassignIndex(this->root, index);
	}
};

#endif