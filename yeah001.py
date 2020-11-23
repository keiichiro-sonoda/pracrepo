import matplotlib as mpl
import matplotlib.pyplot as plt

print(mpl.rcParams["figure.figsize"])
print(mpl.rcParams["figure.dpi"])
print([mpl.rcParams["figure.subplot." + name] for name in ["left", "right", "top", "bottom", "wspace", "hspace"]])

fig = plt.figure(facecolor="#ff0000")
ax = fig.add_subplot(111)

print(fig)
print(ax)

fig.savefig("//home//sonoda//Pictures//001.png", facecolor="#00ff00")
fig.savefig("//home//sonoda//Pictures//001_tight.png", facecolor="#00ff00", bbox_inches="tight")