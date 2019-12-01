#include <iostream>
#include <vector>
#include <cmath>
#include <utility>
#include <tuple>

long long int inverse(long long int a, long long int m);

long long int find_x1(long long int A, long long int M, long long int x2, long long int x3);

using namespace std;

/**
 * Return prime factors, each prime included only once
 * @param n - number to factorize
 * @return vector of prime factors
 */
vector<long long int> prime_factors_set(long long int n) {
    vector<long long int> factors;
    auto handlePossibleFactor = [&](long long int f) {
        if (n % f == 0) {
            factors.push_back(f);
            n /= f;
        }
        while (n % f == 0) {
            n /= f;
        }
    };

    handlePossibleFactor(2);
    long long int stop = (long long int) sqrt(n)+1;
    for (long long int i = 3;  i < stop; i+= 2){
        handlePossibleFactor(i);
    }
    if (n > 2) {
        factors.push_back(n);
    }
    return factors;
}

long long int gcd(long long int a, long long int b) {
    if (a < b) {
        swap(a, b);
    }
    while (a % b > 0) {
        tie(a, b) = make_pair(b, a % b);
    }
    return b;
}

int main() {
    ios_base::sync_with_stdio(false);
    long long int M, x2, x3;
    cin >> M >> x2 >> x3;
    auto factors = prime_factors_set(M);
    long long int candidate = 1;
    for (auto p : factors) {
        candidate *= p;
    }
    if (M % 4 == 0) {
//        candidate *= gcd(candidate, 4);
        candidate *= 2; //if M divisible by 4, candidate already constructed by multiplying 2, so just multiply again
    }

    long long int x1_max = -1;
    int64_t x1_min = INT64_MAX;
    uint64_t count = 0;

    for (long long int A = candidate+1; A < M; A+=candidate) {
        long long int x1 = find_x1(A, M, x2, x3);
        if (x1_max < x1) {
            x1_max = x1;
        }
        if (x1_min > x1) {
            x1_min = x1;
        }
        count++;
    }

    cout << count << " " << x1_min << " " << x1_max << endl;

    return 0;
}

long long int find_x1(long long int A, long long int M, long long int x2, long long int x3) {
    long long int C = (x3 - A*x2) % M;
    long long int shift = (x2 - C) % M;
    long long int x1 = (inverse(A, M) * shift) % M;

    return x1 < 0 ? x1 + M : x1;
}

long long int inverse(long long int a, long long int m) {
    long long int t = 0, r = m, newt = 1, newr = a;
    while (newr != 0) {
        long long int q = r / newr;
        tie(t, newt) = make_pair(newt, t - (q * newt));
        tie(r, newr) = make_pair(newr, r - (q * newr));
    }
    if (t < 0) {
        t += m;
    }
    return t;
}
