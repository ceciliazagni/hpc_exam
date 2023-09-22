import pandas as pd
import numpy as np
import seaborn as sns
import sys
import matplotlib.pyplot as plt

# pass as argument the jobid of the results you want to plot
jobid = str(sys.argv[1])

# csv file to plot
heatmap = "heatmap_"+jobid+".csv"

# name of the output png file
results_path = "res_"+jobid+".png"

# import the data
df = pd.read_csv(heatmap,header=None)
df.columns = ['from','to','latency']

# reshape the data
df = df.pivot(index='from', columns='to', values='latency')

# setting the dimensions of the plot
fig, ax = plt.subplots(figsize=(33,30))

# drawing the plot (print only 1 every 4 ticklabels)
#sns.heatmap(df,cmap='flare',cbar_kws={'label': 'Latency'}, vmin=0.1, vmax=1.2,xticklabels=4,yticklabels=4)
sns.heatmap(df,cmap='flare',cbar_kws={'label': 'Latency'}, xticklabels=4,yticklabels=4)
ax.figure.axes[-1].yaxis.label.set_size(20)
ax.figure.axes[-1].tick_params(labelsize=20)
plt.tick_params(axis='both', labelsize=20, labelbottom = True, bottom=True, top = True, labeltop=True)
ax.set_xlabel("To",fontsize=20)
ax.set_ylabel("From",fontsize=20)

# result path + save
plt.savefig(results_path)

