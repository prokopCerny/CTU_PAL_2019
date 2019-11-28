#include <iostream>
#include <vector>
#include <cmath>
#include <utility>

long long int inverse(long long int a, long long int m);

long long int find_x1(long long int A, long long int M, long long int inv, long long int x2, long long int x3);

using namespace std;

vector<long long int> prime_factors_set(long long int n) {
    vector<long long int> factors;
    if (n % 2 == 0) {
        factors.push_back(2);
        n /= 2;
    }

    while (n%2 == 0) {
        n /= 2;
    }

    long long int stop = sqrt(n)+1;
    for (long long int i = 3;  i < stop; i+= 2){
        if (n % i == 0) {
            factors.push_back(i);
            n /= i;
        }
        while (n % i == 0) {
            n /= i;
        }
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
        long long int r = a % b;
        a = b;
        b = r;
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
        candidate *= gcd(candidate, 4);
    }

    long long int x1_max = -1;
    int64_t x1_min = INT64_MAX;
    uint64_t count = 0;

    for (long long int A = candidate+1; A < M; A+=candidate) {
        long long int inv = inverse(A, M);
        long long int x1 = find_x1(A, M, inv, x2, x3);
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

long long int find_x1(long long int A, long long int M, long long int inv, long long int x2, long long int x3) {
    long long int C = (x3 - A*x2) % M;
    long long int shift = (x2 - C) % M;
    long long int x1 = (inv*shift) % M;
    if (x1 < 0) {
        x1 += M;
    }
    return x1;
}

long long int inverse(long long int a, long long int m) {
    long long int t = 0;
    long long int r = m;
    long long int newt = 1;
    long long int newr = a;
    while (newr != 0) {
        long long int q = r / newr;
        long long int tmp = newt;
        newt = t - (q * newt);
        t = tmp;

        tmp = newr;
        newr = r - (q * newr);
        r = tmp;
    }
    if (t < 0) {
        t += m;
    }
    return t;
}
