#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int gcd(int a, int b)
{
    return !b ? a : gcd(b, a % b);
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    vector<int> A(N);
    for (int& v : A) {
        cin >> v;
    }

    int ones = count(A.begin(), A.end(), 1);
    if (ones != 0) {
        cout << N - ones << "\n";
        return 0;
    }
    while (A.size() != 1) {
        for (int i = 0; i < int(A.size()) - 1; ++i) {
            A[i] = gcd(A[i], A[i + 1]);
            if (A[i] == 1) {
                cout << 2 * N - int(A.size()) << "\n";
                return 0;
            }
        }
        A.pop_back();
    }
    cout << "-1\n";
    return 0;
}