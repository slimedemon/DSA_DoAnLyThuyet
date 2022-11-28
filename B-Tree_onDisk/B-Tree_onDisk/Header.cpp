#include "Header.h"

int32_t BTree::count = 0;

set<BTreeNode*> memoryManagement;

void clearMemory()
{
	for(BTreeNode* node : memoryManagement) {
		if (!node) continue;
		delete[] node->keys;
		delete[] node->pages;
		delete node;
	}

	memoryManagement.clear();
}

int32_t getSizeOfRecord(int32_t t) {
	return sizeof(int32_t) * 3 + sizeof(int32_t) * (2 * t - 1) + sizeof(int32_t) * (2 * t) + sizeof(int32_t);
}

// write pageNumber, number of keys, minimun degree, all keys, all pages, leaf
void writeDisk(fstream& os, BTreeNode* node) {
	try {
		os.seekp(calcOffset(node->pageNumber, node->t), ios_base::beg);

		os.write((char*)(&node->pageNumber), sizeof(int32_t));
		os.write((char*)(&node->n), sizeof(int32_t));
		os.write((char*)(&node->t), sizeof(int32_t));
		os.write((char*)(&node->leaf), sizeof(int32_t));

		os.write((char*)node->keys, sizeof(int32_t) * (2 * node->t - 1));
		os.write((char*)node->pages, sizeof(int32_t) * (2 * node->t));

		os.flush();
	}
	catch (...) {
		cout << "Failed write file" << endl;
	}
}

BTreeNode* readDisk(fstream& is, int32_t pageNumber, int32_t t) {
	BTreeNode* node = nullptr;

	try {
		is.seekg(calcOffset(pageNumber, t), ios_base::beg);

		node = new BTreeNode();

		is.read((char*)(&node->pageNumber), sizeof(int32_t));
		is.read((char*)(&node->n), sizeof(int32_t));
		is.read((char*)(&node->t), sizeof(int32_t));
		is.read((char*)(&node->leaf), sizeof(int32_t));


		node->keys = new int32_t[2 * node->t - 1]{};
		node->pages = new int32_t[2 * node->t]{};

		is.read((char*)node->keys, sizeof(int32_t) * (2 * node->t - 1));
		is.read((char*)node->pages, sizeof(int32_t) * (2 * node->t));
	}
	catch (...) {
		cout << "Failed allocation or can't read from disk %%%" << endl;
		if (!node) return nullptr;

		delete[] node->keys;
		delete[] node->pages;
		delete node;

		return nullptr;
	}

	return node;
}

int32_t calcOffset(int32_t pageNumber, int32_t t) {
	return getSizeOfRecord(t) * pageNumber;
}

//int32_t BTreeNode::calcPageNumber(int32_t indexOfChild) {
//	// find first start pageNumber each level
//	// find level (height , base = 0) of child
//	// res = pageNumberStartEachChildLevel + (pageNumber - pageNumberStartEachCurrLevel) * (2 * t) + indexOfChild
//
//	int32_t height = 0;
//	int32_t pageNumberStartEachCurrLevel = 0;
//	int32_t pageNumberStartEachChildLevel = 0;
//	int32_t product = 1;
//	while (pageNumber >= pageNumberStartEachChildLevel) {
//		height++;
//		pageNumberStartEachCurrLevel = pageNumberStartEachChildLevel;
//		pageNumberStartEachChildLevel += product;
//		product *= 2 * t;
//	}
//
//	return (pageNumberStartEachChildLevel + (pageNumber - pageNumberStartEachCurrLevel) * (2 * t) + indexOfChild);
//}

BTreeNode::BTreeNode() {
	pageNumber = -1;
	n = 0;
	t = 0;
	keys = nullptr;
	pages = nullptr;
	leaf = 1;
}

