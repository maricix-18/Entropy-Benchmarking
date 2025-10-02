import json
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

matplotlib.use('TkAgg') 
# Load JSON data from two file

print("Read data")
with open('/Benchmarking/Data_test/ClassicalShadows_metrics/results_debugging/Q3_m320_k1000_quest_order_sampling2.json') as f:
    data2 = json.load(f)

with open('C:/Users/maria/Desktop/Entropy_Benchmark_quest/Entropy-Benchmarking/Benchmarking/Data_test/DensityMatrices_metrics/Q3.json') as f:
    data1 = json.load(f)

print("show data")
print("data1: ", data1)
print("data2: ", data2)
# Get all keys that exist in both files (intersection)
fields = set(data1.keys()) & set(data2.keys())

print("fields: ", fields)


x = np.arange(len(data1['all_pur_diff_n']))  # depth axis

# ---- Plot Pur ----
plt.figure(figsize=(8,5))
plt.plot(x, data1['all_pur_diff_n'], 'k-', label='exact')
plt.errorbar(x, data2['all_pur_mean_diff_n'], yerr=data2['all_pur_std_diff_n'],
             fmt='none',ecolor='r', capsize=5, label='Classical Shadow')

plt.xlabel('Depth')
plt.ylabel('Purity')
plt.title('Purity classical shadows')
plt.legend()
plt.grid(True)
plt.tight_layout()

# ---- Plot R2d ----
plt.figure(figsize=(8,5))
plt.plot(x, data1['all_R2d_diff_n'], 'k-', label='exact')
plt.errorbar(x, data2['all_R2d_mean_diff_n'], yerr=data2['all_R2d_std_diff_n'],
             fmt='none',ecolor='r', capsize=5,label='Classical Shadow')

plt.xlabel('Depth')
plt.ylabel('Renyi-2')
plt.title('Renyi-2 entropy')
plt.legend()
plt.grid(True)
plt.tight_layout()

plt.show()