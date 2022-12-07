#include "Header.h"

int32_t main()
{
	int choice = 0;
	int32_t key = 0;
	int32_t numberOfItems = 0;
	char c = '\n';
	int res = 0;
	fstream is, os;

	while (true) {
		system("cls");

		cout << "Start AVL-Tree:..." << endl;
		cout << "1. New AVL-Tree" << endl;
		cout << "2. Recent AVL-Tree" << endl;
		cin >> choice;

		if (choice == 1) {
			cout << "Are you sure? It will make lost recently AVL-Tree" << endl;
			cout << "(y) to continue, other to escape" << endl;
			cin >> c;
			if (c != 'y' && c != 'Y') continue;
		}

		if (choice >= 1 && choice <= 2) break;
	}

	AVLTree tree;
	initTree(tree);
	
	if (choice == 2) {
		is.open("AVLTree.bin", fstream::in | fstream::binary);
		
		if (!is.is_open()) {
			cout << "Can't open file data.bin" << endl;
			return -1;
		}

		readAVLTree(is, tree.pRoot);
		is.close();
	}

	fstream stream("data.bin", fstream::in | fstream::out | fstream::binary);

	if (!stream.is_open()) {
		cout << "Can't open file data.bin" << endl;
		return -1;
	}

	while (true) {
		system("cls");
		//bfs(stream, tree);
		cout << "----------------------------------------------------------" << endl;
		cout << "1. Generate AVL-Tree (Key from 0 -> n-1)" << endl;
		cout << "2. Insert" << endl;
		cout << "3. Remove" << endl;
		cout << "4. Search" << endl;
		cout << "5. Print AVL-Tree" << endl;
		cout << "6. Save AVL-Tree" << endl;
		cout << "7. Exit" << endl;

		cin >> choice;
		if (choice < 0 || choice > 7) continue;
		if (choice == 7) break;

		switch (choice) {
		case 1:
			cout << "-------------------------------------------------------" << endl;
			cout << "Generate AVL-Tree (Key from 0 -> n-1)" << endl;

			cout << "Are you sure? Only recommand with new B-Tree case!!!" << endl;
			cout << "(y) to continue, other to escape" << endl;

			cin >> c;
			if (c != 'y' && c != 'Y') break;

			cout << "Number of items: " << endl;
			cin >> numberOfItems;
			for (key = 0; key < numberOfItems; key++) {
				insertFromRoot(stream, tree.pRoot, key);
			}

			break;
		case 2:
			cout << "-------------------------------------------------------" << endl;
			cout << "Insert" << endl;
			cout << "Enter key" << endl;
			cin >> key;
			insertFromRoot(stream, tree.pRoot, key);
			break;
		case 3:
			cout << "-------------------------------------------------------" << endl;
			cout << "Remove" << endl;
			cout << "Enter key" << endl;
			cin >> key;
			removeFromRoot(stream, tree.pRoot, key);
			break;
		case 4:
			cout << "-------------------------------------------------------" << endl;
			cout << "Search" << endl;
			cout << "Enter key: ";
			cin >> key;
			searchFromRoot(stream, tree.pRoot, key);
			system("PAUSE");
			break;
		case 5:
			cout << "-------------------------------------------------------" << endl;
			cout << "Print B-Tree" << endl;

			cout << "Are you sure? It will take long time to display" << endl;
			cout << "(y) to continue, other to escape" << endl;
			cin >> c;
			if (c != 'y' && c != 'Y') break;
			cout << "------------------------------------------------------" << endl;
			bfs(stream, tree);
			system("PAUSE");
			break;
		case 6:
			os.open("AVLTree.bin", fstream::out | fstream::binary);
			writeAVLTree(os, tree.pRoot);
			os.close();
		}
	}

	os.open("AVLTree.bin", fstream::out | fstream::binary);
	clearTree(os, tree.pRoot);

	os.close();
	stream.close();
	return 0;
}

/*
1
100
1
200
1
300
1
400
1
500
1
600
1
700
1
800
1
900
1
1000
1
1100
*/