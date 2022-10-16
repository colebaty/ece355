# import socket module
from multiprocessing import connection
from socket import *
import sys 

serverSocket = socket(AF_INET, SOCK_STREAM)

# prepare a server socket
# fill in start
serverPort = 56712 # 0xe10c in hex, which is "cole" backwards
serverSocket.bind(('', serverPort)) # bind socket to localhost:56712
serverSocket.listen(1) # listen for connections
# fill in end

while True:
    # establish the connection
    print ("Ready to serve...")
    (connectionSocket, addr) = serverSocket.accept() # accept connection from client

    try:
        """
            Receive the request from the client. The call to `split` tokenizes
            the message, using whitespace as a delimeter. The filename of the 
            requested file is the second element in the resulting list, at index
            [1].
        """
        message = connectionSocket.recv(1024).decode() # receive request
        filename = message.split()[1] # strip out filename
        f = open(filename[1:]) # system call to open file
        outputdata = f.read() # read file into memory

        # send one HTTP header line into the socket
        # fill in start
        """
            Bare-bones header line. HTTP/1.0 (aka non-persistent) because we
            explicitly close the connection at the end of each block in the
            try-catch structure.
            The header terminates with CRLF x 2 per RFC 1945. File contents
            follow.
        """
        header = "HTTP/1.0 200 OK\r\n\r\n"
        connectionSocket.send(header.encode())
        # fill in end

        # send the content of the requested file to the client
        for i in range(0, len(outputdata)): # this loop sends one byte at a time
            connectionSocket.send(outputdata[i].encode())
        connectionSocket.send("\r\n".encode()) # signal end of file

        connectionSocket.close() # close connection to client

    except IOError:
        # send response message for file not found
        # fill in start
        # send 404 message
        connectionSocket.send("HTTP/1.0 404 Not Found\r\n".encode())
        # fill in end

        # close client socket
        # fill in start
        connectionSocket.close() # close connection to client
        # fill in end

    # `finally` block needed to ensure this code is reached
    finally:
        serverSocket.close() # close socket on server
        sys.exit() # terminate the program after sending corresponding data