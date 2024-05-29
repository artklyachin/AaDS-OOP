#include <iostream>
#include <string>
#include <vector>


std::vector<int> z_function(const std::string& text)
{
	std::vector<int> res;
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
	return res;
}


int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::string text, pattern;
	int num_of_mistakes;
	std::cin >> text >> pattern >> num_of_mistakes;

	int text_size = int(text.size()), pattern_size = int(pattern.size()), shift = pattern_size + 1;
	std::string z_func_str(pattern_size + 1 + text_size, '#');

	copy(pattern.begin(), pattern.end(), z_func_str.begin());
	copy(text.begin(), text.end(), z_func_str.begin() + shift);
	auto result1 = z_function(z_func_str);

	copy(pattern.rbegin(), pattern.rend(), z_func_str.begin());
	copy(text.rbegin(), text.rend(), z_func_str.begin() + shift);
	auto result2 = z_function(z_func_str);

	std::vector<int> result;
	for (int i = 0; i + pattern_size <= text_size; ++i) {
		if (result1[shift + i] + num_of_mistakes + result2[shift + text_size - (pattern_size + i)] >= pattern_size) {
			result.push_back(i);
		}
	}
	std::cout << result.size() << std::endl;
	for (int& v : result) {
		std::cout << v + 1 << " ";
	}
	std::cout << std::endl;
	return 0;
}


/*
mxxgdpylr axg 2                                                                                                         2                                                                                                                       1 2
*/