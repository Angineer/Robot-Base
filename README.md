# robot-base

Code to run on the base station for my service robot. The base station will monitor and dispense inventory, as well as dispatch the robot based on user requests. See also `robot-web-server` (https://github.com/Angineer/robot-web-server) for an HTTP interface to the base.

Dependencies:
- robot-comm (https://github.com/Angineer/robot-comm)

To install, ensure you have the dependencies installed, then build with CMake:

`cd robot-mobile`\
`mkdir build && cd build`\
`cmake ..`\
`make`\
`sudo make install`

There is also a systemd service file included for running the program as a service.

See https://www.adtme.com/projects/Robot.html for more info about the overall project.
