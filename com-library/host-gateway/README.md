# Host Gateway for Transparent Communication

The purpose of this host gateway is to connect VCA/SGX nodes to externally
connected clients or other VCA/SGX nodes without requiring to use network
sockets. Every communication partner uses
[libvcacom](com-library/libvcacom) to start a connection and send
messages to other communication partners.  The setup relies on the
fact that libvcacom first establishes a connection to a host gateway
which then relays the messages to the communication partner.

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

## Host Gateway Syntax

The syntax for the host gateway application is as follows:

```
./host-gateway -i <ip> -np <port> -hp <port> [-v <number>] 
```

| Argument | Description | Default |
|----------|-------------|---------|
| -i <ip> | Specifies the IP <ip> to listen to for new connections and messages. Is used as advertised name of all VCA/SGX nodes. | REQUIRED |
| -np <port> | Specifies the port <port> that VCA/SGX nodes should connect to.  | REQUIRED |
| -hp <port> | Specifies the port <port> that external clients connect to. Is also used to name of the port of connected VCA/SGX cards. | REQUIRED |
| -v <number> | Specifies the number of VCA/SGX that will connect to the host gateway. For each VCA/SGX card up to 3 nodes may connect to the same host gateway. | 1 |

## Code Structure

The code is structured into a few modules. The *host-host-gateway*
connects external clients connecting via ZMQ_STREAM sockets to the
host gateway and provides functions to establish connections and
deliver messages. The *host-node-gateway* connects VCA/SGX nodes to
the host gateway and provides functions to accept memory sharing
requests by nodes and deliver messages from a node. 

All accepted connections are store in a *connection store* whose 
key is the destination identifier (IP, port, card id). 

Messages are send via the message module which delivers messages
based on libvcacom's implementation to send messages via different
communication channels.

When the host gateway start, it creates a separate thread to accept
new VCA/SGX node connections. Afterwards it spins to receive messages via
the external sockets or any of the VCA/SGX node memory sharing queues. All
of these receives are non-blocking. In case a message is received on
one of these channels, the host gateway consults the connection store
to find the corresponding libvcacom connection.  If the connection is
found, the message is delivered immediately.  Otherwise a connection
is first established, stored in the connection store for future use
and then the message is send.