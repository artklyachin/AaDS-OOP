#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <istream>
#include <array>
#include <unordered_map>


class AhoCorasick
{
    static constexpr char FIRST_CHAR = 'a';
    static constexpr char LAST_CHAR = 'z';
    static constexpr int ALPHABET_LEN = LAST_CHAR - FIRST_CHAR + 1;
    static constexpr int INIT_VALUE = -1;

    struct Node;
    std::vector<Node> nodes;
    int nodeCount = 0;
    std::vector<int> copyOf;
    const int ROOT = 0;
    std::string source;
    std::vector<std::string> targets;
    std::unordered_map<int, std::vector<int>> results;
    std::vector<int> copyOf2;

public:
    AhoCorasick();
    AhoCorasick(std::string source, std::vector<std::string> targets);
    std::vector<int> getResult(int id) const;
    int numberOfTargets() const;

private:
    int getSuffixLink(int node);
    int getSuffixNext(int node, char symbol);
    int getSuffixLeaf(int node);
    void add(int id);
    void createTree();

public:
    void searchForOccurrences();
};

struct AhoCorasick::Node
{
    std::array<int, ALPHABET_LEN> next;
    int targetId = INIT_VALUE;
    int parent = INIT_VALUE;
    char parentChar = FIRST_CHAR;
    int suffixLink = INIT_VALUE;
    std::array<int, ALPHABET_LEN> suffixNext;
    int suffixLeaf = INIT_VALUE;

    Node()
    {
        std::fill(std::begin(next), std::end(next), INIT_VALUE);
        std::fill(std::begin(suffixNext), std::end(suffixNext), INIT_VALUE);
    }

    void setParent(int parent_, char parentChar_)
    {
        parent = parent_;
        parentChar = parentChar_;
    }
};

AhoCorasick::AhoCorasick()
{
}

AhoCorasick::AhoCorasick(std::string source, std::vector<std::string> targets)
    : source(source), targets(targets)
{
    createTree();
}

void AhoCorasick::createTree()
{
    int len = 0;
    for (auto& str : targets) {
        len += str.size();
    }
    nodes.resize(len + 1);
    nodes[ROOT].setParent(ROOT, LAST_CHAR);
    nodeCount = 1;

    copyOf.assign(targets.size(), INIT_VALUE);
    for (int id = 0; id < int(targets.size()); ++id) {
        add(id);
    }
}

void AhoCorasick::add(int id)
{
    int node = ROOT;
    for (int i = 0; i < int(targets[id].size()); ++i) {
        char edge_lable = targets[id][i] - FIRST_CHAR;
        if (nodes[node].next[edge_lable] == INIT_VALUE) {
            nodes[node].next[edge_lable] = nodeCount;
            nodes[nodeCount].setParent(node, targets[id][i]);
            ++nodeCount;
        }
        node = nodes[node].next[edge_lable];
    }
    copyOf[id] = nodes[node].targetId;
    nodes[node].targetId = id;
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
    if (nodes[node].suffixNext[edge_lable] != INIT_VALUE) {
        return nodes[node].suffixNext[edge_lable];
    } else if (nodes[node].next[edge_lable] != INIT_VALUE) {
        return nodes[node].suffixNext[edge_lable] = nodes[node].next[edge_lable];
    } else if (node == ROOT) {
        return nodes[node].suffixNext[edge_lable] = ROOT;
    } else {
        return nodes[node].suffixNext[edge_lable] = getSuffixNext(getSuffixLink(node), symbol);
    }
}

int AhoCorasick::getSuffixLeaf(int node)
{
    if (nodes[node].suffixLeaf != INIT_VALUE) {
        return nodes[node].suffixLeaf;
    } else if (getSuffixLink(node) == ROOT) {
        return nodes[node].suffixLeaf = ROOT;
    } else if (nodes[getSuffixLink(node)].targetId != INIT_VALUE) {
        return nodes[node].suffixLeaf = getSuffixLink(node);
    } else {
        return nodes[node].suffixLeaf = getSuffixLeaf(getSuffixLink(node));
    }
}

void AhoCorasick::searchForOccurrences()
{
    int node = ROOT;
    for (int i = 0; i < int(source.size()); ++i) {
        node = getSuffixNext(node, source[i]);
        for (int node2 = node; node2 != ROOT; node2 = getSuffixLeaf(node2)) {
            if (nodes[node2].targetId != INIT_VALUE) {
                results[nodes[node2].targetId].push_back(i + 1 - int(targets[nodes[node2].targetId].size()));
            }
        }
    }

    copyOf2.resize(targets.size(), -1);
    for (int id = int(targets.size()) - 1; id >= 0; --id) {
        if (copyOf2[id] == INIT_VALUE) {
            copyOf2[id] = id;
        }
        if (copyOf[id] != INIT_VALUE) {
            copyOf2[copyOf[id]] = copyOf2[id];
        }
    }
}

std::vector<int> AhoCorasick::getResult(int id) const
{
    if (results.find(copyOf2[id]) == results.end()) {
        return std::vector<int>();
    }
    return results.at(copyOf2[id]);
}

int AhoCorasick::numberOfTargets() const
{
    return targets.size();
}

AhoCorasick readInput(std::istream& in = std::cin)
{
    int N;
    std::string source;
    in >> source >> N;
    std::vector<std::string> targets(N);
    for (auto& str : targets) {
        in >> str;
    }
    return AhoCorasick(source, targets);
}

void outputResult(const AhoCorasick& search, std::ostream& out = std::cout)
{
    for (int id = 0; id < search.numberOfTargets(); ++id) {
        std::vector<int> res = search.getResult(id);
        out << res.size();
        for (int i : res) {
            out << " " << i + 1;
        }
        out << std::endl;
    }
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    AhoCorasick search = readInput();
    search.searchForOccurrences();
    outputResult(search);
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