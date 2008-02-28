
def gcd_aux(a, b):
    # a > b
    c = a % b
    while c != 0:
        a = b
        b = c
        c = a % b
    return b

def gcd(a, b):
    if a > b:
        return gcd_aux(a, b)
    else:
        return gcd_aux(b, a)

def fib(n):
    if n < 2:
        return 1
    else:
        return fib(n - 1) + fib(n - 2)

print gcd(fib(15), fib(14))
print gcd(fib(17), fib(16))
print gcd(fib(19), fib(18))
print gcd(fib(21), fib(20))
print gcd(fib(23), fib(22))
print gcd(fib(25), fib(24))

