### 1.0 Proof of Concept
Data will be collected using a laptop on the Monterey bike path. Ideally, with a similar weatherproof microphone as would be used by the real system. Each time any person or object passes in front of the laptop, the user will press a button to collect audio data as if a motion sensor had been triggered.

#### 1.1 Data Collection
1. Create a program with a simple UI to record 1 second of audio to a file upon pressing a button.
    * Use highest sampling rate possible for flexibility later
    * There will be a box to input name of the location that the recording was made in.
    * There will be two buttons, both will record 1 second of audio. But one will identify the audio as `bike` and the other will identify it as `notbike`
    * The audio file will be saved with a name that indicates location, timestamp, and bike/notbike classification.
1. Collect data on Monterey bike path
    * Go to 5 different locations and get at least 20 `bike` and at least 20 `notbike` samples at each.

#### 1.2 Model Construction
1. Data pre-processing
    1. select first x ms of data (maybe somewhere in the 100 ms to 300 ms range). This is the sampling period T expressed in seconds.
    1. optionally resample raw data to create a lower sample rate copy as a lower sample rate would be beneficial when running the model on the real power-constrained devices later
    1. run discrete FFT
    1. calculate one-sided magnitude or absolute values from raw output
    1. consider some other scaling (PSD?) and/or binning of this data. The raw number of output bins will be `(Fs * T) / 2) + 1` and each bin will be h Hz wide where `h = 1/T` and they will span the range of 0 to Fs/2 Hz.
1. Assemble data in pandas data frame
    * each row contains data from one sample. One column for each bin of pre-processed fft data (X) plus one binary column for `bike` (y)
1. Split data into 4 folds for cross validation using the `bike` classification to stratify so that each fold has a similar ratio of `bike` to `notbike` as the full original data.
1. Run cross validated grid fitting k-NearsetNeighbor model for range of k's for each combination of 3 folds training data, 1 fold test data.
1. Find k value K that performs best when averaged across all 4 fold runs and then train new K-NearestNeighbor model using all data as training data. This is the final model to be used when classifying new samples as `bike` or `notbike`.

#### 1.3 Alternative Model Construction
May also try deep learning or some other models using different data pre-processing.

### 2.0 Transferring to Actual System
If the Proof of Concept works, we can move on to implementing the system with the real trail counter hardware.

1. Collect new training data from the actual counters once they are installed on the trails with microphone hardware using lowest reasonable sampling rate determined by Proof of Concept experimentation.
1. Follow the steps from 2.2 using what we learned about sampling rate, fft scaling, and binning to train a new model.
1. Implement that model in the trail counter firmware.

 The sounds of a mountain bike on a trail in the woods are likely to be different enough from a road bike on pavement that we need an entirely new model from the one created for the Proof of Concept, but the same process of creating it should work. It may also be that we could train a single model from both sets of data that would be useful in more types of trail environments - TBD.

 Another issue we may find is that there is enough difference between a bike going fast downhill and one going slowly uphill that we need separate classifications for each that we later merge together. It would be prudent to collect this information with the training data on the real trails in case it is needed. Perhaps we need `notbike`, `bikepedaling`, and `bikecoasting` categories.