#include <iostream>
#include <string>
#include <vector>


class Find_Equels
{
private:
    std::vector<int> ans;
    int64_t MOD = 1e9 + 7;

public:

    Find_Equels(std::string text, std::string pattern)
    {
        int64_t hash_s = 0, hash_t = 0, r = 5, power_r = 1;
        for (int i = 0; i < pattern.size(); ++i) {
            hash_t += power_r * (pattern[i] - 'a');
            hash_s += power_r * (text[i] - 'a');
            power_r *= r;
            hash_t %= MOD;
            hash_s %= MOD;
            power_r %= MOD;
        }

        std::vector<int64_t> h(text.size());
        int64_t st_r = 1;
        for (int i = 0; i < text.size(); ++i) {
            h[i] = (text[i] - 'a') * st_r % MOD;
            st_r *= r;
            st_r %= MOD;
        }

        for (int j = pattern.size() - 1; j < text.size(); ++j) {
            if (hash_s == hash_t) {
                ans.push_back(j - pattern.size() + 1);
            }
            if (j == text.size() - 1) {
                break;
            }
            hash_s += MOD - h[j - pattern.size() + 1];
            hash_s %= MOD;
            hash_s += h[j + 1];
            hash_s %= MOD;
            hash_t *= r;
            hash_t %= MOD;
        }
    }

    void Print_equels()
    {
        for (auto& elem : ans) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }

};

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string text, pattern;
    std::cin >> text >> pattern;
    Find_Equels elem(text, pattern);
    elem.Print_equels();
    return 0;
}