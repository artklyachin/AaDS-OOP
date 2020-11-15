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
	~Heap();
	const Heap operator() (const Heap& h) = delete;
	const Heap operator= (const Heap& h) = delete;

	void Push(int64_t el, int64_t move);
	int64_t Pop();
	int64_t Get_Min();
	void Change_Elem(int64_t num_of_move, int64_t diff);
	void Print_Heap(int64_t move);

private:
	vector<int64_t> h;
	vector<int64_t> i_h;
	vector<int64_t> h_i;

	void Lift_Up(int64_t index);
	void Pull_Down(int64_t index);
	void Swap_Elem(int64_t i, int64_t j);
	void Add_Elem(int64_t el, int64_t move);
};

Heap::Heap()
{
	h = vector<int64_t>(1);
	i_h = vector<int64_t>(1);
	h_i = vector<int64_t>(1);
}


Heap::~Heap()
{
	h.clear();
	i_h.clear();
	h_i.clear();
}


void Heap::Swap_Elem(int64_t i, int64_t j)
{
	swap(i_h[i], i_h[j]);
	h_i[i_h[i]] = i;
	h_i[i_h[j]] = j;
	swap(h[i], h[j]);
}


void Heap::Add_Elem(int64_t el, int64_t move)
{
	h.push_back(el);
	i_h.push_back(move);
	h_i.push_back(int64_t(h.size()) - 1);
}


void Heap::Lift_Up(int64_t index)
{
	int64_t p = index / 2;
	while (p > 0 && h[index] < h[p]) {
		Swap_Elem(index, p);
		index = p;
		p = index / 2;
	}
}


void Heap::Pull_Down(int64_t index)
{
	int64_t size = h.size();
	while (2 * index < size) {
		int64_t left = 2 * index, right = 2 * index + 1;
		int64_t j = left;
		if (right < size && h[left] > h[right]) {
			j = right;
		}
		if (h[j] >= h[index]) {
			break;
		}
		Swap_Elem(index, j);
		index = j;
	}
}


void Heap::Push(int64_t el, int64_t move)
{
	Add_Elem(el, move);
	Lift_Up(int64_t(h.size()) - 1);
}


int64_t Heap::Pop()
{
	assert(int64_t(h.size()) > 1LL);
	h_i.push_back(-1);
	int64_t returnValue = h[1];
	Swap_Elem(1, int64_t(h.size()) - 1);
	h_i[i_h.back()] = -1;
	h.pop_back();
	i_h.pop_back();
	Pull_Down(1);
	return returnValue;
}


int64_t Heap::Get_Min()
{
	assert(int64_t(h.size()) > 1LL);
	h_i.push_back(-1);
	return h[1];
}


void Heap::Change_Elem(int64_t num_of_move, int64_t diff)
{
	h_i.push_back(-1);
	int64_t index = h_i[num_of_move];
	h[index] -= diff;
	Lift_Up(index);
}


int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	int64_t q;
	cin >> q;
	Heap heap;
	for (int64_t move = 1; move <= q; ++move) {
		string s;
		cin >> s;
		if (s == "insert") {
			int64_t x;
			cin >> x;
			heap.Push(x, move);
		} else if (s == "decreaseKey") {
			int64_t move, diff;
			cin >> move >> diff;
			heap.Change_Elem(move, diff);
		} else if (s == "getMin") {
			cout << heap.Get_Min() << "\n";
		} else if (s == "extractMin") {
			heap.Pop();
		}
	}
    return 0;
}
