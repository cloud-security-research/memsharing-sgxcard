# Examples to communicate between host gateway and clients

This example includes an application to send and another to receive
messages via a communication channel to a [host
gateway](com-library/host-gateway). 

recvHello connects to the specified host gateway and waits for a
message afterwards. Once a message is received it prints the message
to standard output.

sendHello connects to the specified host gateway and sends a hello
message to the specified destination.

## Prerequisites for build

- Installed libzmq

## Build

Run:

```
make
```

For debug prints run:

```
CFLAGS=-DCOML_DBG make
```

## recvHello Syntax

Receives message to node via host gateway from source.

```
./recvHello -ni <node ip> -np <node port> -hi <host ip> -hp <host port> [-s]
```

| Argument | Description | Default|
|----------|-------------|--------|
| -ni <node ip> | IP of the client connecting to the host | REQUIRED |
| -np <node port> | Port of the client connecting | REQUIRED |
| -hi <host ip> | IP of the host gateway to connect to | REQUIRED |
| -hp <host port> | Port of the host gateway to connect to | REQUIRED |
| -s | If specified, connection via socket. Otherwise connection via memory sharing. | Connect via memory sharing. |

## sendHello Syntax

Sends Hello message from node via host gateway to destination.

```
./sendHello -ni <node ip> -np <node port> [-hi <host ip> -hp <host port>] -di <dst ip> -dp <dst port> -dc <dst card> [-s]
```

| Argument | Description | Default|
|----------|-------------|--------|
| -ni <node ip> | IP of the client connecting to the host | REQUIRED |
| -np <node port> | Port of the client connecting | REQUIRED |
| -hi <host ip> | IP of the host gateway to connect to | REQUIRED for memory sharing |
| -hp <host port> | Port of the host gateway to connect to | REQUIRED for memory sharing |
| -di <dst ip> | IP of the destination | REQUIRED |
| -dp <dst port> | Port of the destination | REQUIRED |
| -dc <dst card> | VCA/SGX card identifier of the destination | REQUIRED |
| -s | If specified, connection via socket. Otherwise connection via memory sharing. | Connect via memory sharing. |

