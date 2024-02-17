import serial
import numpy as np
from matplotlib import pyplot as plt


def parse_serial(string):
    # original string is in the format: "x:%+06d,y:%+06d,z:%+06d\n"
    string_splits = string.split(',')
    values = []
    for idx, split in enumerate(string_splits):
        num_string = split.split(':')[1]
        num = int(num_string)
        values.append(num)
    return np.array(values)


class MagSampleRecorder:
    def __init__(self, com_port):
        self.ser = serial.Serial(port=com_port)

    def __del__(self):
        self.ser.close()  # is this needed or does pyserial do it automatically

    def record_sample(self, directory, ms_to_record):
        self.ser.reset_input_buffer()
        values = []
        for i in range(ms_to_record//10):
            raw_data_string = self.ser.readline().decode("utf-8")
            values.append(parse_serial(raw_data_string))
        np_values = np.array(values)
        np.save(directory + '.npy', np_values)
        print("Mag file saved: " + directory + ".npy")

    def test_plot_mag(self, ms_to_record):
        self.ser.flush()
        values = []
        for i in range(ms_to_record//10):
            raw_data_string = self.ser.readline().decode("utf-8")
            values.append(parse_serial(raw_data_string))
        np_values = np.array(values)
        plt.plot(np_values[:,0])
        plt.plot(np_values[:,1])
        plt.plot(np_values[:,2])
        plt.show()


def main():
    msr = MagSampleRecorder('COM5')
    msr.test_plot_mag(1000)
    del msr


if __name__ == "__main__":
    main()