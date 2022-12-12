#include"Header.h"

using namespace std;

int cnt = 0;

set<Node*> memoryManagement;

void clearMemory() {
	for (Node* node : memoryManagement) {
		if (!node) continue;

		delete node;
	}

	memoryManagement.clear();
}

int getSizeOfRecord() {
	return sizeof(int) * 5 + sizeof(bool);
}

int calcOffset(int pageNumber) {
	return getSizeOfRecord() * pageNumber;
}

void writeDisk(fstream& os, Node* node) {
	if (!node) return;
	try {
		os.seekp(calcOffset(node->pageNumber), ios_base::beg);

		os.write((char*)(&node->pageNumber), sizeof(int));
		os.write((char*)(&node->key), sizeof(int));
		os.write((char*)(&node->pParent), sizeof(int));
		os.write((char*)(&node->pLeft), sizeof(int));
		os.write((char*)(&node->pRight), sizeof(int));
		os.write((char*)(&node->isRed), sizeof(bool));
		

		os.flush();
	}
	catch (...) {
		cout << "Failed write file" << endl;
	}
}

// read data of node from disk and this node is inserted into memoryManagement
Node* readDisk(fstream& is, int pageNumber) {
	Node* node = nullptr;
	if (pageNumber == -1) return nullptr;
	try {
		is.seekg(calcOffset(pageNumber), ios_base::beg);

		node = new Node{};

		is.read((char*)(&node->pageNumber), sizeof(int));
		is.read((char*)(&node->key), sizeof(int));
		is.read((char*)(&node->pParent), sizeof(int));
		is.read((char*)(&node->pLeft), sizeof(int));
		is.read((char*)(&node->pRight), sizeof(int));
		is.read((char*)(&node->isRed), sizeof(bool));
	}
	catch (...) {
		cout << "Failed allocation or can't read from disk %%%" << endl;
		delete node;
		return nullptr;
	}

	memoryManagement.insert(node);

	return node;
}

Node* readDisk_nonManagement(fstream& is, int pageNumber) {
	Node* node = nullptr;
	if (pageNumber == -1) return nullptr;
	try {
		is.seekg(calcOffset(pageNumber), ios_base::beg);

		node = new Node{};

		is.read((char*)(&node->pageNumber), sizeof(int));
		is.read((char*)(&node->key), sizeof(int));
		is.read((char*)(&node->pParent), sizeof(int));
		is.read((char*)(&node->pLeft), sizeof(int));
		is.read((char*)(&node->pRight), sizeof(int));
		is.read((char*)(&node->isRed), sizeof(bool));
	}
	catch (...) {
		cout << "Failed allocation or can't read from disk %%%" << endl;
		delete node;
		return nullptr;
	}

	//memoryManagement.insert(node);

	return node;
}

void initTree(RBTree& tree) {
	tree.pRoot = nullptr;
}

Node* createNode(int key) {
	try {
		Node* node = new Node{ 0, key, -1, -1, -1, true };
		return node;
	}
	catch (...) {
		cout << "Failed allocation" << endl;
	}
}

//Node* leftRotate(fstream& stream, Node* y)
//{
//	Node* x = nullptr, *t = nullptr;
//	
//	x = readDisk(stream, y->pRight);
//	if (x->pLeft != -1) t = readDisk(stream, x->pLeft);
//
//	
//	y->pRight = x->pLeft;
//	if (x->pLeft != -1) //t != null
//	{
//		t->pParent = y->pageNumber;
//	}
//
//	x->pLeft = y->pageNumber;
//	y->pParent = x->pageNumber;
//	writeDisk(stream, y);
//  writeDisk(stream, x);
//}
//
//Node* rightRotate(fstream& stream, Node* y)
//{
//	Node* x = nullptr, * t = nullptr;
//
//	x = readDisk(stream, y->pLeft);
//	if (x->pRight != -1) t = readDisk(stream, x->pRight);
//
//	y->pLeft = x->pRight;
//	if (x->pRight != -1) //t != null
//	{
//		t->pParent = y->pageNumber;
//	}
//
//	x->pRight = y->pageNumber;
//	y->pParent = x->pageNumber;
//	writeDisk(stream, y);
//  writeDisk(stream, x);
// 
//}

