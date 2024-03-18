import numpy as np
from matplotlib import pyplot as plt
import os
from tkinter.filedialog import askdirectory
import glob
from scipy.signal import butter, sosfilt
from scipy.ndimage import gaussian_filter1d

LOW_PASS_FREQUENCY = 5  # Hz
FILTER_ORDER = 4
SAMPLING_RATE = 100


def get_data_directory():
    directory = askdirectory(title="Data Directory",
                             initialdir=os.getcwd())
    directory = os.path.normpath(directory)
    return directory


def get_file_names(directory):
    file_list = []
    for i in glob.glob(os.path.join(directory, '*.npy')):
        file_list.append(i)
    return file_list


def low_pass_filter_data(npdata):
    sos = butter(FILTER_ORDER, LOW_PASS_FREQUENCY, btype='low',
                 fs=SAMPLING_RATE, output='sos')
    col_indexes = np.arange(npdata.shape[1])
    filtered_data = []
    for col in col_indexes:
        filtered_data.append(sosfilt(sos, npdata[:, col]))
    return np.transpose(np.array(filtered_data))


def plot_axes_vs_time_recentered(file_name):
    npdata = np.load(file_name)
    recentered_data = npdata - np.mean(npdata, axis=0)
    plt.plot(recentered_data[:, 0], label='x')
    plt.plot(recentered_data[:, 1], label='y')
    plt.plot(recentered_data[:, 2], label='z')
    plt.xlabel("time (10s of ms)")
    plt.ylabel("field strength recentered to 0")
    plt.legend()
    plt.title(file_name.split('.')[0].split(os.path.normpath('/'))[-1])
    plt.show()


def plot_axes_vs_time_recentered_filtered(file_name):
    npdata = np.load(file_name)
    recentered_data = npdata - np.mean(npdata, axis=0)
    npdata_filtered = low_pass_filter_data(recentered_data)
    plt.plot(npdata_filtered[:, 0], label='x')
    plt.plot(npdata_filtered[:, 1], label='y')
    plt.plot(npdata_filtered[:, 2], label='z')
    plt.xlabel("time (10s of ms)")
    plt.ylabel("field strength recentered to 0")
    plt.legend()
    plt.title(file_name.split('.')[0].split(os.path.normpath('/'))[-1])
    plt.show()


def plot_axes_vs_time(file_name):
    npdata = np.load(file_name)
    plt.plot(npdata[:, 0], label='x')
    plt.plot(npdata[:, 1], label='y')
    plt.plot(npdata[:, 2], label='z')
    plt.xlabel("time (10s of ms)")
    plt.ylabel("field strength")
    plt.legend()
    plt.title(file_name.split('.')[0].split(os.path.normpath('/'))[-1])
    plt.show()


def plot_mag_3dscatter(file_name):
    npdata = np.load(file_name)
    index = np.arange(npdata.shape[0])
    ax = plt.axes(projection='3d')
    ax.scatter3D(npdata[:, 0],
                 npdata[:, 1],
                 npdata[:, 2],
                 c=index)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    ax.set_title(file_name.split('.')[0].split(os.path.normpath('/'))[-1])
    plt.show()


def plot_magnitude_vs_time(file_name):
    npdata = np.load(file_name)
    magnitude = np.apply_along_axis(np.linalg.norm, 1, npdata)
    plt.plot(magnitude)
    plt.show()


def plot_magnitude_vs_time_all_files(file_list):
    for file in file_list:
        npdata = np.load(file)
        magnitude = np.apply_along_axis(np.linalg.norm, 1, npdata)
        if '_baseline' in file:
            color = 'b'
        elif '_notbike' in file:
            color = 'r'
        else:
            color = 'g'
        plt.plot(magnitude, color)
    plt.show()


def plot_magnitude_range_all_files(file_list):
    for index, file in enumerate(file_list):
        npdata = np.load(file)
        magnitude = np.apply_along_axis(np.linalg.norm, 1, npdata)
        max = magnitude.max()
        min = magnitude.min()
        if '_baseline' in file:
            color = 'b'
        elif '_notbike' in file:
            color = 'r'
        else:
            color = 'g'
        # plt.scatter(index, max - min, c=color)
        plt.scatter([index, index], [max, min], c=color)
    plt.show()


