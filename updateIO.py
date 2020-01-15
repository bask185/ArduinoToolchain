#!/usr/bin/python

def binary(num, pre='0b', length=16, spacer=0):
    return '{0}{{:{1}>{2}}}'.format(pre, spacer, length).format(bin(num)[2:])

i = 0
tab = {}
tab2 = {}
indices = []
names = []
iodirs = []

regularIO = 0
mcpIO = 0
mcpIO0 = 0
mcpIO1 = 0
mcpIO2 = 0
mcpIO3 = 0
mcpIO4 = 0
mcpIO5 = 0
mcpIO6 = 0
mcpIO7 = 0

with open("io.tab", "r") as f:
	lines = f.readlines()
	for line in lines:
		tab = line.split('\t')
		if tab[0] != '\n':				# if not newline char
			if tab[0][:-2] == "MCP":	# if not MCP
				mcpIO += 1
				
			elif mcpIO == 1:

				mcpIO0 += 1
			elif mcpIO == 2:
				mcpIO1 += 1
			elif mcpIO == 3:
				mcpIO2 += 1
			elif mcpIO == 4:
				mcpIO3 += 1
			elif mcpIO == 5:
				mcpIO4 += 1
			elif mcpIO == 6:
				mcpIO5 += 1
			elif mcpIO == 7:
				mcpIO6 += 1
			elif mcpIO == 8:
				mcpIO7 += 1
			else:	# regular IO
				regularIO += 1
			if tab[0][:-2] != "MCP":
				indices.append( tab[0] )
				names.append( tab[1] )
				iodirs.append( tab[2][:-1] )
				
	f.close()

highestIO = 0
nMCPp = 0
nPin = 0


#structNames = ["mcp1", "mcp2", "mcp3", "mcp4", "mcp5", "mcp6", "mcp7", "mcp8"]
#print("{}    {}    {}".format(indices, names, iodirs))

### HEADER FILE
with open("io.h", "w") as f:
	for i in range(0, regularIO):
		if names[i] != "-":
			f.write("\n#define " + names[i] + " " + indices[i])
	f.write("\n\nextern void initIO(void);")
	f.close()

iodir = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
pullup = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
ioCounter = 0
address = 0x20


