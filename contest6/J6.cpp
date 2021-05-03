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


class Backpack {
    vector<int> weights;
    vector<int> weights_table;
public:
    Backpack(vector<int>& A, int S)
    {
        weights = A;
        weights_table.resize(S + 1, 0);
    }

    void possible_weights()
    {
        int N = weights.size();
        weights_table[0] = 1;
        for (int i = 0; i < N; ++i) {
            for (int j = weights_table.size() - 1; j >= 0; --j) {
                if (weights_table[j] == 1 && j + weights_table[i] <= weights_table.size() - 1) {
                    weights_table[j + weights[i]] = 1;
                }
            }
        }
    }

    int find_max_weights()
    {
        possible_weights();
        for (int j = weights_table.size() - 1; j >= 0; --j) {
            if (weights_table[j] == 1) {
                return j;
                break;
            }
        }
    }
};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int S, N;
    cin >> S >> N;
    vector<int> a(N);
    for (int i = 0; i < N; ++i) {
        cin >> a[i];
    }
    Backpack backpack();
    vector<int> r(S + 1, 0);
    r[0] = 1;
    for (int i = 0; i < N; ++i) {
        for (int j = S; j >= 0; --j) {
            if (r[j] == 1 && j + a[i] <= S) {
                r[j + a[i]] = 1;
            }
        }
    }
    for (int j = S; j >= 0; --j) {
        if (r[j] == 1) {
            cout << j << endl;
            break;
        }
    }
    return 0;
}