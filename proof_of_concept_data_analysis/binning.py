import numpy as np


def bin_and_average_fft(fft_freqs, fft_values, num_bins, linear=True):
    # Define frequency bins
    if linear:
        bins = np.linspace(start=0, stop=np.max(fft_freqs), num=num_bins+1)
    else:
        bins = np.logspace(start=1.69897, stop=4.176, num=num_bins+1)

    # Initialize arrays to store binned FFT values and counts
    binned_values = np.zeros(num_bins)
    counts = np.zeros(num_bins)

    # Bin FFT values by frequency and accumulate the values and counts
    for i in range(num_bins):
        bin_start = bins[i]
        bin_end = bins[i + 1]
        mask = (fft_freqs > bin_start) & (fft_freqs <= bin_end)
        fft_values_in_bin = fft_values[mask]
        # Sum FFT values within the bin
        binned_values[i] += np.sum(np.abs(fft_values_in_bin))
        # Count number of data points contributing to the bin
        counts[i] += np.sum(mask)

    # Average FFT values within each bin
    for i in range(num_bins):
        if counts[i] > 0:
            binned_values[i] /= counts[i]

    return bins[:-1], binned_values
