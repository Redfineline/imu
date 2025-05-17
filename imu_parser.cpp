#include "imu_parser.h"
#include <fcntl.h> // For open()
#include <termios.h> // For configuring UART (termios structs)
#include <unistd.h> // For read(), close(), etc.
#include <arpa/inet.h> // For ntohl() (network-to-host long)
#include <cstring> // For memcpy(), memset(), memcmp()
#include <iostream> // For std::cerr and std::perror





//fd is a file descriptor for the UART port.
//packet is the output struct to fill in with parsed data.
//Returns true if a complete, valid packet is read; false otherwise.
bool readPacket(int fd, IMUPacket &packet) {

    //Expected 4-byte header that marks the start of a valid packet.
    const uint8_t HEADER[4] = {0x7F, 0xF0, 0x1C, 0xAF};
    
    //Temporary buffer to store the whole 20-byte packet
    uint8_t buffer[20];

    while (true) {
        uint8_t byte;
        if (read(fd, &byte, 1) != 1) return false;

        if (byte == HEADER[0]) {
            buffer[0] = byte;
            if (read(fd, &buffer[1], 3) != 3) continue;

            if (memcmp(buffer, HEADER, 4) == 0) {
                if (read(fd, &buffer[4], 16) != 16) return false;

                // Packet count (uint32_t)
                uint32_t netPacketCount;
                std::memcpy(&netPacketCount, &buffer[4], 4);
                packet.packetCount = ntohl(netPacketCount);

                // X, Y, Z float values
                for (int i = 0; i < 3; ++i) {
                    uint32_t netFloat;
                    std::memcpy(&netFloat, &buffer[8 + i * 4], 4);
                    netFloat = ntohl(netFloat);
                    std::memcpy(((float*)&packet.xRate) + i, &netFloat, 4);
                }
                  std::cout << "Parsed Packet:\n";
                std::cout << "  Packet Count: " << packet.packetCount << "\n";
                std::cout << "  X Rate (rdps): " << packet.xRate << "\n";
                std::cout << "  Y Rate (rdps): " << packet.yRate << "\n";
                std::cout << "  Z Rate (rdps): " << packet.zRate << "\n";
                std::cout << "-------------------------\n";
                return true;
            }
        }
    }
}

int configureUART(const char* device) {
    int fd = open(device, O_RDONLY | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct termios tty{};
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    cfsetispeed(&tty, B921600);
    cfsetospeed(&tty, B921600);
    tty.c_cflag |= (CLOCAL | CREAD);
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}

