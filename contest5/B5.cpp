#include <iostream>
#include <unordered_map>

using namespace std;

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    unordered_map<int, int> H;
    int N;
    cin >> N;
    for (int i = 0; i < N; ++i) {
        int a, b;
        cin >> a >> b;
        int& va = H[a];
        int& vb = H[b];
        if (va == 0) {
            va = a;
        }
        if (vb == 0) {
            vb = b;
        }
        swap(va, vb);
        cout << abs(va - vb) << "\n";
    }
    return 0;
}