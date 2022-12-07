#include "Header.h"

// global set
set<Node*> memoryManagement;

int32_t cnt = 0;

void clearMemory() {
	for (Node* node : memoryManagement) {
		if (!node) continue;

		delete node;
	}

	memoryManagement.clear();
}

// get size of node
int32_t getSizeOfRecord() {
	return sizeof(int32_t) * 5;
}

// calculate offset
int32_t calcOffset(int32_t pageNumber) {
	return getSizeOfRecord() * pageNumber;
}

// write data of node into disk
void writeDisk(fstream& os, Node* node) {
	try {
		os.seekp(calcOffset(node->pageNumber), ios_base::beg);

		os.write((char*)(&node->pageNumber), sizeof(int32_t));
		os.write((char*)(&node->key), sizeof(int32_t));
		os.write((char*)(&node->height), sizeof(int32_t));
		os.write((char*)(&node->pLeft), sizeof(int32_t));
		os.write((char*)(&node->pRight), sizeof(int32_t));

		os.flush();
	}
	catch (...) {
		cout << "Failed write file" << endl;
	}
}

// read data of node from disk and this node is inserted into memoryManagement
Node* readDisk(fstream& is, int32_t pageNumber) {
	Node* node = nullptr;

	try {
		is.seekg(calcOffset(pageNumber), ios_base::beg);

		node = new Node{};

		is.read((char*)(&node->pageNumber), sizeof(int32_t));
		is.read((char*)(&node->key), sizeof(int32_t));
		is.read((char*)(&node->height), sizeof(int32_t));
		is.read((char*)(&node->pLeft), sizeof(int32_t));
		is.read((char*)(&node->pRight), sizeof(int32_t));
	}
	catch (...) {
		cout << "Failed allocation or can't read from disk %%%" << endl;
		delete node;
		return nullptr;
	}

	memoryManagement.insert(node);

	return node;
}

Node* readDisk_nonManagement(fstream& is, int32_t pageNumber) {
	Node* node = nullptr;

	try {
		is.seekg(calcOffset(pageNumber), ios_base::beg);

		node = new Node{};

		is.read((char*)(&node->pageNumber), sizeof(int32_t));
		is.read((char*)(&node->key), sizeof(int32_t));
		is.read((char*)(&node->height), sizeof(int32_t));
		is.read((char*)(&node->pLeft), sizeof(int32_t));
		is.read((char*)(&node->pRight), sizeof(int32_t));
	}
	catch (...) {
		cout << "Failed allocation or can't read from disk %%%" << endl;
		delete node;
		return nullptr;
	}

	return node;
}

void initTree(AVLTree& tree) {
	tree.pRoot = nullptr;
}

int32_t getHeight(fstream& stream, int32_t pageNumber) {
	if (pageNumber == -1) return 0;

	return readDisk(stream, pageNumber)->height;
}

int32_t getHeight(Node* node) {
	if (!node) return 0;

	return node->height;
}

int32_t getBalance(fstream& stream, Node* node) {
	if (node == NULL)
		return 0;

	return getHeight(stream, node->pLeft) - getHeight(stream, node->pRight);
}

Node* createNode(int32_t key) {
	Node* node = nullptr;

	try {
		node = new Node{ 0, key, 1, -1, -1 };
	}
	catch (...) {
		cout << "Failed allocation" << endl;
	}

	return node;
}

Node* leftRotate(fstream& stream, Node* y) {
	Node* x = nullptr;
	Node* t = nullptr;

	x = readDisk(stream, y->pRight);

	if (x->pLeft != -1) t = readDisk(stream, x->pLeft);

	x->pLeft = y->pageNumber;
	if (t) y->pRight = t->pageNumber;
	else y->pRight = -1;

	y->height = max(getHeight(stream, y->pLeft), getHeight(t)) + 1;
	x->height = max(getHeight(y), getHeight(stream, x->pRight)) + 1;

	writeDisk(stream, y);
	writeDisk(stream, x);

	return x;
}

Node* rightRotate(fstream& stream, Node* y) {
	Node* x = nullptr;
	Node* t = nullptr;

	x = readDisk(stream, y->pLeft);

	if (x->pRight != -1) t = readDisk(stream, x->pRight);

	x->pRight = y->pageNumber;
	if (t) y->pLeft = t->pageNumber;
	else y->pLeft = -1;

	y->height = max(getHeight(t), getHeight(stream, y->pRight)) + 1;
	x->height = max(getHeight(stream, x->pLeft), getHeight(y)) + 1;

	writeDisk(stream, y);
	writeDisk(stream, x);

	return x;
}

