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


class Sets {
public:
    Sets(int64_t m);
    void Add(int64_t e, int64_t s);
    void Delete(int64_t e, int64_t s);
    void Clear(int64_t s);
    vector<int64_t> Listset(int64_t s) const;
    vector<int64_t> Listsetsof(int64_t e);
private:
    vector<set<int64_t>> S;
    vector<set<int64_t>> E;
    vector<int64_t> i_to_num;
    map<int64_t, int64_t> num_to_i;
};

Sets::Sets(int64_t m)
{
    S.resize(m + 1, set<int64_t>());
}


void Sets::Add(int64_t e, int64_t s)
{
    if (num_to_i.find(e) == num_to_i.end()) {
        i_to_num.push_back(e);
        num_to_i[e] = int64_t(i_to_num.size()) - 1;
        E.push_back(set<int64_t>());
    }
    int64_t index = num_to_i[e];
    S[s].insert(index);
    E[index].insert(s);
}


void Sets::Delete(int64_t e, int64_t s)
{
    int64_t index = num_to_i[e];
    S[s].erase(index);
    E[index].erase(s);
}


void Sets::Clear(int64_t s)
{
    for (int64_t i : S[s]) {
        E[i].erase(s);
    }
    S[s].clear();
}


vector<int64_t> Sets::Listset(int64_t s) const
{
    vector<int64_t> ans;
    if (S[s].empty()) {
        return ans;
    }
    for (auto it = S[s].begin(); it != S[s].end(); ++it) {
        ans.push_back(i_to_num[*it]);
    }
    sort(ans.begin(), ans.end());
    return ans;
}


vector<int64_t> Sets::Listsetsof(int64_t e)
{
    vector<int64_t> ans;
    if (num_to_i.find(e) == num_to_i.end() || E[num_to_i[e]].empty()) {
        return ans;
    }
    int64_t index = num_to_i[e]; // из-за этого не использую const
    for (auto it = E[index].rbegin(); it != E[index].rend(); ++it) {
        ans.push_back(*it);
    }
    sort(ans.begin(), ans.end());
    return ans;
}


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int64_t n, m, k;
    cin >> n >> m >> k;
    Sets S(m);
    for (int64_t i = 0; i < k; ++i) {
        string st;
        cin >> st;
        if (st == "ADD") {
            int64_t e, s;
            cin >> e >> s;
            S.Add(e, s);
        } else if (st == "DELETE") {
            int64_t e, s;
            cin >> e >> s;
            S.Delete(e, s);
        } else if (st == "CLEAR") {
            int64_t s;
            cin >> s;
            S.Clear(s);
        } else if (st == "LISTSET") {
            int64_t s;
            cin >> s;
            vector<int64_t> ans = S.Listset(s);
            if (ans.empty()) {
                cout << "-1" << endl;
            } else {
                for (int64_t i = 0; i < int64_t(ans.size()); ++i) {
                    cout << ans[i] << ' ';
                }
                cout << endl;
            }
        } else if (st == "LISTSETSOF") {
            int64_t e;
            cin >> e;
            vector<int64_t> ans = S.Listsetsof(e);
            if (ans.empty()) {
                cout << "-1" << endl;
            } else {
                for (int64_t i = 0; i < int64_t(ans.size()); ++i) {
                    cout << ans[i] << ' ';
                }
                cout << endl;
            }
        }
    }
    return 0;
}

/*
10 1
10
ADD 9223372036854775807 0
ADD 9223372036854775807 1
ADD 2 0
LISTSET 0
LISTSETSOF 0
DELETE 9223372036854775807 0
DELETE 2 0
LISTSET 0
CLEAR 0
LISTSET 0
*/