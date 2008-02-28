print []                                # []
print [ 0, 1, 2, 3 ]                    # [ 0, 1, 2, 3 ]
print len([ 0, 1, 2, 3 ])               # 4
print [ 0, 1, 2, 3 ][2]                 # 2

print [ 0, 1, 2, 3 ] + [ 4, 5, 6 ]      # [ 0, 1, 2, 3, 4, 5, 6 ]

L = range(0, 100)
print L[10]                             # 10
L[10] = 200
print L[10]                             # 200
print L.pop(0)                          # 0
print L[9]                              # 200
del L[8]
print L[8]                              # 200

print 0 in L                            # 0 (0 has been poped)
print 10 in L                           # 0 (10 has been replaced with 200)
print 11 in L                           # 1
print 200 in L                          # 1

print L is L                            # 1
print [ 0, 1 ] is [ 0, 1 ]              # 0
print [ 0, 1 ] == [ 0, 1 ]              # 1
print [ 0, 1 ] == [ 0, 1, 2 ]           # 0

