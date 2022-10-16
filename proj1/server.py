# import socket module
from multiprocessing import connection
from socket import *
import sys 

serverSocket = socket(AF_INET, SOCK_STREAM)

# prepare a server socket
# fill in start
serverPort = 56712
serverSocket.bind(('', serverPort))
serverSocket.listen(1)
# fill in end

while True:
    # establish the connection
    print ("Ready to serve...")
    (connectionSocket, addr) = serverSocket.accept()
    try:
        message = connectionSocket.recv(1024).decode()
        filename = message.split()[1]
        f = open(filename[1:])
        outputdata = f.read()
        # send one HTTP header line into the socket
        # fill in start
        header = "HTTP/1.0 200 OK\r\n\r\n"
        connectionSocket.send(header.encode())
        # fill in end

        # send the content of the requested file to the client
        for i in range(0, len(outputdata)):
            connectionSocket.send(outputdata[i].encode())
        connectionSocket.send("\r\n".encode())

        connectionSocket.close()

    except IOError:
        # send response message for file not found
        # fill in start
        connectionSocket.send("HTTP/1.0 404 Not Found\r\n".encode())
        # fill in end

        # close client socket
        # fill in start
        connectionSocket.close()
        # fill in end

    finally:
        serverSocket.close()
        sys.exit() # terminate the program after sending corresponding data