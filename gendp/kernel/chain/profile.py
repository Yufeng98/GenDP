from os import listdir
from os.path import isfile, join
import sys

myfile = sys.argv[1]
f = open(myfile, 'r')
max_iter = {}
iter = {}
lines = f.readlines()
for line in lines:
    if line[:3] == "max":
        max_iter_item = int(line.split(":")[-1])
        if max_iter_item not in max_iter.keys():
            max_iter[max_iter_item] = 1
        else:
            max_iter[max_iter_item] += 1
    elif line[:4] == "iter":
        iter_item = int(line.split(":")[-1])
        if iter_item not in iter.keys():
            iter[iter_item] = 1
        else:
            iter[iter_item] += 1

max_iter_key_list = list(max_iter.keys())
max_iter_key_list.sort()
iter_key_list = list(iter.keys())
iter_key_list.sort()

print("max_iter")
num_max = 0
for key in max_iter_key_list:
    num_max += max_iter[key]
    print(key, max_iter[key])
print("num_max: {}".format(num_max))

num_iter = 0
print("iter")
for key in iter_key_list:
    num_iter += iter[key]
    print(key, iter[key])
print("num_iter: {}".format(num_iter))