BTreeNode::BTreeNode(int32_t t, int32_t leaf) {
	this->t = t;
	this->leaf = leaf;
	pageNumber = -1;
	n = 0;

	try {
		keys = new int32_t[2 * t - 1]{};
		pages = new int32_t[2 * t]{};
	}
	catch (...) {
		cout << "Failed allocation" << endl;
		delete[] keys;
		pages = nullptr;
		keys = nullptr;
	}
}

void BTreeNode::traversal(fstream& stream) {
	BTreeNode* node = nullptr;

	for (int32_t i = 0; i < n; i++) {
		node = readDisk(stream, pages[i], t);
		memoryManagement.insert(node);
		if (node) node->traversal(stream);
		cout << keys[i] << endl;
	}

	node = readDisk(stream, pages[n], t);
	memoryManagement.insert(node);
	if (node) node->traversal(stream);
}

// this method return pageNumber of node, -1 if can't find, -2 if error.
int32_t BTreeNode::search(fstream& stream, int32_t key) {
	int32_t index = 0;
	BTreeNode* node = nullptr;
	int32_t res = -1;

	while (index < n && key > keys[index]) index++;

	if (index < n && keys[index] == key) return pageNumber;
	if (leaf) return -1;

	node = readDisk(stream, pages[index], t);
	memoryManagement.insert(node);
	if (!node) return -2;

	res = node->search(stream, key);

	return res;
}

//-------------------------------------------------------
// implement for insert
//--------------------------------------

bool BTreeNode::isFullBTreeNode() {
	if (n == 2 * t - 1) return true;

	return false;
}

void BTreeNode::splitBTreeNode(fstream& stream, int32_t pos, BTreeNode* node) {
	BTreeNode* newNode = nullptr;

	try {
		newNode = new BTreeNode(node->t, node->leaf);
	}
	catch (...) {
		cout << "Failed allocation" << endl;
	}

	memoryManagement.insert(newNode);

	for (int32_t i = t; i < 2 * t - 1; i++) {
		newNode->keys[i - t] = node->keys[i];
	}

	if (!node->leaf) {
		for (int32_t i = t; i < 2 * t; i++) {
			newNode->pages[i - t] = node->pages[i];
		}
	}

	newNode->n = t - 1;
	BTree::count++;
	newNode->pageNumber = BTree::count;


	for (int32_t i = n - 1; i >= pos; i--) keys[i + 1] = keys[i];

	keys[pos] = node->keys[t - 1];


	for (int32_t i = n; i >= pos + 1; i--) pages[i + 1] = pages[i];

	pages[pos + 1] = newNode->pageNumber;

	node->n = t - 1;
	n = n + 1;

	writeDisk(stream, newNode);
	writeDisk(stream, this);
	writeDisk(stream, node);
}

void BTreeNode::insertNonFull(fstream& stream, int32_t key) {
	int32_t i = n - 1;

	if (leaf) {
		while (i >= 0 && key < keys[i]) {
			keys[i + 1] = keys[i];
			i--;
		}

		keys[i + 1] = key;
		n++;

		writeDisk(stream, this);
		return;
	}

	while (i >= 0 && key < keys[i]) i--;

	// case: child node is full
	BTreeNode* node = readDisk(stream, pages[i + 1], t);
	if (!node) return;
	memoryManagement.insert(node);

	if (node->n == 2 * t - 1) {
		splitBTreeNode(stream, i + 1, node);
		if (keys[i + 1] < key) i++;
	}

	// Maybe node just update, read file again.
	node = readDisk(stream, pages[i + 1], t);
	if (!node) return;
	memoryManagement.insert(node);
	node->insertNonFull(stream, key);
}

//---------------------------------------------------------------------
// implement for remove
//-------------------------------------------------------

// find the first key is greater or equal than key, return value: index.
int32_t BTreeNode::findKey(int32_t key) {
	int32_t index = 0;
	while (index < n && keys[index] < key) index++;

	return index;
}

