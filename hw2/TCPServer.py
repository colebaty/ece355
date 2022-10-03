from socket import *

serverPort = 1337
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind(('',serverPort))
serverSocket.listen(1)
print('The server is ready to receive')

while True:
    connectionSocket, addr = serverSocket.accept()
    print("connection from {0}".format(addr))
    sentence = connectionSocket.recv(2048)
    print(sentence.decode())
    print()
    connectionSocket.close()
