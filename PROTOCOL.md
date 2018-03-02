# Communication protocol
All types are in litte endian.  
Positions are in relation to the current block.  
  
The sender of a packet will receive a disconnect notice if sending a packet of any defined type, without having an established connection.  
Duplicates, already received sequence numbers, of packets are ignored, but acknowledged if reliable flag in [Packet flags](#Packet%20flags) is 1.

# Index
- 1 - Packet block
  - 1.1 - Packet type
  - 1.2 - Packet flags
  - 1.3 - Data blocks
    - 1.3.1 - Connection block
      - 1.3.1.1 - Connection block type
      - 1.3.1.2 - Accepting block
      - 1.3.1.3 - Rejection block
    - 1.3.2 - Disconnection block
    - 1.3.3 - Acknowledgement block
    - 1.3.4 - Linking block
    - 1.3.5 - Entity block
  - 1.4 - Sequence
  - 1.5 - Timestamp
- 2 - Connection
  - 2.1 - Handshake
  - 2.2 - Rejection

# 1 - Packet block
| Position | Type                            | Byte count | Data type | Description                   |
| -------- | ------------------------------- | ---------- | ----------|------------------------------ |
| 0        | [Packet type](#Packet%20type)   | 1          | Enum      | Type of packet.               |
| ...      | ...                             | ...        | ...       | ...                           |

Packet continues with one of the following blocks, depending on the value of [Packet block](#Packet%20block) - [Packet type](#Packet%20type).

| Position | Packet type | Type                                              | Notice              |
| -------- | ----------- | ------------------------------------------------- | -------------------- |
| 1        | 0x00        | [Connection block](#Connection%20block)           |                      |
|          | 0x01        | [Disconnection block](#Disconnection%20block)     | Only sent by server. |
|          | 0x02        | [Acknowledgement block](#Acknowledgement%20block) |                      |
|          | 0x03        | [Linking block](#Linking%20block)                 |                      |
|          | 0x04        | [Entity block](#Entity%20block)                   |                      |

## 1.1 - Packet type
| Value | Sender        | Description                |
| ----- | ------------- | -------------------------- |
| 0x00  | Server/client | Connection packet.         |
| 0x01  | Server/client | Disconnection packet.      |
| 0x02  | Server/client | Acknowledgement packet.    |
| 0x03  | Server        | Linking packet.            |
| 0x04  | Server        | Entity packet.             |

## 1.2 - Packet flags
Each bit represents different properties of the packet.

| Bit | Value | Description                                                                                                     |
| --- | ----  | --------------------------------------------------------------------------------------------------------------- |
| 0   | 0     | The packet is unreliable, no need for acknowledgement by receiver.                                              |
|     | 1     | The packet is reliable and is always acknowledned by the receiver.                                              |
| 1   | 0     | The order of receiving packets does not matter.                                                                 |
|     | 1     | The packet is handled in sequence order by receiver. Value of bit 0 cannot be 0 in combination with this value. |

# 1.3 - Data blocks

### 1.3.1 - Connection block

| Position |Type                                                 | Byte count | Data type | Description                |
| -------- |---------------------------------------------------- | ---------- | ----------|--------------------------- |
| 0        | [Connection block type](#Connection%20block%20type) | 1          | Enum      | Type of connection block.  |
| 1        | [Packet flags](#Packet%20flags)                     | 1          | bit field | Packet properties.         |
| 2        | [Sequence](#Sequence)                               | 2          | ushort    | Sequence number of packet. |
| ...      | ...                                                 | ...        | ...       | ...                        |

Block continues with one of the following blocks, depending on value of [Connection block type](#Connection%20block%20type).

| Position | Connection block type | Type                                          |
| -------- | --------------------- | --------------------------------------------- |
| 4        | 0x01                  | [Accepting block](#Accepting%20block)         |
|          | 0x02                  | [Rejection block](#Rejection%20block)         |

#### 1.3.1.1 - Connection block type
Describes the type of connection block.

| Value | Sender | Description     |
| ----- | ------ | --------------- |
| 0x00  | client | Initialization. |
| 0x01  | Server | Accepting.      |
| 0x02  | Server | Rejection.      |
| 0x03  | Client | Established.    |

##### Initialization
[Connection block](#Connection%20block) - [Sequence](#Header%20flags) is always 0.

##### Established
Reliable packet flag in [Packet flags](#[Packet%20flags]) is always 1.

#### 1.3.1.2 - Accepting block
Reliable packet flag in [Packet flags](#[Packet%20flags]) is always 0.  
[Connection block](#Connection%20block) - [Sequence](#Header%20flags) is always 1.

| Position | Type                            | Byte count | Data type | Description       |
| -------- | ------------------------------- | ---------- | ----------|------------------ |
| 0        | [Timestamp](#Timestamp)         | 8          | uint64    | Sender timestamp. |

#### 1.3.1.3 - Rejection block
| Position | Byte count | Data type   | Description      |
| -------- | ---------- | ------------|----------------- |
| 0        | 1          | uchar       | Size of message. |
| 1        | max 255    | uchar array | Message.         |

### 1.3.2 - Disconnection block
| Position | Byte count | Data type   | Description          |
| -------- | ---------- | ------------|--------------------- |
| 0        | 1          | uchar       | Size of reason text. |
| 1        | max 255    | uchar array | Reason text.         |

### 1.3.3 - Acknowledgement block
| Position | Type                  | Byte count | Data type | Description                                |
| -------- | --------------------- | ---------- | --------- | ------------------------------------------ |
| 0        | [Sequence](#Sequence) | 2          | ushort    | Sequence number of acknowledged packet.    |

### 1.3.4 - Linking block

### 1.3.5 - Entity block

## 1.4 - Sequence
Sequence numbers are represented by 16 bit unsigned integers.  
Each packet has it's own unique sequence number.  
Unacknowledged resent reliable packets keeps it's original value.

## 1.5 - Timestamp
Consists of 8 bytes, forming an unsigned 64 bit integer, representing microseconds.  

The server can chose to intiailize the timestamp clock by any value, even 0. Timestamps sent by clients are always in relation to the server time and syncronized when handshaking the connection.

# 2 - Connection
Connections are established via handshaking.

## 2.1 - Handshake
| Packet no. | Sender | [Connection block type](#Connection%20block%20type) | Packet size |
| ---------- | -------| --------------------------------------------------- | ----------- |
| 1          | Client | 0x00                                                | 5           |
| 2          | Server | 0x01                                                | 13          |
| 3          | Client | 0x03                                                | 5           |

## 2.2 - Rejection
Rejection(0x02) packet includes an optional reason text.

| Packet no. | Sender | [Connection block type](#Connection%20block%20type) | Packet size |
| ---------- | -------| --------------------------------------------------- | ----------- |
| 1          | Client | 0x00                                                | 5           |
| 2          | Server | 0x02                                                | 5 - 231     |

# 3 - Disconnection
Disconnection packets can be sent by server or client at any time.  
Packet sent by server includes an optional reason text.

