## bike_identification

The [Santa Cruz Mountains Trail Stewardship](https://santacruztrails.org/) organization has installed a number of trail counters in the areas that they manage. These counters are made by [Chip McClelland](https://github.com/chipmc) from [See Insights](https://seeinsights.com/). The counters use IR motion sensors to detect when a warm object (typically a person) crosses in front of them. However, they have no way to distingush between a person on foot and a person on a bike. Several methods have been proposed and some have been tested to varying degrees of success including:
* pressure sensors in the trail
* magnetic sensors
* cameras
* microphones
* inductive loops

These methods have different strengths and weaknesses in various categories including:
* cost to install/maintain
* power required to run
* precision/accuracy
* intrusiveness of installed system to trail users

The goal of this project is do further research into two of these options: magnetic sensors and microphones. A sensitive magnetometer can detect the steel in a passing bike's drivetrain, though the useful range of this method may be too short. A microphone can record a short sample of audio each time the motion sensor reports an event and then put that sample through a trained machine learning model to classify it as either a `bike` or `not a bike`.

Documents:  
* [development plan](/docs/development_plan.md)  
* [preliminary magnetometer testing report](/docs/preliminary_magnetometer_testing_plan.md)