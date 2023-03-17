import sys
file_name = sys.argv[1]
read_byte = int(sys.argv[2])
write_byte = int(sys.argv[3])

f = open(file_name, 'w')

# cache line granularity is 64-Byte 

for i in range(read_byte//64):
    f.write("{} {}\n".format(hex(i*64), "R"))

for i in range(read_byte//64, read_byte//64 + write_byte//64):
    f.write("{} {}\n".format(hex(i*64), "W"))