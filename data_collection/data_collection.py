import os
import datetime
import tkinter as tk
import threading
from tkinter.filedialog import askdirectory
import audio_sampling
import mag_sampling
import video_sampling
import serial

DEFAULT_MS_TO_RECORD = 2000
DEFAULT_COM_PORT = 'COM5'
DEFAULT_WEBCAM_NUMBER = 0


class EventSampleRecorder:
    __DEFAULT_DIRECTORY_STRING = "no directory set"
    __DEFAULT_LOCATION_STRING = "no location set"

    def __init__(self):
        self.__root = tk.Tk()
        self.__root.geometry("1000x600")
        self.__root.title("Sample Recording Application")

        self.__entry_frame = tk.Frame(self.__root)
        self.__entry_frame.columnconfigure(0, weight=1)
        self.__entry_frame.columnconfigure(1, weight=1)

        self.__sample_duration_label = tk.Label(
            self.__entry_frame, text="Sample Recording Duration (ms)")
        self.__sample_duration_label.grid(row=0, column=0,
                                          sticky=tk.W+tk.E, pady=10)
        self.__sample_duration_text_box = tk.Entry(self.__entry_frame,
                                                   width=100)
        self.__sample_duration_text_box.insert(0, DEFAULT_MS_TO_RECORD)
        self.__sample_duration_text_box.grid(
            row=0, column=1, sticky=tk.W+tk.E, pady=10)

        self.__set_output_directory_button = (
            tk.Button(self.__entry_frame,
                      text="Set Output Directory",
                      command=self.__set_output_directory))
        self.__set_output_directory_button.grid(row=1, column=0,
                                                sticky=tk.W+tk.E)
        self.__directory_text_box = tk.Entry(self.__entry_frame, width=100)
        self.__directory_text_box.insert(
            0, EventSampleRecorder.__DEFAULT_DIRECTORY_STRING)
        self.__directory_text_box.grid(
            row=1, column=1, sticky=tk.W+tk.E, pady=10)

        self.__location_label = tk.Label(
            self.__entry_frame, text="Sample Location")
        self.__location_label.grid(row=2, column=0, sticky=tk.W+tk.E, pady=10)
        self.__location_text_box = tk.Entry(self.__entry_frame)
        self.__location_text_box.insert(0, self.__DEFAULT_LOCATION_STRING)
        self.__location_text_box.grid(
            row=2, column=1, sticky=tk.W+tk.E, pady=10)

        self.__connect_mag_button = (
            tk.Button(self.__entry_frame,
                      text="Connect Magnetometer",
                      command=self.__connect_mag))
        self.__connect_mag_button.grid(
            row=3, column=0, sticky=tk.W+tk.E)
        self.__mag_com_port_text_box = tk.Entry(self.__entry_frame, width=100)
        self.__mag_com_port_text_box.insert(0, DEFAULT_COM_PORT)
        self.__mag_com_port_text_box.grid(
            row=3, column=1, sticky=tk.W+tk.E, pady=10)

        self.__webcam_number_label = tk.Label(
            self.__entry_frame, text="Webcam Number")
        self.__webcam_number_label.grid(row=4, column=0,
                                        sticky=tk.W+tk.E, pady=10)
        self.__webcam_number_text_box = tk.Entry(self.__entry_frame, width=100)
        self.__webcam_number_text_box.insert(0, str(DEFAULT_WEBCAM_NUMBER))
        self.__webcam_number_text_box.grid(
            row=4, column=1, sticky=tk.W+tk.E, pady=10)

        self.__entry_frame.pack(pady=25)

        self.__checkbox_frame = tk.Frame(self.__root)
        self.__checkbox_frame.columnconfigure(0, weight=1)
        self.__checkbox_frame.columnconfigure(1, weight=1)
        self.__checkbox_frame.columnconfigure(2, weight=1)

        self.__mag_check_state = tk.IntVar(value=1)
        self.__audio_check_state = tk.IntVar(value=1)
        self.__video_check_state = tk.IntVar(value=0)

        self.__mag_check_box = tk.Checkbutton(
            self.__checkbox_frame, text="Record magnetometer data",
            variable=self.__mag_check_state)
        self.__audio_check_box = tk.Checkbutton(
            self.__checkbox_frame, text="Record audio data",
            variable=self.__audio_check_state)
        self.__video_check_box = tk.Checkbutton(
            self.__checkbox_frame, text="Record video",
            variable=self.__video_check_state,
            command=self.__configure_video_recorder)
        self.__mag_check_box.grid(
            row=0, column=0, sticky=tk.W+tk.E, pady=10)
        self.__audio_check_box.grid(
            row=0, column=1, sticky=tk.W+tk.E, pady=10)
        self.__video_check_box.grid(
            row=0, column=2, sticky=tk.W+tk.E, pady=10)

        self.__checkbox_frame.pack()

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
        record_mag = self.__mag_check_state.get()
        record_audio = self.__audio_check_state.get()
        record_video = self.__video_check_state.get()
        if directory == EventSampleRecorder.__DEFAULT_DIRECTORY_STRING:
            print('No output directory set')
            return
        if not os.path.exists(directory):
            print('directory does not exist')
            return
        if location_string == EventSampleRecorder.__DEFAULT_LOCATION_STRING:
            print('No location set')
            return
        if record_mag:
            try:
                self.__mag_sample_recorder
            except AttributeError:
                print('Magnetometer port not connected')
                return
        if not record_audio and not record_mag:
            print('not set to record audio or magnetometer data')

        bike_string = "bike" if bike else "notbike"
        timestamp = datetime.datetime.now()
        timestamp_string = timestamp.strftime("%Y-%m-%d-%H%M%S")
        file_name = (timestamp_string + "_" +
                     location_string + "_" +
                     bike_string)
        full_directory = os.path.join(directory, file_name)
        sample_duration = int(self.__sample_duration_text_box.get())

        if record_audio:
            t1 = threading.Thread(
                target=audio_sampling.record_sample,
                args=(full_directory, sample_duration))
            t1.start()
        if record_mag:
            t2 = threading.Thread(
                target=self.__mag_sample_recorder.record_sample,
                args=(full_directory, sample_duration))
            t2.start()
        if record_video:
            t3 = threading.Thread(
                target=self.__video_recorder.record_sample,
                args=(full_directory, sample_duration))
            t3.start()
        if record_audio:
            t1.join()
        if record_mag:
            t2.join()
        if record_video:
            t3.join()

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
        try:
            com_string = self.__mag_com_port_text_box.get()
            self.__mag_sample_recorder = (
                mag_sampling.MagSampleRecorder(com_string))
            self.__mag_com_port_text_box.delete(0, tk.END)
            self.__mag_com_port_text_box.insert(
                0, "magnetometer at " + com_string + " connected")
            self.__mag_com_port_text_box.config(state=tk.DISABLED)
            self.__connect_mag_button.config(state=tk.DISABLED)
        except serial.serialutil.SerialException:
            self.__connect_mag_button.config(fg="red")

    def __configure_video_recorder(self):
        if self.__video_check_state.get():
            webcam_number = int(self.__webcam_number_text_box.get())
            self.__video_recorder = video_sampling.VideoRecorder(webcam_number)
            if not self.__video_recorder.ready:
                del self.__video_recorder
                self.__video_check_state.set(0)
        else:
            del self.__video_recorder