void makeBalance(fstream& stream, Node*& node) {
	int32_t balance = getBalance(stream, node);

	Node* temp = nullptr;
	int32_t res = 0;

	if (balance > 1) {
		temp = readDisk(stream, node->pLeft);
		res = getBalance(stream, temp);
		// left left
		if (balance > 1 && res >= 0) {
			node = rightRotate(stream, node);
		}

		// left right
		if (balance > 1 && res < 0) {
			node->pLeft = leftRotate(stream, temp)->pageNumber;
			node = rightRotate(stream, node);

		}
	}

	if (balance < -1) {
		temp = readDisk(stream, node->pRight);
		res = getBalance(stream, temp);

		// right right
		if (balance < -1 && res < 0) {
			node = leftRotate(stream, node);
		}

		// right left
		if (balance < -1 && res >= 0) {
			node->pRight = rightRotate(stream, temp)->pageNumber;
			node = leftRotate(stream, node);
		}
	}
}

void insertFromNonRoot(fstream& stream, int32_t& pageNumber, int32_t key) {
	if (pageNumber < 0) {
		Node* newNode = createNode(key);

		memoryManagement.insert(newNode);
		cnt++;
		newNode->pageNumber = cnt;
		pageNumber = cnt;
		writeDisk(stream, newNode);

		return;
	}

	Node* node = readDisk(stream, pageNumber);

	if (node->key > key) insertFromNonRoot(stream, node->pLeft, key);
	else if (node->key < key) insertFromNonRoot(stream, node->pRight, key);
	else if (node->key == key) return;

	// update height.
	node->height = max(getHeight(stream, node->pLeft), getHeight(stream, node->pRight)) + 1;

	// make tree balance
	makeBalance(stream, node);
	pageNumber = node->pageNumber;

	writeDisk(stream, node);
}

void insertFromRoot(fstream& stream, Node*& pRoot, int32_t key) {
	if (!pRoot) {
		pRoot = createNode(key);
		pRoot->pageNumber = cnt;
		writeDisk(stream, pRoot);

		return;
	}

	if (key == pRoot->key) return;
	else if (key < pRoot->key) insertFromNonRoot(stream, pRoot->pLeft, key);
	else insertFromNonRoot(stream, pRoot->pRight, key);

	pRoot->height = max(getHeight(stream, pRoot->pLeft), getHeight(stream, pRoot->pRight)) + 1;

	Node* temp = pRoot;
	makeBalance(stream, pRoot);
	writeDisk(stream, pRoot);

	if (temp->key != pRoot->key) {
		memoryManagement.insert(temp);

		pRoot = readDisk_nonManagement(stream, pRoot->pageNumber);
	}

	clearMemory();
}

void removeFromNonRoot(fstream& stream, int32_t& pageNumber, int32_t key) {
	if (pageNumber == -1) return;

	Node* node = readDisk(stream, pageNumber);

	if (node->key > key) removeFromNonRoot(stream, node->pLeft, key);
	else if (node->key < key) removeFromNonRoot(stream, node->pRight, key);
	else if (node->key == key) {
		if (node->pRight < 0 && node->pLeft < 0) {
			pageNumber = -1;
			return;
		}

		if (node->pRight >= 0 && node->pLeft >= 0) {
			Node* decentdant = readDisk(stream, node->pRight);
			while (decentdant->pLeft >= 0) decentdant = readDisk(stream, decentdant->pLeft);

			node->key = decentdant->key;
			removeFromNonRoot(stream, node->pRight, decentdant->key);
		}
		else {
			int32_t childPage = (node->pLeft >= 0) ? node->pLeft : node->pRight;
			pageNumber = childPage;
			node = readDisk(stream, pageNumber);
		}
	}

	// update height
	node->height = max(getHeight(stream, node->pLeft), getHeight(stream, node->pRight)) + 1;

	// make tree balance
	makeBalance(stream, node);
	pageNumber = node->pageNumber;

	writeDisk(stream, node);
}

