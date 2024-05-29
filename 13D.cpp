#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <numeric>

using namespace std;

void build_sieve(vector<int>& sieve)
{
    int count = 0;
    iota(sieve.begin(), sieve.end(), 0);
    int64_t limit = sieve.size() - 1;
    for (int64_t n = 2; n <= limit; ++n) {
        if (sieve[n] == n) {
            ++count;
            for (int64_t k = n * n; k <= limit; k += n) {
                sieve[k] = int(n);
            }
        }
    }
    //cerr << "found " << count << " primes upto " << limit << "\n";
}


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<int> sieve(10000000);
    build_sieve(sieve);
    int N;
    cin >> N;
    vector<int> A(N);
    for (int& v : A) {
        cin >> v;
    }
    unordered_set<int> usedPrimes(N);
    vector<int> B;
    B.reserve(A.size());
    for (int i = 0; i < N; ++i) {
        for (int v = A[i];; ++v) {
            int w = v;
            while (w != 1) {
                if (usedPrimes.count(sieve[w])) {
                    break;
                }
                w /= sieve[w];
            }
            if (w == 1) {
                B.push_back(v);
                break;
            }
        }
        int w = B[i];
        while (w != 1) {
            usedPrimes.insert(sieve[w]);
            w /= sieve[w];
        }
        if (B[i] != A[i]) {
            break;
        }
    }
    int p = 2;
    for (int i = int(B.size()); i < N; ++i) {
        while (sieve[p] != p || usedPrimes.count(p) != 0) {
            ++p;
        }
        B.push_back(p);
        ++p;
    }
    for (int v : B) {
        cout << v << " ";
    }
    cout << "\n";
    return 0;
}
