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
		# tab = line.split('\t','\t\t','\t\t\t','\t\t\t\t','\s*')
		tab = line.split('\t')
		if tab[0] != '\n':				# if not newline char
			if tab[0][:-2] == "MCP":	# if MCP
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
ioCounter = 0
with open("src/basics/io.h", "w") as f:
	for i in range(0, regularIO):
		if names[i] != "-":
			f.write("\n#define " + names[i] + " " + indices[i])

	# f.write('\n')
	if mcpIO > 0:
		ioCounter += regularIO
		for i in range(0, mcpIO0):
			f.write("{} {} {}".format("\n#define", names[i + ioCounter], indices[i + ioCounter]))
	
	# f.write('\n')
	if mcpIO > 1:
		ioCounter += mcpIO0
		for i in range(0, mcpIO1):
			f.write("{} {} {}".format("\n#define", names[i + ioCounter], int(indices[i + ioCounter])+16))
	
	# f.write('\n')
	if mcpIO > 2:
		ioCounter += mcpIO1
		for i in range(0, mcpIO2):
			f.write("{} {} {}".format("\n#define", names[i + ioCounter], int(indices[i + ioCounter])+32))
	
	# f.write('\n')
	if mcpIO > 3:
		ioCounter += mcpIO2
		for i in range(0, mcpIO3):
			f.write("{} {} {}".format("\n#define", names[i + ioCounter], int(indices[i + ioCounter])+48))
	
	# f.write('\n')
	if mcpIO > 4:
		ioCounter += mcpIO3
		for i in range(0, mcpIO4):
			f.write("{} {} {}".format("\n#define", names[i + ioCounter], int(indices[i + ioCounter])+64))
	
	# f.write('\n')
	if mcpIO > 5:
		ioCounter += mcpIO4
		for i in range(0, mcpIO5):
			f.write("{} {} {}".format("\n#define", names[i + ioCounter], int(indices[i + ioCounter])+80))
	
	# f.write('\n')
	if mcpIO > 6:
		ioCounter += mcpIO5
		for i in range(0, mcpIO6):
			f.write("{} {} {}".format("\n#define", names[i + ioCounter], int(indices[i + ioCounter])+96))

	# f.write('\n')
	if mcpIO > 7:
		ioCounter += mcpIO6
		for i in range(0, mcpIO7):
			f.write("{} {} {}".format("\n#define", names[i + ioCounter], int(indices[i + ioCounter])+112))



	f.write("\n\nextern void initIO(void);")
	if mcpIO > 0:
		f.write("""
extern void mcpWrite(unsigned char pin, unsigned char state);
extern unsigned char mcpRead(unsigned char pin);""")
	f.close()

iodir = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
pullup = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
ioCounter = 0
address = 0x20


## cpp FILE
with open("src/basics/io.cpp", "w") as f:
	f.write('#include <Arduino.h>\n')
	f.write('#include "io.h"\n')
	if mcpIO > 0:
		f.write('#include <Wire.h>\n')
		# I2C extenders
		f.write("""
#define portA	0x12
#define portB	0x13
#define iodirRegA	0x00
#define pullUpRegA	0x0C

static void initMcp(unsigned char address, unsigned int iodir, unsigned int pullUp) {
	Wire.beginTransmission(address);
	Wire.write(iodirRegA);
	Wire.write(iodir >> 8);
	Wire.write(iodir);
	Wire.endTransmission();

	Wire.beginTransmission(address);
	Wire.write(pullUpRegA);
	Wire.write(pullUp >> 8);
	Wire.write(pullUp);
	Wire.endTransmission();
}

extern void mcpWrite(unsigned char pin, unsigned char state) {
	unsigned char address = 0x20, port, IO;
	address += (pin / 16);				// select address

	if(pin % 16 < 8)	port = portA;	// select port
	else				port = portB;

	Wire.beginTransmission(address);
	Wire.write(port);
	Wire.endTransmission();
	Wire.requestFrom(address, 1);
	IO = Wire.read();					// fetch current IO status

	Wire.beginTransmission(address);
	Wire.write(port);
	if(state)	IO |=  (1 << pin);		// OR 'IO' with pin
	else		IO &= ~(1 << pin);		// or AND the inverse of pin
	Wire.write(IO);						// transmit the updated IO
	Wire.endTransmission();
}

extern unsigned char mcpRead(unsigned char pin) {
	unsigned char address = 0x20, port, IO;
	address += (pin / 16);				// select address

	if((pin % 16) < 8)	port = portA;	// select port
	else				port = portB;

	Wire.beginTransmission(address);
	Wire.write(port);
	Wire.endTransmission();
	Wire.requestFrom(address, 1);
	IO = Wire.read();

	if(IO & (1 << pin))	return 1;
	else				return 0;
}

""")
	f.write("extern void initIO(void) {\n")
	if mcpIO > 0:
		f.write("\tWire.begin();\n")
	for i in range(0, regularIO):
		f.write("\tpinMode(" + names[i] + ', ' + iodirs[i] + ");\n")

	try:

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
