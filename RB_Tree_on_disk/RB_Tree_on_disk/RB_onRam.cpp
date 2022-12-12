#include <iostream>
#include <cmath>
using namespace std;




// STRUCTURE

struct tNode {
	int key;
	tNode* left;
	tNode* right;
	tNode* parent;
	int color;
};

using tree = tNode*;




// DEFINITION

void init(tree& root) {
	root = nullptr;
}

tNode* createNode(const int& x) {
	return new tNode{ x, nullptr, nullptr, nullptr, 1 };
}

int heightNode(tree& root) {
    if (!root) {
        return -1;
    }
    return max(heightNode(root->left), heightNode(root->right)) + 1;
}

tNode* grandparent(tNode*& node) {
    if (!node->parent) {
        return nullptr;
    }
    return node->parent->parent;
}

tNode* uncle(tNode*& node) {
    if (!node->parent || !grandparent(node)) {
        return nullptr;
    }
    if (node->parent == grandparent(node)->left) {
        return grandparent(node)->right;
    }
    return grandparent(node)->left;
}

tNode* sibling(tNode*& node) {
    if (!node->parent) {
        return nullptr;
    }
    if (node == node->parent->left) {
        return node->parent->right;
    }
    return node->parent->left;
}

tNode* treeRoot(tNode*& temp) {
    while (temp->parent) {
        temp = temp->parent;
    }
    temp->color = 0;
    return temp;
}




// ROTATION

tNode* turnLeft(tNode*& root) {
    tNode* child = root->right->left;
    tNode* par = root->parent;

    root->right->left = root;
    root = root->right;

    root->left->right = child;
    if (child) child->parent = root->left;

    root->left->parent = root;
    root->parent = par;

    int temp = root->left->color;
    root->left->color = root->color;
    root->color = temp;

    if (!par) {
        return root;
    }
    if (par->left == root->left) {
        par->left = root;
    }
    else {
        par->right = root;
    }

    return root;
}

tNode* turnRight(tNode*& root) {
    tNode* child = root->left->right;
    tNode* par = root->parent;

    root->left->right = root;
    root = root->left;

    root->right->left = child;
    if (child) child->parent = root->right;

    root->right->parent = root;
    root->parent = par;

    int temp = root->right->color;
    root->right->color = root->color;
    root->color = temp;

    if (!par) {
        return root;
    }
    if (par->left == root->right) {
        par->left = root;
    }
    else {
        par->right = root;
    }
    return root;
}




// INSERT & DELETE

void insertFix(tree& root, tNode*& node) {
    tNode* u = nullptr;
    tNode* gr = nullptr;
    while (node->parent && node->parent->color == 1) {
        u = uncle(node);
        gr = grandparent(node);

        if (!gr) {
            return;
        }

        if (u && u->color == 1) {
            u->color = 0;
            node->parent->color = 0;
            gr->color = 1;
            node = gr;
        }
        else {
            if (node->parent == gr->right) {
                if (node == node->parent->left) {
                    node = node->parent;
                    node = turnRight(node);

                }
                node = gr;
                node = turnLeft(node);
            }
            else {
                if (node == node->parent->right) {
                    node = node->parent;
                    node = turnLeft(node);
                }
                node = gr;
                node = turnRight(node);

            }
        }

        if (!node->parent) {
            break;
        }

        if (!node->color) {
            break;
        }
    }
}

void deleteFix(tNode*& node) {
    tNode* sib = sibling(node);
    if (node->parent && node->color == 0) {
        if (sib->color == 0) {
            if (node->parent->color == 0) {
                if (sib->left) {
                    sib->color = 1;
                    sib->left->color = 0;
                }
                if (sib->right) {
                    sib->color = 1;
                    sib->right->color = 0;
                }
            }
            else {
                if (node == node->parent->left) {
                    if (sib->right) {
                        sib->right->color = 0;
                    }
                    else sib->left->color = 0;
                }
                else {
                    if (sib->left) {
                        sib->left->color = 0;
                    }
                    else sib->right->color = 0;
                }
            }
        }
        if (node == node->parent->left) {
            if (sib->left && !sib->right) {
                sib = turnRight(sib);
            }
            node->parent = turnLeft(node->parent);
        }
        else {
            if (sib->right && !sib->left) {
                sib = turnLeft(sib);
            }
            node->parent = turnRight(node->parent);
        }
    }
}

void insertNode(tree& root, tNode*& node) {
    if (!node) {
        return;
    }
    if (!root) {
        root = node;
        return;
    }
    if (root->key == node->key) {
        return;
    }
    if (root->key > node->key) {
        insertNode(root->left, node);
        root->left->parent = root;
    }
    else {
        insertNode(root->right, node);
        root->right->parent = root;
    }
}

tNode* minValue(tree& root) {
    tree temp = root;
    while (temp->left) {
        temp = temp->left;
    }
    return temp;
}

tNode* maxValue(tree& root) {
    tree temp = root;
    while (temp->right) {
        temp = temp->right;
    }
    return temp;
}

void deleteNode(tree& root, const int& x) {
    if (!root) {
        return;
    }
    if (root->key < x) {
        deleteNode(root->right, x);
        return;
    }
    if (root->key > x) {
        deleteNode(root->left, x);
        return;
    }
    tNode* temp = nullptr;
    if (!root->left && !root->right) {
        deleteFix(root);
        delete root;
        root = nullptr;
        return;
    }
    if (!root->right || heightNode(root->left) > heightNode(root->right)) {
        temp = maxValue(root->left);
        root->key = temp->key;
        deleteNode(root->left, temp->key);
        return;
    }
    temp = minValue(root->right);
    root->key = temp->key;
    deleteNode(root->right, temp->key);
}


void insertRB(tree& root, tNode*& node) {
    insertNode(root, node);
    insertFix(root, node);
    root = treeRoot(root);
}

void deleteRB(tree& root, const int& x) {
    deleteNode(root, x);
    root = treeRoot(root);
}




// INPUT & OUTPUT & FREE

void cinTree(tree& root) {
    int n = 0;
    cout << "   Enter number of elements: ";
    cin >> n;
    cout << "Enter tree: \n";
    int x = 0;
    for (int i = 1; i <= n; i++) {
        cin >> x;
        tNode* p = createNode(x);
        insertRB(root, p);
    }
}

void printTree(tree& root) {
    if (!root) {
        return;
    }
    cout << root->key << "  ";
    printTree(root->left);
    printTree(root->right);
}

void printLevel(tree& root, const int& level) {
    if (!root) {
        return;
    }
    if (level == 0) {
        cout << root->key << "(" << root->color << ")   ";
        return;
    }
    printLevel(root->left, level - 1);
    printLevel(root->right, level - 1);
}

void levelOrder(tree& root) {
    if (!root) {
        return;
    }
    int maxLevel = heightNode(root);
    cout << "\n";
    for (int i = 0; i <= maxLevel; i++) {
        printLevel(root, i);
        cout << "\n";
    }
}

void deleteTree(tree& root) {
    if (!root) {
        return;
    }
    if (root != nullptr) {
        deleteTree(root->left);
        deleteTree(root->right);
        delete root;
    }
}




//int main() {
//    tree root;
//    init(root);
//    cinTree(root);
//
//    levelOrder(root);
//    deleteRB(root, 2);
//    levelOrder(root);
//
//   deleteTree(root);
//   return 0;
//}

//11 3 14 2 7 5 1
//9 5 15 4 8 12 3 19 7 10 13 16 21 17
//9 8 7 6 5 4 3 2 11 14 12 13 10 15
