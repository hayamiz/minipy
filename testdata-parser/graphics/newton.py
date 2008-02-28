# see README for how to run this program 

#
# You can run this program with native python if you uncomment the following three lines.
# ---------------- FOR NATIVE PYTHON ----------------
# import sys
# def print_string(s):
#     sys.stdout.write(s)
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
xmag = 200.0
ymag = 200.0

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

def differentiate(f, x, dx):
    return (f(x + dx) - f(x)) / dx

def my_abs(x):
    if x < 0.0:
        return -x
    else:
        return x
                  
def newton(f, x0):
    x = x0
    while 1:
        y = f(x)
        x_ = x - y / differentiate(f, x, 0.001)
        if with_graphics:
            draw_segment(x, 0.0, x, y)
            draw_segment(x, y, x_, 0.0)
        d = my_abs(x - x_)
        if d < 0.00000001:
            return x_
        x = x_

def f(x):
    return x * x - 0.02

def newton_main():
    Ax = -1.0
    Bx = 1.0
    Ay = -1.0
    By = 1.0
    if with_graphics:
        print_PS_comment()
        print_newline()
        draw_x_axis(Ax, Bx)
        draw_y_axis(Ay, By)
        draw_curve_function(f, Ax + 0.1, Bx - 0.1, 0.005)
    s = newton(f, 1.0)
    if with_graphics:
        print_showpage()
        print_newline()
    print_PS_comment()
    print_float(s)
    print_newline()

newton_main()
