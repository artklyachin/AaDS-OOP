#include <iostream>
#include <vector>
#include <string>
#include <algorithm>


struct Solution
{
    static constexpr int ALPHABET_LEN = 2;

    struct Node
    {
        int next[ALPHABET_LEN];
        bool leaf = false;
        int parent = -1;
        char parentChar = '0';
        int suffixLink = -1;
        int suffixNext[ALPHABET_LEN];
        int suffixLeaf = -1;

        Node()
        {
            std::fill(std::begin(next), std::end(next), -1);
            std::fill(std::begin(suffixNext), std::end(suffixNext), -1);
        }

        void setParent(int parent_, char parentChar_)
        {
            parent = parent_;
            parentChar = parentChar_;
        }
    };

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
        return false;
    }

public:
    bool solve(const std::vector<std::string> dict)
    {
        int len = 0;
        for (auto& code : dict) {
            len += int(code.size());
        }
        nodes.resize(len + 1);
        nodes[0].setParent(0, '0');
        nodeCount = 1;
        for (auto& code : dict) {
            add(code);
        }
        visited.assign(nodeCount, 0);
        return dfs(0);
    }
};

int main()
{
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
    return 0;
}