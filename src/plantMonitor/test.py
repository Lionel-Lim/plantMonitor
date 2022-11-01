
# Online Python - IDE, Editor, Compiler, Interpreter

import math


def num_to_range(num, inMin, inMax, outMin, outMax):
  return outMin + (float(num - inMin) / float(inMax - inMin) * (outMax
                  - outMin))

i = 20
for index in range(i):
    print(math.tanh(num_to_range(index * 20, 0, 500, 0, 3)))
