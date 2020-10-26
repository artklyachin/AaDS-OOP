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
	~Heap() {};
	const Heap operator() (const Heap& h) = delete;
	const Heap operator= (const Heap& h) = delete;

	void Push(int64_t el);
	int64_t Pop();
	int64_t Top();
	int64_t Size();

private:
	int64_t size;
	int64_t* h;
	int64_t max_size = 1e5 + 7;

	void Lift_Up(int64_t index);
	void Pull_Down(int64_t index);
};

Heap::Heap():
	size(1)
{
	h = new int64_t[max_size];
}



void Heap::Lift_Up(int64_t index)
{
	int64_t p = index / 2;
	while (p > 0 && h[index] > h[p]) {
		swap(h[index], h[p]);
		index = p;
		p = index / 2;
	}
}


void Heap::Pull_Down(int64_t index)
{
	while (2 * index < size) {
		int64_t left = 2 * index, right = 2 * index + 1;
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
	int64_t index = size;
	h[size] = el;
	++size;
	Lift_Up(index);
}


int64_t Heap::Pop()
{
	assert(size > 1);
	int64_t returnValue = h[1];
	swap(h[1], h[size - 1]);
	--size;
	Pull_Down(1);
	return returnValue;
}


int64_t Heap::Top()
{
	assert(size > 1);
	return h[1];
}


int64_t Heap::Size()
{
	return size;
}


int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	int64_t n, k;
	cin >> n >> k;
	Heap heap;
	for (int64_t i = 0; i < n; ++i) {
		int64_t x;
		cin >> x;
		heap.Push(x);
		if (heap.Size() > k + 1) {
			heap.Pop();
		}
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
