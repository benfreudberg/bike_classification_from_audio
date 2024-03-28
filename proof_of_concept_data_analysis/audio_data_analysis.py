import os
from tkinter.filedialog import askdirectory
import glob
import pandas as pd
import librosa
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.metrics import confusion_matrix
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


def preprocess_audio_files(file_list):
    X_data = []
    y_labels = []
    for file in file_list:
        audio, sr = librosa.load(file, sr=None)
        # If audio has multiple channels, take the mean
        if audio.ndim > 1:
            audio = np.mean(audio, axis=1)

        fft = np.abs(np.fft.fft(audio))
        freqs = np.fft.fftfreq(len(fft), 1/sr)

        bins, binned_values = binning.bin_and_average_fft(freqs,
                                                          fft,
                                                          5000,
                                                          linear=True)

        # Add a column based on filename
        label = 1 if '_bike' in os.path.basename(file) else 0

        # Append to the data lists
        X_data.append(binned_values)
        y_labels.append(label)

    # Create dataframe for X_data
    columns = bins
    df_X = pd.DataFrame(X_data, columns=columns)

    # Create dataframe for y_labels
    df_y = pd.DataFrame(y_labels, columns=['bike'])

    return df_X, df_y


def main():
    file_list = get_file_names(get_data_directory())
    emt_top_file_names = [file for file in file_list if '_emt' in file
                          and '_bottom' not in file]
    emt_bottom_file_names = [file for file in file_list if '_emt' in file
                             and '_bottom' in file]
    mbp_file_names = [file for file in file_list if '_volleyball' in file]
    X, y = preprocess_audio_files(file_list)
    X_train, X_test, y_train, y_test = train_test_split(X.values,
                                                        y.values[:, 0],
                                                        test_size=0.35,
                                                        random_state=21,
                                                        stratify=y)
    knn = train_and_test.KNN_model(X_train, X_test, y_train, y_test)
    rfc = train_and_test.RFC_model(X_train, X_test, y_train, y_test)
    lsvc = train_and_test.LSVC_model(X_train, X_test, y_train, y_test)
    lr = train_and_test.LR_model(X_train, X_test, y_train, y_test)

    X, y = preprocess_audio_files(emt_top_file_names)
    y_pred = knn.predict(X)
    print('knn confusion_matrix for emt not bottom:')
    print(confusion_matrix(y, y_pred))
    y_pred = rfc.predict(X)
    print('rfc confusion_matrix for emt not bottom:')
    print(confusion_matrix(y, y_pred))
    y_pred = lsvc.predict(X)
    print('lsvc confusion_matrix for emt not bottom:')
    print(confusion_matrix(y, y_pred))
    y_pred = lr.predict(X)
    print('lr confusion_matrix for emt not bottom:')
    print(confusion_matrix(y, y_pred))

    X, y = preprocess_audio_files(emt_bottom_file_names)
    y_pred = knn.predict(X)
    print('knn confusion_matrix for emt bottom:')
    print(confusion_matrix(y, y_pred))
    y_pred = rfc.predict(X)
    print('rfc confusion_matrix for emt bottom:')
    print(confusion_matrix(y, y_pred))
    y_pred = lsvc.predict(X)
    print('lsvc confusion_matrix for emt bottom:')
    print(confusion_matrix(y, y_pred))
    y_pred = lr.predict(X)
    print('lr confusion_matrix for emt bottom:')
    print(confusion_matrix(y, y_pred))

    X, y = preprocess_audio_files(mbp_file_names)
    y_pred = knn.predict(X)
    print('knn confusion_matrix for mbp:')
    print(confusion_matrix(y, y_pred))
    y_pred = rfc.predict(X)
    print('rfc confusion_matrix for mbp:')
    print(confusion_matrix(y, y_pred))
    y_pred = lsvc.predict(X)
    print('lsvc confusion_matrix for mbp:')
    print(confusion_matrix(y, y_pred))
    y_pred = lr.predict(X)
    print('lr confusion_matrix for mbp:')
    print(confusion_matrix(y, y_pred))


if __name__ == "__main__":
    main()
