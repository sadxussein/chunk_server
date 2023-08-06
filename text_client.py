# TODO: check sending password, encoding could be redundant
import socket
import hashlib
import threading
import time
import msvcrt
import sys


class Server:
    server_host_ip = '172.27.191.126'
    login_server_tcp_port = 40000
    game_server_tcp_port = 40001
    chunk_server_tcp_port = 40002
    chunk_server_udp_port = 40003

    @staticmethod
    def get_server_host_ip():
        return Server.server_host_ip

    @staticmethod
    def get_login_server_tcp_port():
        return Server.login_server_tcp_port

    @staticmethod
    def get_game_server_tcp_port():
        return Server.game_server_tcp_port

    @staticmethod
    def get_chunk_server_tcp_port():
        return Server.chunk_server_tcp_port

    @staticmethod
    def get_chunk_server_udp_port():
        return Server.chunk_server_udp_port


# simple login logic, TCP only
def login(sock):
    username = input("Please enter username: ")
    password = input("Please enter password: ")
    # hashing password
    hashed_password = hashlib.sha256(password.encode())
    # send them to the server
    sock.send(username.encode())
    sock.send(hashed_password.hexdigest().encode())
    # get response from server
    response = sock.recv(1500).decode()
    print(response)
    # if user/password are ok and server accepted them
    if response[0] == 'W':
        # and start counting heartbeats
        thread = threading.Thread(target=heartbeat, args=(sock,))
        thread.start()
    # if not start login procedure again
    else:
        login(sock)

def game(sock):
    character = input("Please enter character index: ")
    sock.send(character.encode())
    response = sock.recv(1500).decode()
    print(response)


# ping server every 5 seconds
def heartbeat(sock):
    while True:
        sock.send("1".encode())
        time.sleep(5)


# chunk operates player movement and local chat
def chunk(sock_udp, sock_tcp):
    movement_thread = threading.Thread(target=movement, args=(sock_udp, sock_tcp))
    movement_thread.start()


# chat function
def chat(sock_tcp):
    while True:
        # get user message
        message = input("Please enter message: ")
        # send it to the server
        sock_tcp.send(message.encode())
        # get server response
        response = sock_tcp.recv(1500).decode()
        print("Server response:", response)
        # if no messages from client close loop
        if not message:
            break


# movement function
def movement(sock_udp, sock_tcp):
    while True:
        # reading one symbol at a time and send it to server
        if msvcrt.kbhit():
            message = msvcrt.getch().decode('utf-8')
            remote = (Server.get_server_host_ip(), Server.get_chunk_server_udp_port())
            sock_udp.sendto(message.encode(), remote)
            # temporarily reading data from server
            data, server_address = sock_udp.recvfrom(1500)
            # and printing it on client side
            print(data.decode())
            # open chat on 'y'
            if message == 'y':
                chat_thread = threading.Thread(target=chat, args=(sock_tcp,))
                chat_thread.start()
                chat_thread.join()
            elif message == 'q':
                break


# 1. Connect to login server and check credentials
# 2. Connect to game server to pick character
# 3. Main loop, after login, chunk operates user movement and chat
# 3. Close all connections


# main function
if __name__ == '__main__':

    print(Server.get_server_host_ip(),
          Server.get_login_server_tcp_port(),
          Server.get_game_server_tcp_port(),
          Server.get_chunk_server_tcp_port(),
          Server.get_chunk_server_udp_port())

    # 1. Connect to login server and check credentials
    login_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    login_tcp_connection = (Server.get_server_host_ip(), Server.get_login_server_tcp_port())
    login_socket.connect(login_tcp_connection)
    login(login_socket)

    # 2. Connect to game server to pick character
    game_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    game_tcp_connection = (Server.get_server_host_ip(), Server.get_game_server_tcp_port())
    game_socket.connect(game_tcp_connection)
    game(game_socket)

    # 3. Main loop, after login, chunk operates user movement and chat
    chunk_socket_tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    chunk_tcp_connection = (Server.get_server_host_ip(), Server.get_chunk_server_tcp_port())
    chunk_socket_tcp.connect(chunk_tcp_connection)
    chunk_socket_udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    chunk(chunk_socket_udp, chunk_socket_tcp)

    # 4. Close all connections
    # login_socket.close()
    # chunk_socket_tcp.close()
    # chunk_socket_udp.close()