## cpp FILE
with open("io.cpp", "w") as f:
	f.write('#include "io.h"\n')
	if mcpIO > 0:
		f.write('#include "modules/mcp23017.h"\n')
	f.write("extern void initIO(void) {\n")
	for i in range(0, regularIO):
		f.write("\tpinMode(" + names[i] + ', ' + iodirs[i] + ");\n")

	try:
		# I2C extenders
		if mcpIO > 0:
			ioCounter += regularIO
			for i in range(0, mcpIO0):
			
				if iodirs[i+ioCounter] == "INPUT_PULLUP" or iodirs[i + ioCounter] == "INPUT":
					iodir[0] |= (1 << int(indices[i + ioCounter]))
					if iodirs[i + ioCounter] == "INPUT_PULLUP":
						pullup[0] |= (1 << int(indices[i + ioCounter]))

			f.write("\tinitMcp(0x20, {0:b}".format(iodir[0])+ ', ' + "{0:b}".format(pullup[0]) +");\n")

		if mcpIO > 1:
			ioCounter += mcpIO0
			for i in range(0, mcpIO1):
			
				if iodirs[i + ioCounter] == "INPUT_PULLUP" or iodirs[i + ioCounter] == "INPUT":
					iodir[1] |= (1 << int(indices[i + ioCounter]))
					if iodirs[i + ioCounter] == "INPUT_PULLUP":
						pullup[1] |= (1 << int(indices[i + ioCounter]))

			f.write("\tinitMcp(0x21, {0:b}".format(iodir[1])+ ', ' + "{0:b}".format(pullup[1]) +");\n")

		if mcpIO > 2:
			ioCounter += mcpIO1
			for i in range(0, mcpIO2):
				if iodirs[i + ioCounter] == "INPUT_PULLUP" or iodirs[i + ioCounter] == "INPUT":
					iodir[2] |= (1 << int(indices[i + ioCounter]))
					if iodirs[i + ioCounter] == "INPUT_PULLUP":
						pullup[2] |= (1 << int(indices[i + ioCounter]))

			f.write("\tinitMcp(0x22, {0:b}".format(iodir[2])+ ', ' + "{0:b}".format(pullup[2]) +");\n") 

		if mcpIO > 3:
			ioCounter += mcpIO2
			for i in range(0, mcpIO3):
				if iodirs[i + ioCounter] == "INPUT_PULLUP" or iodirs[i + ioCounter] == "INPUT":
					iodir[3] |= (1 << int(indices[i + ioCounter]))
					if iodirs[i + ioCounter] == "INPUT_PULLUP":
						pullup[3] |= (1 << int(indices[i + ioCounter]))

			f.write("\tinitMcp(0x23, {0:b}".format(iodir[3])+ ', ' + "{0:b}".format(pullup[3]) +");\n")

		if mcpIO > 4:
			ioCounter += mcpIO3
			for i in range(0, mcpIO4):
				if iodirs[i + ioCounter] == "INPUT_PULLUP" or iodirs[i + ioCounter] == "INPUT":
					iodir[4] |= (1 << int(indices[i + ioCounter]))
					if iodirs[i + ioCounter] == "INPUT_PULLUP":
						pullup[4] |= (1 << int(indices[i + ioCounter]))

			f.write("\tinitMcp(0x24, {0:b}".format(iodir[4])+ ', ' + "{0:b}".format(pullup[4]) +");\n")

		if mcpIO > 5:
			ioCounter += mcpIO4
			for i in range(0, mcpIO5):
				if iodirs[i + ioCounter] == "INPUT_PULLUP" or iodirs[i + ioCounter] == "INPUT":
					iodir[5] |= (1 << int(indices[i + ioCounter]))
					if iodirs[i + ioCounter] == "INPUT_PULLUP":
						pullup[5] |= (1 << int(indices[i + ioCounter]))

			f.write("\tinitMcp(0x25, {0:b}".format(iodir[5])+ ', ' + "{0:b}".format(pullup[5]) +");\n")

		if mcpIO > 6:
			ioCounter += mcpIO5
			for i in range(0, mcpIO6):
				if iodirs[i + ioCounter] == "INPUT_PULLUP" or iodirs[i + ioCounter] == "INPUT":
					iodir[6] |= (1 << int(indices[i + ioCounter]))
					if iodirs[i + ioCounter] == "INPUT_PULLUP":
						pullup[6] |= (1 << int(indices[i + ioCounter]))

			f.write("\tinitMcp(0x26, {0:b}".format(iodir[6])+ ', ' + "{0:b}".format(pullup[6]) +");\n")

		if mcpIO > 7:
			ioCounter += mcpIO6
			for i in range(0, mcpIO7):
				if iodirs[i + ioCounter] == "INPUT_PULLUP" or iodirs[i + ioCounter] == "INPUT":
					iodir[7] |= (1 << int(indices[i + ioCounter]))
					if iodirs[i + ioCounter] == "INPUT_PULLUP":
						pullup[7] |= (1 << int(indices[i + ioCounter]))

			f.write("\tinitMcp(0x27, {0:b}".format(iodir[7])+ ', ' + "{0:b}".format(pullup[7]) +");\n")

		f.write("}")
	except:
		print("{}   {}".format(i, ioCounter))

	f.close()

# ### HEADER FILE
# with open("ignoreMe.h", "w") as f:
# 	f.write("extern void updateOutputs(void);\n")
# 	f.write("extern void initializeIO(void);\n")
# 	f.write("extern void updateInputs(void);\n\n")

# 	for i in range(0,7): # 0 to 7 max
# 		f.write("struct " + structNames[i + ioCounter] + " {\n")
# 		f.write("\tunsigned int prev;\n")
# 		f.write("\tunsigned char address;\n")
# 		f.write("\tvoid (*setIO)(unsigned char, unsigned int);\n")
# 		f.write("\tvoid (*getIO)(unsigned char, unsigned int*);\n")
# 		f.write("\tvoid (*initIO)(unsigned char, unsigned int);\n\n")
# 		f.write("\tunion {\n")
# 		f.write("\t\tunsigned int port;\n\n")
# 		for j in range(0,15):
# 			f.write("\t\tunsigned int bit" + str(j) + ":1;\n");
# 		f.write("\t\tunsigned int bit15:1;};};\n\n")
# 		f.write("extern struct " + structNames[i + ioCounter] + " _" + structNames[i + ioCounter] + ";\n\n")
# 		#f.write("extern unsigned int " + structNames[i] + ".prev;\n\n")
# 		for pin in range(0,16):
# 			nPin = 16 * i + pin # should reach all 96 pin numbers if there are 8 io devices
# 			if names[nPin] != "-":
# 				f.write("#define " + names[nPin] + " " + structNames[i]+".bit" + str(pin) + "\n")
# 		f.write("\n\n")
# 		if i >= nMCP - 1:
# 			break

