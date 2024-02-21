import pyaudio
import time
import wave


def record_sample(directory, ms_to_record):
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
    t_end = time.time() + ms_to_record/1000
    while time.time() < t_end:
        data = stream.read(FRAMES_PER_BUFFER)
        frames.append(data)

    stream.stop_stream()
    stream.close()
    audio.terminate()

    sound_file = wave.open(directory + ".wav", "wb")
    sound_file.setnchannels(1)
    sound_file.setsampwidth(audio.get_sample_size(AUDIO_FORMAT))
    sound_file.setframerate(SAMPLING_RATE)
    sound_file.writeframes(b''.join(frames))
    sound_file.close()
    print("Audio file saved: " + directory + ".wav")
