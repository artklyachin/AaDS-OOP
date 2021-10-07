#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <vector>


using namespace std;

    
pair<int64_t, int64_t> Partition(vector<int64_t>& A, int64_t l, int64_t r, int64_t x)
{
    int64_t less_than_x = 0, equal_to_x = 0, more_than_x = 0;
    for (int64_t i = l; i < r; ++i) {
        if (A[i] < x) {
            ++less_than_x;
        } else if (A[i] == x) {
            ++equal_to_x;
        } else {
            ++more_than_x;
        }
    }
    int64_t k2 = l + less_than_x, k3 = l + less_than_x + equal_to_x;
    for (int64_t i = l; i < r; ++i) {
        while (true) {
            int64_t elem = A[i];
            if (i - l < less_than_x) {
                if (elem < x) {
                    break;
                } else if (elem == x) {
                    swap(A[i], A[k2]);
                    ++k2;
                } else {
                    swap(A[i], A[k3]);
                    ++k3;
                }
            } else if (i - l < less_than_x + equal_to_x) {
                
                if (elem == x) {
                    break;
                } else {
                    swap(A[i], A[k3]);
                    ++k3;
                }
            } else {
                break;
            }
        }
    }
    return { less_than_x, equal_to_x };
}



void sorted(vector<int64_t>& A, int64_t l, int64_t r, int64_t d)
{
    for (int64_t i = (d + 1) / 2; i < A.size() + d; i += d) {
        int64_t index_first = l + d * i + (d / 5 + 1) / 2, step = d / 5;
        if (index_first >= r) continue;
        int64_t& i0 = A[index_first];
        if (index_first + step >= r) continue;
        int64_t& i1 = A[index_first + step];
        if (i1 > i0) swap(i1, i0);
        if (index_first + 2 * step >= r) continue;
        int64_t& i2 = A[index_first + 2 * step];
        if (i2 > i0) swap(i2, i0);
        if (i2 > i1) swap(i2, i1);
        if (index_first + 3 * step >= r) continue;
        int64_t& i3 = A[index_first + 3 * step];
        if (i3 > i0) swap(i3, i0);
        if (i3 > i1) swap(i3, i1);
        if (i3 > i2) swap(i3, i2);
        if (index_first + 4 * step >= r) continue;
        int64_t& i4 = A[index_first + 4 * step];
        if (i4 > i0) swap(i4, i0);
        if (i4 > i1) swap(i4, i1);
        if (i4 > i2) swap(i4, i2);
        if (i4 > i3) swap(i4, i3);
    }
}


int64_t median(vector<int64_t>& A, int64_t l, int64_t r, int64_t d)
{
    sorted(A, l, r, d);
    int64_t index_first = l + (d / 5 + 1) / 2, step = d / 5;
    if (r - l <= d) {
        if (r <= index_first + 2 * step) return A[index_first];
        else if (r <= index_first + 4 * step) return A[index_first + step];
        else if (r <= index_first + 6 * step) return A[index_first + 2 * step];
    }
    return median(A, l, r, 5 * d);
}


int64_t find_statistics(vector<int64_t>& A, int64_t l, int64_t r, int64_t k)
{
    while (r - l > 1) {
        int64_t x = median(A, l, r, 5);
        auto mid = Partition(A, l, r, x);
        int64_t less_than_x = mid.first;
        int64_t equal_to_x = mid.second;
        if (k >= less_than_x && k < less_than_x + equal_to_x) {
            return A[l + less_than_x];
        }
        if (k < less_than_x) {
            r = l + less_than_x;
        } else {
            l += less_than_x + equal_to_x;
            k -= less_than_x + equal_to_x;
        }
    }
    return A[l];
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int64_t n, k;
    cin >> n >> k;
    vector<int64_t> A(n);
    for (int64_t i = 0; i < n; ++i) {
        cin >> A[i];
    }
    cout << find_statistics(A, 0, n, k) << "\n";
}