def fft_analysis(file_name, plot=True):
    npdata = np.load(file_name)
    fft_result = np.fft.fft(npdata, axis=0)
    frequencies = np.fft.fftfreq(npdata.shape[0], 1/SAMPLING_RATE)
    frequency_indices_to_keep = (frequencies > 0) & (frequencies < 30)

    if plot:
        # Plot original signals
        title = ['x', 'y', 'z']
        for i in range(npdata.shape[1]):
            plt.subplot(3, npdata.shape[1], i+1)
            plt.plot(npdata[:, i])
            plt.title(title[i])
            plt.xlabel('Time (10s of ms)')
            plt.ylabel('Field Strength')

        # Plot FFT results
        for i in range(fft_result.shape[1]):
            plt.subplot(3, npdata.shape[1], npdata.shape[1]+i+1)
            plt.plot(frequencies[frequency_indices_to_keep],
                     np.abs(fft_result[frequency_indices_to_keep, i]))
            plt.title(f'FFT of {title[i]}')
            plt.xlabel('Frequency (Hz)')
            plt.ylabel('Magnitude')

    # Compute elementwise product of FFT values for positive frequencies for
    # all columns
    elementwise_product_result = np.prod(
        np.abs(fft_result[frequency_indices_to_keep, :]), axis=1)

    # Normalize the elementwise product result
    normalized_result = (elementwise_product_result /
                         np.max(elementwise_product_result))
    '''
    is it better to filter before normalizing or after or both?
    '''
    # Apply a Gaussian filter for smoothing
    sigma = 1  # Adjust the sigma value as needed
    smoothed_result = gaussian_filter1d(elementwise_product_result, sigma)

    # Normalize the smoothed elementwise product result
    normalized_smoothed_result = smoothed_result / np.max(smoothed_result)

    # # Apply a Gaussian filter for smoothing
    # sigma = 1  # Adjust the sigma value as needed
    # normalized_smoothed_result = gaussian_filter1d(
    #     normalized_smoothed_result, sigma)

    # Calculate the mean of normalized data
    mean_value = np.mean(normalized_result)
    mean_smoothed_value = np.mean(normalized_smoothed_result)

    THRESHOLD = 0.1

    if plot:
        # Display the normalized elementwise product result
        plt.subplot(3, npdata.shape[1], 2*npdata.shape[1]+1)
        plt.plot(frequencies[frequency_indices_to_keep],
                 normalized_result, label='Normalized')
        plt.title('Normalized Elementwise Product of FFT Results')
        plt.xlabel('Frequency (Hz)')
        plt.ylabel('Normalized Magnitude Product')
        plt.legend()
        plt.ylim(0, 1)

        # Display the mean with different text colors based on a threshold
        if mean_value < 0.22:
            mean_color = 'green'
        else:
            mean_color = 'blue'

        plt.text(15, 0.5, f'Mean: {mean_value:.2f}', color=mean_color,
                 fontsize=12, ha='center', va='center')

        # Display the normalized and smoothed elementwise product result
        plt.subplot(3, npdata.shape[1], 2*npdata.shape[1]+2)
        plt.plot(frequencies[frequency_indices_to_keep],
                 normalized_smoothed_result, label='Normalized and Smoothed')
        plt.title('Smoothed and then Normalized Elementwise Product of FFT '
                  'Results')
        plt.xlabel('Frequency (Hz)')
        plt.ylabel('Normalized Magnitude Product')
        plt.legend()
        plt.ylim(0, 1)

        # Display the mean with different text colors based on a threshold
        if mean_smoothed_value < THRESHOLD:
            mean_smoothed_color = 'green'
        else:
            mean_smoothed_color = 'blue'

        plt.text(15, 0.5, f'Mean: {mean_smoothed_value:.2f}',
                 color=mean_smoothed_color, fontsize=12,
                 ha='center', va='center')

        plt.suptitle(file_name.split('.')[0].split(os.path.normpath('/'))[-1])
        fig_manager = plt.get_current_fig_manager()
        fig_manager.window.state('zoomed')
        plt.tight_layout()
        plt.show()

    results = np.array(
        [[('_bike' in file_name) and (mean_smoothed_value < THRESHOLD),
          ('_bike' not in file_name) and (mean_smoothed_value < THRESHOLD)],
         [('_bike' in file_name) and (mean_smoothed_value >= THRESHOLD),
          ('_bike' not in file_name) and (mean_smoothed_value >= THRESHOLD)]])
    return results


def main():
    file_list = get_file_names(get_data_directory())
    street_file_list = []
    for file in file_list:
        if '_street' in file:
            street_file_list.append(file)

    results = np.array([[0, 0], [0, 0]])
    for file in file_list:
        # if '143844' in file:
        # if '143625' in file:
        if '_emt' in file and '_baseline' not in file and '_bottom' in file:
            # plot_axes_vs_time(file)
            # plot_axes_vs_time_recentered(file)
            # plot_axes_vs_time_recentered_filtered(file)
            # plot_mag_3dscatter(file)
            results += fft_analysis(file, plot=False)
    print(results)

    # for file in street_file_list:
    #     # if '_bike' in file:
    #         # plot_axes_vs_time(file)
    #         # plot_axes_vs_time_recentered(file)
    #         # plot_axes_vs_time_recentered_filtered(file)
    #         fft_analysis(file)
    #         # plot_mag_3dscatter(file)


if __name__ == "__main__":
    main()
