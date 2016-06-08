import numpy as np
import matplotlib.pyplot as plt

#-------------------------------------------------------------------------------
with open("coordinates.txt", "r") as f:
	x = []
	y = []
	for line in f:
		if not line.strip():
			continue
		else:
		
			row = line.split(' ')
	        	x.append(int(row[0]))        		
			y.append(int(row[1]))


plt.title('Coordinates latitude')
plt.xlabel('x')
plt.ylabel('y')
plt.ylim(0,20)
plt.plot(x,y, 'ro')
plt.legend([''], loc='upper left')
plt.show()
#plt.savefig("text.png", dpi=300)


