# image-display
"Project Memory" - An Arduino project for displaying images on an LCD display
This project uses an Arduino Uno with an LCD screen shield and an SD card reader to display the images stored on the SD card to the user. These images must be formatted for use on the screen before they are placed on the SD card. These images can be formatted using a NodeJS script.

This project was completed as a gift. As such, this project's completion was time sensitive. However, there are a few key features that could be revised were this project to be revisited in the future.
- A latching switch could be added that, when switched on, will freeze the screen on the given image
- If an Arduino Mega were used (with the Mega's LCD shield), a free analogue pin could be used to generate a random seed for the Arduino to use, thereby making the image selection truly random
- The project could be redesigned to be used with an Arduino Nano to create a smaller form factor
