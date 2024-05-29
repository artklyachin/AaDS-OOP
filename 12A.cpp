#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>


class SuffArray {
    std::vector<int> suff_array;

public:

    SuffArray(std::string s)
    {
        int old_N = int(s.size());
        s.push_back('$');
        int logarithm = int(ceil(log2(old_N + 1)));
        int N = 1 << logarithm;
        s.resize(N, '$');
        std::vector<int> phase = get_primary_phase(s), phase_color(N);
        for (int i = 1; i < N; ++i) {
            phase_color[i] = phase_color[i - 1];
            if (s[phase[i - 1]] != s[phase[i]]) {
                ++phase_color[i];
            }
        }

        for (int pow = 1; pow < N; pow *= 2) {
            std::vector<int> shifted_phase(N), next_phase(N);
            for (int j = 0; j < N; ++j) {
                shifted_phase[j] = phase[j] - pow;
                if (shifted_phase[j] < 0) {
                    shifted_phase[j] += N;
                }
            }
            std::vector<int> count(phase_color.size());
            for (int j = 0; j < N; ++j) {
                ++count[phase_color[j]];
            }
            for (int j = 1; j < phase_color.size(); ++j) {
                count[j] += count[j - 1];
            }
            std::vector<int> position_color(N);
            for (int j = 0; j < N; ++j) {
                position_color[phase[j]] = phase_color[j];
            }
            std::vector<std::pair<int, int>> next_phase_color(N);
            for (int j = N - 1; j >= 0; --j) {
                int new_index = --count[position_color[shifted_phase[j]]];
                next_phase[new_index] = shifted_phase[j];
                next_phase_color[new_index] = { position_color[shifted_phase[j]] , phase_color[j] };
            }
            for (int j = 1; j < N; ++j) {
                if (next_phase_color[j - 1] != next_phase_color[j]) {
                    phase_color[j] = phase_color[j - 1] + 1;
                } else {
                    phase_color[j] = phase_color[j - 1];
                }
            }
            phase = next_phase;
        }
        suff_array.resize(old_N);
        copy(phase.begin() + N - old_N, phase.end(), suff_array.begin());
    }

    const std::vector<int>& getArray()
    {
        return suff_array;
    }

private:
    std::vector<int> get_primary_phase(const std::string& s)
    {
        int N = s.size();
        std::vector<std::pair<char, int>> sorted_pos(N);
        for (int i = 0; i < N; ++i) {
            sorted_pos[i] = { s[i], i };
        }
        sort(sorted_pos.begin(), sorted_pos.end());
        std::vector<int> phase(N);
        for (int i = 0; i < N; ++i) {
            phase[i] = sorted_pos[i].second;
        }
        return phase;
    }
};

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string s;
    std::cin >> s;
    SuffArray suff_array(s);
    std::vector<int> ans = suff_array.getArray();
    for (auto it = ans.begin(); it < ans.end(); ++it) {
        std::cout << *it + 1 << " ";
    }
    std::cout << std::endl;
    return 0;
}