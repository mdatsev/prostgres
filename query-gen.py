import random
import sys

ntables = 100
ncols = 100
nrows = 10000

def printstderr(s):
  sys.stderr.write(s + '\n')
  sys.stderr.flush()

def get_value():
  return random.randint(-99999999, 99999999)

for t in range(ntables):
  printstderr(f'{t}/{ntables}')
  print(f"create table x ({','.join(['x int'] * ncols)});")
  
  for r in range(nrows):
    print(f"insert into _{t+1} ({','.join(['x'] * ncols)}) values (", end='')
    for c in range(ncols):
      print(get_value(), end=('' if c==ncols-1 else ','))
    print(');')
  
# 10 min to generate
# 3 min to process