
x = 0
lk = mutex()

def job():
  global x
  global lk
  for i in range(0,100000):
    lk.lock()
    x = x + 1
    y = y + 1
    lk.unlock()

th_a = start_new_thread(job, ())
th_b = start_new_thread(job, ())

thread_join(th_a)
thread_join(th_b)

print x
print y
