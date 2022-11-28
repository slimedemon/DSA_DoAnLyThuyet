#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include <queue>

using namespace std;

class BTreeNode {
private:   
	int* keys;
	BTreeNode** c;
	int n;
	int t;
	bool leaf;

public:
	BTreeNode(int t, bool leaf) {
		this->t = t;
		this->leaf = leaf;

		n = 0;
		keys = new (std::nothrow) int[2 * t - 1];
		c = new (std::nothrow) BTreeNode * [2 * t] {};
	}

	void traversal() {
		for (int i = 0; i < n; i++){
			if (c[i]) c[i]->traversal();
			cout << keys[i] << endl;
		}
		
		if (c[n]) c[n]->traversal();
	}

	BTreeNode* search(int key) {
		int index = 0;
		while (index < n && key > keys[index]) index++;

		if (keys[index] == key) return this;

		if(c[index]) return c[index]->search(key);

		return nullptr;
	}

	// implement for insert

	bool isFullBTreeNode() {
		if (n == 2 * t - 1) return true;
		
		return false;
	}

	void splitBTreeNode(int pos,  BTreeNode* node) {
		BTreeNode* newNode = new BTreeNode(node->t, node->leaf);

		for (int i = t; i < 2 * t - 1; i++) {
			newNode->keys[i - t] = node->keys[i];
		}

		if (!node->leaf) {
			for (int i = t; i < 2 * t; i++) {
				newNode->c[i - t] = node->c[i];
			}
		}

		newNode->n = t - 1;
		
		for (int i = n-1; i >= pos; i--) keys[i + 1] = keys[i];

		keys[pos] = node->keys[t - 1];
		

		for (int i = n; i >= pos +1; i--) c[i + 1] = c[i];

		c[pos + 1] = newNode;
		
		node->n = t - 1;
		n = n + 1;
	}

	void insertNonFull(int key) {
		int i = n - 1;

		if (leaf) {
			while (i >= 0 && key < keys[i]) {
				keys[i + 1] = keys[i];
				i--;
			}

			keys[i+1] = key;
			n++;
			return;
		}

		while (i >= 0 && key < keys[i]) i--;

		// case: child node is full
		if (c[i+1]->n == 2 * t - 1) {

			splitBTreeNode(i + 1, c[i + 1]);

			if (keys[i + 1] < key) i++;

			c[i + 1]->insertNonFull(key);
			return;
		}

		c[i + 1]->insertNonFull(key);
	}

	// implement for remove

	// find the first key is greater or equal than key, return value: index.
	int findKey(int key) {
		int index = 0;
		while (index < n && keys[index] < key) index++;

		return index;
	}

	int getLeft(int index) {
		BTreeNode* node = c[index];

		while (!node->leaf) node = node->c[node->n];
		return node->keys[node->n-1];
	}

	int getRight(int index) {
		BTreeNode* node = c[index + 1];

		while (!node->leaf) node = node->c[0];
		return node->keys[0];
	}

	void merge(int index) {
		BTreeNode* child = c[index];
		BTreeNode* sibling = c[index + 1];

		// copy keys from c[index+ 1] to c[index]
		for (int i = 0; i < sibling->n; i++) child->keys[i + t] = sibling->keys[i];

		// copy child from c[index+1] to c[index]
		if (!child->leaf) {
			for (int i = 0; i <= sibling->n; i++) child->c[i + t] = sibling->c[i];
		}

		child->keys[t - 1] = keys[index];
		
		// delete keys[index] in node.
		for (int i = index; i < n - 1; i++) keys[i] = keys[i + 1];
		for (int i = index + 1; i < n; i++)  c[i] = c[i + 1];
		child->n += sibling->n +1;
		n--;
		delete sibling;
	}

	void borrowFromLeft(int index) {
		BTreeNode* child = c[index];
		BTreeNode* sibling = c[index-1];

		// add key into child
		for (int i = n - 1; i >= 0; i--) child->keys[i + 1] = child->keys[i];
		child->keys[0] = keys[index-1];

		// case: child is not leaf
		if (!child->leaf) {
			// add child of sibling into child
			for (int i = child->n; i >= 0; i--) child->c[i + 1] = child->c[i];
			child->c[0] = sibling->c[sibling->n];
		}

		keys[index - 1] = sibling->keys[sibling->n - 1];
		child->n++;
		sibling->n--;
	}

