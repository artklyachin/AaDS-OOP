#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct Solution
{
    static constexpr int K = 'z' - 'a' + 1;

    struct Node
    {
        int next[K] = {};
        int targetId = -1;
        int parent = -1;
        char parentChar = 'a';
        int suffixLink = -1;
        int suffixNext[K] = {};
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
    vector<int> copyOf;

    void add(const string& s, int id)
    {
        int p = 0;
        for (int i = 0; i < int(s.size()); ++i) {
            char q = s[i] - 'a';
            if (nodes[p].next[q] == -1) {
                nodes[p].next[q] = nodeCount;
                nodes[nodeCount].setParent(p, s[i]);
                ++nodeCount;
            }
            p = nodes[p].next[q];
        }
        copyOf[id] = nodes[p].targetId;
        nodes[p].targetId = id;
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
        int q = c - 'a';
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
        } else if (nodes[getSuffixLink(p)].targetId != -1) {
            return nodes[p].suffixLeaf = getSuffixLink(p);
        } else {
            return nodes[p].suffixLeaf = getSuffixLeaf(getSuffixLink(p));
        }
    }

public:
    vector<vector<int>> solve(const string& source, const vector<string> targets)
    {
        int len = 0;
        for (auto& s : targets) {
            len += s.size();
        }
        nodes.resize(len + 1);
        nodes[0].setParent(0, 'z');
        nodeCount = 1;

        copyOf.assign(targets.size(), -1);
        for (int id = 0; id < int(targets.size()); ++id) {
            add(targets[id], id);
        }
        vector<vector<int>> results(targets.size());
        int p = 0;
        for (int i = 0; i < int(source.size()); ++i) {
            p = getSuffixNext(p, source[i]);
            for (int p2 = p; p2 != 0; p2 = getSuffixLeaf(p2)) {
                if (nodes[p2].targetId != -1) {
                    results[nodes[p2].targetId].push_back(i + 1 - int(targets[nodes[p2].targetId].size()));
                }
            }
        }
        for (int id = int(targets.size()) - 1; id >= 0; --id) {
            if (copyOf[id] != -1) {
                results[copyOf[id]] = results[id];
            }
        }
        return results;
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string source;
    int N;
    cin >> source >> N;
    vector<string> targets(N);
    for (auto& s : targets) {
        cin >> s;
    }
    auto result = Solution().solve(source, targets);
    for (int id = 0; id < int(targets.size()); ++id) {
        cout << result[id].size();
        for (int i : result[id]) {
            cout << " " << i + 1;
        }
        cout << endl;
    }
    return 0;
}


/*
abracadabra
8
abra
ab
marazm
cadabra
abra
ab
marazm
cadabra
*/