import numpy as np
from matplotlib import pyplot as plt


file = "C:\\Users\\Ben Laptop\\Documents\\github\\bike_identification\\proof_of_concept_data_samples\\2024-02-21-202557_street_bike.npy"

np_values = np.load(file)
plt.plot(np_values[:, 0])
plt.plot(np_values[:, 1])
plt.plot(np_values[:, 2])
plt.show()