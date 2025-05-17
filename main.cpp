#include <thread> //For thred
#include <iostream> // For std::cerr
#include <chrono> //For time to sleep in ms
#include <sys/socket.h>// For socket(), sendto()
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h> //For inet_pton
#include "imu_parser.h" // For IMUPacket, readPacket, configureUART
#define BROADCAST_PORT 5000

void imuThread() {
    
    //Opens the UART device /dev/tty1 using the previously defined function
    //If failed, logs an error and exits
    int fd = configureUART("/dev/pts/10"); //# Replace with your actual virtual or real serial port name. Reference README
    if (fd < 0) {
        std::cout << "Failed to open UART\n";
        return;
    }

    //Creates a UDP socket using IPv4 (AF_INET)
    //SOCK_DGRAM = UDP
    //If it fails, exits
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    /*
    Sets up a destination socket address for the UDP packet:
        AF_INET: IPv4
        htons(5000): Port 5000 in network byte order
        inet_pton: Sends to localhost (127.0.0.1)
    
    */ 
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(BROADCAST_PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    while (true) {
        IMUPacket packet;
        //Receive a valid packet, and if it is good resend it back onto the localhost
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

