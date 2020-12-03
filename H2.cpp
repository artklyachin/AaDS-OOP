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


void LSD(vector<int64_t>& A, int64_t l, int64_t r, int64_t byte_number)
{
	if (r - l <= 1) return;
	if (byte_number == -1) return;
	vector<int64_t> count(256, 0);
	vector<int64_t> position(256, 0);
	for (int64_t i = l; i < r; ++i) {
		int64_t byte = A[i] / (1LL << (8 * byte_number)) % 256;
		++count[byte];
	}
	for (int64_t i = 1; i < count.size(); ++i) {
		count[i] += count[i - 1];
	}
	int64_t j = l;
	while(j < r) {
		int64_t byte = A[j] / (1LL << (8 * byte_number)) % 256;
		if (byte == 0) {
			++j;
		} else {
			if (j < l + count[byte - 1]) {
				swap(A[j], A[l + count[byte - 1] + position[byte]]);
				++position[byte];
			} else {
				++j;
			}
		}
	}
	int64_t prevl = l;
	for (int64_t i = 0; i < 256; ++i) {
		LSD(A, prevl, l + count[i], byte_number - 1);
		prevl = l + count[i];
	}
	count.clear();
	position.clear();
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
	LSD(A, 0, n, 7);
	for (int64_t i = 0; i < n; ++i) {
		cout << A[i] << " ";
	}
	cout << endl;
	return 0;
}
