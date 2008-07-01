
x = 0

def job():
  global x
  for i in range(0,100000):
    x = x + 1

th_a = start_new_thread(print_string, ("hoge",))
th_b = start_new_thread(print_string, ("fuga",))

thread_join(th_a)
thread_join(th_b)

print x
