# IMU
This project implements a complete system for parsing and broadcasting data from an Inertial Measurement Unit (IMU) over a high-speed UART connection. A C++ program running on Linux reads IMU packets formatted in a specific binary structure, extracts angular rate data (X, Y, Z) and a packet counter, and broadcasts the parsed results as UDP packets on the localhost network every 80 milliseconds. A Python-based simulator is included to generate and transmit mock IMU data, allowing the system to be tested end-to-end without real hardware. This design supports integration in resource-constrained, multi-threaded environments.


## Installation
1.  Clone the repository: `git clone [repository URL]`
## Usage

To build the c++ executable use the gollowing command:

    "g++ main.cpp imu_parser.cpp -o imu_parser"


To run the Python imu_simulator that sends mock IMU packets over UART and reads back the broadcast data follow these steps:
1. Use a loopback serial port via socat
    * In Linux terminal do the folowing (For Ubuntu):
        
            sudo apt install socat

            socat -d -d pty,raw,echo=0 pty,raw,echo=0

        Note: Following output should look like this:
        
            PTY is /dev/pts/3 <--use this as you serial out in imu.simulator.py
        
            PTY is /dev/pts/4 <--use this as you tty in configureUART(..) in the main 

2. Build and run the c++ executable
    * In linux terminal do the following (For Ubuntu):
        
             ./imu_parser  # This reads from /dev/tty1 (or /dev/pts/4 in test setup)

3. Run the python simulator script
   * In linux terminal do the following (For Ubuntu):
            
            python3 imu_simulator.py
