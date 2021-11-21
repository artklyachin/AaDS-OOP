#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <complex>

using namespace std;

# define PI 3.14159265358979323846

struct Solution
{
private:
    static constexpr int64_t MOD = 1e9 + 7;

    vector<complex<double>> DFT(vector<complex<double>> A, int64_t sign)
    {
        if (A.size() == 1) {
            return A;
        }

        int64_t N = A.size();
        vector<complex<double>> A0, A1;
        for (int64_t i = 0; i < N / 2; ++i) {
            A0.push_back(A[2 * i]);
            A1.push_back(A[2 * i + 1]);
        }
        vector<complex<double>> values_A0 = DFT(A0, sign);
        vector<complex<double>> values_A1 = DFT(A1, sign);

        vector<complex<double>> values_A(N);
        double ang = sign * 2 * PI / N;
        complex<double> w = 1;
        for (int64_t i = 0; i < N / 2; ++i) {
            values_A[i] = values_A0[i] + w * values_A1[i];
            values_A[i + N / 2] = values_A0[i] - w * values_A1[i];
            w = complex<double>(cos((i + 1) * ang), sin((i + 1) * ang));
        }
        return values_A;
    }

public:
    vector<int64_t> solve(vector<int64_t>& A, vector<int64_t>& B)
    {
        int64_t ans_len = A.size() + B.size() - 1;
        int64_t N = 1;
        while (N < ans_len) N *= 2;
        A.resize(N);
        B.resize(N);
        vector<complex<double>> complex_A(N), complex_B(N);
        for (int64_t i = 0; i < N; ++i) {
            complex_A[i] = A[i];
        }
        for (int64_t i = 0; i < N; ++i) {
            complex_B[i] = B[i];
        }

        vector<complex<double>> value_A = DFT(complex_A, 1);
        vector<complex<double>> value_B = DFT(complex_B, 1);
        vector<complex<double>> value_C(value_A.size());
        for (size_t i = 0; i < N; ++i) {
            value_C[i] = value_A[i] * value_B[i];
        }

        vector<complex<double>> complex_C = DFT(value_C, -1);
        for (int64_t i = 0; i < N; ++i) {
            complex_C[i] /= N;
        }
        vector<int64_t> C(N);
        for (int64_t i = 0; i < N; ++i) {
            C[i] = int64_t(round(real(complex_C[i])));
        }
        C.resize(ans_len);
        return C;
    }
};


int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    vector<int64_t> A, B;
    int64_t n, m;
    cin >> n;
    for (int64_t i = 0; i <= n; ++i) {
        int64_t num;
        cin >> num;
        A.push_back(num);
    }
    cin >> m;
    for (int64_t i = 0; i <= m; ++i) {
        int64_t num;
        cin >> num;
        B.push_back(num);
    }
    reverse(A.begin(), A.end());
    reverse(B.begin(), B.end());
    vector<int64_t> C = Solution().solve(A, B);
    reverse(C.begin(), C.end());
    cout << int64_t(C.size()) - 1 << " ";
    for (int64_t i : C) {
        cout << i << " ";
    }
    cout << endl;
    return 0;
}

/*

4 3
2 1 0 2
1 2 3
*/