int heightNode(fstream& stream, Node* pRoot) {
	if (!pRoot) {
		return -1;
	}
	Node* left = nullptr, * right = nullptr;
	if (pRoot->pLeft != -1) left = readDisk(stream, pRoot->pLeft);
	if (pRoot->pRight != -1) right = readDisk(stream, pRoot->pRight);

	return max(heightNode(stream, left), heightNode(stream, right)) + 1;
}

Node* rightRotate_Advanced(Node*& pRoot, fstream& stream, Node*& y)
{
	Node* x = nullptr, * t = nullptr, * par = nullptr;

	x = readDisk(stream, y->pLeft);
	if (x->pRight != -1) t = readDisk(stream, x->pRight);
	if (y->pParent != -1) par = readDisk(stream, y->pParent);

	y->pLeft = x->pRight; //t
	if (x->pRight != -1) //t != null
	{
		t->pParent = y->pageNumber;
	}
	bool temp = x->isRed;
	x->isRed = y->isRed;
	y->isRed = temp;

	x->pParent = y->pParent;
	if (!par || y->pParent == -1 || y->pageNumber == pRoot->pageNumber) //y is Root, excess conditions...	
	{
		pRoot = x;
	}
	else if (y->pageNumber == par->pRight)
	{
		par->pRight = x->pageNumber;
	}
	else
	{
		par->pLeft = x->pageNumber;
	}

	x->pRight = y->pageNumber;
	y->pParent = x->pageNumber;

	writeDisk(stream, y);
	writeDisk(stream, x);
	writeDisk(stream, par);
	writeDisk(stream, t);

	/*if (par && par->pageNumber == pRoot->pageNumber)
	{
		pRoot = readDisk_nonManagement(stream, par->pageNumber);
	}*/

	return x;
}

Node* leftRotate_Advanced(Node*& pRoot, fstream& stream, Node*& y)
{
	Node* x = nullptr, * t = nullptr, * par = nullptr;

	x = readDisk(stream, y->pRight);
	if (x->pLeft != -1) t = readDisk(stream, x->pLeft);
	if (y->pParent != -1) par = readDisk(stream, y->pParent);

	y->pRight = x->pLeft; //t
	if (x->pLeft != -1) //t != null
	{
		t->pParent = y->pageNumber;
	}

	x->pParent = y->pParent;

	bool temp = x->isRed;
	x->isRed = y->isRed;
	y->isRed = temp;

	if (!par || y->pParent == -1 || y->pageNumber == pRoot->pageNumber) //y is Root	
	{
		pRoot = x;
	}
	else if (y->pageNumber == par->pRight)
	{
		par->pRight = x->pageNumber;
	}
	else
	{
		par->pLeft = x->pageNumber;
	}

	x->pLeft = y->pageNumber;
	y->pParent = x->pageNumber;

	writeDisk(stream, y);
	writeDisk(stream, x);
	writeDisk(stream, par);
	writeDisk(stream, t);

	/*if (par && par->pageNumber == pRoot->pageNumber)
	{
		pRoot = readDisk_nonManagement(stream, par->pageNumber);
	}*/

	return x;
}

Node* parent(fstream& stream, Node*& node)
{
	if (node->pParent == -1)
	{
		return nullptr;
	}
	return readDisk(stream, node->pParent);
}

Node* grandparent(fstream& stream, Node*& node) {
	Node* par = nullptr;

	if (node->pParent == -1) {
		return nullptr;
	}
	par = readDisk(stream, node->pParent);
	if (par->pParent == -1) {
		return nullptr;
	}

	return readDisk(stream, par->pParent); //grandpar
}

