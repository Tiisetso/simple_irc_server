import socket
import sys

HOST: str = "127.0.0.1"
PORT: int = 16676

def check(reply: str, expected: str) -> None:
    if expected not in reply:
        print("TEST FAILED")
        print("Expected:", expected)
        print("Received:")
        print(reply)
        sys.exit(1)

def server_reply(client: socket.socket) -> str:
    message_bytes: bytes = client.recv(4096)
    message_str: str = message_bytes.decode("utf-8")
    return message_str


def send_command(client: socket.socket, command: str) -> None:
    client.sendall((command + "\r\n").encode("utf-8"))


def register_client(client:socket.socket, client_name: str) -> None:
        send_command(client, "PASS testpass\r\nNICK " + client_name + "\r\nUSER " + client_name + " 0 * :" + client_name)


with (socket.socket(socket.AF_INET, socket.SOCK_STREAM) as yuan,
     socket.socket(socket.AF_INET, socket.SOCK_STREAM) as peter,
     socket.socket(socket.AF_INET, socket.SOCK_STREAM) as titi,
     socket.socket(socket.AF_INET, socket.SOCK_STREAM) as ghost):

    yuan.connect((HOST, PORT))
    register_client(yuan, "yuan")
    yuan_reply = server_reply(yuan)

    titi.connect((HOST, PORT))
    register_client(titi, "titi")
    titi_reply = server_reply(titi)

    peter.connect((HOST, PORT))
    register_client(peter, "peter")
    peter_reply = server_reply(peter)

    ghost.connect((HOST, PORT))
    register_client(ghost, "ghost")
    ghost_reply = server_reply(ghost)

    # Yuan is now the operator
    send_command(yuan, "JOIN #test11")
    yuan_reply += server_reply(yuan)

   # Titi joins before i mode is set
    send_command(titi, "JOIN #test11")
    titi_reply += server_reply(titi)
    yuan_reply += server_reply(yuan)

    # Yuan sets channel modes
    send_command(yuan, "MODE #test11 +iklo secret 3 titi")
    yuan_reply += server_reply(yuan)
    titi_reply += server_reply(titi)

    # Check the modes were stored
    send_command(yuan, "MODE #test11")
    yuan_reply += server_reply(yuan)

    check(yuan_reply, "324 yuan #test11 ikl secret 3")

    # +i blocks Peter
    send_command(peter, "JOIN #test11 secret")
    peter_reply += server_reply(peter)

    check(peter_reply, "473 peter #test11")

    # Titi removed i mode
    send_command(titi, "MODE #test11 -i")
    titi_reply += server_reply(titi)
    yuan_reply += server_reply(yuan)

    check(titi_reply, "MODE #test11 -i")

    # Peter tries the wrong key
    send_command(peter, "JOIN #test11 wrong")
    peter_reply += server_reply(peter)

    check(peter_reply, "475 peter #test11")

    # Peter tries the right key
    send_command(peter, "JOIN #test11 secret")
    peter_reply += server_reply(peter)
    yuan_reply += server_reply(yuan)
    titi_reply += server_reply(titi)

    check(peter_reply, "353 peter")

    # Ghost tries to join the full channel
    send_command(ghost, "JOIN #test11 secret")
    ghost_reply += server_reply(ghost)

    check(ghost_reply, "471 ghost")

    # Titi removes multiple modes
    send_command(titi, "MODE #test11 -iolk yuan")
    titi_reply += server_reply(titi)
    yuan_reply += server_reply(yuan)
    peter_reply += server_reply(peter)

    # Yuan is no longer an operator
    send_command(yuan, "MODE #test11 +o yuan")
    yuan_reply += server_reply(yuan)

    check(yuan_reply, "482 yuan")

    print("MODE smoke test passed")