# 	f.write("""
# extern void updateOutputs(void);
# extern void debounce(void);
# """)
# 	f.close()
	


# ### CPP file
# with open("ignoreMe.cpp", "w") as f:
# 	f.write('#include "io.h"\n')
# 	f.write('#include <Wire.h>\n')
# 	f.write('#include <Arduino.h>\n')
# 	#f.write('#include "buttonClass.h"\n\n\n')

# 	for i in range(0,7):
# 		f.write("struct " + structNames[i] + " _" + structNames[i] + ";\n")

# 		if i >= nMCP - 1:
# 			break

# 	f.write("\n\nextern void updateOutputs(void) {\n")
# 	for i in range(1,nMCP+1):
# 		f.write("\tif((_mcp" + str(i) + ".port & ~_mcp" + str(i) + ".inputMask) != _mcp"  + str(i) + ".prev) {\n")
# 		f.write("\t\t_mcp"   + str(i) + ".prev = _mcp"  + str(i) + ".port & ~_mcp"        + str(i) + ".inputMask;\n")
# 		f.write("\t\t_mcp"   + str(i) + ".setIO(_mcp"   + str(i) + ".address, _mcp"       + str(i) + ".port & ~mcp"  + str(i) + ".inputMask); }\n\n")
# 	f.write("}\n\n")

# 	f.write("extern void initializeIO(void) {\n")
# 	f.write("\tWire.begin();\n\n")
# # iodirs
# 	for i in range(0,nMCP):
# 		iodir = 0
# 		for j in range(0, 16):
# 			if iodirs[16 * i + j] == "INPUT_PULLUP":
# 				iodir |= 1
# 			if j < 15:
# 				iodir <<= 1
# 		baseAddress = 0x20
# 		f.write("{}{}{}{}{}".                      format("\tmcp", i + 1,".inputMask = ", binary(iodir), ";\n"))
# 		f.write("{0:s}{1:d}{2:s}{3:02X}{4:s}".     format("\tmcp", i + 1,".adress = 0x", baseAddress + i, ";\n"))
# 		f.write("{}{}{}".                          format("\tmcp", i + 1,".setIO =  &_setIO;\n"))
# 		f.write("{}{}{}".                          format("\tmcp", i + 1,".getIO =  &_getIO;\n"))
# 		f.write("{}{}{}".                          format("\tmcp", i + 1,".initIO = &_initIO;\n"))
# 		f.write("{}{}{}{}{}{}{}".                  format("\tmcp", i + 1,".initIO(mcp", i + 1,".address, mcp", i + 1, ".inputMask);\n\n"))
# 	f.write("}\n\n")

# 	f.write("""

# #define portA	0x12
# #define ioDirA	0x00
# #define pullUpA	0x0C

# static void _setIO(unsigned char address, unsigned int output) {
# 	Wire.beginTransmission(address);
# 	Wire.write(portA);
# 	Wire.write(output >> 8); // HB
# 	Wire.write(output);      // LB
# 	Wire.endTransmission(); }

# static void _getIO(unsigned char address, unsigned int *input) {
# 	Wire.beginTransmission(address);
# 	Wire.write(portA);				// select port A
# 	Wire.endTransmission(); 

# 	Wire.requestFrom(address, 2);	// request port A and B
# 	*input = Wire.read() << 8 | Wire.read(); }

# static void _initIO(unsigned char address, unsigned int ioDir) {
# 	Wire.beginTransmission(address);
# 	Wire.write(ioDirA);
# 	Wire.write(ioDir >> 8);	// HB
# 	Wire.write(ioDir);		// LB
# 	Wire.endTransmission();
	
# 	Wire.beginTransmission(address);
# 	Wire.write(pullUpA);
# 	Wire.write(ioDir >> 8);	// HB
# 	Wire.write(ioDir);		// LB
# 	Wire.endTransmission(); }""")

# 	f.close()

###
