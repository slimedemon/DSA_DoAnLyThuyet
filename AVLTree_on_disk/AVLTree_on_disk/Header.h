#pragma once

#include<iostream>
#include<string>
#include<string.h>
#include<cmath>
#include<queue>
#include <stdint.h>
#include <set>
#include <fstream>

using namespace std;

struct Node {
	int32_t pageNumber;
	int32_t key;
	int32_t height;
	int32_t pLeft;
	int32_t pRight;
};

struct AVLTree {
	Node* pRoot;
};

void clearMemory();

// get size of node
int32_t getSizeOfRecord();

// calculate offset
int32_t calcOffset(int32_t pageNumber);

// write data of node into disk
void writeDisk(fstream& os, Node* node);

// read data of node from disk and this node is inserted into memoryManagement
Node* readDisk(fstream& is, int32_t pageNumber);

Node* readDisk_nonManagement(fstream& is, int32_t pageNumber);

void initTree(AVLTree& tree);

int32_t getHeight(fstream& stream, int32_t pageNumber);

int32_t getHeight(Node* node);

int32_t getBalance(fstream& stream, Node* node);

Node* createNode(int32_t key);

Node* leftRotate(fstream& stream, Node* y);

Node* rightRotate(fstream& stream, Node* y);

void makeBalance(fstream& stream, Node*& node);

void insertFromNonRoot(fstream& stream, int32_t& pageNumber, int32_t key);

void insertFromRoot(fstream& stream, Node*& pRoot, int32_t key);

void removeFromNonRoot(fstream& stream, int32_t& pageNumber, int32_t key);

void removeFromRoot(fstream& stream, Node*& pRoot, int32_t key);

void bfs(fstream& stream, const AVLTree& tree);

int32_t searchFromNonRoot(fstream& stream, int32_t pageNumber, int32_t);

void searchFromRoot(fstream& stream, Node* pRoot, int32_t key);

void writeAVLTree(fstream& os, Node* pRoot);

void readAVLTree(fstream& is, Node*& pRoot);

void clearTree(fstream& os, Node*& pRoot);