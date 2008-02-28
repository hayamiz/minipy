def mk_dict(n):
    D = {}
    i = 0
    while i < n:
        D[i] = i
        i = i + 1
    return D
    
def sum_dict(D):
    s = 0
    for v in D.values():
        s = s + v
    return s
    
def sum_dict_key(D):
    s = 0
    for k in D.keys():
        s = s + D[k]
    return s
    
i = 0
while i < 10:
    print len(mk_dict(10))
    print len(mk_dict(100))
    print len(mk_dict(1000))
    print sum_dict(mk_dict(10))
    print sum_dict_key(mk_dict(10))
    print sum_dict(mk_dict(100))
    print sum_dict_key(mk_dict(100))
    # print sum_dict(mk_dict(1000))
    # print sum_dict_key(mk_dict(1000))
    # print sum_dict(mk_dict(10000))
    # print sum_dict_key(mk_dict(10000))
    i = i + 1

