# see ../graphics/README
#
# You can run this program with native python if you uncomment the following three lines.
# ---------------- FOR NATIVE PYTHON ----------------
# import sys
# def print_string(s):
#    sys.stdout.write(s)
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

# ----------------------- REAL PORGRAM SECTION BEGINS -----------------------

with_graphics = 1

center_x = 300.0
center_y = 300.0
xmag = 100.0
ymag = 100.0

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

def draw_curve_parameter(P, Q, A, B, dt):
    t = A
    while t < B:
        draw_segment(P(t), Q(t), P(t+dt), Q(t+dt))
        t = t + dt

def identity_function(x):
    return x

def draw_curve_function(f, A, B, dx):
    draw_curve_parameter(identity_function, f, A, B, dx)

def draw_dict(f, A, B, n, C, D, m, min_z, max_z):
    dx = (B - A) / (n + 1)
    dy = (D - C) / (m + 1)
    for i in range(0, n + 1):
        x = (A * (n + 1 - i) + B * i) / (n + 1)
        for j in range(0, m + 1):
            if f.has_key((i, j)):
                y = (C * (m + 1 - j) + D * j) / (m + 1)
                z = f[i,j]
                c = (z - min_z) / (max_z - min_z)
                fill_rectangle(x, y, dx, dy, c)

def init(n, m):
    f = {}
    for j in range(1, m):
        f[0,j] = 1.0
        f[n,j] = 0.0
    for i in range(1, n):
        f[i,0] = 1.0
        f[i,m] = 0.0
    for i in range(1, n):
        for j in range(1, m):
            f[i,j] = 0.0
    return f

def my_abs(x):
    if x < 0.0:
        return -x
    else:
        return x
    
def solve_aux(f, n, m):
    for i in range(1, n):
        for j in range(1, m):
            f[i,j] = (f[i - 1,j] + f[i + 1,j] + f[i,j - 1] + f[i,j + 1]) * 0.25
    epsilon = 0.0
    for i in range(1, n):
        for j in range(1, m):
            d = f[i,j] - (f[i - 1,j] + f[i + 1,j] + f[i,j - 1] + f[i,j + 1]) * 0.25
            epsilon = epsilon + my_abs(d)
    return epsilon
            
def solve(f, Ax, Bx, n, Ay, By, m):
    while 1:
        draw_dict(f, Ax, Bx, n, Ay, By, m, 0.0, 1.0)
        print_showpage()
        print_newline()
        ep = solve_aux(f, n, m)
        print_PS_comment()
        print_space()
        print_float(ep)
        print_newline()
        if ep < 1.0:
            break
    draw_dict(f, Ax, Bx, n, Ay, By, m, 0.0, 1.0)
    print_showpage()
    print_newline()
    
def drawmap_main():
    print_PS_comment()
    print_newline()
    Ax = -2.0
    Ay = -2.0
    Bx = 2.0
    By = 2.0
    draw_x_axis(Ax, Bx)
    draw_y_axis(Ay, By)
    n = 30
    m = 30
    f = init(n, m)
    solve(f, Ax, Bx, n, Ay, By, m)

drawmap_main()
