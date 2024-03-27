import cv2
import time


FRAME_WIDTH = 1920
FRAME_HEIGHT = 1080


class VideoRecorder:
    def __init__(self, webcam_number=0):
        self.ready = False
        # Open the webcam (change the index if you have multiple cameras)
        self.__cap = cv2.VideoCapture(webcam_number)

        # Check if the webcam is opened correctly
        if not self.__cap.isOpened():
            print("Error: Could not open webcam")
            return

        # Get the frame width and height for the webcam
        self.__cap.set(cv2.CAP_PROP_FRAME_WIDTH, FRAME_WIDTH)
        self.__cap.set(cv2.CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT)

        # Need to do this here otherwise there is a delay when the actual video
        # capture begins
        self.__cap.read()

        self.ready = True
        print(f'VideoRecorder for webcam {webcam_number} ready')

    def __del__(self):
        self.__cap.release()

    def record_sample(self, output_file, ms_to_record):
        output_file += '.avi'

        # Define the codec and create a VideoWriter object
        fourcc = cv2.VideoWriter_fourcc(*'XVID')
        self.__out = cv2.VideoWriter(output_file, fourcc, 30.0,
                                     (FRAME_WIDTH, FRAME_HEIGHT))

        # Capture frames and write to the output video file until the specified
        # duration
        t_end = time.time() + ms_to_record/1000
        while time.time() < t_end:
            ret, frame = self.__cap.read()  # Read a frame from the webcam

            if not ret:
                print("Error: Failed to capture frame")
                break

            self.__out.write(frame)  # Write the frame to the output video file

        # Release the VideoWriter
        self.__out.release()
        print("Video file saved: " + output_file)


def main():
    video_recorder = VideoRecorder()
    ms_to_record = 5000  # Duration in ms
    output_file = "captured_clip"  # Output video file name
    input("Press Enter to continue...")
    video_recorder.record_sample(output_file, ms_to_record)
    del video_recorder


if __name__ == "__main__":
    main()
