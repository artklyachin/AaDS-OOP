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
    vector<int> s;
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
    Zfunc(vector<int>& old_s)
    {
        s.resize(old_s.size());
        copy(old_s.begin(), old_s.end(), s.begin());
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

class Boxs {
private:

public:

    void print_points(vector<int>& boxs)
    {
        vector<int> palindrom(2 * boxs.size()), ans;
        copy(boxs.begin(), boxs.end(), palindrom.begin());
        copy(boxs.rbegin(), boxs.rend(), palindrom.begin() + boxs.size());
        Zfunc elem(palindrom);


        for (int i = boxs.size(); i < palindrom.size(); ++i) {
            if (elem.get(i) == palindrom.size() - i && (palindrom.size() - i) % 2 == 0) {
                ans.push_back(i / 2);
            }
        }
        ans.push_back(boxs.size());
        for (auto it = ans.begin(); it != ans.end(); ++it) {
            cout << *it << " ";
        }
        cout << endl;
    }
};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    cin >> n >> m;
    vector<int> s(n);
    for (auto it = s.begin(); it < s.end(); ++it) {
        cin >> *it;
    }
    Boxs elem;
    elem.print_points(s);
    return 0;
}
