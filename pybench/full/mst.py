# see ../graphics/README
#
# You can run this program with native python if you uncomment the following five lines.
# ---------------- FOR NATIVE PYTHON ----------------
import sys,random
def print_string(s):
    sys.stdout.write(s)
def ntv_random():
    return random.randint(0, (1 << 30))
#
# ---------------- FOR mini-PYTHON ----------------
# (1) if you do not support string, you should define the following print_* functions as
# native functions and comment out definitions below.
# they are all simple functions that can be easily added.
# 
# (2) if you do support string, you only have to define two native functions, print_float(x) and
# print_string(s). other print_* functions can be defined using print_string.
# 

def print_float(x):
    print_string(str(x))
def print_space():
    print_string(" ")
def print_newline():
    print_string("\n")
def print_moveto():
    print_string("moveto")
def print_lineto():
    print_string("lineto")
def print_stroke():
    print_string("stroke")
def print_PS_comment():
    print_string("%!")
def print_showpage():
    print_string("showpage")
def print_setrgbcolor():
    print_string("setrgbcolor")
def print_closepath():
    print_string("closepath")
def print_fill():
    print_string("fill")

#
# also, you should have two functions ntv_random and ntv_srand.
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
# ----------------------- REAL PORGRAM SECTION BEGINS -----------------------

with_graphics = 1

center_x = 100.0
center_y = 100.0
xmag = 500.0
ymag = 500.0

def draw_segment(ax, ay, bx, by):
    print_float(center_x + ax * xmag)
    print_space()
    print_float(center_y + ay * ymag)
    print_space()
    print_moveto()
    print_space()
    print_float(center_x + bx * xmag)
    print_space()
    print_float(center_y + by * ymag)
    print_space()
    print_lineto()
    print_space()
    print_stroke()
    print_newline()
    
def fill_rectangle(x, y, w, h, z):
    print_float(z)
    print_space()
    print_float(0.0)
    print_space()
    print_float(1 - z)
    print_space()
    print_setrgbcolor()
    print_newline()
    
    print_float(center_x + x * xmag)
    print_space()
    print_float(center_y + y * ymag)
    print_space()
    print_moveto()
    print_space()
    
    print_float(center_x + (x + w) * xmag)
    print_space()
    print_float(center_y + y * ymag)
    print_space()
    print_lineto()
    print_space()

    print_float(center_x + (x + w) * xmag)
    print_space()
    print_float(center_y + (y + h) * ymag)
    print_space()
    print_lineto()
    print_space()

    print_float(center_x + x * xmag)
    print_space()
    print_float(center_y + (y + h) * ymag)
    print_space()
    print_lineto()
    print_space()

    print_closepath()
    print_space()
    print_fill()
    print_newline()

def draw_x_axis(A, B):
    draw_segment(A, 0, B, 0)
    x = A
    while x < B:
        draw_segment(x, -0.02, x, 0.02)
        x = x + 1.0
    
def draw_y_axis(A, B):
    draw_segment(0, A, 0, B)
    y = A
    while y < B:
        draw_segment(-0.02, y, 0.02, y)
        y = y + 1.0

#def ntv_random():
#    import random
#    return random.randint(0, (1 << 30))

def random_float():
    r = ntv_random()
    r = r & ~(-1 << 25)
    return (r + 0.0) / (1 << 25)

def dist2(P, Q):
    dx = P[0] - Q[0]
    dy = P[1] - Q[1]
    return dx * dx + dy * dy

def gen_geograph(n, th):
    nodes = {}
    edges = {}
    for i in range(0, n):
        x = random_float()
        y = random_float()
        nodes[i] = (x,y)
    for i in range(0, n):
        for j in range(i + 1, n):
            d2 = dist2(nodes[i], nodes[j])
            if d2 <= th:
                edges[i,j] = d2
    return nodes,edges

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

def my_sort(L):
    my_sort_aux(L, 0, len(L))
    return L

def all_edges(E):
    Es = []
    for sd in E.keys():
        Es.append((E[sd], sd))
    my_sort(Es)
    Et = []
    for x_sd in Es:
        Et.append(x_sd[1])
    return Et

def mk_parent_map(n):
    P = {}
    for i in range(0, n):
        P[i] = (i, 0)
    return P

def find_root(P, x):
    while 1:
        p = P[x][0]
        if p == x:
            return p
        else:
            x = p

def union(P, x, y):
    # P[x] == x,dx
    # P[y] == y,dy
    dx = P[x][1]
    dy = P[y][1]
    if dx < dy:
        P[x] = (y,dx)
    elif dy > dx:
        P[y] = (x,dy)
    else:
        P[y] = (x,dy)
        P[x] = (x,dx + 1)

def mst(G):
    V = G[0]
    E = G[1]
    P = mk_parent_map(len(V))
    taken = {}
    for sd in all_edges(E):
        s = sd[0]
        d = sd[1]
        s_ = find_root(P, s)
        d_ = find_root(P, d)
        if s_ != d_:
            taken[s,d] = 1
            union(P, s_, d_)
    return V,taken

def show_graph(G):
    V = G[0]
    E = G[1]
    for e in E.keys():
        s = e[0]                        # src
        d = e[1]                        # dest
        ax = V[s][0]
        ay = V[s][1]
        bx = V[d][0]
        by = V[d][1]
        draw_segment(ax, ay, bx, by)
    for n_xy in V.items():
        x = n_xy[1][0]
        y = n_xy[1][1]
        w = 0.01
        fill_rectangle(x - w / 2.0, y - w / 2.0, w, w, 1.0)
    
def main():
    print_PS_comment()
    print_newline()
    draw_x_axis(0.0, 1.0)
    draw_y_axis(0.0, 1.0)
    G = gen_geograph(20, 0.2)
    show_graph(G)
    T = mst(G)
    show_graph(T)
    print_showpage()
    print_newline()
    
main()
