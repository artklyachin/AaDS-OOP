#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <random>
#include <assert.h>
#include <algorithm>


using namespace std;


struct Node
{
    Node(int x)
    {
        key = x;
    }
    int key, height = 0;
    Node* Left = nullptr;
    Node* Right = nullptr;
};


class BinTree {
public:
    BinTree();
    const BinTree operator() (const BinTree& h) = delete;
    const BinTree operator= (const BinTree& h) = delete;
    ~BinTree();
    void Add(int x);
    void Delete(int x);
    bool Exists(int x) const;
    int Next(int x, bool& flag) const;
    int Prev(int x, bool& flag) const;
private:
    Node* root = nullptr;
    void deleteTree(Node* tree);
    Node* addToTree(int x, Node* node);
    Node* deleteElem(Node* node, int x);
    pair<Node*, int> FindAndRemoveMinInTree(Node* node);
    Node* Rotate(Node* node);
    Node* rotateLeft(Node* node);
    Node* rotateRight(Node* node);
    void Recalculation(Node* node);
    int Balance_difference(Node* node);
};


BinTree::BinTree() {}


BinTree::~BinTree()
{
    deleteTree(root);
}


void BinTree::deleteTree(Node* tree)
{
    if (tree == nullptr) {
        return;
    }
    deleteTree(tree->Left);
    deleteTree(tree->Right);
    delete tree;
};


void BinTree::Add(int x)
{
    root = addToTree(x, root);
}


Node* BinTree::addToTree(int x, Node* root)
{
    if (root == nullptr) {
        root = new Node(x);
        return root;
    }
    if (x == root->key) {
        return root;
    }
    if (x < root->key) {
        root->Left = addToTree(x, root->Left);
    } else {
        root->Right = addToTree(x, root->Right);
    }
    root = Rotate(root);
    return root;
}


bool BinTree::Exists(int x) const
{
    Node* node = root;
    while (node != nullptr) {
        if (x == node->key) {
            return true;
        }
        if (x < node->key) {
            node = node->Left;
        } else {
            node = node->Right;
        }
    }
    return false;
}


int BinTree::Next(int x, bool& flag) const
{
    Node* node = root;
    Node* bestnode = nullptr;
    while (node != nullptr) {
        if (x < node->key) {
            bestnode = node;
            node = node->Left;
        } else {
            node = node->Right;
        }
    }
    if (bestnode == nullptr) {
        flag = true;
        return 0;
    }
    flag = false;
    return bestnode->key;
}


int BinTree::Prev(int x, bool& flag) const
{
    Node* node = root;
    Node* bestnode = nullptr;
    while (node != nullptr) {
        if (x > node->key) {
            bestnode = node;
            node = node->Right;
        } else {
            node = node->Left;
        }
    }
    if (bestnode == nullptr) {
        flag = true;
        return 0;
    }
    flag = false;
    return bestnode->key;
}


void BinTree::Delete(int x)
{
    root = deleteElem(root, x);
}


pair<Node*, int> BinTree::FindAndRemoveMinInTree(Node* node)
{
    if (node->Left == nullptr) {
        Node* part = node->Right;
        int min = node->key;
        delete node;
        return { part, min };
    }
    auto result = FindAndRemoveMinInTree(node->Left);
    node->Left = result.first;
    return { Rotate(node), result.second };
}


Node* BinTree::deleteElem(Node* root, int x)
{
    if (root == nullptr) return nullptr;
    if (x < root->key) {
        root->Left = deleteElem(root->Left, x);
        return Rotate(root);
    } else if (x > root->key) {
        root->Right = deleteElem(root->Right, x);
        return Rotate(root);
    } else {
        Node* left = root->Left;
        Node* right = root->Right;
        delete root;
        if (right == nullptr) {
            return left;
        }
        auto result = FindAndRemoveMinInTree(right);
        Node* mewroot = new Node(result.second);
        mewroot->Right = result.first;
        mewroot->Left = left;
        return Rotate(mewroot);
    }
}


void BinTree::Recalculation(Node* node)
{
    if (node->Left == nullptr && node->Right == nullptr) {
        node->height = 0;
    } else if (node->Left != nullptr && node->Right == nullptr) {
        node->height = node->Left->height + 1;
    } else if (node->Left == nullptr && node->Right != nullptr) {
        node->height = node->Right->height + 1;
    } else if (node->Left != nullptr && node->Right != nullptr) {
        node->height = max(node->Left->height, node->Right->height) + 1;
    }
}



int BinTree::Balance_difference(Node* root)
{
    if (root->Left == nullptr && root->Right == nullptr) {
        return 0;
    } else if (root->Left != nullptr && root->Right == nullptr) {
        return -(root->Left->height) - 1;
    } else if (root->Left == nullptr && root->Right != nullptr) {
        return root->Right->height + 1;
    } else if (root->Left != nullptr && root->Right != nullptr) {
        return -(root->Left->height) + root->Right->height;
    }
}


Node* BinTree::Rotate(Node* root)
{
    Recalculation(root);
    while (Balance_difference(root) >= 2) {
        if (Balance_difference(root->Right) < 0)
            root->Right = rotateRight(root->Right);
        return rotateLeft(root);
    }
    if (Balance_difference(root) <= -2) {
        if (Balance_difference(root->Left) > 0)
            root->Left = rotateLeft(root->Left);
        return rotateRight(root);
    }
    return root;
}


Node* BinTree::rotateRight(Node* root)
{
    Node* newroot = root->Left;
    root->Left = newroot->Right;
    newroot->Right = root;
    Recalculation(root);
    Recalculation(newroot);
    return newroot;
}

Node* BinTree::rotateLeft(Node* root)
{
    Node* newroot = root->Right;
    root->Right = newroot->Left;
    newroot->Left = root;
    Recalculation(root);
    Recalculation(newroot);
    return newroot;
}

/*void Print_tree(Node* root)
{
    int count = 1;
    while (count != 0) {

    }
}
*/

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    BinTree tree;
    string s;
    while (cin >> s) {
        int x;
        cin >> x;
        if (s == "insert") {
            tree.Add(x);
        } else if (s == "delete") {
            tree.Delete(x);
        } else if (s == "exists") {
            bool result = tree.Exists(x);
            if (result) {
                cout << "true" << endl;
            } else {
                cout << "false" << endl;
            }
        } else if (s == "next") {
            bool flag;
            int result = tree.Next(x, flag);
            if (flag) {
                cout << "none" << endl;
            } else {
                cout << result << endl;
            }
        } else if (s == "prev") {
            bool flag;
            int result = tree.Prev(x, flag);
            if (flag) {
                cout << "none" << endl;
            } else {
                cout << result << endl;
            }
        }
    }
    return 0;
}

/*

insert 2
insert 5
insert 3
exists 2
exists 4
next 4
prev 4
delete 5
next 4
prev 4
*/