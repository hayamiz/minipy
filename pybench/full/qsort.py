#
# quicksort
#
#
# You can run this program with native python if you uncomment the following three lines.
# ---------------- FOR NATIVE PYTHON ----------------
import random
def ntv_random():
    return random.randint(0, (1 << 30))
#
# ---------------- FOR mini-PYTHON ----------------
#
# you should have two functions ntv_random and ntv_srand.
# ntv_random should return a random integer. ntv_srand is used to set random seed.
# they can be implemented by calling C library's random/srand functions, respectively.
# for example, you can add something like the following to your interpreter.
#
# py_val_t ntv_random(env_t genv, stack_trace_t bt, src_pos_t pos)
# {
#   int r = random() & ~(1 << 30);
#   return mk_py_int(r);
# }
# py_val_t ntv_srand(env_t genv, stack_trace_t bt, src_pos_t pos, py_val_t seed)
# {
#   srand(py_val_int(seed, bt, pos));
#   return py_val_none;
# }
#
#

# ----------------------- REAL PORGRAM SECTION BEGINS -----------------------
#
#
# create a random list of n elements.
#

def mk_random_list(n):
    L = []
    for i in range(0, n):
        L.append(ntv_random())
    return L

def swap(L, i, j):
    t = L[i]
    L[i] = L[j]
    L[j] = t

def baka_sort(L):
    for i in range(0, len(L) - 1):
        mv = L[i]
        mj = i
        for j in range(i + 1, len(L)):
            if L[j] < mv:
                mj = j
                mv = L[j]
        swap(L, i, mj)
    return L

def my_sort_aux(L, p, q):
    if q - p < 5:                       # 0, 1, 2
        baka_sort(L)
    else:
        abc = baka_sort([ L[p], L[(p + q)/2], L[q - 1] ])
        piv = abc[1]
        L[p] = abc[0]
        L[(p + q)/2] = piv
        L[q - 1] = abc[2]
        i = p
        j = q - 1
        while i < j:
            while L[i] < piv:
                i = i + 1
            while L[j] > piv:
                j = j - 1
            if i < j:
                swap(L, i, j)
                i = i + 1
                j = j - 1
        my_sort_aux(L, p, j - 1)
        my_sort_aux(L, i + 1, q)

def sort(L):
    my_sort_aux(L, 0, len(L))
    return L

def check_sorted(L):
    if len(L) > 0:
        t = L[0]
        for x in L:
            if x < t:
                bomb()

def main():
    L = mk_random_list(150)
    sort(L)                            # sort(L)
    check_sorted(L)
    print L
    
main()
