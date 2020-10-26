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


void sorted(vector<int64_t>& A, int64_t l, int64_t r, int64_t bit)
{
	int64_t j = r - 1;
	for (int64_t i = l; i < j; ++i) {
		if (((A[i] >> bit) & 1LL) == 0) continue;
		while (j > i && ((A[j] >> bit) & 1LL) == 1) --j;
		if (j > i) swap(A[i], A[j]);
	}
}


void LSD(vector<int64_t>& A, int64_t l, int64_t r, int64_t bit)
{
	if (r - l <= 1) return;
	if (bit == -1) return;
	sorted(A, l, r, bit);
	int64_t j = l;
	while (j < r && ((A[j] >> bit) & 1LL) == 0) ++j;
	LSD(A, l, j, bit - 1);
	LSD(A, j, r, bit - 1);
}


int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	int64_t n;
	cin >> n;
	vector<int64_t> A(n);
	for (int64_t i = 0; i < n; ++i) {
		cin >> A[i];
	}
	LSD(A, 0, n, 63);
	for (int64_t i = 0; i < n; ++i) {
		cout << A[i] << " ";
	}
	cout << endl;
	return 0;
}
