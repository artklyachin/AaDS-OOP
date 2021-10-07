#include <iostream>
#include <algorithm>
#include <cassert>
#include <set>
#include <vector>


using namespace std;


class Tree23
{
    struct Node
    {
        Node* Parent = nullptr;
        Node* Sons[4] = {};
        int64_t length = 0; 
        int64_t maxKey = 0;
        int64_t _size = 0; 

        Node(Node* Parent_of_node, int64_t key)
            : Parent(Parent_of_node), length(0), maxKey(key), _size(1)
        {
        }

        Node(Node* Parent_of_node, Node* son0, Node* son1)
            : Parent(Parent_of_node), Sons{ son0, son1 }, length(2), _size(son0->_size + son1->_size)
        {
            son0->Parent = this;
            son1->Parent = this;
            if (son0->maxKey > son1->maxKey) {
                std::swap(Sons[0], Sons[1]);
            }
            maxKey = son1->maxKey;
        }

        bool isLeaf() const
        {
            return length == 0;
        }

        void addSon(Node* newNode)
        {
            assert(newNode && length <= 3);
            newNode->Parent = this;
            Sons[length++] = newNode;
            std::sort(Sons, Sons + length, [](const Node* lhs, const Node* rhs) { return lhs->maxKey < rhs->maxKey; });
            maxKey = Sons[length - 1]->maxKey;
            _size += newNode->_size;
        }

        void eraseSon(Node* node)
        {
            assert(length > 1);
            int64_t pos = std::find(Sons, Sons + length, node) - Sons;
            assert(pos >= 0 && pos < length);
            --length;
            for (int64_t i = pos; i < length; ++i) {
                Sons[i] = Sons[i + 1];
            }
            _size -= node->_size;
            Sons[length] = nullptr;
            maxKey = Sons[length - 1]->maxKey;
        }

        void update()
        {
            assert(length >= 1);
            _size = 0;
            for (int64_t i = 0; i < length; ++i) {
                _size += Sons[i]->_size;
            }
            maxKey = Sons[length - 1]->maxKey;
        }
    };

    Node* root = nullptr;

    Node* searchLeaf(int64_t key) const
    {
        Node* node = root;
        if (node) {
            while (!node->isLeaf()) {
                if (node->length == 3 && node->Sons[1]->maxKey < key) {
                    node = node->Sons[2];
                } else if (node->Sons[0]->maxKey < key) {
                    node = node->Sons[1];
                } else {
                    node = node->Sons[0];
                }
            }
        }
        return node;
    }
    void update(Node* node);
    void split(Node* t);
    void erase(Node* child);
    int64_t nth_key(Node* node, int64_t n) const;



public:
    int64_t size() const;
    int64_t count(int64_t key) const;
    void insert(int64_t key);
    void erase(int64_t key);
    int64_t nth_key(int64_t n) const;
};


int64_t Tree23::size() const
{
    return root == nullptr ? 0 : root->_size;
}

int64_t Tree23::count(int64_t key) const
{
    Node* node = searchLeaf(key);
    return (node && node->maxKey == key) ? 1 : 0;
}

void Tree23::insert(int64_t key)
{
    if (root == nullptr) {
        root = new Node(nullptr, key);
    } else if (root->isLeaf()) {
        if (root->maxKey != key) {
            root = new Node(nullptr, root, new Node(nullptr, key));
        }
    } else {
        Node* a = searchLeaf(key);
        if (a->maxKey != key) {
            a->Parent->addSon(new Node(a->Parent, key));
            split(a->Parent);
        }
    }
}

void Tree23::erase(int64_t key)
{
    Node* node = searchLeaf(key);
    if (!node || node->maxKey != key) {
        return;
    }
    erase(node);
}

int64_t Tree23::nth_key(int64_t n) const
{
    assert(n >= 0 && n < size());
    n = root->_size - n - 1;
    return nth_key(root, n);
}

void Tree23::update(Node* node)
{
    for (Node* p = node; p != nullptr; p = p->Parent) {
        p->update();
    }
}

