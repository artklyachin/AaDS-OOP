#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

void prefix_function(string::iterator s, int n, vector<int>& pf)
{
	pf[0] = 0;
	for (int i = 1; i < n; ++i) {
		int j = pf[i - 1];
		while (j > 0 && s[i] != s[j]) {
			j = pf[j - 1];
		}
		if (s[i] == s[j]) {
			++j;
		}
		pf[i] = j;
	}
}

int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	string s;
	cin >> s;
	reverse(s.begin(), s.end());
	int sz = static_cast<int>(s.size());
	vector<int> pf(sz);
	int64_t count_len = 0, count_num = 0;
	for (int i = 0; i < sz; ++i) {
		int n = sz - i;
		prefix_function(s.begin() + i, n, pf);
		int k = *max_element(pf.begin(), pf.begin() + n);
		count_num += n - k;
		count_len += n * (n + 1) / 2 - k * (k + 1) / 2;
	}
	cout << count_len << "\n";
	return 0;
}
