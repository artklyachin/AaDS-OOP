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
    int N, M;
    cin >> N >> M;
    vector<int> a(N), b(M);
    for (int i = 0; i < N; ++i) {
        cin >> a[i];
    }
    for (int i = 0; i < M; ++i) {
        cin >> b[i];
    }
    vector<vector<int>> dp(N + 1, vector<int>(M + 1, 0));
    vector<int> bp(N + 1, 0);
    for (int j = 1; j < M + 1; ++j) {
        for (int i = 1; i < N + 1; ++i) {
            dp[i][j] = dp[i - 1][j];
            if (a[i - 1] > b[j - 1] && dp[i - 1][j] > bp[i]) {
                bp[i] = dp[i - 1][j];
            }
            if (a[i - 1] == b[j - 1] && dp[i - 1][j] < bp[i] + 1) {
                dp[i][j] = bp[i] + 1;
            }
        }
    }
    /*for (int i = 0; i <= N; ++i) {
        for (int j = 0; j <= M; ++j) {
            cout << dp[i][j] << " ";
        }
        cout << endl;
    }*/
    int ans = 0;
    for (int i = 1; i < M + 1; ++i) {
        ans = max(ans, dp[N][i]);
    }
    cout << ans << endl;
    return 0;
}