int32_t BTreeNode::getLeft(fstream& stream, int32_t index) {
	BTreeNode* node = readDisk(stream, pages[index], t);
	if (!node) throw runtime_error("Can't read file");
	memoryManagement.insert(node);

	while (!node->leaf) {
		node = readDisk(stream, node->pages[node->n], node->t);
		if (!node) throw runtime_error("Can't read file");
		memoryManagement.insert(node);
	}

	return node->keys[node->n - 1];
}

int32_t BTreeNode::getRight(fstream& stream, int32_t index) {
	BTreeNode* node = readDisk(stream, pages[index + 1], t);
	if (!node) throw runtime_error("Can't read file");
	memoryManagement.insert(node);

	while (!node->leaf) {
		node = readDisk(stream, node->pages[0], node->t);
		if (!node) throw runtime_error("Can't read file");
		memoryManagement.insert(node);
	}

	return node->keys[0];
}

void BTreeNode::merge(fstream& stream, int32_t index) {
	BTreeNode* child = readDisk(stream, pages[index], t);
	if (!child) return;
	memoryManagement.insert(child);

	BTreeNode* sibling = readDisk(stream, pages[index + 1], t);
	if (!sibling) return;
	memoryManagement.insert(sibling);

	// copy keys from c[index+ 1] to c[index]
	for (int32_t i = 0; i < sibling->n; i++) child->keys[i + t] = sibling->keys[i];

	// copy child from c[index+1] to c[index]
	if (!child->leaf) {
		for (int32_t i = 0; i <= sibling->n; i++) child->pages[i + t] = sibling->pages[i];
	}

	child->keys[t - 1] = keys[index];

	// delete keys[index] in node.
	for (int32_t i = index; i < n - 1; i++) keys[i] = keys[i + 1];
	for (int32_t i = index + 1; i < n; i++)  pages[i] = pages[i + 1];
	child->n += sibling->n + 1;
	n--;

	writeDisk(stream, this);
	writeDisk(stream, child);
}

void BTreeNode::borrowFromLeft(fstream& stream, int32_t index) {
	BTreeNode* child = readDisk(stream, pages[index], t);
	if (!child) return;
	memoryManagement.insert(child);

	BTreeNode* sibling = readDisk(stream, pages[index - 1], t);
	if (!sibling) return;
	memoryManagement.insert(sibling);

	// add key into child
	for (int32_t i = n - 1; i >= 0; i--) child->keys[i + 1] = child->keys[i];
	child->keys[0] = keys[index - 1];

	// case: child is not leaf
	if (!child->leaf) {
		// add child of sibling into child
		for (int32_t i = child->n; i >= 0; i--) child->pages[i + 1] = child->pages[i];
		child->pages[0] = sibling->pages[sibling->n];
	}

	keys[index - 1] = sibling->keys[sibling->n - 1];
	child->n++;
	sibling->n--;

	writeDisk(stream, this);
	writeDisk(stream, child);
	writeDisk(stream, sibling);
}

void BTreeNode::borrowFromRight(fstream& stream, int32_t index) {
	BTreeNode* child = readDisk(stream, pages[index], t);
	if (!child) return;
	memoryManagement.insert(child);

	BTreeNode* sibling = readDisk(stream, pages[index + 1], t);
	if (!sibling) return;
	memoryManagement.insert(sibling);

	// add key into child
	child->keys[child->n] = keys[index];

	// case: child is leaf
	if (!child->leaf) {
		// add child of sibling into child
		child->pages[child->n + 1] = sibling->pages[0];
	}

	keys[index] = sibling->keys[0];

	//delete key and child in sibling
	for (int32_t i = 0; i < sibling->n - 1; i++) sibling->keys[i] = sibling->keys[i + 1];
	if (!sibling->leaf) {
		for (int32_t i = 0; i < sibling->n; i++) sibling->pages[i] = sibling->pages[i + 1];
	}

	child->n++;
	sibling->n--;

	writeDisk(stream, this);
	writeDisk(stream, child);
	writeDisk(stream, sibling);
}

