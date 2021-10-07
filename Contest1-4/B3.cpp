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


int64_t INF = 1e18 + 7;


struct Node
{
    Node(int64_t x) :
        key(x), sum_in_tree(x)
    { }
    int64_t key, sum_in_tree;
    Node* Parent = nullptr;
    Node* Left = nullptr;
    Node* Right = nullptr;
};


class SplayTree {
public:
    SplayTree();
    const SplayTree operator() (const SplayTree& h) = delete;
    const SplayTree operator= (const SplayTree& h) = delete;
    ~SplayTree();
    void Add(int64_t x);
    int64_t Sum(int64_t left, int64_t right);
private:
    Node* root = nullptr;
    void deleteTree(Node* tree);
    Node* addToTree(int64_t x, Node* node);
    Node* Rotate(Node* node);
    void zig(Node* c);
    void zigZig_zigZag(Node* c);
    void Recalculate(Node* node);
    int64_t count_elem_in_tree(Node* node);
    int64_t findSum(Node* node, int64_t l, int64_t r);
    pair<Node*, Node*>  split(int64_t x, Node* root);
    Node*  merge(Node* node1, Node* node2);
    Node* x_to_the_top(int64_t x, Node* v);
};


SplayTree::SplayTree() {}


SplayTree::~SplayTree()
{
    deleteTree(root);
}


void SplayTree::deleteTree(Node* node)
{
    if (node == nullptr) {
        return;
    }
    deleteTree(node->Left);
    deleteTree(node->Right);
    delete node;
};


void SplayTree::Add(int64_t x)
{
    root = addToTree(x, root);
}


Node* SplayTree::addToTree(int64_t x, Node* node)
{
    auto split_tree = split(x, node);
    Node* left = split_tree.first;
    Node* right = split_tree.second;
    if (left != nullptr && left->key == x) {
        left->Right = right;
        Recalculate(left);
        return left;
    } else {
        Node* newroot = new Node(x);
        newroot->Left = left;
        newroot->Right = right;
        Recalculate(newroot);
        return newroot;
    }
}



void SplayTree::Recalculate(Node* node)
{
    if (node == nullptr) {
        return;
    }
    node->sum_in_tree = node->key;
    if (node->Left != nullptr) {
        node->Left->Parent = node;
        node->sum_in_tree += node->Left->sum_in_tree;
    }
    if (node->Right != nullptr) {
        node->Right->Parent = node;
        node->sum_in_tree += node->Right->sum_in_tree;
    }
}


int64_t SplayTree::count_elem_in_tree(Node* node)
{
    if (node == nullptr) {
        return 0;
    }
    return node->sum_in_tree;
}



int64_t SplayTree::Sum(int64_t l, int64_t r)
{
    //return findSum(root, l, r);
    if (root == nullptr) {
        return 0;
    }
    auto split_tree1 = split(l - 1, root);
    Node* left = split_tree1.first;
    Node* right = split_tree1.second;
    auto split_tree2 = split(r, right);
    Node* central = split_tree2.first;
    right = split_tree2.second;
    int64_t ans = 0;
    if (central != nullptr) {
        ans = central->sum_in_tree;
    }
    right = merge(central, right);
    root = merge(left, right);
    return ans;
}


int64_t SplayTree::findSum(Node* node, int64_t l, int64_t r)
{
    if (node == nullptr) {
        return 0;
    }
    /*if (node->key > r) {
        return findSum(node->Left, l, r);
    }
    if (node->key < l) {
        return findSum(node->Right, l, r);
    }
    int64_t ans = node->key;
    if (l == -INF) {
        ans += count_elem_in_tree(node->Left);
    } else {
        ans += findSum(node->Left, l, INF);
    }
    if (r == INF) {
        ans += count_elem_in_tree(node->Right);
    } else {
        ans += findSum(node->Right, -INF, r);
    }
    return ans;*/
}


void SplayTree::zig(Node* c)
{
    Node* p = c->Parent;
    if (p->Left == c) {
        p->Left = c->Right;
        c->Right = p;
    } else {
        p->Right = c->Left;
        c->Left = p;
    }
    Node* g = p->Parent;
    if (g != nullptr) {
        if (g->Left == p) {
            g->Left = c;
        } else {
            g->Right = c;
        }
    }
    c->Parent = g;
    Recalculate(p);
    Recalculate(c);
}


void SplayTree::zigZig_zigZag(Node* c)
{
    Node* p = c->Parent;
    Node* g = p->Parent;
    if ((g->Left == p) && (p->Left == c) || (g->Right == p) && (p->Right == c)) { //zig-zig
        zig(p);
        zig(c);
    } else { //zig-zag
        zig(c);
        zig(c);
    };
}


Node* SplayTree::Rotate(Node* c)
{
    while (c->Parent != nullptr && c->Parent->Parent != nullptr) {
        zigZig_zigZag(c);
    }
    if (c->Parent != nullptr && c->Parent->Parent == nullptr) {
        zig(c);
    }
    return c;
}

pair<Node*, Node*>  SplayTree::split(int64_t x, Node* node)
{
    if (node == nullptr) {
        return { nullptr, nullptr };
    }
    node = x_to_the_top(x, node);
    if (node->key <= x) {
        Node* right = node->Right;
        if (right != nullptr) {
            right->Parent = nullptr;
        }
        node->Right = nullptr;
        node->sum_in_tree = node->key + count_elem_in_tree(node->Left);
        return { node, right };
    } else {
        Node* left = node->Left;
        if (left != nullptr) {
            left->Parent = nullptr;
        }
        node->Left = nullptr;
        node->sum_in_tree = node->key + count_elem_in_tree(node->Right);
        return {left, node};
    }
}


Node* SplayTree::merge(Node* node1, Node* node2)
{
    if (node1 == nullptr && node2 == nullptr) {
        return nullptr;
    }
    if (node1 == nullptr) {
        return node2;
    }
    if (node2 == nullptr) {
        return node1;
    }
    node1 = x_to_the_top(INF, node1);
    node2 = x_to_the_top(-INF, node2);
    node1->Right = node2;
    node2->Parent = node1;
    node1->sum_in_tree += node2->sum_in_tree;
    return node1;
}


Node* SplayTree::x_to_the_top(int64_t x, Node* v)
{
    if (v == nullptr) {
        return nullptr;
    }
    if (x < v->key && v->Left != nullptr) {
        v->Left = x_to_the_top(x, v->Left);
        zig(v->Left);
        return v->Parent;
    }
    if (x > v->key && v->Right != nullptr) {
        v->Right = x_to_the_top(x, v->Right);
        zig(v->Right);
        return v->Parent;
    }
    return v;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    SplayTree* tree = new SplayTree();
    int64_t q, mod = 1e9;
    cin >> q;
    int64_t res = 0;
    for (int64_t i = 0; i < q; ++i) {
        char c;
        cin >> c;
        if (c == '+') {
            int64_t x;
            cin >> x;
            x = (x + res) % mod;
            res = 0;
            tree->Add(x);
            //cout << "=" << x << endl;
        } else if (c == '?') {
            int64_t left, right;
            cin >> left >> right;
            res = tree->Sum(left, right);
            cout << res << endl;
        }
    }
    return 0;
}

/*
* 
2 2

5
+ 1
+ 0
+ 3
+ 0
? 3 4

10
+ 2
+ 1
+ 4
+ 3

10
+ 2
+ 1
+ 4
+ 5
+ 7
+ 3




6
+ 1
+ 3
+ 3
? 2 4
+ 1
? 2 4

*/