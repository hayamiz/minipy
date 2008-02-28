# see README for how to run this program 

#
# You can run this program with native python if you uncomment the following three lines.
# ---------------- FOR NATIVE PYTHON ----------------
import sys
def print_string(s):
    sys.stdout.write(s)
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

def my_sqrt(C):
    x = C
    while 1:
        y = x * x - C
        x_ = x - y / (2.0 * x)
        d = my_abs(x - x_)
        if d < 0.000001:
            return x_
        x = x_

def project_X(x, y, z, a, b, c):
    D = a * a + b * b
    s = (- b * x + a * y) / my_sqrt(D)
    return s

def project_Y(x, y, z, a, b, c):
    D = a * a + b * b
    A = my_sqrt(a * a * c * c + b * b * c * c + D * D)
    t = (- a * c * x - b * c * y + D * z) / A
    return t

def draw_segment_3D(ax, ay, az, bx, by, bz):
    a = 1.0
    b = 0.8
    c = 0.8
    Ax = project_X(ax, ay, az, a, b, c)
    Ay = project_Y(ax, ay, az, a, b, c)
    Bx = project_X(bx, by, bz, a, b, c)
    By = project_Y(bx, by, bz, a, b, c)
    draw_segment(Ax, Ay, Bx, By)

def draw_x_axis_3D(A, B):
    draw_segment_3D(A, 0.0, 0.0, B, 0.0, 0.0)
    x = A
    while x < B:
        draw_segment_3D(x, 0.0, -0.02, x, 0.0, 0.02)
        x = x + 1.0
    
def draw_y_axis_3D(A, B):
    draw_segment_3D(0.0, A, 0.0, 0.0, B, 0.0)
    y = A
    while y < B:
        draw_segment_3D(0.0, y, -0.02, 0.0, y, 0.02)
        y = y + 1.0

def draw_z_axis_3D(A, B):
    draw_segment_3D(0.0, 0.0, A, 0.0, 0.0, B)
    z = A
    while z < B:
        draw_segment_3D(0.0, -0.02, z, 0.0, 0.02, z)
        z = z + 1.0

def draw_surface_3D_parameter(P, Q, R, A, B, C, D, ds, dt):
    s = A
    while s < B:
        t = C
        while t < D:
            draw_segment_3D(P(s, t), Q(s, t), R(s, t), P(s+ds, t), Q(s+ds, t), R(s+ds, t))
            draw_segment_3D(P(s+ds, t), Q(s+ds, t), R(s+ds, t), P(s+ds, t+dt), Q(s+ds, t+dt), R(s+ds, t+dt))
            draw_segment_3D(P(s+ds, t+dt), Q(s+ds, t+dt), R(s+ds, t+dt), P(s, t+dt), Q(s, t+dt), R(s, t+dt))
            draw_segment_3D(P(s, t+dt), Q(s, t+dt), R(s, t+dt), P(s, t), Q(s, t), R(s, t))
            t = t + dt
        s = s + ds

def identity_x(x, y):
    return x

def identity_y(x, y):
    return y

def draw_surface_3D_function(f, A, B, C, D, dx, dy):
    draw_surface_3D_parameter(identity_x, identity_y, f, A, B, C, D, dx, dy)

def ff(x, y):
    return x * x - y * y

def drawgraph_3D_main():
    print_PS_comment()
    print_newline()
    draw_x_axis_3D(0.0, 2.0)
    draw_y_axis_3D(0.0, 2.0)
    draw_z_axis_3D(0.0, 4.0)
    draw_surface_3D_function(ff, -1.0, 1.0, -1.0, 1.0, 0.05, 0.05)
    print_showpage()
    print_newline()
    
drawgraph_3D_main()