Node* uncle(fstream& stream, Node*& node) {
	Node * grandpar = nullptr;

	grandpar = grandparent(stream, node);

	if (!grandpar) {
		return nullptr;
	}
	if (node->pParent == -1)
	{
		return nullptr;
	}
	
	if (readDisk(stream, node->pParent)->pageNumber == grandpar->pLeft) //par == grand->left => uncle = grand->right
	{
		if (grandpar->pRight == -1) return nullptr;
		return readDisk(stream, grandpar->pRight);
	}

	if (grandpar->pLeft == -1) return nullptr;
	return readDisk(stream, grandpar->pLeft);
}

Node* sibling(fstream& stream, Node*& node) {
	if (node->pParent == -1) return nullptr;

	Node* par = readDisk(stream, node->pParent);

	if (node->pageNumber == par->pLeft) {
		if (par->pRight == -1) return nullptr;
		return readDisk(stream, par->pRight);
	}
	if (par->pLeft == -1) return nullptr;
	return readDisk(stream, par->pLeft);
}

void insertFix(fstream& stream, Node*& pRoot, Node*& node)
{
	//handle red conflict

	Node* temp = pRoot;
	Node* u = nullptr;
	Node* gr = nullptr;
	Node* par = parent(stream, node);

	while (par && par->isRed)
	{
		u = uncle(stream, node);
		gr = grandparent(stream, node);

		if (!gr)
		{
			return;
		}

		if (u && u->isRed) //par & uncle are both red => par, uncle are black & grandpa is black 
		{
			u->isRed = false; 
			par->isRed = false; 
			gr->isRed = true;

			writeDisk(stream, u);
			writeDisk(stream, par);
			writeDisk(stream, gr);

			node = gr;
		}
		else //par is red, uncle is black => rotate...
		{

			if (node->pParent == gr->pRight) {
				par = parent(stream, node);
				if (node->pageNumber == par->pLeft) {
					node = par;
					node = rightRotate_Advanced(pRoot,stream, node);

				}
				node = gr;
				node = leftRotate_Advanced(pRoot, stream, node);
			}
			else
			{
				par = parent(stream, node);
				if (node->pageNumber == par->pRight) {
					node = par;
					node = leftRotate_Advanced(pRoot, stream, node);
				}
				node = gr;
				node = rightRotate_Advanced(pRoot, stream, node);
			}
		}

		par = parent(stream, node);
		if (!par) break;

		writeDisk(stream, u);
		writeDisk(stream, par);
		writeDisk(stream, gr);
		writeDisk(stream, node);

		if (!node->isRed) {
			break;
		}
	}
	/*if (temp->key != pRoot->key)
	{
		
	}
	else pRoot = readDisk_nonManagement(stream, pRoot->pageNumber);*/
	
	memoryManagement.insert(temp);
	pRoot = readDisk_nonManagement(stream, pRoot->pageNumber);
}

Node* insertFromNonRoot(fstream& stream, int32_t& pageNumber, int32_t key, Node* par)
{
	Node* newNode = nullptr;
	if (pageNumber < 0) {
		
		newNode = createNode(key);
		cnt++;
		newNode->pageNumber = cnt;
		newNode->pParent = par->pageNumber;
		memoryManagement.insert(newNode);
		pageNumber = cnt;
		writeDisk(stream, newNode);

		return newNode;
	}

	Node* node = readDisk(stream, pageNumber);

	if (node->key > key) newNode = insertFromNonRoot(stream, node->pLeft, key, node);
	else if (node->key < key) newNode = insertFromNonRoot(stream, node->pRight, key, node);
	else if (node->key == key) newNode = node;

	pageNumber = node->pageNumber;
	writeDisk(stream, node);

	return newNode;
}

