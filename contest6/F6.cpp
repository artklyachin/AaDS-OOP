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


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int64_t n;
    cin >> n;
    vector<vector<int64_t>> dp(n + 1, vector<int64_t>(n + 1, 0));
    for (int64_t i = 0; i <= n; ++i) {
        dp[0][i] = 1;
    }
    for (int64_t i = 1; i <= n; ++i) {
        for (int64_t j = 1; j <= i; ++j) {
            dp[i][j] = dp[i][j - 1] + dp[i - j][j / 2];
            //cout << dp[i][j] << " ";
        }
        for (int64_t j = i + 1; j <= n; ++j) {
            dp[i][j] = dp[i][i];
        }
        //cout << endl;
    }
    cout << dp[n][n];
    return 0;
}