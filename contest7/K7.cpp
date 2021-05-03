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


class Board {
    int MAX_MASK, N;
    vector<int> dp, dp2;
public:
    Board(int N)
        : N(N)
    {
        MAX_MASK = (1 << (N + 1)) - 1;
        dp.resize(MAX_MASK + 1, 1);
        dp2.resize(MAX_MASK + 1, 0);
    }


    int number_of_coloring_board(int M)
    {
        for (int i = 1; i < M; ++i) {
            if (i != 1) {
                for (int mask = 0; mask < MAX_MASK + 1; ++mask) {
                    int next_mask0 = (mask >> 1), next_mask1 = next_mask0 + (1 << N);
                    dp2[next_mask0] += dp[mask];
                    dp2[next_mask1] += dp[mask];
                }
                copy(dp2.begin(), dp2.end(), dp.begin());
                for (int mask = 0; mask < MAX_MASK + 1; ++mask) dp2[mask] = 0;
            }
            for (int j = 1; j < N; ++j) {
                for (int mask = 0; mask < MAX_MASK + 1; ++mask) {
                    int c1 = mask % 2, c2 = mask % 4 / 2, c3 = mask >> N;
                    int next_mask0 = (mask >> 1), next_mask1 = next_mask0 + (1 << N);
                    if (!(c1 == c2 && c2 == c3 && c3 == 1)) {
                        dp2[next_mask1] += dp[mask];
                    }
                    if (!(c1 == c2 && c2 == c3 && c3 == 0)) {
                        dp2[next_mask0] += dp[mask];
                    }
                }
                copy(dp2.begin(), dp2.end(), dp.begin());
                for (int mask = 0; mask < MAX_MASK + 1; ++mask) dp2[mask] = 0;
            }
        }
        int ans = 0;
        for (int mask = 0; mask < MAX_MASK + 1; ++mask) {
            ans += dp[mask];
        }
        return ans;
    }
};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, M;
    cin >> N >> M;
    if (N > M) {
        swap(N, M);
    }
    if (N == 1) {
        cout << (1 << M) << endl;
        return 0;
    }
    Board bord(N);
    cout << bord.number_of_coloring_board(M) << endl;
    return 0;
}