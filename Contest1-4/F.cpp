#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include<fstream>


using namespace std;


int64_t number_of_inversions_between_parts(vector<int64_t>& A, int64_t begin_a, int64_t end_a, int64_t begin_b, int64_t end_b)
{
	int64_t ia = begin_a, ib = begin_b, ans = 0;
	vector<int64_t> C;
	C.reserve(end_b - begin_a);
	while (ia < end_a && ib < end_b) {
		if (A[ia] <= A[ib]) {
			C.push_back(A[ia]);
			++ia;
		} else {
			C.push_back(A[ib]);
			ans += end_a - ia;
			++ib;
		}
	}
	while (ia < end_a) {
		C.push_back(A[ia]);
		++ia;
	}
	while (ib < end_b) {
		C.push_back(A[ib]);
		ans += end_a - ia;
		++ib;
	}
	for (int64_t i = begin_a; i < end_b; ++i) {
		A[i] = C[i - begin_a];
	}
	return ans;
}


int64_t number_of_inversions(vector<int64_t>& A, int64_t begin_a, int64_t end_a)
{
	int64_t n = end_a - begin_a, m = (begin_a + end_a) / 2;
	if (n == 1) return 0;
	int64_t ans1 = number_of_inversions(A, begin_a, m);
	int64_t ans2 = number_of_inversions(A, m, end_a);
	return ans1 + ans2 + number_of_inversions_between_parts(A, begin_a, m, m, end_a);


}


int main()
{
	ifstream fin("inverse.in");
	ofstream fout("inverse.out");
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	int64_t n;
	fin >> n;
	//cin >> n;
	vector<int64_t> A(n);
	for (int i = 0; i < n; ++i) {
		fin >> A[i];
	}
	int64_t ans = number_of_inversions(A, 0, n);
	//cout << ans << endl;
	fout << ans << endl;
	fin.close();
	fout.close();
	return 0;
}

