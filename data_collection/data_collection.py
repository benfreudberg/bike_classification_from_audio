import os
# vscode says pyaudio could not be resolved from source, but the code runs with
# no problem...
import pyaudio
import wave
import time
import datetime
import tkinter as tk
from tkinter.filedialog import askdirectory

MS_TO_RECORD = 1000


def record_sample(directory):
    # todo: look into how this works and what options there are
    AUDIO_FORMAT = pyaudio.paInt16
    SAMPLING_RATE = 44100  # Hz
    FRAMES_PER_BUFFER = 1024
    audio = pyaudio.PyAudio()
    stream = audio.open(format=AUDIO_FORMAT,
                        channels=1,
                        rate=SAMPLING_RATE,
                        input=True,
                        frames_per_buffer=FRAMES_PER_BUFFER)

    frames = []
    t_end = time.time() + MS_TO_RECORD/1000
    while time.time() < t_end:
        data = stream.read(FRAMES_PER_BUFFER)
        frames.append(data)

    stream.stop_stream()
    stream.close()
    audio.terminate()

    sound_file = wave.open(directory, "wb")
    sound_file.setnchannels(1)
    sound_file.setsampwidth(audio.get_sample_size(AUDIO_FORMAT))
    sound_file.setframerate(SAMPLING_RATE)
    sound_file.writeframes(b''.join(frames))
    sound_file.close()
    print("Audio file saved: " + directory)


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
        if self.__directory_text_box.get() == self.__DEFAULT_DIRECTORY_STRING:
            print('No output directory set')
            return
        bike_string = "bike" if bike else "notbike"
        timestamp = datetime.datetime.now()
        timestamp_string = timestamp.strftime("%Y-%m-%d-%H%M%S")
        location_string = self.__location_text_box.get()
        if location_string == self.__DEFAULT_LOCATION_STRING:
            print('No location set')
            return
        file_name = (timestamp_string + "_" +
                     location_string + "_" +
                     bike_string + ".wav")
        directory = os.path.join(self.__directory_text_box.get(), file_name)
        record_sample(directory)

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
