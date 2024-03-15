import serial
import numpy as np
from matplotlib import pyplot as plt


def parse_serial(string):
    # original string is in the format: "x:%+07ld,y:%+07ld,z:%+07ld\n"
    string_splits = string.split(',')
    values = []
    for split in string_splits:
        num_string = split.split(':')[1]
        num = int(num_string)
        values.append(num)
    return np.array(values)


class MagSampleRecorder:
    def __init__(self, com_port):
        self.__ser = serial.Serial(port=com_port)

    def __collect_data(self, ms_to_record):
        # reset buffer to remove old data and clear one line in case it is
        # partial
        self.__ser.reset_input_buffer()
        self.__ser.readline().decode("utf-8")

        values = []
        for i in range(ms_to_record//10):
            raw_data_string = self.__ser.readline().decode("utf-8")
            values.append(parse_serial(raw_data_string))
        np_values = np.array(values)
        return np_values

    def record_sample(self, directory, ms_to_record):
        np_values = self.__collect_data(ms_to_record)
        np.save(directory + '.npy', np_values)
        print("Mag file saved: " + directory + ".npy")

    def test_plot_mag(self, ms_to_record):
        np_values = self.__collect_data(ms_to_record)
        plt.plot(np_values[:, 0])
        plt.plot(np_values[:, 1])
        plt.plot(np_values[:, 2])
        plt.show()


def main():
    msr = MagSampleRecorder('COM5')  # change to match your local machine
    msr.test_plot_mag(1000)
    del msr


if __name__ == "__main__":
    main()
