#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

<<<<<<< HEAD

struct Solution
{
    static constexpr int ALPHABET_LEN = 2;

    struct Node
    {
        int next[ALPHABET_LEN];
=======
using namespace std;

struct Solution
{
    static constexpr int K = 2;

    struct Node
    {
        int next[K];
>>>>>>> 60b3c1c829f43cf9569de7ae3802f5a82e0d8dd5
        bool leaf = false;
        int parent = -1;
        char parentChar = '0';
        int suffixLink = -1;
<<<<<<< HEAD
        int suffixNext[ALPHABET_LEN];
=======
        int suffixNext[K];
>>>>>>> 60b3c1c829f43cf9569de7ae3802f5a82e0d8dd5
        int suffixLeaf = -1;

        Node()
        {
<<<<<<< HEAD
            std::fill(std::begin(next), std::end(next), -1);
            std::fill(std::begin(suffixNext), std::end(suffixNext), -1);
=======
            fill(begin(next), end(next), -1);
            fill(begin(suffixNext), end(suffixNext), -1);
>>>>>>> 60b3c1c829f43cf9569de7ae3802f5a82e0d8dd5
        }

        void setParent(int parent_, char parentChar_)
        {
            parent = parent_;
            parentChar = parentChar_;
        }
    };

<<<<<<< HEAD
    std::vector<Node> nodes;
    int nodeCount = 0;
    std::vector<char> visited;

    void add(const std::string& code)
    {
        int node = 0;
        for (int i = 0; i < int(code.size()); ++i) {
            char edge_mark = code[i] - '0';
            if (nodes[node].next[edge_mark] == -1) {
                nodes[node].next[edge_mark] = nodeCount;
                nodes[nodeCount].setParent(node, code[i]);
                ++nodeCount;
            }
            node = nodes[node].next[edge_mark];
        }
        nodes[node].leaf = true;
    }

    int getSuffixLink(int node)
    {
        if (nodes[node].suffixLink != -1) {
            return nodes[node].suffixLink;
        } else if (nodes[node].parent <= 0) {
            return nodes[node].suffixLink = 0;
        } else {
            return nodes[node].suffixLink = getSuffixNext(getSuffixLink(nodes[node].parent), nodes[node].parentChar);
        }
    }

    int getSuffixNext(int node, char symbol)
    {
        int edge_mark = symbol - '0';
        if (nodes[node].suffixNext[edge_mark] != -1) {
            return nodes[node].suffixNext[edge_mark];
        } else if (nodes[node].next[edge_mark] != -1) {
            return nodes[node].suffixNext[edge_mark] = nodes[node].next[edge_mark];
        } else if (node == 0) {
            return nodes[node].suffixNext[edge_mark] = 0;
        } else {
            return nodes[node].suffixNext[edge_mark] = getSuffixNext(getSuffixLink(node), symbol);
        }
    }

    int getSuffixLeaf(int node)
    {
        if (nodes[node].suffixLeaf != -1) {
            return nodes[node].suffixLeaf;
        } else if (getSuffixLink(node) == 0) {
            return nodes[node].suffixLeaf = 0;
        } else if (nodes[getSuffixLink(node)].leaf) {
            return nodes[node].suffixLeaf = getSuffixLink(node);
        } else {
            return nodes[node].suffixLeaf = getSuffixLeaf(getSuffixLink(node));
        }
    }

