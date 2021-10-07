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


int INF = 1 << 32;


using namespace std;


template<class T>
struct AlternateNode
{
	T maxValue = -INF;
	int index;

	AlternateNode(int pos = -1, T v = -INF)
		: index(pos), maxValue(v)
	{
	}

	AlternateNode(const AlternateNode& node)
		: index(node.index), maxValue(node.maxValue)
	{
	}

	AlternateNode& operator= (const AlternateNode& node)
	{
		index = node.index;
		maxValue = node.maxValue;
		return *this;
	}

	~AlternateNode() = default;

	AlternateNode& aggregate(const AlternateNode& lhs, const AlternateNode& rhs)
	{
		if (lhs.maxValue < rhs.maxValue) {
			index = rhs.index;
			maxValue = rhs.maxValue;
		} else {
			index = lhs.index;
			maxValue = lhs.maxValue;
		}
		return *this;
	}
};

template<class T, class Node>
class SegmentTree
{
	vector<Node> t;
	int lowerIndex;
	int upperIndex;

	void build(const vector<T>& A, int v, int tl, int tr)
	{
		if (tl == tr) {
			t[v] = Node(tl, A[tl - lowerIndex]);
		} else {
			int tm = (tl + tr) / 2;
			build(A, v * 2, tl, tm);
			build(A, v * 2 + 1, tm + 1, tr);
			t[v].aggregate(t[v * 2], t[v * 2 + 1]);
		}
	}

	void build(int v, int tl, int tr)
	{
		if (tl == tr) {
			t[v] = Node();
		} else {
			int tm = (tl + tr) / 2;
			build(v * 2, tl, tm);
			build(v * 2 + 1, tm + 1, tr);
			t[v].aggregate(t[v * 2], t[v * 2 + 1]);
		}
	}

	void update(int v, int tl, int tr, int pos, int newValue)
	{
		if (tl == tr) {
			t[v] = Node(tl, newValue);
		} else {
			int tm = (tl + tr) / 2;
			if (pos <= tm) {
				update(v * 2, tl, tm, pos, newValue);
			} else {
				update(v * 2 + 1, tm + 1, tr, pos, newValue);
			}
			t[v].aggregate(t[v * 2], t[v * 2 + 1]);
		}
	}

	Node aggregate(int v, int tl, int tr, int l, int r)
	{
		if (l > r) {
			return Node();
		}
		if (l == tl && r == tr) {
			return t[v];
		}
		int tm = (tl + tr) / 2;
		Node r1 = aggregate(v * 2, tl, tm, l, min(r, tm));
		Node r2 = aggregate(v * 2 + 1, tm + 1, tr, max(l, tm + 1), r);
		return Node().aggregate(r1, r2);
	}

public:
	SegmentTree(int lowerIndex_ = 0, int upperIndex_ = 0)
		: lowerIndex(lowerIndex_), upperIndex(upperIndex_)
	{
		t.resize(4 * (upperIndex_ - lowerIndex_ + 1));
	}

	void build(const vector<T>& A, int lowerIndex_)
	{
		lowerIndex = lowerIndex_;
		upperIndex = lowerIndex_ + int(A.size()) - 1;
		t.assign(4 * A.size(), Node());
		build(A, 1, lowerIndex, upperIndex);
	}

	void build(int len, int lowerIndex_)
	{
		lowerIndex = lowerIndex_;
		upperIndex = lowerIndex_ + len - 1;
		t.assign(4 * len, Node());
		build(1, lowerIndex, upperIndex);
	}

	void update(int pos, T newValue)
	{
		return update(1, lowerIndex, upperIndex, pos, newValue);
	}

	Node aggregate(int l, int r)
	{
		return aggregate(1, lowerIndex, upperIndex, l, r);
	}

	Node& top()
	{
		return t[1];
	}

	~SegmentTree()
	{
		t.clear();
	}
};


int main()
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	int n;
	cin >> n;
	vector<pair<int, int>> A(n + 1);
	vector<int> B(n + 1);
	for (int i = 1; i <= n; ++i) {
		cin >> B[i];
		A[i] = { B[i], i };
	}
	sort(A.begin() + 1, A.end(), [](const pair<int, int>& lhs, const pair<int, int>& rhs) { return (lhs.first > rhs.first) || (lhs.first == rhs.first && lhs.second < rhs.second); });
	SegmentTree<int, AlternateNode<int>> st;
	st.build(n, 1);
	vector<int> dp(n + 1), prev(n + 1);
	for (int i = 1; i <= n; ++i) {
		int pos = A[i].second;
		AlternateNode<int> node = st.aggregate(1, pos - 1);
		dp[i] = node.maxValue + 1;
		prev[pos] = node.index;
		st.update(pos, dp[i]);
		//cout << dp[i] << " " << prev[pos] << endl;
	}
	cout << st.top().maxValue << endl;
	vector<int> ans;
	int seq = st.top().index;
	while (seq != -1) {
		ans.push_back(seq);
		seq = prev[seq];
	}
	for (int i = ans.size() - 1; i >= 0; --i) {
		cout << ans[i] << " ";
	}
	return 0;
}