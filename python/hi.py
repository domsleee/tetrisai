from pyswarm import pso
import time
import datetime

def banana(x):
  time.sleep(1.5)
  x1 = x[0]
  x2 = x[1]
  return x1**4 - 2*x2*x1**2 + x2**2 + x1**2 - 2*x1 + 5

def con(x):
  x1 = x[0]
  x2 = x[1]
  return [-(x1 + 0.25)**2 + 0.75*x2]



start = datetime.datetime.now()
lb = [-3, -1]
ub = [2, 6]

xopt, fopt = pso(banana, lb, ub, f_ieqcons=con)
end = datetime.datetime.now()
print(end-start)

