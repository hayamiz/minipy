print {}                                # {}
print { 0 : 1, 2 : 3 }                  # { 0 : 1, 2 : 3 } or similar
print len({ 0 : 1, 2 : 3 })             # 2
print { 0 : 1, 2 : 3 }[2]               # 3

L = {}
L[10] = 200
L["hello"] = "world"
print L.has_key(0)
print L.has_key(10)
print L.has_key("hello")
print L[10]
print L["hello"]
del L[10]
print L.has_key(10)

print L.keys()
print L.values()
print L.items()

print L is L                            # 1
print { 0 : 1 } is { 0 : 1 }            # 0

