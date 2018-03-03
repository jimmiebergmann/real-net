# Communication protocol
All types are in litte endian.  
Positions are in relation to the current block. Unpredictable positions are marked with an aterix.  
  
The sender of a packet will receive a disconnect notice if sending a packet of any defined type, without having an established connection.  

Duplicates, already received sequence numbers, of packets are ignored, but acknowledged if reliable flag in [Packet flags](#Packet%20flags) is 1.  

Ordered packets are supported for linking, unlinking and message packets.  
Linking and unlinking packets share the same order queue and message packets has its own queue.

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
      - 1.3.4.1 - Structure of linking block.
      - 1.3.4.2 - Variable linking block
      - 1.3.4.3 - Entity linking block
      - 1.3.4.4 - Entity variable linking block
    - 1.3.5 - Unlinking block
      - 1.3.5.1 - Unlinking block type
      - 1.3.5.2 - Unlinking variable block
      - 1.3.5.3 - Unlinking entity block
      - 1.3.5.4 - Unlinking entity variable block
    - 1.3.6 - Entity block
      - 1.3.6.1 - Entity link block
      - 1.3.6.2 - Entity link entity block
      - 1.3.6.3 - Entity variable block
    - 1.3.7 - Message block
  - 1.4 - Sequence
  - 1.5 - Timestamp
- 2 - Connection
  - 2.1 - Handshake
  - 2.2 - Rejection
- 3 - Disconnection
- 4 - Alive packet

# 1 - Packet block
| Position | Type                            | Byte count | Data type | Description                   |
| -------- | ------------------------------- | ---------- | --------- | ----------------------------- |
| 0        | [Packet type](#Packet%20type)   | 1          | uchar     | Type of packet.               |
| 1        | ...                             | ...        | ...       | ...                           |

Packet continues with one of the following blocks, depending on the value of [Packet block](#Packet%20block) - [Packet type](#Packet%20type).

| Position | Packet type | Type                                              | Notice               |
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

| Bit | Name     | Value | Description                                                                                                     |
| --- | -------- | ----- | --------------------------------------------------------------------------------------------------------------- |
| 0   | Reliable | 0     |  The packet is unreliable, no need for acknowledgement by receiver.                                             |
|     |          | 1     | The packet is reliable and is always acknowledned by the receiver.                                              |
| 1   | Ordered  | 0     | The order of receiving packets does not matter.                                                                 |
|     |          | 1     | The packet is handled in sequence order by receiver. Value of bit 0 cannot be 0 in combination with this value. |

# 1.3 - Data blocks

### 1.3.1 - Connection block

| Position |Type                                                 | Byte count | Data type | Description                |
| -------- |---------------------------------------------------- | ---------- | --------- | -------------------------- |
| 0        | [Packet flags](#Packet%20flags)                     | 1          | uchar     | Packet properties.         |
| 1        | [Sequence](#Sequence)                               | 2          | ushort    | Sequence number of packet. |
| 3        | [Connection block type](#Connection%20block%20type) | 1          | uchar     | Type of connection block.  |
| 4        | ...                                                 | ...        | ...       | ...                        |

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
[Connection block](#Connection%20block) - [Sequence](#Sequence) is always 0.

##### Established
Reliable flag in [Packet flags](#[Packet%20flags]) is always 1.

#### 1.3.1.2 - Accepting block
Reliable flag in [Packet flags](#[Packet%20flags]) is always 0.  
[Connection block](#Connection%20block) - [Sequence](#Sequence) is always 1.

| Position | Type                    | Byte count | Data type | Description                                         |
| -------- | ----------------------- | ---------- | --------- | --------------------------------------------------- |
| 0        | [Timestamp](#Timestamp) | 8          | uint64    | Sender timestamp.                                   |

#### 1.3.1.3 - Rejection block
| Position | Byte count | Data type | Description          |
| -------- | ---------- | --------- | -------------------- |
| 0        | 1          | uchar     | Size of reason text. |
| 1        | max 255    | uchar []  | Reason text.         |

### 1.3.2 - Disconnection block
| Position | Byte count | Data type | Description          |
| -------- | ---------- | --------- | -------------------- |
| 0        | 1          | uchar     | Size of reason text. |
| 1        | max 255    | uchar []  | Reason text.         |

### 1.3.3 - Acknowledgement block
| Position | Type                  | Byte count | Data type | Description                                |
| -------- | --------------------- | ---------- | --------- | ------------------------------------------ |
| 0        | [Sequence](#Sequence) | 2          | ushort    | Sequence number of acknowledged packet.    |

### 1.3.4 - Linking block
Reliable and Ordered flags in [Packet flags](#[Packet%20flags]) are always 1.

| Position | Type                               | Byte count | Data type | Description                |
| -------- |----------------------------------- | ---------- | --------- | -------------------------- |
| 0        | [Packet flags](#Packet%20flags)    | 1          | uchar     | Packet properties.         |
| 1        | [Sequence](#Sequence)              | 2          | ushort    | Sequence number of packet. |
| 3        |                                    | 1          | uchar     | Variable linking count.    |
| 4        |                                    | 1          | uchar     | Entity linking count.      |
| 5        | ...                                | ...        | ...       | ...                        |

Followed by [Variable linking block](#Variable%20linking%20block), occuring **Variable linking count** times.

#### 1.3.4.1 - Structure of linking block.
- Header data
- [Variable linking block](#Variable%20linking%20block)
- [Entity linking block](#Entity%20linking%20block)
  - [Entity variable linking block](#Entity%20variable%20linking%20block)

#### 1.3.4.2 - Variable linking block
| Byte count | Type                              | Data type | Description         |
| ---------- | --------------------------------- | --------- | ------------------- |
| 2          |                                   | ushot     | Variable link ID.   |
| 1          | [variable type](#Variable%20type) | uchar     | Variable type.      |
| 1          |                                   | uchar     | Variable name size. |
| max 255    |                                   | uchar []  | Variable name.      |

Followed by [Entity linking block](#Entity%20linking%20block), occuring **Entity linking count** times.

#### 1.3.4.3 - Entity linking block
| Byte count | Data type | Description                    |
| ---------- | --------- | ------------------------------ |
| 2          | ushort    | Entity link ID.                |
| 1          | uchar     | Entity name size.              |
| max 255    | uchar []  | Entity name.                   |
| 1          | uchar     | Entity variable linking count. |
| ...        | ...       | ...                            |

Followed by [Entity variable linking block](#Entity%20variable%20linking%20block), occuring **Entity variable linking count** times, for each [Entity linking block](#Entity%20linking%20block).

#### 1.3.4.4 - Entity variable linking block
| Byte count | Type                              | Data type | Description                |
| ---------- | --------------------------------- | --------- | -------------------------- |
| 1          |                                   | uchar     | Entity variable link ID.   |
| 1          | [variable type](#Variable%20type) | uchar     | Variable type.             |
| 1          |                                   | uchar     | Entity variable name size. |
| max 255    |                                   | uchar []  | Entity variable name.      |

#### 1.3.4.5 - Variable type
| Value | Description                | Type byte count |
| ----- | -------------------------- | --------------- |
| 0x00  | char.                      | 1               |
| 0x01  | uchar.                     | 1               |
| 0x02  | short.                     | 2               |
| 0x03  | ushort.                    | 2               |
| 0x04  | int.                       | 4               |
| 0x05  | uint.                      | 4               |
| 0x06  | int64.                     | 8               |
| 0x07  | uint64.                    | 8               |
| 0x08  | float.                     | 4               |
| 0x09  | double.                    | 8               |
| 0x0A  | char []                    | *               |

### 1.3.5 - Unlinking block
Reliable and Ordered flags in [Packet flags](#[Packet%20flags]) are always 1.

| Position |Type                                                | Byte count | Data type | Description                |
| -------- |--------------------------------------------------- | ---------- | --------- | -------------------------- |
| 0        | [Packet flags](#Packet%20flags)                    | 1          | uchar     | Packet properties.         |
| 1        | [Sequence](#Sequence)                              | 2          | ushort    | Sequence number of packet. |
| 3        | [Unlinking block type](#CUnlinking%20block%20type) | 1          | uchar     | Type of unlinking block.   |

Block continues with one of the following blocks, depending on value of [Connection block type](#Connection%20block%20type).

| Position | Connection block type | Type                                                                      |
| -------- | --------------------- | ------------------------------------------------------------------------- |
| 1        | 0x00                  | [Unlinking variable block](#Unlinking%20variable%20block)                 |
|          | 0x01                  | [Unlinking entity block](#Unlinking%20entity%20block)                     |
|          | 0x02                  | [Unlinking entity variable block](#Unlinking%20entity%20variable%20block) |

### 1.3.5.1 - Unlinking block type
Describes the type of unlinking block.

| Value | Description                |
| ----- | -------------------------- |
| 0x00  | Variable unlinking         |
| 0x01  | Entity unlinking.          |
| 0x02  | Entity variable unlinking. |

### 1.3.5.2 - Unlinking variable block
| Position | Byte count | Data type | Description       |
| -------- | ---------- | --------- | ----------------- |
| 0        | 2          | ushort    | Variable link ID. |

### 1.3.5.3 - Unlinking entity block
| Position | Byte count | Data type | Description     |
| -------- | ---------- | --------- | --------------- |
| 0        | 2          | ushort    | Entity link ID. |

### 1.3.5.4 - Unlinking entity variable block
| Position | Byte count | Data type | Description              |
| -------- | ---------- | --------- | ------------------------ |
| 0        | 2          | ushort    | Entity link ID.          |
| 2        | 1          | uchar     | Entity variable link ID. |

### 1.3.6 - Entity block
Ordered flag in [Packet flags](#[Packet%20flags]) is always 0.

| Position | Type                            | Byte count | Data type | Description                |
| -------- | ------------------------------- | ---------- | --------- | -------------------------- |
| 0        | [Packet flags](#Packet%20flags) | 1          | uchar     | Packet properties.         |
| 1        | [Sequence](#Sequence)           | 2          | ushort    | Sequence number of packet. |
| 3        | [Timestamp](#Timestamp)         | 8          | uint64    | Sender timestamp.          |
| 11       |                                 | 1          | uchar     | Entity link IDcount.         |
| 12       | ...                             | ...        | ...       | ...                        |

Followed by [Entity link block](#Entity%20link%20block), occuring **Entity link count** times.

### 1.3.6.1 - Entity link block
| Position | Byte count | Data type | Description                    |
| -------- | ---------- | --------- | ------------------------------ |
| 0        | 2          | ushort    | Entity link ID                 |
| 2        | 1          | uchar     | Entity variable link ID count. |
| 3        | ...        | ...       | ...                            |

Followed by [Entity variable link block](#Entity%20link%20entity%20block), occuring **Entity count** times, for each [Entity link block](#Entity%20link%20block).

### 1.3.6.2 - Entity variable link entity block
| Position | Byte count | Data type | Description              |
| -------- | ---------- | --------- | ------------------------ |
| 0        | 2          | ushort    | Entity variable link ID. |
| 2        | 1          | uchar     | Entity ID count.         |
| 3        | ...        | ...       | ...                      |

Followed by [Entity variable block](#Entity%20variable%20block), occuring **Variable count** times, for each [Entity link entity block](#Entity%20link%20entity%20block).

### 1.3.6.3 - Entity variable block
| Position | Type                      | Byte count | Data type | Description              |
| -------- | ------------------------- | ---------- | --------- | ------------------------ |
| 0        | [Entity ID](#Entity%20ID) | 1          | uchar     |[Entity ID](#Entity%20ID) |
| 1        |                           | 1          | uchar     | Data size.               |
| 2        |                           | max 255    | uchar *   | Data.                    |

### 1.3.7 - Message block
| Position | Type                            | Byte count | Data type | Description                |
| -------- |-------------------------------- | ---------- | --------- | -------------------------- |
| 0        | [Packet flags](#Packet%20flags) | 1          | uchar     | Packet properties.         |
| 1        | [Sequence](#Sequence)           | 2          | ushort    | Sequence number of packet. |
| 3        | [Timestamp](#Timestamp)         | 8          | uint64    | Sender timestamp.          |
| 11       |                                 | 1          | ushort    | Size of message name.      |
| 12       |                                 | max 255    | uchar []  | Message name.              |
| *        |                                 | 2          | ushort    | Size of message.           |
| *        |                                 | max 65535  | uchar []  | Message.                   |

## 1.4 - Sequence
Sequence numbers are represented by 16 bit unsigned integers.  
Each packet has it's own unique sequence number.  
Unacknowledged resent reliable packets keeps it's original value.

## 1.5 - Timestamp
Consists of 8 bytes, forming an unsigned 64 bit integer, representing microseconds.  

The server can chose to intiailize the timestamp clock by any value, even 0. Timestamps sent by clients are always in relation to the server time and syncronized when handshaking the connection.

## 1.6 - Entity ID
Each entity is represented by a unique id.

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

# 4 - Alive packet
If the connection is lost, but no disconnect packet is sent, then it's important to send an "alive" or "ping" packet,  
which is being acknowledged by the receiver. The connection is treated as lost when no new packets are received for any timeout, defined by respective implementation.  
Alive packets are sent every other time from server and client. The server will always send the first one.


