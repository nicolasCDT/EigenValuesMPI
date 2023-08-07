from random import randint
from glob import glob

print("Generate a matrix file")

files = glob("matrices/*")

i = 1

while "matrices/matrix{}.txt".format(i) in files:
	i += 1

filename = "matrices/matrix{}.txt".format(i)
n = int(input("Size of matrix to generate: "))

with open(filename, "w") as file:
	file.write("# Matrix size: {}x{}\n".format(n, n))
	file.write("{}\n".format(n))
	for i in range(n):
		for j in range(n):
			file.write("{} ".format(randint(-50, 50)))
		file.write("\n")

print("The file {} a was created".format(filename))
