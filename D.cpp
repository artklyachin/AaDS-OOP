#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>


using namespace std;



int main()
{
	ios::sync_with_stdio(false);
	queue<int> part1;
	deque<int> part2;
	int n;
	cin >> n;
	for (int i = 0; i < n; ++i) {
		char c;
		int j;
		cin >> c;
		if (c == '+') {
			cin >> j;
			part2.push_back(j);
			if (part2.size() > part1.size()) {
				int top = part2.front();
				part2.pop_front();
				part1.push(top);
			}
		} else if (c == '*') {
			cin >> j;
			if (part2.size() < part1.size()) {
				part2.push_front(j);
			} else {
				part1.push(j);
			}
		} else if (c == '-') {
			cout << part1.front() << endl;
			part1.pop();
			if (part1.size() < part2.size()) {
				int top = part2.front();
				part1.push(top);
				part2.pop_front();
			}
		}
	}
	return 0;
}