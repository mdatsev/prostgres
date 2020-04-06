import random
import sys

ntables = 1
ncols = 4
nrows = 10000000

def printstderr(s):
  sys.stderr.write(s + '\n')
  sys.stderr.flush()

def get_value():
  return random.randint(-99999999, 99999999)

print("create table test (a int, b int, c int, d int);")
for r in range(nrows):
  if (r % 100000 == 0):
    printstderr(f'{r}/{nrows}')
  print("insert into test (a, b, c, d) values (", end='')
  for c in range(ncols):
    print(get_value(), end=('' if c==ncols-1 else ','))
  print(');')
  
# 10 min to generate
# 3 min to process