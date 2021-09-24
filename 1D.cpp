#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

class Solution
{
    struct Node
    {
        int count = 0;
        Node* next[26] = {};
    };

    Node top;
    string s, w;
    int k = 0;

    void dfs(Node* p)
    {
        for (int i = 0; i < p->count; ++i) {
            while (k < int(s.size()) && !islower(s[k])) {
                cout << s[k];
                ++k;
            }
            while (k < int(s.size()) && islower(s[k])) {
                ++k;
            }
            cout << w;
        }
        for (int i = 0; i < 26; ++i) {
            if (p->next[i] != nullptr) {
                w.push_back(char(i) + 'a');
                dfs(p->next[i]);
                w.pop_back();
            }
        }
    }

public:
    void solve()
    {
        cin >> s;
        std::vector<pair<int, int>> g1, g2;
        auto it = s.begin();
        while (it != s.end()) {
            while (it != s.end() && !islower(*it)) {
                ++it;
            }
            auto p = &top;
            while (it != s.end() && islower(*it)) {
                int q = *it - 'a';
                if (p->next[q] == nullptr) {
                    p->next[q] = new Node;
                }
                p = p->next[q];
                ++it;
            }
            if (p != &top) {
                ++p->count;
            }
        }
        k = 0;
        dfs(&top);
        while (k < int(s.size())) {
            cout << s[k];
            ++k;
        }
        cout << endl;
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Solution().solve();
    return 0;
}
