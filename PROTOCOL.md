# Protocol
All types are in litte endian.

# Packet
| Position | Type                      | Description                                 |
| -------- | ------------------------- | ------------------------------------------- |
| 0        | [Header block](#Header)   | Packet header data. See [Header](#Header).  |
| 10       | [Message block](#Message) | Message of packet. See [Message](#Message). |

## Header block
[Header flags](#Header%20flags) are ignored for [Connection message](#Connection%20message) and [Acknowledgement message](#Acknowledgement%20message).

| Position | Type                            | Byte count | Data type | Description                   |
| -------- | ------------------------------- | ---------- | ----------|------------------------------ |
| 0        | [Packet type](#Packet%20type)   | 1          | Enum      | Type of packet.               |
| 1        | [Header flags](#Header%20flags) | 1          | Bit field | Type of packet.               |
| 2        | [Sequence](#Header%20flags)     | 2          | ushort    | Sequence number of packet.    |
| 4        | [Timestamp](#Timestamp)         | 8          | uint64    | Sender timestamp.             |

### Packet type
| Value | Sender        | Description             |
| ----- | ------------- | ----------------------- |
| 0x00  | Server/client | Connection packet.      |
| 0x01  | Server/client | Acknowledgement packet. |
| 0x02  | Server        | Entity packet.          |

### Header flags
Each bit represent different properties of the packet.

| Bit | Value | Description                                                                                                            |
| --- | ----  | ---------------------------------------------------------------------------------------------------------------------- |
| 0   | 0     | The packet is unreliable, no need for acknowledgement by receiver.                                                     |
|     | 1     | The packet is reliable and should always be acknowledned by the receiver.                                              |
| 1   | 0     | The order of receiving packets does not matter.                                                                        |
|     | 1     | The packet should be handled in sequence order by receiver. Value of bit 0 cannot be 0 in combination with this value. |

## Message block
Consists of one of the following, depending on value of [Header block](#Header%20block) - [Packet type](#Packet%20type):

| Packet type | Type                                                  |
| ----------- | ----------------------------------------------------- |
| 0x00        | [Connection message](#Connection%20message)           |
| 0x01        | [Acknowledgement message](#Acknowledgement%20message) |
| 0x02        | [Entity message](#Entity%20message)                   |

## Connection message

| Position |Type                                                     | Byte count | Data type | Description                |
| -------- |-------------------------------------------------------- | ---------- | ----------|--------------------------- |
| 0        | [Connection message type](#Connection%20message%20type) | 1          | Enum      | Type of connectio message. |

The connection message continues with one of the blocks below.

| Position | Connection message type | Type                                            |
| -------- | ----------------------- | ----------------------------------------------- |
| 1        | 0x00                    | [Initialization block](#Initialization%20block) |
|          | 0x01                    | [Accepting block](#Accepting%20block)           |
|          | 0x02                    | [Disconnect block](#Disconnect%20block)         |

### Connection message type
Describes the type of connection block.

| Value | Sender | Description                    |
| ----- | ------ | ------------------------------ |
| 0x00  | client | Connection request.            |
| 0x01  | Server | Accepting.                     |
| 0x02  | Server | Disconnect/Rejecting.          |

### Initialization block
[Header block](#Header%20block) -[Sequence](#Header%20flags) should always be 0.

### Accepting block
[Header block](#Header%20block) -[Sequence](#Header%20flags) should always be 1.

### Disconnect block
This block is empty for disconnections sent by clients.

| Position | Type                                   | Description     |
| -------- | ---------------------------------------| --------------- |
| 0        |  [Data block 255](#Data%20block%20255) | Reason message. |

#### Data block 255
Data starts at position 1 and consists of n bytes.
Length of data is specified in position 0.
| Position | Byte count | Data type   | Description   |
| -------- | ---------- | ------------|-------------- |
| 0        | 1          | uchar       | Size of data. |
| 1        | max 255    | uchar array | Data.         |

## Acknowledgement message

## Entity message





#### Timestamp
Consists of 8 bytes, forming an unsigned 64 bit integer, representing microseconds.

The server can chose to intiailize the timestamp clock by any value, even 0. Timestamps sent by the client should always be in i relation to the server time and syncronized when handshaking the connection.


