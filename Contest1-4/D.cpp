#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>


using namespace std;


const int INF = 1e9 + 7;
const int DefaultBufferSize = 4;

class Queue {
public:
	Queue() {};

	void Push_to_the_end(int el);
	void Push_to_the_middle(int el);
	int Pop();

private:
	queue<int> part1;
	deque<int> part2;
};


void Queue::Push_to_the_end(int index)
{
	part2.push_back(index);
	if (part2.size() > part1.size()) {
		int top = part2.front();
		part2.pop_front();
		part1.push(top);
	}
}


void Queue::Push_to_the_middle(int index)
{
	if (part2.size() < part1.size()) {
		part2.push_front(index);
	} else {
		part1.push(index);
	}
}


int Queue::Pop()
{
	int ans = part1.front();
	part1.pop();
	if (part1.size() < part2.size()) {
		int top = part2.front();
		part1.push(top);
		part2.pop_front();
	}
	return ans;
}


int main()
{
	ios::sync_with_stdio(false);
	Queue st;
	int n;
	cin >> n;
	for (int i = 0; i < n; ++i) {
		char c;
		int visitor_number;
		cin >> c;
		if (c == '+') {
			cin >> visitor_number;
			st.Push_to_the_end(visitor_number);
		} else if (c == '*') {
			cin >> visitor_number;
			st.Push_to_the_middle(visitor_number);
		} else if (c == '-') {
			cout << st.Pop() << endl;
		}
	}
	return 0;
}