void BTreeNode::fill(fstream& stream, int32_t index)
{

	BTreeNode* node = nullptr;
	if (index != 0) {
		node = readDisk(stream, pages[index - 1], t);
		if (!node) return;
		memoryManagement.insert(node);

		if (node->n >= t) {
			borrowFromLeft(stream, index);
			return;
		}
	}

	if (index != n) {
		node = readDisk(stream, pages[index + 1], t);
		if (!node) return;
		memoryManagement.insert(node);

		if (node->n >= t) {
			borrowFromRight(stream, index);
			return;
		}
	}

	if (index != n)
		merge(stream, index);
	else
		merge(stream, index - 1);
}

void BTreeNode::removeFromLeaf(fstream& stream, int32_t index) {
	for (int32_t i = index; i < n - 1; i++) keys[i] = keys[i + 1];
	n--;
	writeDisk(stream, this);
}

void BTreeNode::removeFromNonLeaf(fstream& stream, int32_t index) {
	int32_t key = keys[index];

	BTreeNode* leftNode = readDisk(stream, pages[index], t);
	if (!leftNode) return;
	memoryManagement.insert(leftNode);

	if (leftNode->n >= t) {
		int32_t left = getLeft(stream, index);
		keys[index] = left;
		leftNode->remove(stream, left);
		writeDisk(stream, this);
		return;
	}

	BTreeNode* rightNode = readDisk(stream, pages[index + 1], t);
	if (!rightNode) return;
	memoryManagement.insert(rightNode);

	if (rightNode->n >= t) {
		int32_t right = getRight(stream, index);
		keys[index] = right;
		rightNode->remove(stream, right);
		writeDisk(stream, this);
		return;
	}

	merge(stream, index);

	// Maybe lefNode just update, should read again from file
	leftNode = readDisk(stream, pages[index], t);
	if (!leftNode) return;
	memoryManagement.insert(leftNode);
	leftNode->remove(stream, key);
}

void BTreeNode::remove(fstream& stream, int32_t key) {
	int32_t index = findKey(key);

	if (index < n && keys[index] == key) {
		if (leaf) removeFromLeaf(stream, index);
		else removeFromNonLeaf(stream, index);

		return;
	}

	if (leaf) {
		cout << "Can't find Key in tree!!!" << endl;
		return;
	}

	bool flag = (index == n);

	BTreeNode* node = readDisk(stream, pages[index], t);
	if (!node) return;
	memoryManagement.insert(node);

	if (node->n < t) fill(stream, index);

	if (flag && n < index) index--;

	// Maybe node just update => update from file.
	node = readDisk(stream, pages[index], t);
	if (!node) return;
	memoryManagement.insert(node);
	node->remove(stream, key);
}

BTree::BTree(int32_t t) {
	pRoot = nullptr;
	this->t = t;
	count = 0;
	stream.open("data.bin", fstream::in | fstream::out | fstream::binary);
	if (!stream.is_open()) throw runtime_error("Can't open file\n");
}

BTree::BTree(string btreeFile) {
	ifstream is(btreeFile,fstream::binary);

	if (!is.is_open()) {
		throw runtime_error("Can't open file");
	}

	try {
		is.seekg(0, ios_base::beg);

		is.read((char*)(&t), sizeof(int32_t));

		is.read((char*)(&count), sizeof(int32_t));

		// read pRoot
		pRoot = new BTreeNode();

		is.read((char*)(&pRoot->pageNumber), sizeof(int32_t));
		is.read((char*)(&pRoot->n), sizeof(int32_t));
		is.read((char*)(&pRoot->t), sizeof(int32_t));
		is.read((char*)(&pRoot->leaf), sizeof(int32_t));

		pRoot->keys = new int32_t[2 * pRoot->t - 1]{};
		pRoot->pages = new int32_t[2 * pRoot->t]{};

		is.read((char*)pRoot->keys, sizeof(int32_t) * (2 * pRoot->t - 1));
		is.read((char*)pRoot->pages, sizeof(int32_t) * (2 * pRoot->t));
	}
	catch (...) {
		cout << "Failed allocation or can't read from disk" << endl;
		if (!pRoot) return;

		delete[] pRoot->keys;
		delete[] pRoot->pages;
		delete pRoot;

		return;
	}

	is.close();

	stream.open("data.bin", fstream::in | fstream::out | fstream::binary);
	if (!stream.is_open()) throw runtime_error("Can't open file\n");
}

