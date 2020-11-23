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


const int64_t INF = 1e9 + 7;


class Heap {
public:
	Heap();
	Heap(vector<int64_t>& elems);
	~Heap();
	const Heap operator() (const Heap& h) = delete;
	const Heap operator= (const Heap& h) = delete;

	void Push(int64_t el);
	int64_t Pop();
	int64_t Get_Max();
	int64_t Size();

private:
	vector<int64_t> h;

	void Lift_Up(int64_t index);
	void Pull_Down(int64_t index);
};

Heap::Heap()
{ }


Heap::~Heap() 
{
	h.clear();
}


Heap::Heap(vector<int64_t>& elems)
{
	h = elems;
	sort(h.rbegin(), h.rend());
}


void Heap::Lift_Up(int64_t index)
{
	int64_t p = (index - 1) / 2;
	while (p >= 0 && h[index] > h[p]) {
		swap(h[index], h[p]);
		index = p;
		p = (index - 1) / 2;
	}
}


void Heap::Pull_Down(int64_t index)
{
	int64_t size = h.size();
	while (2 * index + 1 < size) {
		int64_t left = 2 * index + 1, right = 2 * index + 2;
		int64_t j = left;
		if (right < size && h[left] < h[right]) {
			j = right;
		}
		if (h[j] <= h[index]) {
			break;
		}
		swap(h[index], h[j]);
		index = j;
	}
}


void Heap::Push(int64_t el)
{
	h.push_back(el);
	Lift_Up(int64_t(h.size()) - 1);
}


int64_t Heap::Pop()
{
	assert(h.size() > 0);
	int64_t returnValue = h[0];
	swap(h[0], h[int64_t(h.size()) - 1]);
	h.pop_back();
	Pull_Down(0);
	return returnValue;
}


int64_t Heap::Get_Max()
{
	assert(h.size() > 0);
	return h[0];
}



int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	int64_t n, k;
	cin >> n >> k;
	vector<int64_t> first_elem(k);
	for (int64_t i = 0; i < k; ++i) {
		cin >> first_elem[i];
	}
	Heap heap(first_elem);
	for (int64_t i = k; i < n; ++i) {
		int64_t x;
		cin >> x;
		heap.Push(x);
		heap.Pop();
	}
	vector<int> ans(k);
	for (int64_t i = 0; i < k; ++i) {
		ans[i] = heap.Pop();
	}
	for (int64_t i = 0; i < k; ++i) {
		cout << ans[k - i - 1] << " ";
	}
	cout << endl;
    return 0;
}
