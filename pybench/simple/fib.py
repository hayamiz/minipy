def fib(n):
    if n < 2:
        return 1
    else:
        return fib(n - 1) + fib(n - 2)

def ffib(n):
    if n < 2:
        return 1.0
    else:
        return ffib(n - 1) + ffib(n - 2)

print fib(18)
print ffib(18)
print fib(20)
print ffib(20)
print fib(22)
print ffib(22)
print fib(24)
print ffib(24)
print fib(26)
print ffib(26)

