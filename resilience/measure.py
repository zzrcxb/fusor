import os
import subprocess
import time


for s in range(1, 6):
  for r in range(3, 8):
    os.system('python3 ptl.py -i main4.ptl --context "{{ \\"size\\": 16, \\"s_level\\": {s}, \\"r_level\\": {r} }}" | clang -xc - -o {s}-{r}.out'.format(s=s, r=r))

time.sleep(3)

print("============================================\n\n\n")

for s in range(1, 6):
  for r in range(3, 8):
    start_t = time.time()
    p = subprocess.Popen(["/root/pypy3-v6.0.0-linux64/bin/pypy3", "angr_run3.py", "-l4", "-r", "./{s}-{r}.out".format(s=s, r=r)], stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    p.communicate()
    rtime = time.time() - start_t
    print("{s}-{r}: {time:.2f}s".format(s=s, r=r, time=rtime))

