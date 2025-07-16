## Chat Server

C++ implementation of a chat server that emulates an IRC 


## Methodology

- Server
- Client
- Channel
- Connection

A server establishes a connection with a client using a connection
The client subscribes to a channel
Messages sent are marked with the channel they are associated with
Messages in a channel are sent to all members of a channel

All clients are by default part of a channel called 'common' that allows communication from all clients
