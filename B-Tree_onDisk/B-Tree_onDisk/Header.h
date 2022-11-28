#pragma once

#include <iostream>
#include <string>
#include <cmath>
#include <queue>
#include <fstream>
#include <stdint.h>
#include <set>

using namespace std;



class BTreeNode {
private:
	int32_t pageNumber;
	int32_t n;
	int32_t t;
	int32_t* keys;
	int32_t* pages;
	int32_t leaf;

public:
	BTreeNode();

	BTreeNode(int32_t t, int32_t leaf);

	void traversal(fstream& stream);

	// this method return pageNumber of node, -1 if can't find, -2 if error.
	int32_t search(fstream& stream, int32_t key);

	// find the first key is greater or equal than key, return value: index.
	int32_t findKey(int32_t key);

	// implement for insert

	bool isFullBTreeNode();

	void splitBTreeNode(fstream& stream, int32_t pos, BTreeNode* node);

	void insertNonFull(fstream& stream, int32_t key);

	// implement for remove

	int32_t getLeft(fstream& stream, int32_t index);

	int32_t getRight(fstream& stream, int32_t index);

	void merge(fstream& stream, int32_t index);

	void borrowFromLeft(fstream& stream, int32_t index);

	void borrowFromRight(fstream& stream, int32_t index);

	void fill(fstream& stream, int32_t index);

	void removeFromLeaf(fstream& stream, int32_t index);

	void removeFromNonLeaf(fstream& stream, int32_t index);

	void remove(fstream& stream, int32_t key);

	//int32_t calcPageNumber(int32_t indexOfchild);

	// write/read node

	friend void writeDisk(fstream& os, BTreeNode* node);

	friend BTreeNode* readDisk(fstream& is, int32_t pageNumber, int32_t t);

	friend void clearMemory();

	friend class BTree;

};



class BTree {
private:
	int32_t t;
	static int32_t count;
	BTreeNode* pRoot;

	fstream stream;
	
public:
	BTree(int32_t t);

	BTree(string btreeFile);

	void traversal();

	void bfs();

	int32_t search(int32_t key);

	void insert(int32_t key);

	void remove(int32_t key);

	void save();

	void close();

	void printRoot() {
		if (!pRoot) return;
		cout << "Keys: " << endl;
		for (int i = 0; i < pRoot->n; i++) {
			cout << pRoot->keys[i] << " ";
		}
		cout << endl;
		cout << "Pages: " << endl;
		for (int i = 0; i < pRoot->n + 1; i++) {
			cout << pRoot->pages[i] << " ";
		}
		cout << endl;
	}

	friend class BTreeNode;
};



int32_t getSizeOfRecord(int32_t t);

void writeDisk(fstream& os, BTreeNode* node);

BTreeNode* readDisk(fstream& is, int32_t pageNumber, int32_t t);

int32_t calcOffset(int32_t pageNumber, int32_t t);

void clearMemory();




//***********************************************************************************************************************




/*
2
900
y
1000
y
1100
y
800
y
1200
y
1300
y
1400
y
1500
y
1600
y
1700
y
1800
n

*/

/*
2
100
y
200
y
300
y
400
y
500
y
600
y
*/

/*
1
4
1
391

*/