import math
import sys
file_name = sys.argv[1]
file_name_1 = sys.argv[2]
f = open(file_name, 'r')
lines = f.readlines()
f_1 = open(file_name_1, 'r')
lines_1 = f_1.readlines()

n = min(len(lines), len(lines_1))
err = {}
err_sum = 0
for i in range(n):
    score = int(lines[i])
    score_1 = int(lines_1[i])
    if abs(score - score_1) > 5 and abs(score - score_1) > score * 0.01:
        print(i, lines[i][:-1], lines_1[i][:-1])
        e = int(lines[i])-int(lines_1[i])
        err_sum += 1
        if e not in err.keys():
            err[e] = 1
        else:
            err[e] += 1
print(err_sum, "errors out of", n, "scores.")
# print(err)
