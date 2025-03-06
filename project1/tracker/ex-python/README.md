# Example Code - Python UDP Client + Server

To run the code, first start the server specifying the port name as the required argument.

```
% python3 echo-server.py 2000
```

Once the server is running, run the client specifying the hostname and port number as the required arguments:

```
% python3 echo-client.py localhost 2000
```

Optionally, you can adjust the number of times that an echo request is sent from the client as well as the delay between echo requests via the `count` and `delay` optional arguments.

This client is compatible with the C++ server and will send an echo as well as processing
the response.