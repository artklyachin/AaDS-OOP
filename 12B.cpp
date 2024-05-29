#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>

using namespace std;

struct Solution
{
    static constexpr int64_t alphabet_length = 26;

    struct Curl {
        int64_t node = -1;
        int64_t edge_num = -1;
        int64_t len_word_from_node = 0;

        Curl()
        {
        }

        Curl(int64_t new_node)
        {
            goToNode(new_node);
        }

        void goToNode(int64_t new_node, int64_t edge_new_num = -1, int64_t len_word_from_new_node = 0)
        {
            node = new_node;
            edge_num = edge_new_num;
            len_word_from_node = len_word_from_new_node;
        }
    };

    struct Section
    {
        int64_t begin = -1;
        int64_t end = -1;

        int64_t len()
        {
            return end - begin;
        }
    };

    struct Node
    {
        int64_t num_of_node = 0;
        int64_t next[alphabet_length];
        Section edge[alphabet_length];
        int64_t tree_size = 0;
        Curl sufflink;
        bool leaf = false;

        Node() // начальные значения
        {
            fill(begin(next), end(next), -1);
        }

        // вставить вершину посреди ребра
        void embedNewNode(Node& embed_node, int64_t edge_num, int64_t edge_from_embed_node, int64_t place_in_edge)
        {
            embed_node.next[edge_from_embed_node] = next[edge_num];
            embed_node.edge[edge_from_embed_node] = { edge[edge_num].begin + place_in_edge, edge[edge_num].end };
            next[edge_num] = embed_node.num_of_node;
            edge[edge_num] = { edge[edge_num].begin, edge[edge_num].begin + place_in_edge };
        }

        // добавить лист
        void addNewLeaf(Node& new_leaf, int64_t edge_num, int64_t begin, int64_t end)
        {
            next[edge_num] = new_leaf.num_of_node;
            edge[edge_num] = { begin, end };
            new_leaf.leaf = true;
        }
    };

    vector<Node> nodes; // список всех нод дерева
    int64_t nodeCount = 0;
    string source_string = "";

    // построить дерево по строке
    Solution(const string& s)
        : source_string(s)
    {
        nodes.emplace_back();
        nodes[0].num_of_node = 0;
        ++nodeCount;
        Curl curl;
        curl.node = 0;
        for (int64_t j = 0; j < source_string.size(); ++j) {
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
        }
        find_tree_size(0);
    }

    bool addLetter(Curl& curl, int64_t index)
    {
        int64_t num_of_new_edge = source_string[index] - 'a';
        int64_t node_with_new_leaf;
        if (curl.len_word_from_node != 0) { // curl указывает на ребро
            int64_t index_c = nodes[curl.node].edge[curl.edge_num].begin + curl.len_word_from_node;
            if (source_string[index_c] == source_string[index]) { // символ уже присутсвует
                return true;
            } else {
                nodes.push_back(Node());
                int64_t div_node = nodeCount;
                nodes[div_node].num_of_node = nodeCount;
                ++nodeCount;

                int64_t old_next_node = nodes[curl.node].next[curl.edge_num];
                int64_t num_edge_to_old_next_node = source_string[index_c] - 'a';
                nodes[curl.node].embedNewNode(nodes[div_node], curl.edge_num, num_edge_to_old_next_node, curl.len_word_from_node);

                // добавили вершину, возможно придётся заменить суф.ссылку у curl.node.
                /*if (nodes[old_next_node].sufflink.node == curl.node) {
                    nodes[old_next_node].sufflink.node = div_node;
                    nodes[old_next_node].sufflink.len_word_from_node = nodes[div_node].edge[num_edge_to_old_next_node].len() - 1;
                    if (nodes[old_next_node].sufflink.len_word_from_node == 0) {
                        nodes[old_next_node].sufflink.edge_num = -1;
                    } else {
                        nodes[old_next_node].sufflink.edge_num = num_edge_to_old_next_node;
                    }
                }*/

                node_with_new_leaf = div_node;
            }
        } else { // curl указывает на вершину
            if (nodes[curl.node].next[num_of_new_edge] != -1) {
                return true;
            }

            node_with_new_leaf = curl.node;
        }

        // добавляем лист
        nodes.push_back(Node());
        int64_t new_node = nodeCount;
        nodes[new_node].num_of_node = nodeCount;
        ++nodeCount;

        nodes[node_with_new_leaf].addNewLeaf(nodes[new_node], num_of_new_edge, index, source_string.size());

        return false;
    }

