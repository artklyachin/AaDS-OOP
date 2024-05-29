#include <iostream>
#include <fstream>
#include <string>
#include <vector>


class Zfunc {
private:
    std::string text;
    std::vector<int> res;

    void z_function()
    {
        int right = 0, left = 0;
        res.resize(text.size());
        for (int i = 1; i < text.size(); ++i) {
            if (right >= i) {
                res[i] = std::min(right + 1 - i, res[i - left]);
            }
            while (res[i] + i < text.size() && text[res[i]] == text[res[i] + i]) {
                res[i] += 1;
            }
            if (i - 1 + res[i] > right) {
                left = i;
                right = i - 1 + res[i];
            }
        }
    }

public:
    Zfunc(std::string text)
        : text(text)
    {
        z_function();
    }

    int get(int index)
    {
        if (index < 0 || index >= text.size()) {
            return 0;
        }
        return res[index];
    }
};


int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string text;
    std::cin >> text;
    Zfunc elem(text);
    for (int i = 0; i < text.size(); ++i) {
        std::cout << elem.get(i) << " ";
    }
    std::cout << std::endl;
    return 0;
}