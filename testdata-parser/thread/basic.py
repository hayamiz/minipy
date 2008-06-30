
def th_print(arg):
  while 1:
    if arg == "b":
      print(arg)

th_a = start_new_thread(th_print, ("a",))
th_b = start_new_thread(th_print, ("b",))

thread_join(th_a)
thread_join(th_b)

