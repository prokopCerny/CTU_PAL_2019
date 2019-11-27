from math import sqrt
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

if __name__ == '__main__':
    [M, _, _] = [int(i) for i in input().split()]
    factors = prime_factors(M)
    candidate = reduce(mul, set(factors))
    cnt = 0
    for A in range(candidate+1, M+1, candidate):
        if M % 4 == 0:
            if (A-1)%4 == 0:
                cnt+=1
        else:
            cnt += 1
    print(cnt)


