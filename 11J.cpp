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


vector<int> z_function(const string& s)
{
	vector<int> res;
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
	return res;
}


int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	string t, p;
	int k;
	cin >> t >> p >> k;
	int ts = int(t.size()), ps = int(p.size()), shift = ps + 1;
	string s(ps + 1 + ts, '#');
	copy(p.begin(), p.end(), s.begin());
	copy(t.begin(), t.end(), s.begin() + shift);
	auto pf1 = z_function(s);
	copy(p.rbegin(), p.rend(), s.begin());
	copy(t.rbegin(), t.rend(), s.begin() + shift);
	auto pf2 = z_function(s);
	vector<int> result;
	for (int i = 0; i + ps <= ts; ++i) {
		if (pf1[shift + i] + k + pf2[shift + ts - (ps + i)] >= ps) {
			result.push_back(i);
		}
	}
	cout << result.size() << endl;
	for (int& v : result) {
		cout << v + 1 << " ";
	}
	cout << endl;
	return 0;
}


/*
mxxgdpylr axg 2                                                                                                         2                                                                                                                       1 2

*/