void insertFromRoot(fstream& stream, Node*& pRoot, int key) {
	Node* newNode = nullptr;

	if (!pRoot) {
		pRoot = createNode(key);
		pRoot->pageNumber = cnt;
		pRoot->isRed = false;
		writeDisk(stream, pRoot);


		return;
	}
	if (pRoot->key == key) {
		return;
	}
	if (pRoot->key > key) {
		newNode = insertFromNonRoot(stream, pRoot->pLeft, key, pRoot);
		//root->left->parent = root;
	}
	else {
		newNode = insertFromNonRoot(stream, pRoot->pRight, key, pRoot);
		//root->right->parent = root;
	}
	writeDisk(stream, pRoot);

	insertFix(stream, pRoot, newNode);
	pRoot->isRed = false; //pRoot is always black
	writeDisk(stream, pRoot);

	clearMemory();
}

void deleteFix(fstream& stream, Node*& node, Node*& pRoot, Node*& par) {
	Node* sib = sibling(stream, node);
	Node* sib_left = nullptr, * sib_right = nullptr;
	Node* temp = pRoot;

	if (par && node->isRed == false) {
		if (sib && sib->isRed == false) {
			if (sib->pLeft != -1) sib_left = readDisk(stream, sib->pLeft);
			if (sib->pRight != -1) sib_right = readDisk(stream, sib->pRight);

			if (par->isRed == false) {
				if (sib_left) {
					sib->isRed = true;
					sib_left->isRed = false;
				}
				if (sib_right) {
					sib->isRed = true;
					sib_right->isRed = false;
				}
			}
			else {
				if (node->pageNumber == par->pLeft) {
					if (sib_right) {
						sib_right->isRed = false;
					}
					if(sib_left) sib_left->isRed = false;
					//else sib_left->isRed = false;
				}
				else {
					if (sib_left) {
						sib_left->isRed = false;
					}
					if (sib_right) sib_right->isRed = false;
					//else sib_right->isRed = false;
				}
			}
			writeDisk(stream, sib);
			writeDisk(stream, sib_left);
			writeDisk(stream, sib_right);
		}
		if (node->pParent == par->pLeft) {
			if (sib_left && !sib_right) {
				sib = rightRotate_Advanced(pRoot, stream, sib);
			}
			leftRotate_Advanced(pRoot, stream, par);
		}
		else {
			if (sib_right && !sib_left) {
				sib = leftRotate_Advanced(pRoot, stream, sib);
			}
			rightRotate_Advanced(pRoot, stream, par);
		}
	}
	//writeDisk(stream, par);
	
}

void deleteFromNonRoot(fstream& stream, int& pageNumber, int32_t key, Node*& pRoot, Node*& par)
{
	if (pageNumber == -1) return;

	Node* node = readDisk(stream, pageNumber);

	if (node->key > key) deleteFromNonRoot(stream, node->pLeft, key, pRoot, node);
	else if (node->key < key) deleteFromNonRoot(stream, node->pRight, key, pRoot, node);
	else if (node->key == key) {
		if (node->pRight < 0 && node->pLeft < 0) {
			deleteFix(stream, node, pRoot, par);
			pageNumber = -1;
			return;
		}

		if (node->pRight == -1 || heightNode(stream, readDisk(stream, node->pLeft)) > heightNode(stream, readDisk(stream, node->pRight)))
		{
			Node* decentdant = readDisk(stream, node->pLeft);
			while (decentdant->pRight >= 0) decentdant = readDisk(stream, decentdant->pRight);
			node->key = decentdant->key;
			writeDisk(stream, node);
			deleteFromNonRoot(stream, node->pLeft, decentdant->key, pRoot, node);
		}
		else {
			Node* decentdant = readDisk(stream, node->pRight);
			while (decentdant->pLeft >= 0) decentdant = readDisk(stream, decentdant->pLeft);
			node->key = decentdant->key;
			//if()
			writeDisk(stream, node);
			deleteFromNonRoot(stream, node->pRight, decentdant->key, pRoot, node);
		}
	}
	pageNumber = node->pageNumber;

	writeDisk(stream, node);
}

