import sys
file_name = sys.argv[1]
j = int(sys.argv[2])
f = open(file_name, 'r')
lines = f.readlines()
n = 0
th = 10000 * (j+1)
fw = open("/x/yufenggu/input-data/phmm_inputs/phmm_large_{}.in".format(j), "w")
for line in lines:
    if line[0] in "0123456789":
        lst = line.split()
        read = int(lst[0])
        hap = int(lst[1])
        n += read*hap
    if n > th-10000 and n <= th:
        fw.write(line)
fw.close()
print(n)