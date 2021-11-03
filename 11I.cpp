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


class Zfunc {
private:
    string s;
    vector<int> res;

    void z_function()
    {
        int r = 0, l = 0;
        res.resize(s.size());
        for (int i = 1; i < s.size(); ++i) {
            if (r >= i) {
                res[i] = min(r + 1 - i, res[i - l]);
            }
            while (res[i] + i < s.size() && s[res[i]] == s[res[i] + i]) {
                res[i] += 1;
            }
            if (i - 1 + res[i] > r) {
                l = i;
                r = i - 1 + res[i];
            }
        }
    }

public:
    Zfunc(string s)
        : s(s)
    {
        z_function();
    }

    int get(int index)
    {
        if (index < 0 || index >= s.size()) {
            return 0;
        }
        return res[index];
    }
};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string s;
    cin >> s;
    Zfunc elem(s);
    for (int i = 0; i < s.size(); ++i) {
        cout << elem.get(i) << " ";
    }
    cout << endl;
    return 0;
}