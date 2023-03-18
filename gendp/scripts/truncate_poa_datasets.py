import sys
file_name = sys.argv[1]
folder = sys.argv[2]

f = open(file_name, 'r')
lines = f.readlines()
file_num = -1
new_file = ''
num_seq = {}
seq_count = 0
for line in lines:
    if line[:2] != ">0":
        new_file += line
        seq_count += 1
    else:
        file_num += 1
        f_sub = open(folder+"/poa_{}".format(file_num), 'w')
        f_sub.write(new_file)
        f_sub.close()
        new_file = ''
        if seq_count not in num_seq.keys():
            num_seq[seq_count] = 1
        else:
            num_seq[seq_count] += 1
        if seq_count > 201:
            print(file_num, end=" ")
        seq_count = 0
f.close()
# print(num_seq)
# for key in num_seq.keys():
#     if num_seq[key] > 100:
#         print(key, num_seq[key])
print(file_num)