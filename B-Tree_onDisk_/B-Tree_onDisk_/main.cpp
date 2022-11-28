#include "Header.h"

int main()
{
	int choice = 0;
	int32_t key = 0;
	int32_t numberOfItems = 0;
	char c = '\n';
	int res = 0;

	
	
	while(true) {
		system("cls");

		cout << "Start B-Tree:..." << endl;
		cout << "1. New B-Tree" << endl;
		cout << "2. Recent B-Tree" << endl;
		cin >> choice;

		if (choice == 1) {
			cout << "Are you sure? It will make lost recently B-Tree" << endl;
			cout << "(y) to continue, other to escape" << endl;
			cin >> c;
			if (c != 'y' && c != 'Y') continue;
		}

		if (choice >= 1 && choice <= 2) break;
	} 

	BTree* tree = nullptr;

	if (choice == 1) {
		int32_t t;
		cout << "Minimun degree t: ";
		cin >> t;
		tree = new BTree(t);
	}
	else {
		tree = new BTree("BTree.bin");
	}



	while (true) {
		system("cls");
	//	tree->printRoot();
		cout << "-------------------------------------------------------" << endl;
		cout << "1. Generate B-Tree (Key from 0 -> n-1)" << endl;
		cout << "2. Insert" << endl;
		cout << "3. Delete" << endl;
		cout << "4. Search" << endl;
		cout << "5. Print B-Tree" << endl;
		cout << "6. Print root" << endl;
		cout << "7. Save B-Tree" << endl;
		cout << "0. Exit" << endl;

		cin >> choice;

		if (choice < 0 || choice > 7) continue;

		if (choice == 0) break;

		switch (choice) {
		case 1:
			cout << "-------------------------------------------------------" << endl;
			cout << "Generate B-Tree (Key from 0 -> n-1)" << endl;
			
			cout << "Are you sure? Only recommand with new B-Tree case!!!" << endl;
			cout << "(y) to continue, other to escape" << endl;
			cin >> c;
			if (c != 'y' && c != 'Y') break;

			cout << "Number of items: " << endl;
			cin >> numberOfItems;
			for (key = 0; key < numberOfItems; key++) {

				tree->insert(key);
			}	
			break;
		case 2:
			cout << "-------------------------------------------------------" << endl;
			cout << "Insert" << endl;
			cout << "Enter key" << endl;
			cin >> key;
			tree->insert(key);
			break;
		case 3:
			cout << "-------------------------------------------------------" << endl;
			cout << "Remove" << endl;
			cout << "Enter key" << endl;
			cin >> key;
			tree->remove(key);
			break;
		case 4:
			cout << "-------------------------------------------------------" << endl;
			cout << "Search" << endl;
			cout << "Enter key: ";
			cin >> key;
			res = tree->search(key);
			cout << res << endl;
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
			tree->bfs();
			system("PAUSE");
			break;
		case 6:
			cout << "-------------------------------------------------------" << endl;
			cout << "Print root" << endl;
			tree->printRoot();
			system("PAUSE");
			break;
		case 7:
			cout << "-------------------------------------------------------" << endl;
			cout << "Save B-Tree" << endl;
			tree->save();
			break;
		}
	}

	tree->close();
	delete tree;
	tree = nullptr;

	return 0;
}