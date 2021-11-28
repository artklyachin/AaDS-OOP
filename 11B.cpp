#include <iostream>
#include <string>
#include <vector>


class Zfunc {
private:
    std::vector<int> substr;
    std::vector<int> res;

    void z_function()
    {
        int r = 0, l = 0;
        res.resize(substr.size());
        for (int i = 1; i < substr.size(); ++i) {
            if (r >= i) {
                res[i] = std::min(r + 1 - i, res[i - l]);
            }
            while (res[i] + i < substr.size() && substr[res[i]] == substr[res[i] + i]) {
                res[i] += 1;
            }
            if (i - 1 + res[i] > r) {
                l = i;
                r = i - 1 + res[i];
            }
        }
    }

public:
    Zfunc(std::vector<int>& initial_substring)
    {
        substr.resize(initial_substring.size());
        copy(initial_substring.begin(), initial_substring.end(), substr.begin());
        z_function();
    }

    int get(int index)
    {
        if (index < 0 || index >= substr.size()) {
            return 0;
        }
        return res[index];
    }
};

class Boxs {
private:

public:

    void print_points(std::vector<int>& boxs)
    {
        std::vector<int> palindrom(2 * boxs.size()), ans;
        copy(boxs.begin(), boxs.end(), palindrom.begin());
        copy(boxs.rbegin(), boxs.rend(), palindrom.begin() + boxs.size());
        Zfunc elem(palindrom);


        for (int i = boxs.size(); i < palindrom.size(); ++i) {
            if (elem.get(i) == palindrom.size() - i && (palindrom.size() - i) % 2 == 0) {
                ans.push_back(i / 2);
            }
        }
        ans.push_back(boxs.size());
        for (auto it = ans.begin(); it != ans.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    }
};


int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int num_of_cubs, num_of_colors;
    std::cin >> num_of_cubs >> num_of_colors;
    std::vector<int> substr(num_of_cubs);
    for (auto it = substr.begin(); it < substr.end(); ++it) {
        std::cin >> *it;
    }
    Boxs elem;
    elem.print_points(substr);
    return 0;
}