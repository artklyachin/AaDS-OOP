#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <tuple>
#include <algorithm>
#include <numeric>

class AhoCorasick
{
public:
    AhoCorasick(const std::vector<std::string>& targets);
    void add(const std::string& target, int id);
    std::vector<std::vector<int>> findAllTargets(const std::string& source);

private:
    int getSuffixLink(int node);
    int getSuffixNext(int node, char symbol);
    int getSuffixLeaf(int node);

    static constexpr char FIRST_CHAR = 'a';
    static constexpr char LAST_CHAR = 'z';
    static constexpr int ALPHABET_LEN = LAST_CHAR - FIRST_CHAR + 1;
    static constexpr int INIT_VALUE = -1;

    struct Node;

    std::vector<Node> nodes;
    int nodeCount = 0;
    const int ROOT = 0;
    int numTargets = 0;
};

struct AhoCorasick::Node
{
    std::array<int, ALPHABET_LEN> next;
    std::vector<int> targetIds;
    int targetLength = INIT_VALUE;
    int parent = INIT_VALUE;
    char parentChar = FIRST_CHAR;
    int suffixLink = INIT_VALUE;
    int suffixLeaf = INIT_VALUE;

    Node()
    {
        next.fill(INIT_VALUE);
        next.fill(INIT_VALUE);
    }

    void setParent(int parent_, char parentChar_)
    {
        parent = parent_;
        parentChar = parentChar_;
    }
};

AhoCorasick::AhoCorasick(const std::vector<std::string>& targets)
    : numTargets(targets.size())
{
    int len = std::accumulate(targets.begin(), targets.end(), 0,
        [](int acc, const auto& str) { return acc + str.size(); });
    nodes.resize(len + 1);
    nodes[ROOT].setParent(ROOT, LAST_CHAR);
    nodes[ROOT].suffixLeaf = ROOT;
    nodeCount = 1;

    for (int id = 0; id < int(targets.size()); ++id) {
        add(targets[id], id);
    }
}

void AhoCorasick::add(const std::string& target, int id)
{
    int node = ROOT;
    for (char ch : target) {
        char edge_lable = ch - FIRST_CHAR;
        if (nodes[node].next[edge_lable] == INIT_VALUE) {
            nodes[node].next[edge_lable] = nodeCount;
            nodes[nodeCount].setParent(node, ch);
            ++nodeCount;
        }
        node = nodes[node].next[edge_lable];
    }
    nodes[node].targetIds.push_back(id);
    nodes[node].targetLength = target.size();
}

int AhoCorasick::getSuffixLink(int node)
{
    if (nodes[node].suffixLink != INIT_VALUE) {
        return nodes[node].suffixLink;
    } else if (nodes[node].parent == INIT_VALUE || nodes[node].parent == ROOT) { // not assigned or ROOT
        return nodes[node].suffixLink = ROOT;
    } else {
        return nodes[node].suffixLink = getSuffixNext(getSuffixLink(nodes[node].parent), nodes[node].parentChar);
    }
}

int AhoCorasick::getSuffixNext(int node, char symbol)
{
    int edge_lable = symbol - FIRST_CHAR;
    if (nodes[node].next[edge_lable] != INIT_VALUE) {
        return nodes[node].next[edge_lable];
    } else if (node == ROOT) {
        return nodes[node].next[edge_lable] = ROOT;
    } else {
        return nodes[node].next[edge_lable] = getSuffixNext(getSuffixLink(node), symbol);
    }
}

int AhoCorasick::getSuffixLeaf(int node)
{
    if (nodes[node].suffixLeaf != INIT_VALUE) {
        return nodes[node].suffixLeaf;
    } else if (!nodes[getSuffixLink(node)].targetIds.empty()) {
        return nodes[node].suffixLeaf = getSuffixLink(node);
    } else {
        return nodes[node].suffixLeaf = getSuffixLeaf(getSuffixLink(node));
    }
}

std::vector<std::vector<int>> AhoCorasick::findAllTargets(const std::string& source)
{
    std::vector<std::vector<int>> results(numTargets);
    int node = ROOT;
    for (int i = 0; i < int(source.size()); ++i) {
        node = getSuffixNext(node, source[i]);
        for (int leafNode = node; leafNode != ROOT; leafNode = getSuffixLeaf(leafNode)) {
            for (int target_id : nodes[leafNode].targetIds) {
                results[target_id].push_back(i + 1 - nodes[leafNode].targetLength);
            }
        }
    }
    return results;
}

std::tuple<std::string, std::vector<std::string>> readInput(std::istream& in = std::cin)
{
    int N;
    std::string source;
    in >> source >> N;
    std::vector<std::string> targets(N);
    for (auto& str : targets) {
        in >> str;
    }
    return { source, targets };
}

void outputResult(const std::vector<std::vector<int>>& result, std::ostream& out = std::cout)
{
    for (int id = 0; id < int(result.size()); ++id) {
        out << result[id].size();
        for (int i : result[id]) {
            out << " " << i + 1;
        }
        out << std::endl;
    }
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto input = readInput();
    std::string source = std::get<0>(input);
    std::vector<std::string> targets = std::get<1>(input);
    auto result = AhoCorasick(targets).findAllTargets(source);
    outputResult(result);

    return 0;
}


/*
abracadabra
8
abra
ab
marazm
cadabra
abra
ab
marazm
cadabra
*/