	void borrowFromRight(int index) {
		BTreeNode* child = c[index];
		BTreeNode* sibling = c[index + 1];

		// add key into child
		child->keys[child->n] = keys[index];

		// case: child is leaf
		if (!child->leaf) {
			// add child of sibling into child
			child->c[child->n+1] = sibling->c[0];
		}

		keys[index] = sibling->keys[0];
		
		//delete key and child in sibling
		for (int i = 0; i < sibling->n - 1; i++) sibling->keys[i] = sibling->keys[i + 1];
		if (!sibling->leaf) {
			for (int i = 0; i < sibling->n; i++) sibling->c[i] = sibling->c[i + 1];
		}
		
		child->n++;
		sibling->n--;
	}

	void fill(int index)
	{
		if (index != 0 && c[index - 1]->n >= t) {
			borrowFromLeft(index);
			return;
		}

		if (index != n && c[index + 1]->n >= t) {
		borrowFromRight(index);
		return;
		}

		if (index != n)
			merge(index);
		else
			merge(index - 1);
	}

	void removeFromLeaf(int index) {
		for (int i = index; i < n - 1; i++) keys[i] = keys[i + 1];
		n--;	
	}

	void removeFromNonLeaf(int index) {
		int key = keys[index];

		if (c[index]->n >= t) {
			int left = getLeft(index);
			keys[index] = left;
			c[index]->remove(left);
			return;
		}

		if (c[index + 1]->n >= t) {
			int right = getRight(index);
			keys[index] = right;
			c[index + 1]->remove(right);
			return;
		}

		merge(index);
		c[index]->remove(key);
	}
	
	void remove(int key) {
		int index = findKey(key);
		
		if (index < n && keys[index] == key) {
			if (leaf) removeFromLeaf(index);
			else removeFromNonLeaf(index);

			return;
		}

		if (leaf) {
			cout << "Can't find Key in tree!!!" << endl;
			return;
		}

		bool flag = (index == n);

		if (c[index]->n < t) fill(index);

		if (flag && n < index) index--;

		c[index]->remove(key);
	}

	friend class BTree;
};

class BTree {
private:
	BTreeNode* pRoot;
	int t;

protected: 

public:
	BTree(int t) {
		pRoot = nullptr;
		this->t = t;
	}

	void traversal() {
		if (!pRoot) {
			cout << "Empty!!";
			return;
		}
		pRoot->traversal();
	}

	void bfs() {
		if (!pRoot) {
			cout << "Empty!!";
			return;
		}

		queue<BTreeNode*> que1;
		queue<BTreeNode*> que2;

		que1.push(pRoot);
		
		while (true) {
			BTreeNode* u = que1.front();
			que1.pop();

			for (int i = 0; i < u->n; i++)
				cout << u->keys[i] << " ";
			cout << "//";

			for (int i = 0; i <= u->n; i++) {
				if (u->c[i]) que2.push(u->c[i]);
			}

			if (que1.empty()) {
				cout << endl;
				if (que2.empty()) break;

				que1 = que2;
				que2 = queue<BTreeNode*>();
			}
		}
	}

	BTreeNode* search(int key) {
		if (!pRoot) return nullptr;
		return pRoot->search(key);
	}

	void insert(int key) {
		// case: key have already existed
		if (search(key)) return;

		//case: key haven't existed yet
		if (!pRoot) {
			pRoot = new BTreeNode(t, true);
			pRoot->keys[0] = key;
			pRoot->n = 1;
			return;
		}
		
		if (pRoot->n == 2 * t - 1) {
			BTreeNode* newNode = new BTreeNode(t, false);
			
			newNode->c[0] = pRoot;
			newNode->splitBTreeNode(0, pRoot);

			int index = 0;

			if (newNode->keys[0] < key) index++;

			newNode->c[index]->insertNonFull(key);

			pRoot = newNode;
		}
		else {
			pRoot->insertNonFull(key);
		}
	}

	void remove(int key) {
		if (!pRoot) {
			cout << "Tree is empty!!";
			return;
		}

		pRoot->remove(key);

		if (pRoot->n == 0) {
			BTreeNode* temp = pRoot;
			if (pRoot->leaf) {
				pRoot = nullptr;
			}
			else {
				pRoot = pRoot->c[0];
			}

			delete temp;
		}
	}
};

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