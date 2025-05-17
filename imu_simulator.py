import struct
import time
import socket
import serial
import random

def create_imu_packet(packet_count, x, y, z):
    header = bytes([0x7F, 0xF0, 0x1C, 0xAF])
    packet_count_net = struct.pack("!I", packet_count)
    x_net = struct.pack("!f", x)
    y_net = struct.pack("!f", y)
    z_net = struct.pack("!f", z)
    return header + packet_count_net + x_net + y_net + z_net

def send_packets(serial_port):
    ser = serial.Serial(serial_port, 921600)
    packet_count = 0
    while True:
        x = random.uniform(-250, 250)
        y = random.uniform(-250, 250)
        z = random.uniform(-250, 250)
        packet = create_imu_packet(packet_count, x, y, z)
        ser.write(packet)
        packet_count += 1
        time.sleep(0.08)  # simulate 80ms intervals

def receive_broadcast(port=5000):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("127.0.0.1", port))
    print("Listening for broadcasted IMU packets...")
    while True:
        data, _ = sock.recvfrom(1024)
        if len(data) == 16:
            pkt_count, x, y, z = struct.unpack("=Ifff", data)
            print(f"Received: Count={pkt_count}, X={x:.2f}, Y={y:.2f}, Z={z:.2f}")

if __name__ == "__main__":
    import threading

    # Replace with your actual virtual or real serial port name
    serial_out = "/dev/pts/9"  # Simulator sends here
    broadcast_thread = threading.Thread(target=receive_broadcast, daemon=True)
    broadcast_thread.start()

    send_packets(serial_out)

