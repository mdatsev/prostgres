import random
import sys

ntables = 1
ncols = 4
nrows = 10000

def printstderr(s):
  sys.stderr.write(s + '\n')
  sys.stderr.flush()

def get_value():
  return random.randint(-9999999, 9999999)

for r in range(nrows):
  if (r % 100000 == 0):
    printstderr(f'{r}/{nrows}')
  print(f"select a, b, c, d from test where a == {get_value()};")
  
# 10 min to generate
# 3 min to process