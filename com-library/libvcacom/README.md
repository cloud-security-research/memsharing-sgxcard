# LIBVCACOM Interface and Usage

Libvcacom allows communication via the memory sharing library or
regular ZMQ_STREAM sockets. It abstracts both their interfaces into a
common interface to create connections and send/receive messages from
communication partners.

## Functions to establish a connection

``` 
  int init_vca_com(vca_com_t * com, 
		   const char * host_ip, 
		   const char * host_port, 
		   vca_com_addr * self,
		   vca_com_type type);

  int init_vca_com_repeat(vca_com_t * com,
		   const char * host_ip,
		   const char * host_port);
```

`init_vca_com` establishes a connection to a host gateway via the
connection type specified by type. *com* is not allocated within this
function.

| Argument | Description |
|----------|-------------|
| com | Handle to the communication channel for later use |
| host_ip | String of the host ip typically in the for %hhu.%hhu.%hhu.%hhu |
| host_port | String of the host port (number) |
| self | Identifier to be used as a source when sending messages | 
| type | Specifies communication types, should be VCA_COM_[MEM_SHARING | ZMQ_SOCKET] |

`init_vca_com_repeat` allows separate threads to repeat the 
initialization. This should only be used for type memory sharing.

| Argument | Description |
|----------|-------------|
| com | Handle to the communication channel |
| host_ip | String of the host IP |
| host_port | String of the host port |

## Function to close connections

```
  int deinit_vca_com(vca_com_t * com);
```

`deinit_vca_com` closes the connection which was previously
established via a `init_vca_com` call.

## Functions to send messages

```  
  int vca_com_send_msg(vca_com_t * com, 
		       vca_com_addr * dst,
		       char * msg,
		       unsigned long long length,
		       unsigned int c);

  int vca_com_send_hdrless_msg(vca_com_t * com,
  	       	 	       char * msg,
			       unsigned long long length,
			       unsigned int c);
```

`vca_com_send_msg` sends message *msg* to *dst* using communication channel
*com*. In case the channel uses memory sharing, the message is delivered on 
channel *c*.

| Argument | Description |
|----------|-------------|
| com | Communication handle |
| dst | Quin-tuple specifying the destination of the message |
| msg | Buffer of the message to be send to *dst* |
| length | Length of the msg |
| c | Channel to be used to deliver the message, when memory sharing library is used |

`vca_com-send_hdrless_msg` for internal use only. Sends message
directly to communication channel *com*. Arguments are as above.

## Function to receive messages

```
  int vca_com_recv_msg(vca_com_t * com, 
		       vca_com_addr * src, 
		       char * msg,
		       unsigned long long * length,
		       unsigned int * c);
```

`vca_com_recv_msg` receives messages from the communication channel
*com*. The message's contents and length are returned in *msg* and
*length* respectively. The origin of a message is described in the
quin-tuple *src*. And the received channel (in case of memory sharing)
is *c*.

| Argument | Description |
|----------|-------------|
| com | Communication handle |
| src | Quin-tuple describing the origin of the message, can be used to respond |
| msg | Contents of the message |
| length | Length of the message |
| c | Channel the message was received on (given a memory sharing library connection) |


