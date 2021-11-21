#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Solution
{
    static constexpr int64_t MOD = 1e9 + 7;
    vector<int64_t> prime_numbers;

    Solution()
    {
        int64_t bound = 1e6 + 7;
        for (int64_t i = 2; i < bound; ++i) {
            if (i == findLeastPrimeDivisor(i)) {
                prime_numbers.push_back(i);
            }
        }
    }

    int64_t findLeastPrimeDivisor(int64_t x)
    {
        int64_t least_prime_divisor = x;
        for (int64_t num : prime_numbers) {
            if (num * num > x) break;
            if (x % num == 0) {
                least_prime_divisor = num;
                break;
            }
        }
        return least_prime_divisor;
    }

public:
    int64_t solve(int64_t N)
    {
        int64_t sum = 0;
        for (int64_t i = 3; i <= N; ++i) {
            int64_t least_prime_divisor = findLeastPrimeDivisor(i);
            if (i != least_prime_divisor) {
                sum += least_prime_divisor;
            }
        }
        return sum;
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int64_t N;
    cin >> N;
    cout << Solution().solve(N) << endl;
    return 0;
}

/*
1 2 2 4

1000000006
*/