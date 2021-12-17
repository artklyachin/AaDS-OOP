#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <complex>

const double PI = 3.14159265358979323846;

class Polynomial
{
    std::vector<int64_t> coefficients_;

    static int reverse_bits(int number, int bits)
    {
        int result = 0;
        for (int bit = 0; bit < bits; ++bit)
            if (number & (1 << bit))
                result |= 1 << (bits - 1 - bit);
        return result;
    }

    static void DFT(std::vector<std::complex<double>>& A, int64_t sign)
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

        for (int block_size = 2; block_size <= N; block_size *= 2) {
            double ang = 2 * PI / block_size * sign;
            std::complex<double> step(cos(ang), sin(ang));
            for (int i = 0; i < N; i += block_size) {
                std::complex<double> w(1);
                for (int j = 0; j < block_size / 2; ++j) {
                    std::complex<double> u = A[i + j];
                    std::complex<double> v = A[i + j + block_size / 2] * w;
                    A[i + j] = u + v;
                    A[i + j + block_size / 2] = u - v;
                    w *= step;
                }
            }
        }
    }

public:
    size_t size() const
    {
        return coefficients_.size();
    }

    std::vector<int64_t>& getCoefficients()
    {
        return coefficients_;
    }

    const std::vector<int64_t>& getCoefficients() const
    {
        return coefficients_;
    }

    Polynomial operator*(const Polynomial& other) const
    {
        int64_t result_size = size() + other.size() - 1;
        int64_t N = 1;
        while (N < result_size) {
            N *= 2;
        }
        std::vector<std::complex<double>> complex_A(N), complex_B(N);
        transform(coefficients_.begin(), coefficients_.end(), complex_A.begin(),
            [](const int64_t& value) { return static_cast<double>(value); });
        transform(other.coefficients_.begin(), other.coefficients_.end(), complex_B.begin(),
            [](const int64_t& value) { return static_cast<double>(value); });

        DFT(complex_A, 1);
        DFT(complex_B, 1);
        transform(complex_A.begin(), complex_A.end(), complex_B.begin(), complex_A.begin(),
            [](const std::complex<double>& a, const std::complex<double>& b) { return a * b; });

        DFT(complex_A, -1);
        double alpha = 1 / static_cast<double>(N);

        Polynomial result;
        result.coefficients_.resize(result_size);
        transform(complex_A.begin(), complex_A.begin() + result_size, result.coefficients_.begin(),
            [&](const std::complex<double>& value) { return static_cast<int64_t>(round(value.real() * alpha)); });
        return result;
    }
};

Polynomial readInput(std::istream& is = std::cin)
{
    int n;
    is >> n;
    Polynomial pol;
    pol.getCoefficients().resize(n + 1);
    for (auto it = pol.getCoefficients().rbegin(); it != pol.getCoefficients().rend(); ++it) {
        is >> *it;
    }
    return pol;
}

void outputResult(const Polynomial& pol, std::ostream& os = std::cout)
{
    os << pol.getCoefficients().size() - 1;
    for (auto it = pol.getCoefficients().rbegin(); it != pol.getCoefficients().rend(); ++it) {
        os << " " << *it;
    }
    os << std::endl;
}


int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    auto A = readInput();
    auto B = readInput();
    outputResult(A * B);
    return 0;
}