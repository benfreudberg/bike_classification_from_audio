## Proof of Concept Testing Summary

While we have not yet developed excellent classification models for magnetometer data or audio data (82% and 80% accuracy so far), we have shown that either of these approaches can be used to detect bikes. We have also learned about some of the strengths and weaknesses of each and are well positioned to develop a good model in the future taking into account certain limitations of each.

### Pros and Cons

**Pros of magnetometer approach**:
* Computitionally cheap model
* The model is simple and does not require a ton of training data

**Cons of magnetometer approach**:
* Very sensitive to environment
    * sensor must be kept away from power lines or other sources of magnetic interferance - some of which may not be obvious (eg. buried objects)
    * sensor must be kept perfectly still
* Limited detection range (~1.5m) means sensors may need to be installed under the trail. Perhaps multiple sensors may be required for a wider trail
* Certain situations may cause unavoidable false positives or false negatives
    * steel-toed boots, weighted vests, horse shoes etc may cause a false positive bike detection
    * alternative bike construction (belt driven, etc) may mean there is less steel and may reduce detection range or cause a false negative

___

**Pros of audio approach**:
* no worries about detection range or sensor motion makes for a very simple installation (right next to the motion sensor would be fine)
* very few unavoidable causes for inaccuracy

**Cons of audio approach**:
* the model is more complex so more training data is required for it to work well. Especially to catch edge cases like wind noise or aircraft flying over. Additionally, bikes with different tires, drivetrains, or ridden at different speeds over different surfaces will have different sound profiles and we'll need many data samples of each to build a robust model.
* the model is more computationally expensive, but I think it is unlikely to be a significant burden even on a low power microcontroller (testing required to confirm)

### Next steps

* Coordinate with Chip to see about integrating with existing hardware.
* Collect more training data. It would be good to automate some of this with a camera trap type device if possible.