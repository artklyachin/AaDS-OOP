#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <complex>
#include <tuple>

const double PI = 3.14159265358979323846;

int reverse_bits(int number, int bits)
{
    int result = 0;
    for (int bit = 0; bit < bits; ++bit)
        if (number & (1 << bit))
            result |= 1 << (bits - 1 - bit);
    return result;
}

void DFT(std::vector<std::complex<double>>& A, int64_t sign)
{
    int N = static_cast<int>(A.size());
    int bits = 0;
    while ((1 << bits) < N) {
        ++bits;
    }

    for (int i = 0; i < N; ++i) {
        int j = reverse_bits(i, bits);
        if (i < j) {
            std::swap(A[i], A[j]);
        }
    }

    std::vector<std::complex<double>> roots(N);
    double angle = 2 * PI / N * sign;
    roots[0] = 1.0;
    if (N > 1) {
        roots[1] = std::complex<double>(cos(angle), sin(angle));
        for (int i = 2; i < N; ++i) {
            roots[i] = roots[i - 1] * roots[1];
        }
    }

    for (int block_size = 2; block_size <= N; block_size *= 2) {
        for (int i = 0; i < N; i += block_size) {
            for (int j = 0; j < block_size / 2; ++j) {
                std::complex<double> u = A[i + j];
                std::complex<double> v = A[i + j + block_size / 2] * roots[j * (N / block_size)];
                A[i + j] = u + v;
                A[i + j + block_size / 2] = u - v;
            }
        }
    }
}

std::vector<int64_t> convolution(const std::vector<int64_t>& first, const std::vector<int64_t>& second)
{
    int64_t result_size = first.size() + second.size() - 1;
    int64_t N = 1;
    while (N < result_size) {
        N *= 2;
    }
    std::vector<std::complex<double>> complex_A(N), complex_B(N);
    transform(first.begin(), first.end(), complex_A.begin(),
        [](const int64_t& value) { return static_cast<double>(value); });
    transform(second.begin(), second.end(), complex_B.begin(),
        [](const int64_t& value) { return static_cast<double>(value); });

    DFT(complex_A, 1);
    DFT(complex_B, 1);
    transform(complex_A.begin(), complex_A.end(), complex_B.begin(), complex_A.begin(),
        [](const std::complex<double>& a, const std::complex<double>& b) { return a * b; });

    DFT(complex_A, -1);
    double alpha = 1 / static_cast<double>(N);

    std::vector<int64_t> result;
    result.resize(result_size);
    transform(complex_A.begin(), complex_A.begin() + result_size, result.begin(),
        [&](const std::complex<double>& value) { return static_cast<int64_t>(round(value.real() * alpha)); });
    return result;
}

std::vector<int64_t> readPolynomial(std::istream& is = std::cin)
{
    int n;
    is >> n;
    std::vector<int64_t> coefficients;
    coefficients.resize(n + 1);
    for (auto it = coefficients.rbegin(); it != coefficients.rend(); ++it) {
        is >> *it;
    }
    return coefficients;
}

std::pair<std::vector<int64_t>, std::vector<int64_t>> readInput(std::istream& is = std::cin)
{
    return { readPolynomial(is), readPolynomial(is) };
}

void outputResult(const std::vector<int64_t>& result, std::ostream& os = std::cout)
{
    os << result.size() - 1;
    for (auto it = result.rbegin(); it != result.rend(); ++it) {
        os << " " << *it;
    }
    os << std::endl;
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    const auto [A, B] = readInput();
    auto C = convolution(A, B);
    outputResult(C);
    return 0;
}