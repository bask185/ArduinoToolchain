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
	for i in range(0,7): # 0 to 7 max
		f.write("struct " + structNames[i] + " {\n")
		f.write("\tunion {\n")
		f.write("\t\tunsigned int port;\n\n")
		for j in range(0,15):
			f.write("\t\tunsigned int bit" + str(j) + ":1;\n");
		f.write("\t\tunsigned int bit15:1;};};\n")
		f.write("extern struct " + structNames[i] + " _" + structNames[i] + ";\n")
		f.write("extern unsigned int " + structNames[i] + "Prev;\n\n")
		for pin in range(0,16):
			nPin = 16 * i + pin # should reach all 96 pin numbers if there are 8 io devices
			if names[nPin] != "-":
				f.write("#define " + names[nPin] + " " + structNames[i]+".bit" + str(pin) + "\n")
		f.write("\n\n")
		if i >= nMCP - 1:
			break

	f.write("""
extern void updateMcp(void);
extern void debounce()
""")
	f.close()
	
with open("io.cpp", "w") as f:
	f.write('#include "io.h"\n')
	f.write('#include "buttonClass.h"\n')
	f.write('#include "mcp23017.h"\n')
	f.write('#include "buttonClass.h"\n\n\n')

	for i in range(0,7):
		f.write("struct " + structNames[i] + " _" + structNames[i] + ";\n")


		if i >= nMCP - 1:
			break
	f.write("""
void updateMcp(void) {
	if(nMcp >= 1) {
		if(mcp1.port != mcp1Prev) {
			mcp1Prev = mcp1.port;
			mcp[1].setPorts(mcp1.port;); } }
	else return;

	if(nMcp >= 2) {
		if(mcp2.port != mcp2Prev) {
			mcp2Prev = mcp2.port;
			mcp[2].setPorts(mcp2.port;); } }
	else return;

	if(nMcp >= 3) {
		if(mcp3.port != mcp3Prev) {
			mcp3Prev = mcp3.port;
			mcp[3].setPorts(mcp3.port;); } }
	else return;

	if(nMcp >= 4) {
		if(mcp4.port != mcp4Prev) {
			mcp4Prev = mcp4.port;
			mcp[4].setPorts(mcp4.port;); } }
	else return;

	if(nMcp >= 5) {
		if(mcp5.port != mcp5Prev) {
			mcp5Prev = mcp5.port;
			mcp[5].setPorts(mcp5.port;); } }
	else return;

	if(nMcp >= 6) {
		if(mcp6.port != mcp6Prev) {
			mcp6Prev = mcp6.port;
			mcp[6].setPorts(mcp6.port;); } }
	else return;

	if(nMcp >= 7) {
		if(mcp7.port != mcp7Prev) {
			mcp7Prev = mcp7.port;
			mcp[7].setPorts(mcp7.port;); } }
	else return;""")

	f.close()

#print(names)

# for i in range(0,96):
# 	print(i)