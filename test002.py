import numpy as np
import matplotlib.pyplot as plt

x = np.arange(0, 10, 0.05)
y = np.arange(0, 10, 0.05)

X, Y = np.meshgrid(x, y)
Z = np.sin(X) + np.cos(Y)

plt.pcolormesh(X, Y, Z, cmap='hsv')

pp = plt.colorbar(orientation="vertical")
pp.set_label("Label", fontname="Arial", fontsize=24)

plt.xlabel('X', fontsize=24)
plt.ylabel('Y', fontsize=24)

plt.show()