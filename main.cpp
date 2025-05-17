#include <thread>
#include <iostream>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "imu_parser.h"
#define BROADCAST_PORT 5000

void imuThread() {
    int fd = configureUART("/dev/pts/10");
    if (fd < 0) {
        std::cout << "Failed to open UART\n";
        return;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(BROADCAST_PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    while (true) {
        IMUPacket packet;
        if (readPacket(fd, packet)) {
            sendto(sock, &packet, sizeof(packet), 0, (sockaddr*)&addr, sizeof(addr));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
}

int main() {
    std::thread t(imuThread);
    t.join();
    return 0;
}

