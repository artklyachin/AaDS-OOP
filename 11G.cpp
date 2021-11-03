#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct Solution
{
    static constexpr int K = 2;

    struct Node
    {
        int next[K];
        bool leaf = false;
        int parent = -1;
        char parentChar = '0';
        int suffixLink = -1;
        int suffixNext[K];
        int suffixLeaf = -1;

        Node()
        {
            fill(begin(next), end(next), -1);
            fill(begin(suffixNext), end(suffixNext), -1);
        }

        void setParent(int parent_, char parentChar_)
        {
            parent = parent_;
            parentChar = parentChar_;
        }
    };

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
        } else if (nodes[p].parent <= 0) { // not assigned or root
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
        return false;
    }

public:
    bool solve(const vector<string> dict)
    {
        int len = 0;
        for (auto& s : dict) {
            len += int(s.size());
        }
        nodes.resize(len + 1);
        nodes[0].setParent(0, '0');
        nodeCount = 1;
        for (auto& s : dict) {
            add(s);
        }
        // нам нужно найти цикл в ориентированном графе (построенном боре по ссылкам из suffixNext), не проходящий через листы
        visited.assign(nodeCount, 0);
        return dfs(0);
    }
};

int main()
{
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
    return 0;
}