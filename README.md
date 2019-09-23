# ODOTS-FW
 Software Library, Open Design Orienteering Timing System embedded firmware
 
## Reading Notes

Note that this an application specific implementation of a set of drivers and
middleware designed for running the Open Design Orienteering Timing System, 
functionality may be lacking in certain areas.

Code has been written to try and maximise the chance that it can be read by 
itself. Having said that, reading the following won't hurt:

-The "Commands" section of the CR95HF datasheet.
-Any auto generated documentation.
-Final Design notes from LJones.

## Examples

ODOTSGeneric.ino contains a sketch capabable to fulfilling all ODOTS functionality requirements, with wiring specified by the ODOTS hardware design.

## Documentation
Dedicated documentation of all firmware including its interfaces with the rest of the ODOTS project can be found in the ODOTS [Specification and User Guide](https://raw.githubusercontent.com/ljones278/ODOTS-Release/master/docs/ODOTSManualAndUserGuide.pdf)

## Other Notes

## Acknowledgements

DS1337 Library cloned from [here](https://github.com/richard-clark/ds1337), from user Richard Clark. Largely unchanged apart from minor adjustments to make data interfaces easier to work with.

MRFC522 Library cloned from [here](https://github.com/miguelbalboa/rfid), from user miguelbalboa and others minor changes to how errors are reported and serial dump framing. Source code covered by the Unlicence. 


LowPower Library Cloned form [here](https://github.com/rocketscream/Low-Power), written by Lim Phang Moh and covered by the [Creative Commons Attribution-ShareAlike 3.0
* Unported License](https://creativecommons.org/licenses/by-sa/3.0/) . As a share alike license unfortunately this licenses ODOTSGeneric.ino under the same license. As the creator of the non-power saving functionality in these files I am will reissue these files without the LowPower code under a more permissive license on request. 

## Licensing

All files other than ODOTS.cpp and ODOTSGeneric.ino are covered by the Apache 2.0 License.

Development funding provided by the [Orienteering Foundation](https://www.orienteeringfoundation.org.uk/).