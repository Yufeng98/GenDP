import sys

file_sim = sys.argv[1]
file_kernel = sys.argv[2]
begin_index = int(sys.argv[3])

f_sim = open(file_sim, "r")
f_kernel = open(file_kernel, "r")
lines_sim = f_sim.readlines()
lines_kernel = f_kernel.readlines()
input_dimension = {}
sim = {}
sim_index = -1
kernel = {}
kernel_index = -1 - begin_index
for line in lines_sim:
    if line[0] == "O":
        sim_index += 1
        sim[sim_index] = []
    else:
        sim[sim_index].append(line.split())
    
for line in lines_kernel:
    if line[0] == "D":
        str = line.split("(")[1]
        str = str.split(")")[0]
        lst = str.split()
        len_y = int(lst[0])
        len_x = int(lst[1])
        kernel_index += 1
        input_dimension[kernel_index] = (len_y, len_x)
        kernel[kernel_index] = []
    else:
        kernel[kernel_index].append(line.split())
    
for j in range(len(sim.keys())):
    # print("input[{}]".format(j))
    len_x_padding = input_dimension[j][1] + 4 - 1
    len_y = input_dimension[j][0]
    last_iter = input_dimension[j][0]//4
    y_padding = len_y - last_iter*4
    # print(len_y, len_x_padding)
    for i in range(len(sim[j])):
        if i//len_x_padding == last_iter:
            if y_padding == 0:
                if sim[j][i][1:] != kernel[j][i]:
                    print("iter={}".format(i//len_x_padding), "i={}".format(i//len_x_padding*4), "j={}".format(i%len_x_padding), sim[j][i], kernel[j][i])
            if y_padding == 1:
                if sim[j][i][3:] != kernel[j][i][2:]:
                    print("iter={}".format(i//len_x_padding), "i={}".format(i//len_x_padding*4), "j={}".format(i%len_x_padding), sim[j][i], kernel[j][i])
            if y_padding == 2:
                if sim[j][i][5:] != kernel[j][i][4:]:
                    print("iter={}".format(i//len_x_padding), "i={}".format(i//len_x_padding*4), "j={}".format(i%len_x_padding), sim[j][i], kernel[j][i])
            if y_padding == 3:
                if sim[j][i][7:] != kernel[j][i][6:]:
                    print("iter={}".format(i//len_x_padding), "i={}".format(i//len_x_padding*4), "j={}".format(i%len_x_padding), sim[j][i], kernel[j][i])
        else:
            if sim[j][i][1:] != kernel[j][i]:
                print("iter={}".format(i//len_x_padding), "i={}".format(i//len_x_padding*4), "j={}".format(i%len_x_padding), sim[j][i], kernel[j][i])