from pyniryo2 import *
import socket

HOST = ''  # Symbolic name meaning all available interfaces
PORT = 8888  # Arbitrary non-privileged port

robot = NiryoRobot("169.254.200.200") # bind to robot
robot.arm.calibrate_auto() # calibrate robot if needed

# Create a socket
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    # Bind the socket to a specific address and port
    s.bind((HOST, PORT))
    # Listen for incoming connections
    s.listen(1)
    # Accept incoming connections
    conn, addr = s.accept()
    print(f'Connected by {addr}')
    print(f'Listening on port {PORT}')
    while True:
        # Receive data from the client
        data = conn.recv(2048)
        if data:
            # Decode the data and call the function
            joints = list(map(float, data.decode().split(',')))
            print(joints)
            robot.arm.move_joints(joints[:6])
            if(joints[6] == 1):
                robot.tool.open_gripper()
            else:
                robot.tool.close_gripper()
            # Encode the result and send it back to the client
            conn.sendall(str(200).encode())
