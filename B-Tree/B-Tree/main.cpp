#include "Header.h"

using namespace std;

int main()
{	
	int t;
	cout << "Minimun degree t: ";
	cin >> t;

	BTree tree(t);

	int key;
	char c;


	for (int i = 0; i < 350; i++) {
		//system("cls");
		//tree.bfs();
		//cout << "-------------------------------------------------------" << endl;
		//cout << "Insert" << endl;
		//cout << "Enter key: ";
		//cin >> key;


		tree.insert(i);

		//system("cls");
		//tree.bfs();
		//cout << "-------------------------------------------------------" << endl;

		//cout << "(y) to continue, other to stop: ";
		//cin >> c;
		//if (c != 'y' && c != 'Y') break;
	}

	while (true) {
		system("cls");
		tree.bfs();
		cout << "-------------------------------------------------------" << endl;
		cout << "remove" << endl;
		cout << "Enter key: ";
		cin >> key;


		tree.remove(key);

		system("cls");
		tree.bfs();
		cout << "-------------------------------------------------------" << endl;

		cout << "(y) to continue, other to stop: ";
		cin >> c;
		if (c != 'y' && c != 'Y') break;
	}

	system("cls");
	tree.bfs();
	system("PAUSE");

	return 0;
}