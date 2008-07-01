
#queue impl
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


#HTTP server impl

port = 10000

server_addr = make_sockaddr(port)

sock = py_socket()

while py_bind(sock, server_addr) < 0:
  print "bind error"
  port = port + 1
  server_addr = make_sockaddr(port)

print "bounded port=" + str(port)

if(py_listen(sock) < 0):
  print "listen error"

job_queue = make_queue()

def worker_func():
  global job_queue
  print "start job"

  res = "HTTP/1.x 200 OK\n\n<html><head><title>Hello</title></head><body><h1>Hello mini python.</h1></body></html>"

  while 1:
    job = job_queue.q_pop()
    fd = job[0]
    client_addr = job[1]
    
    req = py_recv(fd)
    print "received request:"
    print req
    
    py_send(fd, res)

    print "response:"
    print res
    py_close(fd)
    sys_sleep(10)

worker_threads = []

for i in range(0, 5):
  worker_threads.append(start_new_thread(worker_func, ()))

while 1:
  client_addr = make_sockaddr(0)
  fd = py_accept(sock, client_addr)
  job = (fd, client_addr)
  job_queue.q_push(job)

for i in range(0, 5):
  thread_join(worker_thread[i])
