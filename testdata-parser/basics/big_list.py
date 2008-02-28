def mk_list_fast(n):
    L = []
    i = 0
    while i < n:
        L.append(i)                     # takes O(1) time
        i = i + 1
    return L
    
def mk_list_slow(n):
    L = []
    i = 0
    while i < n:
        L = L + [ i ]                   # takes O(#L) time
        i = i + 1
    return L

def list_sum_fast(L):
    s = 0
    for i in L:
        s = s + i
    return s

def list_sum_perhaps_slow(L):
    i = 0
    s = 0
    n = len(L)
    while i < n:
        s = s + L[i]
        i = i + 1
    return s

i = 0
while i < 10:
    print len(mk_list_fast(10))
    print len(mk_list_slow(10))
    print len(mk_list_fast(100))
    print len(mk_list_slow(100))
    print len(mk_list_fast(1000))
    print len(mk_list_slow(1000))
    # print len(mk_list_fast(3000))
    # print len(mk_list_slow(3000))
    print list_sum_fast(mk_list_fast(10))
    print list_sum_perhaps_slow(mk_list_fast(10))
    print list_sum_fast(mk_list_fast(100))
    print list_sum_perhaps_slow(mk_list_fast(1000))
    print list_sum_fast(mk_list_fast(1000))
    print list_sum_perhaps_slow(mk_list_fast(1000))
    # print list_sum_fast(mk_list_fast(3000))
    # print list_sum_perhaps_slow(mk_list_fast(3000))
    i = i + 1
    

