#include <iostream>
#include <string>


using namespace std;


const int INF = 1e9 + 7;
const int DefaultBufferSize = 4;

class Stack {
public:
	Stack();
	~Stack();
	void operator() (Stack& st) = delete;
	void operator= (Stack& st) = delete;

	void Push(int el);
	int Pop();
	int Size();
	int Top();
	void Clear();
	bool IsEmpty() const;

private:
	struct Node {
		int Data;
		Node* Next;

		Node(int data) : Data(data), Next(nullptr) {}
	};
	Node* head;
	int size;
};

Stack::Stack() :
	head(nullptr), size(0)
{
}

Stack::~Stack()
{
	while (head != nullptr) {
		Node* nodeToDelete = head;
		head = head->Next;
		delete nodeToDelete;
	}
}

void Stack::Push(int el)
{
	Node* newNode = new Node(el);
	newNode->Next = head;
	head = newNode;
	++size;
}

int Stack::Pop()
{
	if (head == nullptr) {
		return INF;
	} else {
		int returnValue = head->Data;
		Node* nodeToDelete = head;
		head = head->Next;
		delete nodeToDelete;
		--size;
		return returnValue;
	}
}


int Stack::Size()
{
	return size;
}


int Stack::Top()
{
	return head->Data;
}


void Stack::Clear()
{
	while (head != nullptr) {
		Node* nodeToDelete = head;
		head = head->Next;
		delete nodeToDelete;
	}
	size = 0;
}


bool Stack::IsEmpty() const
{
	return head == nullptr;
}


int main()
{
	Stack st;
	while (true) {
		string s;
		cin >> s;

		if (s == "push") {
			int elem;
			cin >> elem;
			st.Push(elem);
			cout << "ok" << endl;
		} else if (s == "pop") {
			if (st.IsEmpty()) {
				cout << "error" << endl;
			} else {
				cout << st.Pop() << endl;
			}
		} else if (s == "back") {
			if (st.IsEmpty()) {
				cout << "error" << endl;
			} else {
				cout << st.Top() << endl;
			}
		} else if (s == "size") {
			cout << st.Size() << endl;
		} else if (s == "clear") {
			st.Clear();
			cout << "ok" << endl;
		} else if (s == "exit") {
			cout << "bye" << endl;
			break;
		}
	}
	return 0;
}