void BTree::traversal() {
	if (!pRoot) {
		cout << "Empty!!";
		return;
	}
	pRoot->traversal(stream);

	clearMemory();
}

void BTree::bfs() {
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

		for (int32_t i = 0; i < u->n; i++)
			cout << u->keys[i] << " ";
		cout << "//";

		if (!u->leaf) {
			for (int32_t i = 0; i <= u->n; i++) {
				BTreeNode* node = readDisk(stream, u->pages[i], u->t);
				//memoryManagement.push(node);
				if (node) que2.push(node);
			}
		}

		if (u != pRoot) {
			delete u;
			u = nullptr;
		}

		if (que1.empty()) {
			cout << endl;
			if (que2.empty()) break;

			que1 = que2;
			que2 = queue<BTreeNode*>();
		}
	}
}

int32_t BTree::search(int32_t key) {
	if (!pRoot) return -1;
	int32_t res = pRoot->search(stream, key);

	cout << "Number read file: " << memoryManagement.size() << endl;
	clearMemory();
	return res;
}

void BTree::insert(int32_t key) {
	// case: key have already existed
	// if (search(key) != -1) return;

	//case: key haven't existed yet
	if (!pRoot) {
		pRoot = new BTreeNode(t, 1);
		pRoot->keys[0] = key;
		pRoot->pageNumber = 0;
		pRoot->n = 1;

		writeDisk(stream, pRoot);
		return;
	}

	if (pRoot->n == 2 * t - 1) {
		BTreeNode* newNode = nullptr;

		try {
			newNode = new BTreeNode(t, 0);
		}
		catch (...) {
			cout << "Failed allocation" << endl;
			return;
		}

		count++;
		newNode->pageNumber = count;
		newNode->pages[0] = pRoot->pageNumber;
		newNode->splitBTreeNode(stream, 0, pRoot);

		int32_t index = 0;

		if (newNode->keys[0] < key) index++;

		BTreeNode* node = readDisk(stream, newNode->pages[index], t);
		memoryManagement.insert(node);
		if (node) node->insertNonFull(stream, key);

		memoryManagement.insert(pRoot);
		pRoot = newNode;
	}
	else {
		pRoot->insertNonFull(stream, key);
	}

	clearMemory();
}

void BTree::remove(int32_t key) {
	if (!pRoot) {
		cout << "Tree is empty!!";
		return;
	}

	pRoot->remove(stream, key);

	if (pRoot->n == 0) {
		BTreeNode* temp = pRoot;
		if (pRoot->leaf) {
			pRoot = nullptr;
		}
		else {
			pRoot = readDisk(stream, pRoot->pages[0], t);
		}

		delete temp;
		temp = nullptr;
	}
}

void BTree::save() {
	ofstream os("BTree.bin", fstream::binary);
	os.seekp(0, ios_base::beg);
	os.write((char*)(&t), sizeof(int32_t));
	os.write((char*)(&count), sizeof(int32_t));

	os.write((char*)(&pRoot->pageNumber), sizeof(int32_t));
	os.write((char*)(&pRoot->n), sizeof(int32_t));
	os.write((char*)(&pRoot->t), sizeof(int32_t));
	os.write((char*)(&pRoot->leaf), sizeof(int32_t));
	os.write((char*)pRoot->keys, sizeof(int32_t) * (2 * pRoot->t - 1));
	os.write((char*)pRoot->pages, sizeof(int32_t) * (2 * pRoot->t));

	os.flush();
	os.close();
}

void BTree::close() {
	stream.close();
	
	//printRoot();
	
	save();
}

