#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    unordered_map<string, int64_t> H;
    while (true) {
        string s;
        int d;
        cin >> s >> d;
        if (s.empty()) {
            break;
        }
        cout << (H[s] += d) << "\n";
    }
    return 0;
}