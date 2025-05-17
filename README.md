# imu
To build the C++ executable use the gollowing command:
"g++ main.cpp imu_parser.cpp -o imu_parser"



To run the Python imu_simulator that sends mock IMU packets over UART and reads back the broadcast data follow these steps:
1. Use a loopback serial port via socat
    In Linux terminal do the folowing (For Ubuntu):
        # sudo apt install socat
        # socat -d -d pty,raw,echo=0 pty,raw,echo=0

        Note: Following output should look like this:
        PTY is /dev/pts/3 <--use this as you serial out in imu.simulator.py
        PTY is /dev/pts/4 <--use this as you tty in configureUART(..) in the main 

2. Build and run the c++ executable
    In linux terminal do the following (For Ubuntu)
        # ./imu_parser  # This reads from /dev/tty1 (or /dev/pts/4 in test setup)

3. Run the python simulator script
    In linux terminal do the following (For Ubuntu)
        # python3 imu_simulator.py
