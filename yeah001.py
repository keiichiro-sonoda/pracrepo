import matplotlib as mpl
import matplotlib.pyplot as plt

print(mpl.rcParams["figure.figsize"])
print(mpl.rcParams["figure.dpi"])
print([mpl.rcParams["figure.subplot." + name] for name in ["left", "right", "top", "bottom", "wspace", "hspace"]])

fig = plt.figure(facecolor="skyblue", linewidth=100, edgecolor="green")
ax = fig.add_subplot(111, ylabel="ylabel")
fig.subplots_adjust(left=0, right=1, bottom=0.2, top=0.8)

print(fig)
print(ax)
fig.savefig("//home//sonoda//Pictures//002.png", facecolor="#0000ff", edgecolor="orange")
fig.savefig("//home//sonoda//Pictures//002_tight.png", facecolor="#0000ff", edgecolor="#ff00ff", bbox_inches="tight")

plt.show()