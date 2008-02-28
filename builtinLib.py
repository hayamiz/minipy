#==========================
#======== operator ========
#==========================
#def add(a,b):
#    if is_int(a):
#        if is_int(b):
#            return add_int(a,b)
#        elif is_float(b):
#            return add_float(itof(a),b)
#        else:
#            type_error()
#    elif is_float(a):
#        if is_int(b):
#            return add_float(a,itof(b))
#        elif is_float(b):
#            return add_float(a,b)
#        else:
#            type_error()
#    elif is_string(a) and is_string(b):
#        return add_string(a,b)
#    elif is_list(a) and is_list(b):
#        # ab=[]
#        #         for x in a:
#        #             ab.append(x)
#        #         for x in b:
#        #             ab.append(x)
#        #             return ab
#        return add_list(a, b)
#    elif is_tuple(a) and is_tuple(b):
#        return add_tuple(a,b)
#    else:
#        type_error()

#def prefix_add(a):
#    if is_int(a) or is_float(a):
#        return a
#    else:
#        type_error()

# python -> C++
# def sub(a,b):
#     if is_int(a):
#         if is_int(b):
#             return sub_int(a,b)
#         elif is_float(b):
#             return sub_float(itof(a),b)
#         else:
#             type_error()
#     elif is_float(a):
#         if is_int(b):
#             return sub_float(a,itof(b))
#         elif is_float(b):
#             return sub_float(a,b)
#         else:
#             type_error()
#     else:
#         type_error()

# python -> C++
# def prefix_sub(a):
#     if is_int(a):
#         return mul_int(a,-1)
#     elif is_float(a):
#         return mul_float(a,itof(-1))
#     else:
#         type_error()

# def mul(a,b):
#     if is_int(a):
#         if is_int(b):
#             return mul_int(a, b)
#         elif is_float(b):
#             return mul_float(itof(a),b)
#         else:
#             type_error()
#     elif is_float(a):
#         if is_int(b):
#             return mul_float(a,itof(b))
#         elif is_float(b):
#             return mul_float(a,b)
#         else:
#             type_error()
#     elif is_int(b):
#         if is_string(a):
#             return mul_string(a, b)
#         elif is_tuple(a):
#             tmp_t =[]
#             for i in range(b):
#                 tmp_t = add_tuple(tmp_t,a)
#             return tmp_t
#         elif is_list(a):
#             tmp_l =[]
#             for i in range(b):
#                 tmp_l = add_list(tmp_l,a)
#             return tmp_l
#         else:
#              type_error()
#     else:
#         type_error()

# def div(a,b):
#     if is_int(a):
#         if is_int(b):
#             return div_int(a,b)
#         elif is_float(b):
#             return div_float(itof(a),b)
#         else:
#             type_error()
#     elif is_float(a):
#         if is_int(b):
#             return div_float(a,itof(b))
#         elif is_float(b):
#             return div_float(a,b)
#         else:
#             type_error()
#     else:
#         type_error()

# def mod(a,b):
#     if is_int(a) and is_int(b):
#         return mod_int(a,b)
#     elif is_string(a):
#         if(is_int(b) or is_float(b)):
#             b = str(b)
#             return replace_string(a,b)
#         elif(is_string(b) or is_tuple(b)):
#             return replace_string(a,b)
#         else:
#             type_error()
#     else:
#         type_error()
    
#def invert(a):
#    if is_int(a):
#        return invert_int(a)
#    else:
#        type_error()

#def lshift(a,b):
#    if is_int(a) and is_int(b):
#        return lshift_int(a,b)
#    else:
#        type_error()
    
#def rshift(a,b):
#    if is_int(a) and is_int(b):
#        return rshift_int(a,b)
#    else:
#        type_error()

#def and_(a, b):
#    if is_int(a) and is_int(b):
#        return and_int(a, b)
#    else:
#        type_error()

#def or_(a, b):
#    if is_int(a) and is_int(b):
#        return or_int(a, b)
#    else:
#        type_error()

#def xor(a,b):#
#    if is_int#(a) and is_int(b):
#        return xor_int(a,b)
#    else:
#        type_error()
#    if is_int(a) and is_int(b):
#        return xor_int(a, b)
#    else:
#        type_error()

# def contains(a,b): # a in b
#     if is_tuple(b):
#         return hasitem_tuple(b,a)
#     elif is_list(b):
#         return hasitem_list(b,a)
#     else:
#         type_error()
        

# def not_contains(a,b): # a not in b
#     return not_(contains(a,b))

#def ntv_is(a,b):
#    if is_int(a) and is_int(b):
#        return not_(cmp_int(a,b))
#    elif is_float(a) and is_float(b):
#        return not_(cmp_float(a,b))
#    elif is_string(a) and is_string(b):
#        return eq_string(a,b)
#    elif (is_tuple(a) and is_tuple(b)) or (is_list(a) and is_list(b)) or (is_dict(a) and is_dict(b)):
#        return eq_pointer(a,b)
#    else:
#        type_error()

#def ntv_is_not(a,b):
#    return not_(ntv_is(a,b))

#def eq(a,b):
#    if is_int(a) and is_int(b):
#        return eq_int(a,b)
#    else:
#        type_error()

#def ne(a,b):
#    return not_(eq(a,b))

#def gt(a,b): #>
#    if is_int(a) and is_int(b):
#        return gt_int(a,b)
#    else:
#        type_error()

#def ge(a,b):#>=
#    return gt(a,b) or eq(a,b)

#def lt(a,b):
#    return not_(ge(a,b))

#def le(a,b):
#    return not_(gt(a,b))

# def str(a):
#     return str_(a)

#==========================
#===== tuple list dict =====
#==========================

# def getitem(a,b):
#     if is_string(a) and is_int(b):
#         return getitem_string(a,b)
#     elif is_tuple(a) and is_int(b):
#         return getitem_tuple(a,b)
#     elif is_list(a) and is_int(b):
#         return getitem_list(a,b)
#     elif is_dict(a):
#         return getitem_dict(a,b)
#     else:
#         type_error()

# def delitem(a,b):
#     if is_list(a) and is_int(b):
#         return delitem_list(a,b)
#     elif is_dict(a):
#         return delitem_dict(a,b)
#     else:
#         type_error()

# def setitem(container, key, val):
#     if is_list(container):
#         return setitem_list(container, key, val)
#     elif is_dict(container):
#         return setitem_dict(container, key, val)
#     else:
#         type_error()

# def append(a): // C++
# def pop(a,b):// C++
#def len(a):
#    if is_string(a):
#        return len_string(a)
#    elif is_list(a):
#        return len_list(a)
#    elif is_dict(a):
#        return len_dict(a)
#    elif is_tuple(a):
#        return len_tuple(a)
#    else:
#        type_error()


######## dict ########
# def has_key(a,b):
#     if is_dict(a):
#         return has_key_dict(a,b)
#     else:
#         type_error()

# def keys(a,b):
#     if is_dict(a):
#         return key_dict(a,b)
#     else:
#         type_error()

# def values(a,b):
#     if is_dict(a):
#         return values_dict(a,b)
#     else:
#         type_error()

# def items(a,b):
#     if is_dict(a):
#         return items_dict(a,b)
#     else:
#         type_error()



####### Sample programs not have these function  #######
# def extend(a):
# def insert(a,i,x):
# def remove(x):
# def index(x):
# def reverse():
# def join():
# def sort():
