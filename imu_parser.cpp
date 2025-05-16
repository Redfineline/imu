#include "imu_parser.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

bool readPacket(int fd, IMUPacket &packet) {
    const uint8_t HEADER[4] = {0x7F, 0xF0, 0x1C, 0xAF};
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

