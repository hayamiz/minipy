# see ../graphics/README
#
# You can run this program with native python if you uncomment the following five lines.
# ---------------- FOR NATIVE PYTHON ----------------
# import sys,random
# def print_string(s):
#     sys.stdout.write(s)
# def ntv_random():
#     return random.randint(0, (1 << 30))
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
#

center_x = 200.0
center_y = 200.0
xmag = 20.0
ymag = 20.0

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

# [ [x, y], [vx, vy] ]
#
# A - B

def subvec(A, B):
    Ax = A[0]
    Ay = A[1]
    Bx = B[0]
    By = B[1]
    return (Ax - Bx, Ay - By)

def addvec(A, B):
    Ax = A[0]
    Ay = A[1]
    Bx = B[0]
    By = B[1]
    return (Ax + Bx, Ay + By)

def scalar(k, A):
    Ax = A[0]
    Ay = A[1]
    return (k * Ax, k * Ay)

def my_abs(x):
    if x < 0.0:
        return -x
    else:
        return x

def my_sqrt(C):
    x = C
    while 1:
        y = x * x - C
        x_ = x - y / (2.0 * x)
        d = my_abs(x - x_)
        if d < 0.0001:
            return x_
        x = x_

def dist2(P, Q):
    dx = P[0] - Q[0]
    dy = P[1] - Q[1]
    d2 = dx * dx + dy * dy
    return d2

def calc_accel(P, Q):
    r2 = dist2(P, Q)
    r = my_sqrt(r2)
    r3 = r2 * r
    return scalar(r, subvec(Q, P))

def calc_accels(C):
    A = {}
    n = len(C)
    for i in range(0, n):
        A[i] = [ 0.0, 0.0 ]
    for i in range(0, n):
        for j in range(i + 1, n):
            a = calc_accel(C[i], C[j])
            A[i] = addvec(A[i], a)
            A[j] = subvec(A[j], a)
    return A
            
def move_bodies(C, V, A, dt):
    for i in A.keys():
        V[i] = addvec(V[i], scalar(dt, A[i]))
    for i in A.keys():
        C[i] = addvec(C[i], scalar(dt, V[i]))

def step(C, V, dt):
    A = calc_accels(C)
    move_bodies(C, V, A, dt)

def show_bodies(C):
    for c in C.values():
        x = c[0]
        y = c[1]
        w = 0.11
        fill_rectangle(x - w, y - w, 2.0 * w, 2.0 * w, 1.0)
    print_showpage()
    print_newline()
    
def steps(C, V, dt, T):
    for t in range(0, T):
        step(C, V, dt)
        show_bodies(C)

def random_float():
    r = ntv_random()
    r = r & ~(-1 << 25)
    x = (r + 0.0) / (1 << 25)
    return x

def gen_random_bodies(n):
    C = {}
    V = {}
    for i in range(0, n):
        C[i] = [ 10.0 * random_float(), 10.0 * random_float() ]
        V[i] = [ 0.0, 0.0 ]
    return (C, V)

def main():
    print_PS_comment()
    print_newline()
    
    CV = gen_random_bodies(20)
    C = CV[0]
    V = CV[1]
    steps(C, V, 0.01, 50)

main()