    bool dfs(int node)
    {
        if (nodes[node].leaf || getSuffixLeaf(node) != 0 || visited[node] == 2) {
            return false;
        }
        if (visited[node] == 1) {
            return true;
        }
        visited[node] = 1;
        int p0 = getSuffixNext(node, '0');
        int p1 = getSuffixNext(node, '1');
        if (dfs(p0) || dfs(p1)) {
            return true;
        }
        visited[node] = 2;
=======
    vector<Node> nodes;
    int nodeCount = 0;
    vector<char> visited;

    void add(const string& s)
    {
        int p = 0;
        for (int i = 0; i < int(s.size()); ++i) {
            char q = s[i] - '0';
            if (nodes[p].next[q] == -1) {
                nodes[p].next[q] = nodeCount;
                nodes[nodeCount].setParent(p, s[i]);
                ++nodeCount;
            }
            p = nodes[p].next[q];
        }
        nodes[p].leaf = true;
    }

    int getSuffixLink(int p)
    {
        if (nodes[p].suffixLink != -1) {
            return nodes[p].suffixLink;
        } else if (nodes[p].parent <= 0) { 
            return nodes[p].suffixLink = 0;
        } else {
            return nodes[p].suffixLink = getSuffixNext(getSuffixLink(nodes[p].parent), nodes[p].parentChar);
        }
    }

    int getSuffixNext(int p, char c)
    {
        int q = c - '0';
        if (nodes[p].suffixNext[q] != -1) {
            return nodes[p].suffixNext[q];
        } else if (nodes[p].next[q] != -1) {
            return nodes[p].suffixNext[q] = nodes[p].next[q];
        } else if (p == 0) {
            return nodes[p].suffixNext[q] = 0;
        } else {
            return nodes[p].suffixNext[q] = getSuffixNext(getSuffixLink(p), c);
        }
    }

    int getSuffixLeaf(int p)
    {
        if (nodes[p].suffixLeaf != -1) {
            return nodes[p].suffixLeaf;
        } else if (getSuffixLink(p) == 0) {
            return nodes[p].suffixLeaf = 0;
        } else if (nodes[getSuffixLink(p)].leaf) {
            return nodes[p].suffixLeaf = getSuffixLink(p);
        } else {
            return nodes[p].suffixLeaf = getSuffixLeaf(getSuffixLink(p));
        }
    }

    bool dfs(int p)
    {
        if (nodes[p].leaf || getSuffixLeaf(p) != 0 || visited[p] == 2) {
            return false;
        }
        if (visited[p] == 1) {
            return true;
        }
        visited[p] = 1;
        int p0 = getSuffixNext(p, '0');
        int p1 = getSuffixNext(p, '1');
        if (dfs(p0) || dfs(p1)) {
            return true;
        }
        visited[p] = 2;
>>>>>>> 60b3c1c829f43cf9569de7ae3802f5a82e0d8dd5
        return false;
    }

public:
<<<<<<< HEAD
    bool solve(const std::vector<std::string> dict)
    {
        int len = 0;
        for (auto& code : dict) {
            len += int(code.size());
=======
    bool solve(const vector<string> dict)
    {
        int len = 0;
        for (auto& s : dict) {
            len += int(s.size());
>>>>>>> 60b3c1c829f43cf9569de7ae3802f5a82e0d8dd5
        }
        nodes.resize(len + 1);
        nodes[0].setParent(0, '0');
        nodeCount = 1;
<<<<<<< HEAD
        for (auto& code : dict) {
            add(code);
=======
        for (auto& s : dict) {
            add(s);
>>>>>>> 60b3c1c829f43cf9569de7ae3802f5a82e0d8dd5
        }
        visited.assign(nodeCount, 0);
        return dfs(0);
    }
};

int main()
{
<<<<<<< HEAD
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int number_of_codes;
    std::cin >> number_of_codes;
    std::vector<std::string> dict(number_of_codes);
    for (auto& code : dict) {
        std::cin >> code;
    }
    auto result = Solution().solve(dict);
    std::cout << (result ? "TAK" : "NIE") << std::endl;
=======
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    vector<string> dict(N);
    for (auto& s : dict) {
        cin >> s;
    }
    auto result = Solution().solve(dict);
    cout << (result ? "TAK" : "NIE") << endl;
>>>>>>> 60b3c1c829f43cf9569de7ae3802f5a82e0d8dd5
    return 0;
}