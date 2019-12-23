# -*- coding: utf-8 -*-

import numpy as np
import struct
import matplotlib.pyplot as plt

fft = np.array([])

with open("/home/romancomelli/Documents/education/courses/DSI/digital_signal_processing/practice/pulsador/python/data_z_output_abnormal.txt", "r") as f:
    lines = f.readlines()
f.close()

for i in range(0, len(lines)):
    split_line = lines[i].split()
    for j in range(0, len(split_line) / 4):
        index = ((j + 1) * 4) - 1
        hexa = split_line[index] + split_line[index - 1] + split_line[index - 2] + split_line[index - 3]
        fft = np.append(fft, struct.unpack('!f', hexa.decode('hex'))[0])

plt.plot(fft) #La función stem también sirve para graficar, aunque la idea es para señales discretas.
plt.grid()
plt.title('fft(n)')
plt.xlabel("n")
plt.ylabel("Magnitud")
plt.show()
