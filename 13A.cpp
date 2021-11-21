#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Solution
{
private:
    static constexpr int64_t MOD = 1e9 + 7;

    int64_t power(int64_t x, int64_t n)
    {
        int64_t pow = x, result = 1;
        while (n > 0) {
            if (n % 2) {
                result *= pow;
                result %= MOD;
            }
            n /= 2;
            pow *= pow;
            pow %= MOD;
        }
        return result;
    }

    int64_t multiplicative_inverse(int64_t x)
    {
        return power(x, MOD - 2);
    }

public:
    int64_t solve(int64_t a, int64_t b, int64_t c, int64_t d)
    {
        a = a < 0 ? a + MOD : a;
        b = b < 0 ? b + MOD : b;
        c = c < 0 ? c + MOD : c;
        d = d < 0 ? d + MOD : d;
        int64_t result = (a * d) % MOD + (b * c) % MOD;
        result %= MOD;
        result *= multiplicative_inverse((b * d) % MOD);
        result %= MOD;

        return result;
    }
};

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int64_t a, b, c, d;
    cin >> a >> b >> c >> d;
    cout << Solution().solve(a, b, c, d) << endl;
    return 0;
}

/*
1 2 2 4

1000000006
*/