    // записываем суф.ссылку для новой ноды (если она была добавлена)
    // переходим по суффиксной ссылке
    void updateValuesAndGoToSuffixLink(Curl& curl)
    {
        int64_t left_border = 0;
        int64_t right_border = 0;
        int64_t prev_node; // нода, для которой обновим суффикс

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

            int64_t edge = source_string[left_border] - 'a';
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

    void jump_by_letter(Curl& curl, int64_t index)
    {
        if (curl.edge_num != -1) {
            curl.len_word_from_node += 1;
        } else {
            int64_t num_of_new_edge = source_string[index] - 'a';
            curl.edge_num = num_of_new_edge;
            curl.len_word_from_node = 1;
        }
        // Если curl.len_word_from_node переполнился
        if (curl.len_word_from_node == nodes[curl.node].edge[curl.edge_num].len()) {
            curl.goToNode(nodes[curl.node].next[curl.edge_num]);
        }
    }

    void find_tree_size(int64_t node)
    {
        nodes[node].tree_size = 0;
        for (int64_t i = 0; i < alphabet_length; ++i) {
            if (nodes[node].next[i] == -1) continue;
            find_tree_size(nodes[node].next[i]);
            nodes[node].tree_size += nodes[node].edge[i].len() + nodes[nodes[node].next[i]].tree_size;
        }
    }

    string dfs(int64_t k, int64_t node)
    {
        for (int64_t i = 0; i < alphabet_length; ++i) {
            if (nodes[node].next[i] == -1) continue;
            int64_t size_of_subtree = nodes[nodes[node].next[i]].tree_size + nodes[node].edge[i].len();
            if (size_of_subtree >= k) { // подстрока находится в поддереве
                if (nodes[node].edge[i].len() >= k) { // подстрока находится на ребре
                    return source_string.substr(nodes[node].edge[i].begin, k);
                } else {
                    k -= nodes[node].edge[i].len();
                    return source_string.substr(nodes[node].edge[i].begin, nodes[node].edge[i].len())
                        + dfs(k, nodes[node].next[i]);
                }
            } else {
                k -= size_of_subtree;
            }
        }
        return "";
    }

    string max_substr(int64_t node)
    {
        for (int64_t i = alphabet_length - 1; i >= 0; --i) {
            if (nodes[node].next[i] == -1) continue;
            return source_string.substr(nodes[node].edge[i].begin, nodes[node].edge[i].len())
                + max_substr(nodes[node].next[i]);
        }
        return "";
    }

public:

    string find_in_order(int64_t k)
    {
        string result = dfs(k, 0);
        if (result.size() > 0) {
            return result;
        } else {
            return max_substr(0);
        }
    }
};


void stress()
{
    while (true) {
        int64_t n = rand() % 20 + 1;
        string s;
        for (int64_t i = 0; i < n; ++i) {
            s.push_back('a' + rand() % 4);
        }
        vector<string> ans1;
        unordered_set<string> st;
        for (int64_t i = 0; i < n; ++i) {
            for (int64_t j = i + 1; j <= n; ++j) {
                st.insert(s.substr(i, j - i));
            }
        }
        for (string i : st) {
            ans1.push_back(i);
        }
        sort(ans1.begin(), ans1.end());

        cout << s << endl;

        cout << endl << "ans1:" << endl;
        for (string i : ans1) {
            cout << i << endl;
        }

        Solution sol(s);
        vector<string> ans2;
        string result = sol.find_in_order(1);
        string prev_result = "";
        for (int64_t i = 2; prev_result != result; ++i) {
            prev_result = result;
            ans2.push_back(result);
            result = sol.find_in_order(i);
        }

        cout << endl << "ans2:" << endl;
        for (string i : ans2) {
            cout << i << endl;
        }

        if (ans1 != ans2) {
            cout << "!" << endl;
            return;
        }

        cout << "-----" << endl;
    }
}


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string s;
    int64_t k;
    //stress();
    cin >> s;
    cin >> k;
    string result = Solution(s).find_in_order(k);
    cout << result << endl;
    //for (int64_t k = 1; k < 100; ++k) {
    //    string result = Solution(s).find_in_order(k);
    //    cout << result << endl;
    //}
    return 0;
}

/*

aaacaab
1

ans1:
a
aa
aaa
aaac
aaaca
aaacaa
aaacaab
aab
aac
aaca
aacaa
aacaab
ab
ac
aca
acaa
acaab
b
c
ca
caa
caab

ans2:
a
aa
aaa
aaac
aaaca
aaacaa
aaacaab
aab
aac
aaca
aacaa
aacaab
ac
aca
acaa
acaab
b
c
ca
caa
caab
*/