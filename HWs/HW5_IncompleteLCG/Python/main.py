from math import sqrt
from math import inf
from functools import reduce
from operator import mul


def prime_factors(n):
    factors = []
    while n % 2 == 0:
        factors.append(2)
        n //= 2
    for i in range(3, int(sqrt(n)+1), 2):
        while n % i == 0:
            factors.append(i)
            n //= i

    if n > 2:
        factors.append(n)
    return factors


def gcd(a, b):
    a, b = (a, b) if a >= b else (b, a)
    while a % b > 0:
        a, b = b, a % b
    return b


def pos_mod(a, m):
    tmp = a % m
    return tmp if tmp >= 0 else tmp + m


def inverse(a, n):
    t, r, newt, newr = 0, n, 1, a
    while newr != 0:
        q = r // newr
        t, newt = newt, t - (q * newt)
        r, newr = newr, r - (q * newr)
    if r > 1:
        raise ValueError("not invertible")
    if t < 0:
        t += n
    return t


def find_x1(A, M, inv, x2, x3):
    C = (x3 - A*x2) % M
    shift = (x2 - C) % M
    x1 = pos_mod((inv * shift), M)
    return x1


if __name__ == '__main__':
    [M, x2, x3] = [int(i) for i in input().split()]
    factors = prime_factors(M)
    candidate = reduce(mul, set(factors))
    cnt = 0
    x1_max = -inf
    x1_min = inf
    if M % 4 == 0:
        candidate *= gcd(candidate, 4)
    for A in range(candidate+1, M, candidate):
        inv = inverse(A, M)
        x1 = find_x1(A, M, inv, x2, x3)
        if x1_max < x1:
            x1_max = x1
        if x1_min > x1:
            x1_min = x1
        cnt += 1
    print(f'{cnt} {x1_min} {x1_max}')


