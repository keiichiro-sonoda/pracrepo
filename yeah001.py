import matplotlib as mpl

print(mpl.rcParams["figure.figsize"])
print(mpl.rcParams["figure.dpi"])
print([mpl.rcParams["figure.subplot." + name] for name in ["left", "right", "top", "bottom", "wspace", "hspace"]])