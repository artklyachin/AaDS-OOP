#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

template<typename ValueType>
class HashTable
{
    size_t collisions = 0;
    vector<vector<pair<string, ValueType>>> T;

    size_t hash(const string& k) const
    {
        size_t h = 0;
        for (char c : k) {
            h = h * 29 ^ (c - '@');
        }
        return h % T.size();
    }

    int find(size_t h, const string& k) const
    {
        for (int i = 0; i < int(T[h].size()); ++i) {
            if (k == T[h][i].first) {
                return i;
            }
        }
        return -1;
    }

    void rebuild()
    {
        size_t s = T.size();
        T.resize(2 * s);
        collisions = 0;
        for (size_t i = 0; i < s; ++i) {
            for (int j = 0; j < int(T[i].size()); ) {
                size_t h = hash(T[i][j].first);
                if (h != i) {
                    T[i + s].emplace_back(T[i][j]);
                    swap(T[i][j], T[i][T[i].size() - 1]);
                    T[i].pop_back();
                } else {
                    ++j;
                }
            }
            collisions += T[i].size() > 1 ? T[i].size() - 1 : 0;
            collisions += T[i + s].size() > 1 ? T[i + s].size() - 1 : 0;
        }
    }

public:
    HashTable() : T(4)
    {
    }

    ValueType get(const string& k) const
    {
        size_t h = hash(k);
        int i = find(h, k);
        if (i >= 0) {
            return T[h][i].second;
        }
        return ValueType();
    }

    void del(const string& k)
    {
        size_t h = hash(k);
        int i = find(h, k);
        if (i >= 0) {
            swap(T[h][i], T[h][T[h].size() - 1]);
            T[h].pop_back();
            if (!T[h].empty()) {
                --collisions;
            }
        }
    }

    void put(const string& k, const ValueType& v)
    {
        size_t h = hash(k);
        int i = find(h, k);
        if (i >= 0) {
            T[h][i].second = v;
            return;
        }
        T[h].emplace_back(k, v);
        if (T[h].size() != 1) {
            ++collisions;
        }
        if (collisions * 2 > T.size()) {
            rebuild();
        }
    }
};



int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    HashTable<int> H;
    while (true) {
        string s;
        int d;
        cin >> s >> d;
        if (s.empty()) {
            break;
        }
        H.put(s, H.get(s) + d);
        cout << H.get(s) << "\n";
    }
    return 0;
}