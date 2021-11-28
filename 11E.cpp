#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Bor
{
    static constexpr char FIRST_CHAR = 'a';
    static constexpr char LAST_CHAR = 'z';
    static constexpr int ALPHABET_LEN = LAST_CHAR - FIRST_CHAR + 1;
    static constexpr int INIT_VALUE = -1;

    struct Node
    {
        int next[ALPHABET_LEN] = {};
        int targetId = INIT_VALUE;
        int parent = INIT_VALUE;
        char parentChar = FIRST_CHAR;
        int suffixLink = INIT_VALUE;
        int suffixNext[ALPHABET_LEN] = {};
        int suffixLeaf = INIT_VALUE;

        Node()
        {
            fill(begin(next), end(next), INIT_VALUE);
            fill(begin(suffixNext), end(suffixNext), INIT_VALUE);
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
    const int ROOT = 0;


public:

    Bor(const vector<string>& targets)
    {
        int len = 0;
        for (auto& str : targets) {
            len += str.size();
        }
        nodes.resize(len + 1);
        nodes[ROOT].setParent(ROOT, LAST_CHAR);
        nodeCount = 1;

        copyOf.assign(targets.size(), INIT_VALUE);
        for (int id = 0; id < int(targets.size()); ++id) {
            add(targets[id], id);
        }
    }

    void add(const string& target, int id)
    {
        int node = ROOT;
        for (int i = 0; i < int(target.size()); ++i) {
            char edge_lable = target[i] - FIRST_CHAR;
            if (nodes[node].next[edge_lable] == INIT_VALUE) {
                nodes[node].next[edge_lable] = nodeCount;
                nodes[nodeCount].setParent(node, target[i]);
                ++nodeCount;
            }
            node = nodes[node].next[edge_lable];
        }
        copyOf[id] = nodes[node].targetId;
        nodes[node].targetId = id;
    }

private:

    int getSuffixLink(int node)
    {
        if (nodes[node].suffixLink != INIT_VALUE) {
            return nodes[node].suffixLink;
        } else if (nodes[node].parent == INIT_VALUE || nodes[node].parent == ROOT) { // not assigned or ROOT
            return nodes[node].suffixLink = ROOT;
        } else {
            return nodes[node].suffixLink = getSuffixNext(getSuffixLink(nodes[node].parent), nodes[node].parentChar);
        }
    }

    int getSuffixNext(int node, char symbol)
    {
        int edge_lable = symbol - FIRST_CHAR;
        if (nodes[node].suffixNext[edge_lable] != INIT_VALUE) {
            return nodes[node].suffixNext[edge_lable];
        } else if (nodes[node].next[edge_lable] != INIT_VALUE) {
            return nodes[node].suffixNext[edge_lable] = nodes[node].next[edge_lable];
        } else if (node == ROOT) {
            return nodes[node].suffixNext[edge_lable] = ROOT;
        } else {
            return nodes[node].suffixNext[edge_lable] = getSuffixNext(getSuffixLink(node), symbol);
        }
    }

    int getSuffixLeaf(int node)
    {
        if (nodes[node].suffixLeaf != INIT_VALUE) {
            return nodes[node].suffixLeaf;
        } else if (getSuffixLink(node) == ROOT) {
            return nodes[node].suffixLeaf = ROOT;
        } else if (nodes[getSuffixLink(node)].targetId != INIT_VALUE) {
            return nodes[node].suffixLeaf = getSuffixLink(node);
        } else {
            return nodes[node].suffixLeaf = getSuffixLeaf(getSuffixLink(node));
        }
    }

public:

    vector<vector<int>> solve(const string& source, const vector<string>& targets)
    {
        vector<vector<int>> results(targets.size());
        int node = ROOT;
        for (int i = 0; i < int(source.size()); ++i) {
            node = getSuffixNext(node, source[i]);
            for (int node2 = node; node2 != ROOT; node2 = getSuffixLeaf(node2)) {
                if (nodes[node2].targetId != INIT_VALUE) {
                    results[nodes[node2].targetId].push_back(i + 1 - int(targets[nodes[node2].targetId].size()));
                }
            }
        }

        for (int id = int(targets.size()) - 1; id >= 0; --id) {
            if (copyOf[id] != INIT_VALUE) {
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
    for (auto& str : targets) {
        cin >> str;
    }
    auto result = Bor(targets).solve(source, targets);
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