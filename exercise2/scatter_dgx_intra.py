import pandas as pd
import matplotlib.pyplot as plt
import csv

# Multi process single gpu

df = pd.read_csv('scatter_1_20663.csv',sep='\t',skiprows=10)
df = df.drop(['#repetitions','#t[usec]','#avg_latency[usec]','#bandwidth[MB/sec]'],axis=1)
df.columns = ['bytes','bandwidth_1']

for i in range(2,9):
  tmp = pd.read_csv('scatter_'+str(i)+'_20663.csv',sep='\t',skiprows=10*i)
  tmp = tmp.drop(['#repetitions','#t[usec]','#avg_latency[usec]','#bandwidth[MB/sec]'],axis=1)
  tmp.columns = ['bytes','bandwidth_'+str(i)]
  df = df.merge(tmp, how='left', on='bytes')

df.head()

df.tail()

fig, ax = plt.subplots(figsize = (9, 6))
ax.plot(df.bytes, df.bandwidth_1,'o-',color='#2DAADF',linewidth=1,label='1 proc')
ax.plot(df.bytes, df.bandwidth_2,'o-',color='#BB2DDF',linewidth=1,label='2 proc')
ax.plot(df.bytes, df.bandwidth_3,'o-',color='#DF622D',linewidth=1,label='3 proc')
ax.plot(df.bytes, df.bandwidth_4,'o-',color='#51DF2D',linewidth=1,label='4 proc')
ax.plot(df.bytes, df.bandwidth_5,'o-',color='#47EBD6',linewidth=1,label='5 proc')
ax.plot(df.bytes, df.bandwidth_6,'o-',color='#F5F11D',linewidth=1,label='6 proc')
ax.plot(df.bytes, df.bandwidth_7,'o-',color='#F51D8D',linewidth=1,label='7 proc')
ax.plot(df.bytes, df.bandwidth_8,'o-',color='#3F2DDF',linewidth=1,label='8 proc')

ax.set_xscale('log')
plt.xlabel('Message size [byte]')
plt.ylabel('Bandwidth [MB/sec]')

plt.savefig("res_20663_intra_multi_process_single_gpu.png")

# Multi gpu single process

df = pd.read_csv('scatter_1_20665.csv',sep='\t',skiprows=10)
df = df.drop(['#repetitions','#t[usec]','#avg_latency[usec]','#bandwidth[MB/sec]'],axis=1)
df.columns = ['bytes','bandwidth_1']

for i in range(2,5):
  tmp = pd.read_csv('scatter_'+str(i)+'_20665.csv',sep='\t',skiprows=10*i)
  tmp = tmp.drop(['#repetitions','#t[usec]','#avg_latency[usec]','#bandwidth[MB/sec]'],axis=1)
  tmp.columns = ['bytes','bandwidth_'+str(i)]
  df = df.merge(tmp, how='left', on='bytes')

fig, ax = plt.subplots(figsize = (9, 6))
ax.plot(df.bytes, df.bandwidth_1,'o-',color='#2DAADF',linewidth=1,label='1 pp')
ax.plot(df.bytes, df.bandwidth_2,'o-',color='#BB2DDF',linewidth=1,label='2 pp')
ax.plot(df.bytes, df.bandwidth_3,'o-',color='#DF622D',linewidth=1,label='1 pp')
ax.plot(df.bytes, df.bandwidth_4,'o-',color='#51DF2D',linewidth=1,label='1 pp')
ax.set_xscale('log')
plt.xlabel('Message size [byte]')
plt.ylabel('Bandwidth [MB/sec]')

plt.savefig("res_20665_intra_multi_gpu_single_process.png")
