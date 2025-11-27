import socket, threading, time, os
from datetime import datetime

SERVER_HOST = "127.0.0.1"
PORT = 8000


class HTTPServer:

    def __init__(self, host, port):

        self.host = host
        self.port = port
        self.connections = [] # Defines connections handled by threads
        self.server_socket = None # The socket of server
        self.server_log = None # The log file, recording the history access records

    def receive(self, connection, address):
        # set timeout
        timeout = 10
        connection.settimeout(timeout)

        while True: # while loop: enables server to unceasingly try to receive req from client

            try:
                request = connection.recv(1024).decode() # max: read 1024 bytes
                if (request == "") or (request is None):
                    time.sleep(1) # sleep(1): waits for 1 second, and continue to rcv the req
                    continue

                print("Client " + str(address) + " requested: \n\n" + request)
                # check if keep alive?
                keep_alive = False
                for line in request.split("\n"):
                    if ("Connection: keep-alive" in line) or ("Connection: Keep-Alive" in line):
                        keep_alive = True
                        break
                # split the req into method & url
                method = request.split("\n")[0].split(" ")[0]
                url = request.split("\n")[0].split(" ")[1]

                # According to the method name, here execute different operation
                if (method == "GET"):
                    self.response_GET(connection, request, url, address)
                elif (method == "HEAD"):
                    self.response_HEAD(connection, request, url, address)
                else:
                    self.handle_bad_request(connection, url, address)


                #If the connection is not keep-alive, the server immediately closes the connection
                if (keep_alive == False):
                    connection.close()
                    print("\n")
                    print(("Client" + str(address) + " disconnected: no keep-alive").center(81))
                    break

            except socket.timeout:
                #If the connection is keep-alive and the timeout is reached, the server closes the connection
                connection.close()
                print("\n")
                print(("Client " + str(address) + " disconnected: keep-alive timeout").center(81))

                break

            except KeyboardInterrupt:
                # If interrupt happens, the server will take the connection out of the threads.
                self.connections.remove(connection)
                break

    def response_GET(self, connection, request, url, address):
        # This is to send response to GET req, which can check if file exists, sending response. If file not exist: 404 error response
        if url == "/":
            url = "/index.html"

        file_type = self.get_file_type(url)
        try:
            if (file_type == "text/html"):
                file = open("htdocs" + url, "r")
                contents = file.read()
                file.close()
            else:
                file = open("htdocs" + url, "rb")
                contents = file.read()
                file.close()
            # This is to check if file has been modified or not.
            # If modified, sends Ok response, otherwise, send 304 not Modified response
            if "If-Modified-Since" in request:

                for line in request.split("\n"):
                    if "If-Modified-Since" in line:
                        check_time = line[19:48]
                        break

                # change time format
                check_time = datetime.strptime(check_time, '%a, %d %b %Y %H:%M:%S %Z')
                file_time = datetime.fromtimestamp(os.path.getmtime("htdocs" + url))

                if (file_time <= check_time):
                    # Not Modified Case
                    response_parts = ["HTTP/1.1 304 Not Modified\r\n",
                                      "Last-Modified: " + str(
                                          datetime.fromtimestamp(os.path.getmtime("htdocs" + url))) + "\r\n",
                                      "Content-Length: " + str(len(contents)) + "\r\n",
                                      "Content-Type: " + file_type + "\r\n\r\n"]


                    response = "".join(response_parts)
                    connection.sendall(response.encode())

                    print("Server response:\n")
                    print(response)
                    self.write_log(address, url, "304 Not Modified")


                else:
                    #The server sends a 200 OK response if the file is modified
                    response_parts = ["HTTP/1.1 200 OK\r\n",
                                      "Last-Modified: " + str(
                                          datetime.fromtimestamp(os.path.getmtime("htdocs" + url))) + "\r\n",
                                      "Content-Length: " + str(len(contents)) + "\r\n",
                                      "Content-Type: " + file_type + "\r\n\r\n"]
                    response = "".join(response_parts)
                    connection.sendall(response.encode())

                    if (file_type == "text/html"):
                        connection.sendall(contents.encode())
                    else:
                        connection.sendall(contents)

                    print("Server response:\n")
                    print(response)
                    self.write_log(address, url, "200 OK")

            else:
                # The server sends a 200 OK response if there is no "If-Modified-Since" header
                response_parts = ["HTTP/1.1 200 OK\r\n",
                                  "Last-Modified: " + str(
                                      datetime.fromtimestamp(os.path.getmtime("htdocs" + url))) + "\r\n",
                                  "Content-Length: " + str(len(contents)) + "\r\n",
                                  "Content-Type: " + file_type + "\r\n\r\n"]
                response = "".join(response_parts)
                connection.sendall(response.encode())

                if (file_type == "text/html"):
                    connection.sendall(contents.encode())
                else:
                    connection.sendall(contents)

                print("Server response:\n")
                print(response)
                self.write_log(address, url, "200 OK")

        except FileNotFoundError: # if not found: 404
            self.handle_file_not_found(connection, url, address)

    def response_HEAD(self, connection, request, url, address):
        # This is to send response to client, almost same to the GET req,
        # However, without contents of the file
        file_type = self.get_file_type(url)
        if url == "/": # Default case: index.html, see htdocs
            url = "/index.html"
        try:

            if (file_type == "text/html"):
                file = open("htdocs" + url, "r")
                contents = file.read()
                file.close()
            else:
                file = open("htdocs" + url, "rb")
                contents = file.read()
                file.close()

            if ("If-Modified-Since" in request):

                for line in request.split("\n"):
                    if "If-Modified-Since" in line:
                        check_time = line[19:48]
                        break

                check_time = datetime.strptime(check_time, '%a, %d %b %Y %H:%M:%S %Z')
                file_time = datetime.fromtimestamp(os.path.getmtime("htdocs" + url))

                if (file_time <= check_time):

                    response_parts = ["HTTP/1.1 304 Not Modified\r\n",
                                      "Last-Modified: " + str(
                                          datetime.fromtimestamp(os.path.getmtime("htdocs" + url))) + "\r\n",
                                      "Content-Length: " + str(len(contents)) + "\r\n",
                                      "Content-Type: " + file_type + "\r\n\r\n"]
                    response = "".join(response_parts)
                    connection.sendall(response.encode())

                    print("Server response:\n")
                    print(response)
                    self.write_log(address, url, "304 Not Modified")

                else:

                    response_parts = ["HTTP/1.1 200 OK\r\n","Last-Modified: " + str(datetime.fromtimestamp(os.path.getmtime("htdocs" + url))) + "\r\n","Content-Length: " + str(len(contents)) + "\r\n","Content-Type: " + file_type + "\r\n\r\n"]
                    response = "".join(response_parts)
                    connection.sendall(response.encode())
                    print("Server response:\n")
                    print(response)
                    self.write_log(address, url, "200 OK")

            else:
                response_parts = ["HTTP/1.1 200 OK\r\n","Last-Modified: " + str(datetime.fromtimestamp(os.path.getmtime("htdocs" + url))) + "\r\n","Content-Length: " + str(len(contents)) + "\r\n","Content-Type: " + file_type + "\r\n\r\n"]
                response = "".join(response_parts)
                connection.sendall(response.encode())
                print("Server response:\n")
                print(response)
                self.write_log(address, url, "200 OK")

        except FileNotFoundError:
            self.handle_file_not_found(connection, url, address)


    def handle_bad_request(self, connection, url, address):
        # This is to send status code 400, Bad Request to the client

        response_parts = ["HTTP/1.1 400 Bad Request\r\n",
                          "Content-Length : 0\r\n",
                          "Content-Type: text/plain\r\n\r\n"]
        response = "".join(response_parts)
        connection.sendall(response.encode())

        print("Server response:\n")
        print(response)
        self.write_log(address, url, "400 Bad Request")

    def handle_file_not_found(self, connection, url, address):
        # This function handles the case that the file not found error happens. (status code: 404)

        response_parts = ["HTTP/1.1 404 Not Found\r\n",
                          "Content-Length : 0\r\n",
                          "Content-Type: text/plain\r\n\r\n"]
        response = "".join(response_parts)
        connection.sendall(response.encode())

        print("Server response:\n")
        print(response)
        self.write_log(address, url, "404 Not Found")

    def write_log(self, address, file, response_type):
        #This is to write records into the server_log.txt file.

        writing_parts = [str(datetime.now()), ": client", str(address), " requested file ", file,
                         " with response ", response_type, "\n"]
        writing = "".join(writing_parts)
        self.server_log.write(writing)
        self.server_log.flush()

    def get_file_type(self, url):
       #This function can return the type of the req file, which indicated by the url
        if url.endswith(".html"):
            return "text/html"
        elif url.endswith(".jpg"):
            return "image/jpg"
        elif url.endswith(".png"):
            return "image/png"
        else:
            return "text/plain"

    def execute(self):
        #This is to run the server and listen to the port
        try:
            print("\n" )
            print("The HTTP server is now running on port {}".format(self.port).center(81))
            #Server listens to port 8000 here. For every connection, server will create a thread.
            # Also, the server will open the log file.
            self.server_log = open("server_log.txt", "a")
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(5)

            # For every connection: create a thread, which can handle the connection, by calling rcv function
            while True:
                connection, address = self.server_socket.accept()
                thread = threading.Thread(target=self.receive, args=(connection, address))
                thread.start()
                self.connections.append(connection) # add to the list connection.


        except KeyboardInterrupt:
            #if keyboard interrupt happens: print message below
            print("\n" )
            print("Server closes")
            print("Or you can press Ctrl + C0")
        finally:
            # close socket, log file and connections
            self.server_log.close()
            self.server_socket.close()
            for connection in self.connections:
                connection.close()




if __name__ == "__main__":
    # run the program
    server = HTTPServer(SERVER_HOST, PORT)
    server.execute()