void deleteFromRoot(fstream& stream, Node*& pRoot, int key) {
	if (!pRoot) return;

	Node* temp = pRoot;
	Node* parRoot = nullptr;

	if (key < pRoot->key) deleteFromNonRoot(stream, pRoot->pLeft, key, pRoot, pRoot);
	else if(key > pRoot->key) deleteFromNonRoot(stream, pRoot->pRight, key, pRoot, pRoot);
	else
	{
		if (pRoot->pRight < 0 && pRoot->pLeft < 0) {
			
			deleteFix(stream, pRoot, pRoot, parRoot);
			delete pRoot;
			pRoot = nullptr;
			return;
		}

		if (pRoot->pRight == -1 || heightNode(stream, readDisk(stream, pRoot->pLeft)) > heightNode(stream, readDisk(stream, pRoot->pRight)))
		{
			Node* decentdant = readDisk(stream, pRoot->pLeft);
			while (decentdant->pRight >= 0) decentdant = readDisk(stream, decentdant->pRight);
			pRoot->key = decentdant->key;
			writeDisk(stream, pRoot);
			deleteFromNonRoot(stream, pRoot->pLeft, decentdant->key, pRoot, pRoot);
		}
		else {
			Node* decentdant = readDisk(stream, pRoot->pRight);
			while (decentdant->pLeft >= 0) decentdant = readDisk(stream, decentdant->pLeft);
			pRoot->key = decentdant->key;
			writeDisk(stream, pRoot);
			deleteFromNonRoot(stream, pRoot->pRight, decentdant->key, pRoot, pRoot);
		}

	}

	writeDisk(stream, pRoot);
	if (temp->key != pRoot->key) {
		memoryManagement.insert(temp);
		pRoot = readDisk_nonManagement(stream, pRoot->pageNumber);
	}
	pRoot->isRed = false;

	clearMemory();
}

int searchFromNonRoot(fstream& stream, int pageNumber, int key) {
	if (pageNumber < 0) return -1;

	Node* node = readDisk(stream, pageNumber);
	if (node->key > key) return searchFromNonRoot(stream, node->pLeft, key);
	else if (node->key < key) return searchFromNonRoot(stream, node->pRight, key);
	else if (node->key == key) return node->pageNumber;
}

void searchFromRoot(fstream& stream, Node* pRoot, int key) {
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

void bfs(fstream& stream, const RBTree& tree) {
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
		cout << u->key;  
		if (u->isRed) cout << "(R)";
		else cout <<"(B)";
		cout << " ";

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

void writeTree(fstream& os, Node* pRoot) {
	if (!pRoot) return;
	os.write((char*)(&pRoot->pageNumber), sizeof(int));
	os.write((char*)(&pRoot->key), sizeof(int));
	os.write((char*)(&pRoot->pParent), sizeof(int));
	os.write((char*)(&pRoot->pLeft), sizeof(int));
	os.write((char*)(&pRoot->pRight), sizeof(int));
	os.write((char*)(&pRoot->isRed), sizeof(bool));

	os.write((char*)(&cnt), sizeof(int));
}

void readTree(fstream& is, Node*& pRoot) {
	pRoot = createNode(0);

	is.read((char*)(&pRoot->pageNumber), sizeof(int));
	is.read((char*)(&pRoot->key), sizeof(int));
	is.read((char*)(&pRoot->pParent), sizeof(int));
	is.read((char*)(&pRoot->pLeft), sizeof(int));
	is.read((char*)(&pRoot->pRight), sizeof(int));
	is.read((char*)(&pRoot->isRed), sizeof(bool));

	is.read((char*)(&cnt), sizeof(int));
}

void clearTree(fstream& os, Node*& pRoot) {
	writeTree(os, pRoot);

	delete pRoot;
	pRoot = nullptr;
}