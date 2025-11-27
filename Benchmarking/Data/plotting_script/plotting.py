import json
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
matplotlib.use('TkAgg') 

# Make sure you're inside the plotting_script directory
with open('../../Data_test/DensityMatrices_metrics/Q3.json') as f:
    data1 = json.load(f)

with open('../../Data_test/Swap_metrics/Q3_m3000_g3_s3.json') as f:
    data2 = json.load(f)

with open('../../Data_test/ClassicalShadows_metrics/Q3_m320_k1000_g5_s3.json') as f:
    data3 = json.load(f)

with open('../../Data_test/AnalyticalModel_metrics/Q3_globalDP.json') as f:
    data4 = json.load(f)

with open('../../Data_test/AnalyticalModel_metrics/Q3_CS.json') as f:
    data5 = json.load(f)

with open('../../Data_test/AnalyticalModel_metrics/Q3.json') as f:
    data6 = json.load(f)

# print("show data")
# print("data1: ", data1)
# print("data2: ", data2)
# print("data3: ", data3)
# print("data3: ", data3)
# print("data4: ", data4)
# print("data5: ", data5)
# print("data6: ", data6)

# Get all keys that exist in both files (intersection)
fields = set(data1.keys()) & set(data2.keys()) & set(data3.keys()) & set(data4.keys()) & set(data5.keys()) & set(data6.keys())

print("fields: ", fields)


x = np.arange(len(data1['all_pur_diff_n']))  # depth axis
x4 = data4['depth_tab_more_points'] # global DP
x5 = data5['depth_tab_more_points'] # CS
x6 = data6['depth_tab_more_points'] # globalDP localDP
# ---- Plot Pur ----
plt.figure(figsize=(8,5))
plt.plot(x, data1['all_pur_diff_n'], 'k-', label='exact')
# plt.errorbar(x, data2['all_pur_mean_diff_n'], yerr=data2['all_pur_std_diff_n'],
#              fmt='none',ecolor='r', capsize=5, label='Swap')
# plt.errorbar(x, data3['all_pur_mean_diff_n'], yerr=data3['all_pur_std_diff_n'],
#             fmt='none', ecolor='b', capsize=5, label='Classical Shadows') 
# plt.plot(x4, data4['all_pur_diff_n'], label='PM_globalDP', linestyle='dotted')
#plt.plot(x5, data5['all_pur_diff_n'], label='PM_CS', linestyle='dashed')
plt.plot(x6, data6['all_pur_diff_n'], label='PM_globalDP_localDP', linestyle='dotted')



plt.xlabel('Depth')
plt.ylabel('Purity')
plt.title('Purity')
plt.legend()
plt.grid(True)
plt.tight_layout()

# ---- Plot R2d ----
# plt.figure(figsize=(8,5))
# plt.plot(x, data1['all_R2d_diff_n'], 'k-', label='exact')
# # plt.errorbar(x, data2['all_R2d_mean_diff_n'], yerr=data2['all_R2d_std_diff_n'],
# #              fmt='none',ecolor='r', capsize=5,label='Swap')
# # plt.errorbar(x, data3['all_R2d_mean_diff_n'], yerr=data3['all_R2d_std_diff_n'],
# #             fmt='none', ecolor='b', capsize=5, label='Classical Shadows')
# plt.plot(x6, data6['all_R2d_diff_n'], label='PM_globalDP_localDP', linestyle='dotted')

# plt.xticks(x)
# plt.xlabel('Depth')
# plt.ylabel('Renyi-2')
# plt.title('Renyi-2 entropy')
# plt.legend()
# plt.grid(True)
# plt.tight_layout()

plt.show()