i = 0
tab = {}
tab2 = {}
indices = []
names = []
iodirs = []

with open("io.tab", "r") as f:
	lines = f.readlines()
	for line in lines:
		tab = line.split('\t')	
		#print(tab[0])
		#print(tab[1])
		#print(tab[2])

		indices.append( tab[0] )
		names.append( tab[1] )
		iodirs.append( tab[2] )

		#i += 1
	f.close()

# for i in range(0,95):
# 	print(indices[i] + " " + names[i] + " " + iodirs[i])

highestIO = 0
nMCP = 0
nPin = 0

for i in range(0,95):
	if names[i] != '-':
		highestIO = i

nMCP = int(highestIO / 16) + 1

print("{} {}".format("nMCP" , nMCP))
print("{} {}".format("highestIO" , highestIO))

structNames = ["mcp1", "mcp2", "mcp3", "mcp4", "mcp5", "mcp6", "mcp7", "mcp8"]


with open("io.h", "w") as f:
	f.write("#include io.h\n")
	for i in range(0,7): # 0 to 7 max
		f.write("struct " + structNames[i] + " {\n")
		f.write("\tunsigned int bit0:1;\n")
		f.write("\tunsigned int bit1:1;\n")
		f.write("\tunsigned int bit2:1;\n")
		f.write("\tunsigned int bit3:1;\n")
		f.write("\tunsigned int bit4:1;\n")
		f.write("\tunsigned int bit5:1;\n")
		f.write("\tunsigned int bit6:1;\n")
		f.write("\tunsigned int bit7:1;\n")
		f.write("\tunsigned int bit8:1;\n")
		f.write("\tunsigned int bit9:1;\n")
		f.write("\tunsigned int bit10:1;\n")
		f.write("\tunsigned int bit11:1;\n")
		f.write("\tunsigned int bit12:1;\n")
		f.write("\tunsigned int bit13:1;\n")
		f.write("\tunsigned int bit14:1;\n")
		f.write("\tunsigned int bit15:1;};\n\n")
		for pin in range(0,16):
			nPin = 16 * i + pin # should reach all 96 pin numbers if there are 8 io devices
			if names[nPin] != "-":
				f.write("#define " + names[nPin] + " " + structNames[i]+".bit" + str(pin) + " \n")
		f.write("\n\n")
		if i >= nMCP - 1:
			break
	f.close()
	

#print(names)

# for i in range(0,96):
# 	print(i)