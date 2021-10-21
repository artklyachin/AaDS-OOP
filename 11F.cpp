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
        int next[K];
        bool leaf = false;
        int parent = -1;
        char parentChar = 'a';
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
    static constexpr int MOD = 10000;

    void add(const string& s)
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
        } else if (nodes[getSuffixLink(p)].leaf) {
            return nodes[p].suffixLeaf = getSuffixLink(p);
        } else {
            return nodes[p].suffixLeaf = getSuffixLeaf(getSuffixLink(p));
        }
    }

public:
    int solve(int N, const vector<string> dict)
    {
        int len = 0;
        for (auto& s : dict) {
            len += int(s.size());
        }
        nodes.resize(len + 1);
        nodes[0].setParent(0, 'z');
        nodeCount = 1;
        for (auto& s : dict) {
            add(s);
        }
        vector<vector<int>> A(nodeCount, vector<int>(N + 1)); 
        A[0][0] = 1;
        for (int i = 1; i <= N; ++i) {
            for (int p = 0; p < nodeCount; ++p) {
                if (A[p][i - 1] != 0) {
                    for (char c = 'a'; c <= 'z'; ++c) {
                        int q = getSuffixNext(p, c);
                        if (!nodes[q].leaf && getSuffixLeaf(q) == 0) {
                            A[q][i] = (A[q][i] + A[p][i - 1]) % MOD;
                        }
                    }
                }
            }
        }
        int count = 0;
        for (int p = 0; p < nodeCount; ++p) {
            count = (count + A[p][N]) % MOD;
        }
        int answer = 1;
        for (int i = 0; i < N; ++i) {
            answer = answer * K % MOD;
        }
        answer = (answer - count + MOD) % MOD;
        return answer;
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, K;
    cin >> N >> K;
    vector<string> dict(K);
    for (auto& s : dict) {
        cin >> s;
    }
    auto result = Solution().solve(N, dict);
    cout << result << endl;
    return 0;
}