void removeFromRoot(fstream& stream, Node*& pRoot, int32_t key) {
	if (!pRoot) return;

	Node* temp = pRoot;

	if (pRoot->key == key) {
		if (pRoot->pRight < 0 && pRoot->pLeft < 0) {
			delete pRoot;
			pRoot = nullptr;
			return;
		}

		if (pRoot->pRight >= 0 && pRoot->pLeft >= 0) {
			Node* decentdant = readDisk(stream, pRoot->pRight);
			while (decentdant->pLeft >= 0) decentdant = readDisk(stream, decentdant->pLeft);

			pRoot->key = decentdant->key;
			removeFromNonRoot(stream, pRoot->pRight, decentdant->key);
		}
		else {
			int32_t childPage = (pRoot->pLeft >= 0) ? pRoot->pLeft : pRoot->pRight;
			pRoot = readDisk(stream, childPage);
		}
	}
	else if (key < pRoot->key) removeFromNonRoot(stream, pRoot->pLeft, key);
	else if (key > pRoot->key) removeFromNonRoot(stream, pRoot->pRight, key);

	pRoot->height = max(getHeight(stream, pRoot->pLeft), getHeight(stream, pRoot->pRight)) + 1;

	makeBalance(stream, pRoot);
	writeDisk(stream, pRoot);

	if (temp->key != pRoot->key) {
		memoryManagement.insert(temp);
		pRoot = readDisk_nonManagement(stream, pRoot->pageNumber);
	}

	clearMemory();
}

void bfs(fstream& stream, const AVLTree& tree) {
	if (!tree.pRoot) {
		cout << "Empty!!!" << endl;
		return;
	}
	queue<Node*> que1;
	queue<Node*> que2;

	que1.push(tree.pRoot);

	while (true) {
		Node* u = que1.front();
		que1.pop();
		cout << u->key << " ";

		if (u->pLeft >= 0) que2.push(readDisk(stream, u->pLeft));
		if (u->pRight >= 0) que2.push(readDisk(stream, u->pRight));

		if (que1.empty()) {
			cout << endl;
			if (que2.empty()) break;

			que1 = que2;
			que2 = queue<Node*>();
		}
	}

	clearMemory();
}

int32_t searchFromNonRoot(fstream& stream, int32_t pageNumber, int32_t key) {
	if (pageNumber < 0) return -1;

	Node* node = readDisk(stream, pageNumber);
	if (node->key > key) return searchFromNonRoot(stream, node->pLeft, key);
	else if (node->key < key) return searchFromNonRoot(stream, node->pRight, key);
	else if (node->key == key) return node->pageNumber;
}

void searchFromRoot(fstream& stream, Node* pRoot, int32_t key) {
	if (!pRoot) {
		cout << "Empty!!" << endl;
		return;
	}

	if (pRoot->key == key) {
		cout << "Page number: " << pRoot->pageNumber << endl;
		cout << "Number of reading file: " << memoryManagement.size() << endl;
		return;
	}

	if (pRoot->key > key) {
		cout << "Page number: " << searchFromNonRoot(stream, pRoot->pLeft, key) << endl;
	}
	else {
		cout << "Page number: " << searchFromNonRoot(stream, pRoot->pRight, key) << endl;
	}

	cout << "Number of reading file: " << memoryManagement.size() << endl;

	clearMemory();
}



void writeAVLTree(fstream& os, Node* pRoot) {
	os.write((char*)(&pRoot->pageNumber), sizeof(int32_t));
	os.write((char*)(&pRoot->key), sizeof(int32_t));
	os.write((char*)(&pRoot->height), sizeof(int32_t));
	os.write((char*)(&pRoot->pLeft), sizeof(int32_t));
	os.write((char*)(&pRoot->pRight), sizeof(int32_t));

	os.write((char*)(&cnt), sizeof(int32_t));
}

void readAVLTree(fstream& is, Node*& pRoot) {
	pRoot = createNode(0);

	is.read((char*)(&pRoot->pageNumber), sizeof(int32_t));
	is.read((char*)(&pRoot->key), sizeof(int32_t));
	is.read((char*)(&pRoot->height), sizeof(int32_t));
	is.read((char*)(&pRoot->pLeft), sizeof(int32_t));
	is.read((char*)(&pRoot->pRight), sizeof(int32_t));

	is.read((char*)(&cnt), sizeof(int32_t));
}

void clearTree(fstream& os, Node*& pRoot) {
	writeAVLTree(os, pRoot);

	delete pRoot;
	pRoot = nullptr;
}