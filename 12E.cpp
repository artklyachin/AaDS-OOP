#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

struct Solution
{
    static constexpr int alphabet_length = 26;

    struct Curl {
        int node = -1;
        int edge_num = -1;
        int len_word_from_node = 0;

        Curl()
        {
        }

        Curl(int new_node)
        {
            goToNode(new_node);
        }

        void goToNode(int new_node, int edge_new_num = -1, int len_word_from_new_node = 0)
        {
            node = new_node;
            edge_num = edge_new_num;
            len_word_from_node = len_word_from_new_node;
        }
    };

    struct Section
    {
        int begin = -1;
        int end = -1;

        int len()
        {
            return end - begin;
        }
    };

    struct Node
    {
        int num_of_node = 0;
        map<int, int> next;
        map <int, Section> edge;
        Curl sufflink;
        bool leaf = false;

        Node() // начальные значения
        {
        }

        // вставить вершину посреди ребра
        void embedNewNode(Node& embed_node, int edge_num, int edge_from_embed_node, int place_in_edge)
        {
            embed_node.next[edge_from_embed_node] = next[edge_num];
            embed_node.edge[edge_from_embed_node] = { edge[edge_num].begin + place_in_edge, edge[edge_num].end };
            next[edge_num] = embed_node.num_of_node;
            edge[edge_num] = { edge[edge_num].begin, edge[edge_num].begin + place_in_edge };
        }

        // добавить лист
        void addNewLeaf(Node& new_leaf, int edge_num, int begin, int end)
        {
            next[edge_num] = new_leaf.num_of_node;
            edge[edge_num] = { begin, end };
            new_leaf.leaf = true;
        }
    };

    vector<Node> nodes; // список всех нод дерева
    int nodeCount = 0;
    string source_string = "";
    vector<int64_t> count_of_distinct_substr;
    int64_t count_of_leaf = 0;

    // построить дерево по строке
    Solution(const string& s)
        : source_string(s)
    {
        count_of_distinct_substr.resize(source_string.size(), 0);
        nodes.emplace_back();
        nodes[0].num_of_node = 0;
        ++nodeCount;
        Curl curl;
        curl.node = 0;
        for (int j = 0; j < source_string.size(); ++j) {
            while (true) {
                bool already_an_item = addLetter(curl, j);
                if (already_an_item) { // символ уже присутсвовал
                    jump_by_letter(curl, j);
                    break;
                }
                if (curl.node == 0 && curl.edge_num == -1) { // символ был добавлен из корня
                    break;
                }
                updateValuesAndGoToSuffixLink(curl);
            }
            if (j == 0) {
                count_of_distinct_substr[0] = count_of_leaf;
            } else {
                count_of_distinct_substr[j] = count_of_distinct_substr[j - 1] + count_of_leaf;
            }
        }
    }

    bool addLetter(Curl& curl, int index)
    {
        int num_of_new_edge = source_string[index] - 'a';
        int node_with_new_leaf;
        if (curl.len_word_from_node != 0) { // curl указывает на ребро
            int index_c = nodes[curl.node].edge[curl.edge_num].begin + curl.len_word_from_node;
            if (source_string[index_c] == source_string[index]) { // символ уже присутсвует
                return true;
            } else {
                nodes.push_back(Node());
                int div_node = nodeCount;
                nodes[div_node].num_of_node = nodeCount;
                ++nodeCount;

                int old_next_node = nodes[curl.node].next[curl.edge_num];
                int num_edge_to_old_next_node = source_string[index_c] - 'a';
                nodes[curl.node].embedNewNode(nodes[div_node], curl.edge_num, num_edge_to_old_next_node, curl.len_word_from_node);

                node_with_new_leaf = div_node;
            }
        } else { // curl указывает на вершину
            if (nodes[curl.node].next.find(num_of_new_edge) != nodes[curl.node].next.end()) {
                return true;
            }

            node_with_new_leaf = curl.node;
        }

        // добавляем лист
        nodes.push_back(Node());
        int new_node = nodeCount;
        nodes[new_node].num_of_node = nodeCount;
        ++nodeCount;

        nodes[node_with_new_leaf].addNewLeaf(nodes[new_node], num_of_new_edge, index, source_string.size());
        count_of_leaf += 1;

        return false;
    }

    // записываем суф.ссылку для новой ноды (если она была добавлена)
    // переходим по суффиксной ссылке
    void updateValuesAndGoToSuffixLink(Curl& curl)
    {
        int left_border = 0;
        int right_border = 0;
        int prev_node; // нода, для которой обновим суффикс

        // если curl до добавления символа указывал на вершину (некорневую)
        if (curl.edge_num == -1) {
            if (nodes[curl.node].sufflink.edge_num != -1) {
                Curl nodesufflink = nodes[curl.node].sufflink;
                left_border = nodes[nodesufflink.node].edge[nodesufflink.edge_num].begin;
                right_border = left_border + nodesufflink.len_word_from_node;
            }
            prev_node = curl.node;
            curl.node = nodes[curl.node].sufflink.node;
        } else { // curl указывает на добаленную вершину
            prev_node = nodes[curl.node].next[curl.edge_num];
            left_border = nodes[curl.node].edge[curl.edge_num].begin;
            right_border = nodes[curl.node].edge[curl.edge_num].end;
            if (curl.node == 0) { // curl указывает на ребро из корня
                left_border += 1;
            } else {
                left_border -= nodes[curl.node].sufflink.len_word_from_node;
                curl.node = nodes[curl.node].sufflink.node;
            }
        }

        while (right_border > left_border
            && right_border - left_border >= nodes[curl.node].edge[source_string[left_border] - 'a'].len()) {

            int edge = source_string[left_border] - 'a';
            left_border += nodes[curl.node].edge[edge].len();
            curl.goToNode(nodes[curl.node].next[edge]);
        }
        // обновляем curl и записываем суф.ссылку для новой ноды
        curl.len_word_from_node = right_border - left_border;
        if (curl.len_word_from_node == 0) {
            curl.edge_num = -1;
        } else {
            curl.edge_num = source_string[left_border] - 'a';
        }
        nodes[prev_node].sufflink = curl;
    }

    void jump_by_letter(Curl& curl, int index)
    {
        if (curl.edge_num != -1) {
            curl.len_word_from_node += 1;
        } else {
            int num_of_new_edge = source_string[index] - 'a';
            curl.edge_num = num_of_new_edge;
            curl.len_word_from_node = 1;
        }
        // Если curl.len_word_from_node переполнился
        if (curl.len_word_from_node == nodes[curl.node].edge[curl.edge_num].len()) {
            curl.goToNode(nodes[curl.node].next[curl.edge_num]);
        }
    }

public:

    void countOfDistinctSubstr(vector<int64_t>& ans)
    {
        ans = count_of_distinct_substr;
    }
};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string s;
    cin >> s;
    vector<int64_t> ans;
    Solution(s).countOfDistinctSubstr(ans);
    for (auto it = ans.begin(); it != ans.end(); ++it) {
        cout << *it << endl;
    }
    return 0;
}

/*


*/