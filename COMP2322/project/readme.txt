This program implements multi-threaded HTTP web server.

And specifically, this program implements following functions:
1. Initialize the server, and execute the server.
2. receive the request, and handle GET HEAD req, generate response
3. Handle some simple errors, like Bad Request (400), File Not Found (404)
4. generate logs

When compiling, you may enter Powershell, directing to the directory of the file.

1.Enter python webserver.py

2.Then open your browser, testing the code by entering http://127.0.0.1:8000/

3. Try again, by entering http://127.0.0.1:8000/index.html. As it's the default case, it has the same output to step2.

4.Entering 127.0.0.1:8000/index2.html, to test different cases. Like bad request, file not found, and so on.


There is one potential problem that when I attempted to run http://127.0.0.1:8000/not-exist.html, there’ll be likelihood that the code will fail to execute. However, you can repeat 1 or 2 times to execute the program normally.