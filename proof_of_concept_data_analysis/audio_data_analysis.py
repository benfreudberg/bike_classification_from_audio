import os
from tkinter.filedialog import askdirectory
import glob
import pandas as pd
import librosa
import librosa.display
import matplotlib.pyplot as plt
import numpy as np
from sklearn.model_selection import train_test_split
import train_and_test
import binning


def get_data_directory():
    directory = askdirectory(title="Data Directory",
                             initialdir=os.getcwd())
    directory = os.path.normpath(directory)
    return directory


def get_file_names(directory):
    file_list = []
    for i in glob.glob(os.path.join(directory, '*.wav')):
        file_list.append(i)
    return file_list


def fft_of_mfccs(file_name, plot=False):
    audio, sr = librosa.load(file_name, sr=None)
    # If audio has multiple channels, take the mean
    if audio.ndim > 1:
        audio = np.mean(audio, axis=1)

    #  limit to just under first second so all files will work, but ideally,
    #  we would have a longer sample period
    audio = audio[:40000]

    # Adjusting the total number of frames by changing hop_length and
    # win_length
    hop_length = int(0.02 * sr)  # 20ms hop size
    win_length = int(0.04 * sr)  # 40ms window size

    # Compute MFCCs with n_mfcc=50 and specified hop_length and win_length
    mfccs = librosa.feature.mfcc(y=audio, sr=sr, n_mfcc=100,
                                 hop_length=hop_length, win_length=win_length)

    # Calculate the FFT along the rows (axis=1) of the mfccs
    fft_of_mfccs = np.fft.fft(mfccs, axis=1)

    # Keep only the positive frequencies
    sampling_rate = 1 / (hop_length / sr)
    frequencies = np.fft.fftfreq(mfccs.shape[1], 1/sampling_rate)
    frequency_indices_to_keep = (frequencies > 0)
    fft_result_to_keep = np.abs(fft_of_mfccs[:, frequency_indices_to_keep])

    # Bin into 20 groups
    # fft_result_to_keep_binned_list = []
    # for row in fft_result_to_keep:
    #     bins, binned_row = binning.bin_and_average_fft(
    #         frequencies[frequency_indices_to_keep], row, 49)
    #     fft_result_to_keep_binned_list.append(binned_row)
    # fft_result_to_keep_binned = np.array(fft_result_to_keep_binned_list)
    fft_result_to_keep_binned = fft_result_to_keep

    if plot:
        # Display fft of MFCCs
        plt.figure(figsize=(10, 6))
        plt.imshow(fft_result_to_keep_binned, aspect='auto', cmap='hot',
                   origin='lower',
                   extent=([frequencies[0],
                           frequencies[frequency_indices_to_keep][-1],
                           0,
                           sr // 2 + 1]))
        plt.yticks([])
        plt.colorbar()
        plt.title('fft of MFCCs ' + file_name.split('\\')[-1])
        # plt.tight_layout()
        plt.show()

    return fft_result_to_keep_binned


def fft_of_spectrogram(file_name, plot=False):
    # Load the audio sample
    audio, sr = librosa.load(file_name, sr=None)
    # If audio has multiple channels, take the mean
    if audio.ndim > 1:
        audio = np.mean(audio, axis=1)

    #  limit to just under first second so all files will work, but ideally,
    #  we would have a longer sample period
    audio = audio[:40000]

    # Adjusting the total number of frames by changing hop_length and
    # win_length
    hop_length = int(0.02 * sr)  # 20ms hop size
    win_length = int(0.04 * sr)  # 40ms window size

    # Compute the spectrogram
    spectrogram = np.abs(librosa.stft(audio, n_fft=win_length,
                                      hop_length=hop_length))

    # Keep positive values below 10 kHz
    upper_freq_limit = 10000
    freqs = librosa.fft_frequencies(sr=sr, n_fft=win_length)
    spectrogram = spectrogram[freqs < upper_freq_limit]

    # Calculate the FFT along the rows (axis=1) of the spectrogram
    fft_of_spectrogram = np.fft.fft(spectrogram, axis=1)

    # Keep only the positive frequencies
    sampling_rate = 1 / (hop_length / sr)
    frequencies = np.fft.fftfreq(spectrogram.shape[1], 1/sampling_rate)
    frequency_indices_to_keep = (frequencies > 0)
    fft_result_to_keep = np.abs(
        fft_of_spectrogram[:, frequency_indices_to_keep])

    # Bin into 20 groups
    # fft_result_to_keep_binned_list = []
    # for row in fft_result_to_keep:
    #     bins, binned_row = binning.bin_and_average_fft(
    #         frequencies[frequency_indices_to_keep], row, 49)
    #     fft_result_to_keep_binned_list.append(binned_row)
    # fft_result_to_keep_binned = np.array(fft_result_to_keep_binned_list)
    fft_result_to_keep_binned = fft_result_to_keep

    if plot:
        # Plot the magnitude as an intensity plot
        plt.figure(figsize=(10, 6))
        plt.imshow(fft_result_to_keep_binned, aspect='auto', origin='lower',
                   cmap='inferno',
                   extent=([frequencies[0],
                           frequencies[frequency_indices_to_keep][-1],
                           freqs[0],
                           freqs[freqs < upper_freq_limit][-1]]))
        plt.colorbar(label='Magnitude')
        plt.xlabel('Frequency of Frequency')
        plt.ylabel('Frequency (Hz)')
        plt.title(file_name.split('\\')[-1])
        plt.show()

    return fft_result_to_keep_binned


def preprocess_audio_files(file_list):
    X_data = []
    y_labels = []
    for file in file_list:
        audio, sr = librosa.load(file, sr=None)
        # If audio has multiple channels, take the mean
        if audio.ndim > 1:
            audio = np.mean(audio, axis=1)

        # audio = audio[int(0.4 * sr) : int(0.9 * sr)]
        fft = np.abs(np.fft.fft(audio))
        freqs = np.fft.fftfreq(len(fft), 1/sr)
        # frequency_indices_to_keep = (freqs < 10000)

        bins, binned_values = binning.bin_and_average_fft(
            freqs,  # [frequency_indices_to_keep],
            fft,  # [frequency_indices_to_keep],
            5000,
            linear=True)

        # Add a column based on filename
        label = 1 if '_bike' in os.path.basename(file) else 0

        # Append to the data lists
        X_data.append(binned_values)
        # X_data.append(fft[frequency_indices_to_keep])
        y_labels.append(label)

    # Create dataframe for X_data
    columns = bins
    # columns = freqs[frequency_indices_to_keep]
    df_X = pd.DataFrame(X_data, columns=columns)

    # Create dataframe for y_labels
    df_y = pd.DataFrame(y_labels, columns=['bike'])

    return df_X, df_y


def preprocess_audio_files_with_fft_of_mfccs(file_list):
    X_data = []
    y_labels = []
    for file in file_list:
        fft_of_mfccs_results = fft_of_mfccs(file)

        # Add a column based on filename
        label = 1 if '_bike' in os.path.basename(file) else 0

        # Append to the data lists
        X_data.append(fft_of_mfccs_results.flatten())
        y_labels.append(label)

    # Create dataframe for X_data
    df_X = pd.DataFrame(X_data)

    # Create dataframe for y_labels
    df_y = pd.DataFrame(y_labels, columns=['bike'])

    return df_X, df_y


def preprocess_audio_files_with_fft_of_spectrogram(file_list):
    X_data = []
    y_labels = []
    for file in file_list:
        fft_of_spectrogram_results = fft_of_spectrogram(file)

        # Add a column based on filename
        label = 1 if '_bike' in os.path.basename(file) else 0

        # Append to the data lists
        X_data.append(fft_of_spectrogram_results.flatten())
        y_labels.append(label)

    # Create dataframe for X_data
    df_X = pd.DataFrame(X_data)

    # Create dataframe for y_labels
    df_y = pd.DataFrame(y_labels, columns=['bike'])

    return df_X, df_y


def main():
    file_list = get_file_names(get_data_directory())
    # emt_top_file_names = [file for file in file_list if '_emt' in file
    #                       and '_bottom' not in file]
    # emt_bottom_file_names = [file for file in file_list if '_emt' in file
    #                          and '_bottom' in file]
    # mbp_file_names = [file for file in file_list if '_volleyball' in file]
    # long_mbp_file_names = (
    #     [file for file in file_list if '_volleyball' in file])

    # for file in file_list:
    #     fft_of_spectrogram(file, True)
    #     fft_of_mfccs(file, True)
    # return

    print('\nfft of mfccs')
    X, y = preprocess_audio_files_with_fft_of_mfccs(file_list)
    X_train, X_test, y_train, y_test = train_test_split(X.values,
                                                        y.values[:, 0],
                                                        test_size=0.35,
                                                        random_state=21,
                                                        stratify=y)
    train_and_test.KNN_model(X_train, X_test, y_train, y_test)
    train_and_test.RFC_model(X_train, X_test, y_train, y_test)
    train_and_test.LSVC_model(X_train, X_test, y_train, y_test)
    train_and_test.LR_model(X_train, X_test, y_train, y_test)

    print('\nfft of spectrogram')
    X, y = preprocess_audio_files_with_fft_of_spectrogram(file_list)
    X_train, X_test, y_train, y_test = train_test_split(X.values,
                                                        y.values[:, 0],
                                                        test_size=0.35,
                                                        random_state=21,
                                                        stratify=y)
    train_and_test.KNN_model(X_train, X_test, y_train, y_test)
    train_and_test.RFC_model(X_train, X_test, y_train, y_test)
    train_and_test.LSVC_model(X_train, X_test, y_train, y_test)
    train_and_test.LR_model(X_train, X_test, y_train, y_test)

    print('\nfft')
    X, y = preprocess_audio_files(file_list)
    X_train, X_test, y_train, y_test = train_test_split(X.values,
                                                        y.values[:, 0],
                                                        test_size=0.35,
                                                        random_state=21,
                                                        stratify=y)
    train_and_test.KNN_model(X_train, X_test, y_train, y_test)
    train_and_test.RFC_model(X_train, X_test, y_train, y_test)
    train_and_test.LSVC_model(X_train, X_test, y_train, y_test)
    train_and_test.LR_model(X_train, X_test, y_train, y_test)


if __name__ == "__main__":
    main()
