
def make_queue():
  mt = mutex()
  cond = condition(mt)
  return [cond, []]

def q_push(q, val):
  cond = q[0]
  cond.acquire()

  lst = q[1]
  
  lst = [val] + lst
  q[1] = lst
  cond.notify()
  cond.release()
  return val

def q_pop(q):
  cond = q[0]

  cond.acquire()
  lst = q[1]
  while len(lst) ==  0:
    cond.wait()
    lst = q[1]
  ret = lst.pop()
  q[1] = lst
  cond.release()

  return ret

def q_length(q):
  return len(q[1])

q = make_queue()

def worker(i, q):
  while 1:
    val = q.q_pop()
    print "worker " + str(i) + " popped:" + str(val)
    sys_sleep(ntv_random() % 3)

th = [0,0,0,0,0]

for i in range(0,5):
  th[i] = start_new_thread(worker, (i, q))

for i in range(0,100):
  q.q_push(i)
  print "main thread pushed: " + str(i)

for i in range(0,5):
  thread_join(th[i])
