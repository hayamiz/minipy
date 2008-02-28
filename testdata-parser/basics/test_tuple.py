print ()                                # []
print (10)                              # 10 (do not confuse with (10,)
print (10,)                             # (10,) this is a tuple
print (0, 1, 2, 3)

print len(())                           # 0
print len(("hello"))                    # 5, not 1
print len(("hello", ))                  # 1
print len(("hello", "world"))           # 2

print (0, 1, 2, 3) + (4, 5, 6)          # tuple concatination

L = (0, 1, 2)
print L is L                            # 1
print (0, 1, 2) is (0, 1, 2)            # 0
print (0, 1, 2) == (0, 1, 2)            # 1
print (0, 1, 2) == (0, 1, 2, 3)         # 0

