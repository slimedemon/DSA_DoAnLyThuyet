#pragma once

#include<iostream>
#include <fstream>
#include<set>
#include<queue>
#include<cmath>

#define int int32_t

using namespace std;

//enum Color { Black = 0, Red = 1 };

//Struct
struct Node
{
	int pageNumber;
	int key;
	int pLeft;
	int pRight;
	int pParent;
	bool isRed; //1: red, 0: black
};

struct RBTree {
	Node* pRoot;
};

//IO, mem
void clearMemory();
int getSizeOfRecord();
int calcOffset(int pageNumber);
void writeDisk(fstream& os, Node* node);
Node* readDisk(fstream& is, int pageNumber);
Node* readDisk_nonManagement(fstream& is, int pageNumber);
void readTree(fstream& is, Node*& pRoot);
void clearTree(fstream& os, Node*& pRoot);
void writeTree(fstream& os, Node* pRoot);

//..family..??
void initTree(RBTree& tree);
Node* createNode(int key);
int heightNode(fstream& stream, Node* pRoot);
Node* parent(fstream& stream, Node*& node);
Node* grandparent(fstream& stream, Node*& node);
Node* uncle(fstream& stream, Node*& node);
Node* sibling(fstream& stream, Node*& node);

//rotate
Node* rightRotate_Advanced(Node*& pRoot, fstream& stream, Node*& y);
Node* leftRotate_Advanced(Node*& pRoot, fstream& stream, Node*& y);

//insert + remove + search
void insertFix(fstream& stream, Node*& pRoot, Node*& node);
Node* insertFromNonRoot(fstream& stream, int32_t& pageNumber, int32_t key, Node* par);
void insertFromRoot(fstream& stream, Node*& pRoot, int key);
void deleteFix(fstream& stream, Node*& node, Node*& pRoot, Node*& par);
void deleteFromNonRoot(fstream& stream, int& pageNumber, int32_t key, Node*& pRoot, Node*& par);
void deleteFromRoot(fstream& stream, Node*& pRoot, int key);
int searchFromNonRoot(fstream& stream, int pageNumber, int key);
void searchFromRoot(fstream& stream, Node* pRoot, int key);
void bfs(fstream& stream, const RBTree& tree);

