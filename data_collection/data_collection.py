import os
import datetime
import tkinter as tk
import threading
import serial
from tkinter.filedialog import askdirectory
import audio_sampling
import mag_sampling

MS_TO_RECORD = 1000


class EventSampleRecorder:
    __DEFAULT_DIRECTORY_STRING = "no directory set"
    __DEFAULT_LOCATION_STRING = "no location set"

    def __init__(self):
        self.__root = tk.Tk()
        self.__root.geometry("1000x500")
        self.__root.title("Audio Sample Recording Application")

        self.__entry_frame = tk.Frame(self.__root)
        self.__entry_frame.columnconfigure(0, weight=1)
        self.__entry_frame.columnconfigure(1, weight=1)

        self.__set_output_directory_button = (
            tk.Button(self.__entry_frame,
                      text="Set Output Directory",
                      command=self.__set_output_directory))
        self.__set_output_directory_button.grid(row=0, column=0,
                                                sticky=tk.W+tk.E)
        self.__directory_text_box = tk.Entry(self.__entry_frame, width=100)
        self.__directory_text_box.insert(0, self.__DEFAULT_DIRECTORY_STRING)
        self.__directory_text_box.grid(row=0, column=1,
                                       sticky=tk.W+tk.E, pady=10)

        self.__location_label = tk.Label(self.__entry_frame,
                                         text="Sample Location")
        self.__location_label.grid(row=1, column=0, sticky=tk.W+tk.E, pady=10)
        self.__location_text_box = tk.Entry(self.__entry_frame)
        self.__location_text_box.insert(0, self.__DEFAULT_LOCATION_STRING)
        self.__location_text_box.grid(row=1, column=1,
                                      sticky=tk.W+tk.E, pady=10)
        
        self.__connect_mag_button = (
            tk.Button(self.__entry_frame,
                      text="Connect Magnetometer",
                      command=self.__connect_mag))
        self.__connect_mag_button.grid(row=2, column=0,
                                       sticky=tk.W+tk.E)
        self.__mag_com_port_text_box = tk.Entry(self.__entry_frame, width=100)
        self.__mag_com_port_text_box.insert(0, 'COM5')
        self.__mag_com_port_text_box.grid(row=2, column=1,
                                          sticky=tk.W+tk.E, pady=10)

        self.__entry_frame.pack(pady=50)

        self.__button_frame = tk.Frame(self.__root)
        self.__button_frame.columnconfigure(0, weight=1)
        self.__button_frame.columnconfigure(1, weight=1)

        self.__button_bike = tk.Button(self.__button_frame,
                                       text="Record \"bike\"",
                                       font=('Arial', 32),
                                       command=self.__bike_click_handler)
        self.__button_bike.grid(row=0, column=0, sticky=tk.W+tk.E)
        self.__button_not_bike = tk.Button(
                self.__button_frame, text="Record \"notbike\"",
                font=('Arial', 32), command=self.__not_bike_click_handler)
        self.__button_not_bike.grid(row=0, column=1, sticky=tk.W+tk.E)

        self.__button_frame.pack(pady=50)

        self.__root.mainloop()

    def __click_handler(self, bike):
        directory = self.__directory_text_box.get()
        location_string = self.__location_text_box.get()
        if directory == self.__DEFAULT_DIRECTORY_STRING:
            print('No output directory set')
            return
        if not os.path.exists(directory):
            print('directory does not exist')
            return
        if location_string == self.__DEFAULT_LOCATION_STRING:
            print('No location set')
            return
        # todo: add check boxes for whether to record audio and mag

        bike_string = "bike" if bike else "notbike"
        timestamp = datetime.datetime.now()
        timestamp_string = timestamp.strftime("%Y-%m-%d-%H%M%S")
        file_name = (timestamp_string + "_" +
                     location_string + "_" +
                     bike_string)
        full_directory = os.path.join(directory, file_name)

        t1 = threading.Thread(target=audio_sampling.record_sample,
                              args=(full_directory, MS_TO_RECORD))
        t2 = threading.Thread(target=self.__mag_sample_recorder.record_sample,
                              args=(full_directory, MS_TO_RECORD))
        t1.start()
        t2.start()
        t1.join()
        t2.join()

    def __bike_click_handler(self):
        self.__click_handler(True)

    def __not_bike_click_handler(self):
        self.__click_handler(False)

    def __set_output_directory(self):
        directory = askdirectory(title="Set Output Directory",
                                 initialdir=os.getcwd())
        directory = os.path.normpath(directory)
        self.__directory_text_box.delete(0, tk.END)
        self.__directory_text_box.insert(0, directory)
    
    def __connect_mag(self):
        self.__mag_sample_recorder = mag_sampling.MagSampleRecorder(
                self.__mag_com_port_text_box.get())