void Tree23::split(Node* t)
{
    if (t->length > 3) {
        auto newNode = new Node(t->Parent, t->Sons[2], t->Sons[3]);
        t->eraseSon(t->Sons[3]);
        t->eraseSon(t->Sons[2]);
        if (t->Parent == nullptr) {
            root = new Node(nullptr, root, newNode);
        } else {
            t->Parent->addSon(newNode);
            t->Parent->update();
            split(t->Parent);
        }
    } else {
        update(t);
    }
}

void Tree23::erase(Node* child)
{
    Node* node = child->Parent;
    if (!node) {
        delete node;
        root = nullptr;
        return;
    }
    node->eraseSon(child);
    delete child;
    node->update();
    if (node->length == 2) {
        update(node->Parent);
        return;
    }

    assert(node && node->length == 1);
    Node* p = node->Parent;
    if (!p) {
        root = node->Sons[0];
        root->Parent = nullptr;
        delete node;
        return;
    }
    Node* bro = node == p->Sons[1] ? p->Sons[0] : p->Sons[1];
    if (bro->length == 3) {
        if (node->maxKey < bro->maxKey) {
            node->addSon(bro->Sons[0]);
            bro->eraseSon(bro->Sons[0]);
        } else {
            node->addSon(bro->Sons[2]);
            bro->eraseSon(bro->Sons[2]);
        }
        update(p);
        return;
    }
    bro->addSon(node->Sons[0]);
    erase(node);
}

int64_t Tree23::nth_key(Node* node, int64_t n) const
{
    assert(n >= 0 && n < node->_size);
    if (node->isLeaf()) {
        return node->maxKey;
    }
    for (int64_t i = 0; i < node->length; ++i) {
        if (n < node->Sons[i]->_size) {
            return nth_key(node->Sons[i], n);
        }
        n -= node->Sons[i]->_size;
    }
    return 0;
}



void test()
{
    std::cout << "Hello" << std::endl;
    while (true) {
        Tree23 t;
        int64_t q = rand() % 100000;
        std::cout << q << std::endl;
        std::vector<int64_t> ans1;
        std::vector<int64_t> ans2;
        std::set<int64_t> st;
        for (int i = 0; i < q; ++i) {
            int64_t type = rand() % 3;
            if (type == 2 && t.size() == 0) {
                type = rand() % 2;
            }
            //32767 30518
            if (type == 0) {
                int64_t x;
                x = rand() * (rand() % 30518);
                //std::cout << "+1 " << x << std::endl;
                st.insert(x);
                t.insert(x);
            } else if (type == 1) {
                int64_t x;
                x = rand() * (rand() % 30518);
               // std::cout << "-1 " << x << std::endl;
                st.erase(x);
                t.erase(x);
            } else if (type == 2) {
                int64_t x;
                //std::cout << int(st.size()) << "=" << t.size() << std::endl;
                assert(int(st.size()) == t.size());
                x = 1 + rand() % int(t.size());
               // std::cout << "0 " << x << std::endl;
                int64_t res = t.nth_key(x - 1);
                ans1.push_back(res);
                auto it = st.begin();
                for (int i = 0; i < int(st.size()) - x; ++i) {
                    it = std::next(it);
                }
                int sv = *it;
                ans2.push_back(sv);
            }
        }
        for (int i : ans1) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        for (int i : ans2) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        if (ans1.size() != ans2.size() || ans1 != ans2) {
            std::cout << "error" << std::endl;
            return;
        }
        std::cout << std::endl;

    }
}


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    //test();
    //return 0;
    Tree23 tree;
    int64_t n;
    cin >> n;
    for (int64_t i = 0; i < n; ++i) {
        string s;
        cin >> s;
        if (s == "+1" || s == "1") {
            int64_t x;
            cin >> x;
            tree.insert(x);
        } else if (s == "0") {
            int64_t k;
            cin >> k;
            cout << tree.nth_key(k - 1) << endl;
        } else if (s == "-1") {
            int64_t x;
            cin >> x;
            tree.erase(x);
        }
    }
    return 0;
}


/*
* 
* 
* 
7
+1 2
+1 3
+1 1
+1 1
+1 2
+1 0
0 3


7
+1 2
+1 3
+1 1
+1 1
+1 2
+1 0
0 2

11
+1 5
1 3
+1 7
0 1
0 2
0 3
-1 5
1 10
0 1
0 2
0 3

6
+ 1
+ 3
+ 3
? 2
+ 1
? 4


*/