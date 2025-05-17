#include "imu_parser.h"
#include <fcntl.h> // For open()
#include <termios.h> // For configuring UART (termios structs)
#include <unistd.h> // For read(), close(), etc.
#include <arpa/inet.h> // For ntohl() (network-to-host long)
#include <cstring> // For memcpy(), memset(), memcmp()
#include <iostream> // For std::cerr and std::perror






/**
@brief Reads a complete IMU packet from the UART 

@param fd fd is a file descriptor for the UART port.
@param packet is the output struct to fill in with parsed data
@return true if a complete, valid packet is read; false otherwise
*/
bool readPacket(int fd, IMUPacket &packet) {

    //Expected 4-byte header that marks the start of a valid packet.
    const uint8_t HEADER[4] = {0x7F, 0xF0, 0x1C, 0xAF};
    
    //Temporary buffer to store the whole 20-byte packet
    uint8_t buffer[20];

    while (true) {
        uint8_t byte;
        //Reads one byte from UART.
        //If read() fails, return false.
        if (read(fd, &byte, 1) != 1) return false;
        //f it matches the first byte of the header, proceed to read the next 3
        if (byte == HEADER[0]) {
            buffer[0] = byte;
            //Read the remaining 3 header bytes into buffer[1..3]
            if (read(fd, &buffer[1], 3) != 3) continue;
            //Checks if the first 4 bytes match the expected header
            if (memcmp(buffer, HEADER, 4) == 0) {
                //Read the remaining 16 bytes: 4 bytes for packetCount, 12 for 3 floats
                if (read(fd, &buffer[4], 16) != 16) return false;

                // Packet count (uint32_t)
                //Extracts 4 bytes from the buffer, converts from network byte order (big-endian) to host order
                uint32_t netPacketCount;
                std::memcpy(&netPacketCount, &buffer[4], 4);
                packet.packetCount = ntohl(netPacketCount);

                // X, Y, Z float values
                /*
                Iterates over the 3 float fields: X, Y, and Z
                Extracts 4 bytes at a time
                Converts each from network byte order
                Reinterprets the uint32_t bit pattern as a float using memcpy
                
                */
                for (int i = 0; i < 3; ++i) {
                    uint32_t netFloat;
                    std::memcpy(&netFloat, &buffer[8 + i * 4], 4);
                    netFloat = ntohl(netFloat);
                    std::memcpy(((float*)&packet.xRate) + i, &netFloat, 4);
                }
                std::cout << "Parsed Packet:\n";
                std::cout << "  Packet Count: " << packet.packetCount << "\n";
                std::cout << "  X Rate (rps): " << packet.xRate << "\n";
                std::cout << "  Y Rate (rps): " << packet.yRate << "\n";
                std::cout << "  Z Rate (rps): " << packet.zRate << "\n";
                std::cout << "-------------------------\n";
                return true;
            }
        }
    }
}

/**
@brief Reads a complete IMU packet from the UART 

@param device takes a string like "/dev/tty1" and 
@returns a UART file descriptor
*/
int configureUART(const char* device) {
    //Opens the UART in read-only, non-controlling terminal mode
    int fd = open(device, O_RDONLY | O_NOCTTY);
    //If open fails, log the error and return failure
    if (fd < 0) {
        perror("open");
        return -1;
    }
    //Declare a termios struct to hold serial port settings
    struct termios tty{};
    
    
    //Get current UART settings into tty
    //On failure, log and close the file
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }
    //Set input and output baud rate to 921600
    cfsetispeed(&tty, B921600);
    cfsetospeed(&tty, B921600);

    //CLOCAL: Ignore modem control lines
    //CREAD: Enable receiver
    tty.c_cflag |= (CLOCAL | CREAD);
   

    //Apply the modified settings immediately
    //On failure, close the file and return
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }
    //Return the configured file descriptor on success
    return fd;
}

