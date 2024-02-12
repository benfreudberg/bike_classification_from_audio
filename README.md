## bike_classification_from_audio

The [Santa Cruz Mountains Trail Stewardship](https://santacruztrails.org/) organization has installed a number of trail counters in the areas that they manage. These counters are made by [Chip McClelland](https://github.com/chipmc) from [See Insights](https://seeinsights.com/). The counters use IR motion sensors to detect when a warm object (typically a person) crosses in front of them. However, they have no way to distingush between a person on foot and a person on a bike. Several methods have been proposed and some have been tested to varying degrees of success including:
* pressure sensors in the trail
* magnetic sensors
* cameras

These methods have different strengths and weaknesses in various categories including:
* cost to install/maintain
* power required to run
* precision/accuracy
* difficulty to implement software
* intrusiveness of installed system to trail users

The goal of this project is to try another method. Using a microphone to record a short sample of audio each time the motion sensor reports an event and then put that sample through a trained machine learning model to classify it as either a `bike` or `not a bike`.

See the [development plan](/docs/development_plan.md).