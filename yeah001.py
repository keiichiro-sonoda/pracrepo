import matplotlib as mpl
import matplotlib.pyplot as plt

print(mpl.rcParams["figure.figsize"])
print(mpl.rcParams["figure.dpi"])
print([mpl.rcParams["figure.subplot." + name] for name in ["left", "right", "top", "bottom", "wspace", "hspace"]])

fig = plt.figure(facecolor="#ffffff")
ax = fig.add_subplot(111)

print(fig)
print(ax)