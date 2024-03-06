### 1.0 Proof of Concept
Initial testing/training data will be collected using a laptop. Each time any person or object passes in front of the laptop, the user will press a button to collect audio and magnetometer data as if a motion sensor had been triggered.

### 2.0 Data Collection
1. Create a program with a simple UI to record 1 second of audio and magnetometer data to files upon pressing a button.
    * Use highest sampling rate possible for flexibility later
    * There will be a box to input name of the location that the sample was taken.
    * There will be two buttons, both will record 1 second of data. But one will identify the data as `bike` and the other will identify it as `notbike`
    * The data files will be saved with names that indicate location, timestamp, and bike/notbike classification.
1. Collect data on a bike path
    * Go to 5 different locations and get at least 20 `bike` and at least 20 `notbike` samples at each.

### 3.0 Model Construction
#### 3.1 Audio Model Construction
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

#### 3.2 Magnetometer Model Construction
The initial plan for the magnetometer model for bike id is simpler. The data for x, y, and z will each be averaged and then used to calculate a representative average magnitude and direction for the sample. Then each individual data point will have its magnitude and direction calculated and compared to the sample average. If the difference between an individual point's vector and the average vector is more than a tunable cutoff for more than a tunable number of points in a row, the sample will be classified as a bike.

**This method was rejected, see the [preliminary_magnetometer_testing_report](preliminary_magnetometer_testing_report.md) for details of a better method.**

### 4.0 Transferring to Actual System and Other Thoughts
If the Proof of Concept works, we can move on to implementing the system with the real trail counter hardware. We will want to collect new training data from the actual counters once they are installed on the trails with microphone/magnetometer hardware using whatever settings and sampling rates we determined during Proof of Concept experimentation. Then we will need to train new models, however the process should be the same.

The sounds of a mountain bike on a trail in the woods are likely to be different enough from a road bike on pavement that we need an different models for paved trails vs mountain bike trails, but the same process of creating them should work.