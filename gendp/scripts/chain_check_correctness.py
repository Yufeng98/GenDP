import math
import sys
file_name = sys.argv[1]
file_name_1 = sys.argv[2]
f = open(file_name, 'r')
lines = f.readlines()
f_1 = open(file_name_1, 'r')
lines_1 = f_1.readlines()

n = len(lines)
err = {}
err_sum = 0
for i in range(n):
    if lines[i] != lines_1[i]:
        e = int(lines[i])-int(lines_1[i])
        err_sum += 1
        if e not in err.keys():
            err[e] = 1
        else:
            err[e] += 1
print(err_sum, n, "{:.6f}".format(err_sum/n))
# print(err)
