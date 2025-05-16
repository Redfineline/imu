#ifndef IMU_PARSER_H
#define IMU_PARSER_H

#include <cstdint>

// Struct representing a parsed IMU packet
//TODO: Look at naybe including the header data here
struct IMUPacket {
    uint32_t packetCount;
    float xRate;
    float yRate;
    float zRate;
};

// Reads a complete IMU packet from the UART 
// Returns true on success, false on failure or incomplete data
bool readPacket(int fd, IMUPacket &packet);

// Configures and opens a UART device (e.g., /dev/tty1) at 921600 baud
// Returns the file descriptor on success, or -1 on failure
int configureUART(const char* device);

#endif // IMU_PARSER_H

