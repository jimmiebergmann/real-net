# Communication protocol
All types are in litte endian.  
Positions are in relation to the current block. Unpredictable positions are marked with an aterix.  
  
The sender of a packet will receive a disconnect notice if sending a packet of any defined type, without having an established connection.  

Duplicates, already received sequence numbers, of packets are ignored, but acknowledged if reliable flag in [Packet flags](#Packet-flags) is 1.  

Ordered packets are supported for message packets, but always used for linking packets.

# Index
- 1 - [Packet block ](1---Packet-block)
  - 1.1 - [Packet type](1.1---Packet-type)
  - 1.2 - [Packet flags](1.2---Packet-flags)
  - 1.3 - [Data blocks](1.3---Data-blocks)
    - 1.3.1 - [Connection block](1.3.1---Connection-block)
      - 1.3.1.1 - [Connection block type](1.3.1.1---Connection-block-type)
      - 1.3.1.2 - [Accepting block](1.3.1.2---Accepting-block)
      - 1.3.1.3 - [Rejection block](1.3.1.3---Rejection-block)
    - 1.3.2 - [Disconnection block](1.3.2---Disconnection-block)
    - 1.3.3 - [Synchronization block](1.3.3---Synchronizationn-block)
      - 1.3.3.1 - [Synchronization block type](1.3.3.1---Synchronizationn-block-type)
      - 1.3.3.2 - [Synchronization acknowledgement block](1.3.3.2---Synchronizationn-acknowledgement-block)
    - 1.3.4 - [Acknowledgement block](1.3.4---Acknowledgement-block)
    - 1.3.5 - [Linking block](1.3.5---Linking-block)
      - 1.3.5.1 - [Linking block type](1.3.5.1---Linking-block-type)
      - 1.3.5.2 - [Linking table block](1.3.5.2---Linking-table-block)
      - 1.3.5.3 - [Linking variable block](1.3.5.3---Linking-variable-block)
      - 1.3.5.4 - [Linking Entity block](1.3.5.4---Linking-Entity-block)
      - 1.3.5.5 - [Linking entity variable block](1.3.5.5---Linking-entity-variable-block)
      - 1.3.5.6 - [Linking variable data block](1.3.5.6---Linking-variable-data-block)
    - 1.3.6 - [Replication block](1.3.6---Replication-block)
      - 1.3.6.1 - [Replication entity block](1.3.6.1---Replication-block)
      - 1.3.6.2 - [Replication entity variable block](1.3.6.2---Replication-entity-variable-block)
      - 1.3.6.3 - [Replication entity variable data block](1.3.6.3---Replication-entity-variable-data-block)
      - 1.3.6.4 - [Replication variable block](1.3.6.3---Replication-variable-block)
    - 1.3.7 - [Message block](1.3.7---Message-block)
  - 1.4 - [Sequence](1.4---Sequence)
  - 1.5 - [Timestamp](1.5---Timestamp)
- 2 - [Connection](2---Connection)
  - 2.1 - [Handshake](2.1---Handshake)
  - 2.2 - [Rejection](2.2---Rejection)
- 3 - [Disconnection](3---Disconnection)
- 4 - [Acknowledgement](4---Acknowledgement)
- 5 - [Synchronization](5---Synchronization)
- 6 - [Linking](6---Linking)

# 1 - Packet block
| Position | Type                                | Byte count | Data type | Description     |
| -------- | ----------------------------------- | ---------- | --------- | --------------- |
| 0        | [Packet type](#1.1---Packet-type)   | 1          | uchar     | Type of packet. |
| 1        | ...                                 | ...        | ...       | ...             |

Packet continues with one of the following blocks, depending on the value of [Packet block](#1---Packet-block) - [Packet type](#1.1---Packet-type).

| Position | Packet type | Type                                                    | Notice               |
| -------- | ----------- | ------------------------------------------------------- | -------------------- |
| 1        | 0x00        | [Connection block](#1.3.1---Connection-block)           |                      |
|          | 0x01        | [Disconnection block](#1.3.2---Disconnection-block)     | Only sent by server. |
|          | 0x02        | [Synchronization block](#1.3.3---Synchronization-block) |                      |
|          | 0x03        | [Acknowledgement block](#1.3.4---Acknowledgement-block) |                      |
|          | 0x04        | [Linking block](#1.3.5---Linking-block)                 |                      |
|          | 0x05        | [Replication block](#1.3.6---Replication-block)         |                      |
|          | 0x06        | [Message block](#1.3.7---Message-block)                 |                      |

## 1.1 - Packet type
| Value | Sender        | Description             |
| ----- | ------------- | ----------------------- |
| 0x00  | Server/client | Connection packet.      |
| 0x01  | Server/client | Disconnection packet.   |
| 0x02  | Server/client | Synchronization packet. |
| 0x03  | Server/client | Acknowledgement packet. |
| 0x04  | Server        | Linking packet.         |
| 0x05  | Server        | Replication packet.     |
| 0x06  | Server/Client | Message packet.         |

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

| Position |Type                                                       | Byte count | Data type | Description                |
| -------- |---------------------------------------------------------- | ---------- | --------- | -------------------------- |
| 0        | [Packet flags](#1.2---Packet-flags)                       | 1          | uchar     | Packet properties.         |
| 1        | [Sequence](#1.4---Sequence)                               | 2          | ushort    | Sequence number of packet. |
| 3        | [Connection block type](#1.3.1.1---Connection-block-type) | 1          | uchar     | Type of connection block.  |
| 4        | ...                                                       | ...        | ...       | ...                        |

Block continues with one of the following blocks, depending on value of [Connection block type](#Connection-block-type).

| Position | Connection block type | Type                                                                |
| -------- | --------------------- | ------------------------------------------------------------------- |
| 4        | 0x01                  | [Connection accepting block](#1.3.1.2---Connection-accepting-block) |
| 4        | 0x02                  | [Connection rejection block](#1.3.1.3---Connection-rejection-block) |

#### 1.3.1.1 - Connection block type
Describes the type of connection block.

| Value | Sender | Description     |
| ----- | ------ | --------------- |
| 0x00  | client | Initialization. |
| 0x01  | Server | Accepting.      |
| 0x02  | Server | Rejection.      |

##### Connection initialization
[Connection block](#Connection-block) - [Sequence](#Sequence) is always 0.

#### 1.3.1.2 - Connection accepting block
Reliable flag in [Packet flags](#[Packet-flags]) is always 1.  
[Connection block](#Connection-block) - [Sequence](#Sequence) is always 1.

| Position | Type                    | Byte count | Data type | Description                           |
| -------- | ----------------------- | ---------- | --------- | ------------------------------------- |
| 0        | [Timestamp](#Timestamp) | 8          | uint64    | Received timestamp.                   |
| 8        | [Timestamp](#Timestamp) | 8          | uint64    | Sent timestamp.                       |
| 16       |                         | 4          | uint32    | Sync packet interval in microseconds. |

#### 1.3.1.3 - Connection rejection block
| Position | Byte count | Data type | Description          |
| -------- | ---------- | --------- | -------------------- |
| 0        | 1          | uchar     | Size of reason text. |
| 1        | max 255    | uchar *   | Reason text.         |

### 1.3.2 - Disconnection block
| Position | Type                                | Byte count | Data type | Description                |
| -------- | ----------------------------------- | ---------- | --------- | -------------------------- |
| 0        | [Packet flags](#1.2---Packet-flags) | 1          | uchar     | Packet properties.         |
| 1        | [Sequence](#1.4---Sequence)         | 2          | ushort    | Sequence number of packet. |
| 3        |                                     | 1          | uchar     | Size of reason text.       |
| 4        |  ...                                | ...        | ...       | ...                        |

Followed by following data, if the disconnection is sent from the server:

| Position | Byte count | Data type | Description          |
| -------- | ---------- | --------- | -------------------- |
| 1        | max 255    | uchar *   | Reason text.         |

### 1.3.3 - Synchronization block
| Position |Type                                                           | Byte count | Data type | Description                |
| -------- |-------------------------------------------------------------- | ---------- | --------- | -------------------------- |
| 0        | [Sequence](#Sequence)                                         | 2          | ushort    | Sequence number of packet. |
| 2        | [Synchronization block type](#Synchronization-block-type) | 1          | uchar     | Type of connection block.  |
| 3        | ...                                                           | ...        | ...       | ...                        |

Block continues with the following block, depending on value of [Synchronization block type](#Synchronization-block-type).

| Position | Connection block type | Type                                                          |
| -------- | --------------------- | ------------------------------------------------------------- |
| 3        | 0x01                  | [Synchronization acknowledgement block](#Synchronization-acknowledgement-block) |

#### 1.3.3.1 - Synchronization block type
Describes the type of synchronization block.

| Value | Sender        | Description      |
| ----- | ------------- | ---------------- |
| 0x00  | Server        | Initialization. |
| 0x01  | Server/Client | Ackowledgement.  |

#### 1.3.3.2 - Synchronization acknowledgement block
| Position |Type                             | Byte count | Data type | Description                   |
| -------- |-------------------------------- | ---------- | --------- | ----------------------------- |
| 0        | [Sequence](#Sequence)           | 2          | ushort    | Acknowledged packet sequence. |
| 2        | [Timestamp](#Timestamp)         | 8          | uint64    | Sent timestamp.               |
| 10       | [Timestamp](#Timestamp)         | 8          | uint64    | Received timestamp.           |

### 1.3.4 - Acknowledgement block
| Position | Type                  | Byte count | Data type | Description                                |
| -------- | --------------------- | ---------- | --------- | ------------------------------------------ |
| 0        | [Sequence](#Sequence) | 2          | ushort    | Sequence number of acknowledged packet.    |

### 1.3.5 - Linking block
Reliable and Ordered flags in [Packet flags](#[Packet-flags]) are always 1.

| Position | Type                                          | Byte count | Data type | Description                |
| -------- |---------------------------------------------- | ---------- | --------- | -------------------------- |
| 0        | [Packet flags](#Packet-flags)                 | 1          | uchar     | Packet properties.         |
| 1        | [Sequence](#Sequence)                         | 2          | ushort    | Sequence number of packet. |
| 3        | [Linking block type](#Linking-block-type)     | 2          | ushort    | Type of linking packet.    |
| 5        | ...                                           | ...        | ...       | ...                        |

Block continues with one of the following blocks, depending on value of [Linking block type](#Linking-block-type).

| Position | Linking block type | Type                                                |
| -------- | ------------------ | --------------------------------------------------- |
| 5        | 0x01               | [Linking table block](#Linking-table-block)         |
| 5        | 0x02               | [Linking accepting block](#Linking-accepting-block) |

### 1.3.5.1 - Linking block type
Describes the type of linking block.

| Value | Sernder | Description     |
| ----- | ------- |---------------- |
| 0x00  | Client  | Initialization. |
| 0x01  | Server  | Link table.     |
| 0x02  | Client  | Accepting.      |
| 0x03  | Client  | Rejection.      |

Reliable flag in [Packet flags](#[Packet-flags]) is always 1 for all linking block types.

#### 1.3.5.2 - Linking table block
| Position | Byte count | Data type | Description             |
| -------- | ---------- | --------- | ----------------------- |
| 3        | 1          | uchar     | Linking variable count. |
| 4        | 1          | uchar     | Linking entity count.   |
| 5        | ...        | ...       | ...                     |

Followed by [Linking variable block](#Linking-variable-block), occuring **Linking variable count** times,
and followed by [Linking entity block](#Linking-entity-block), occuring **Linking entity count** times.

#### 1.3.5.3 - Linking variable block
| Byte count | Type                                                            | Data type | Description                 |
| ---------- | --------------------------------------------------------------- | --------- | --------------------------- |
| 2          |                                                                 | ushot     | Variable link ID.           |
| 1          | [Linking variable data type](#Linking-variable-data-type) | uchar     | Linking variable data type. |
| 1          |                                                                 | uchar     | Variable name size.         |
| max 255    |                                                                 | uchar []  | Variable name.              |

#### 1.3.5.4 - Linking Entity block
| Byte count | Data type | Description                    |
| ---------- | --------- | ------------------------------ |
| 2          | ushort    | Entity link ID.                |
| 1          | uchar     | Entity name size.              |
| max 255    | uchar []  | Entity name.                   |
| 1          | uchar     | Entity variable linking count. |
| ...        | ...       | ...                            |

Followed by [Entity variable linking block](#Entity-variable-linking-block), occuring **Entity variable linking count** times, for each [Entity linking block](#Entity-linking-block).

#### 1.3.5.5 - Linking entity variable block
| Byte count | Type                                                            | Data type | Description                |
| ---------- | --------------------------------------------------------------- | --------- | -------------------------- |
| 1          |                                                                 | uchar     | Entity variable link ID.   |
| 1          | [Linking variable data type](#Linking-variable-data-type) | uchar     | Linking variable type.     |
| 1          |                                                                 | uchar     | Entity variable name size. |
| max 255    |                                                                 | uchar []  | Entity variable name.      |

#### 1.3.5.6 - Linking variable data type
| Value | Description                | Byte count |
| ----- | -------------------------- | ---------- |
| 0x00  | char.                      | 1          |
| 0x01  | uchar.                     | 1          |
| 0x02  | short.                     | 2          |
| 0x03  | ushort.                    | 2          |
| 0x04  | int.                       | 4          |
| 0x05  | uint.                      | 4          |
| 0x06  | int64.                     | 8          |
| 0x07  | uint64.                    | 8          |
| 0x08  | float.                     | 4          |
| 0x09  | double.                    | 8          |
| 0x0A  | char []                    | *          |


### 1.3.6 - Replication block
Ordered flag in [Packet flags](#[Packet-flags]) is always 0.

| Position | Type                          | Byte count | Data type | Description                |
| -------- | ----------------------------- | ---------- | --------- | -------------------------- |
| 0        | [Packet flags](#Packet-flags) | 1          | uchar     | Packet properties.         |
| 1        | [Sequence](#Sequence)         | 2          | ushort    | Sequence number of packet. |
| 3        | [Timestamp](#Timestamp)       | 8          | uint64    | Sender timestamp.          |
| 11       |                               | 1          | uchar     | Entity link ID count.      |
| 11       |                               | 1          | uchar     | Variable link ID count.    |
| 12       | ...                           | ...        | ...       | ...                        |

Followed by [Replication entity block](#Replication-entity-block), occuring **Entity link ID count** times.  
Then followed by [Replication variable block](#Replication-variable-block), occuring **Variable link ID count** times.

### 1.3.6.1 - Replication entity block
| Position | Byte count | Data type | Description                 |
| -------- | ---------- | --------- | --------------------------- |
| 0        | 2          | ushort    | Entity link ID              |
| 2        | 1          | uchar     | Entity variable link count. |
| 3        | ...        | ...       | ...                         |

Followed by [Entity variable link block](#Entity-variable-link-entity-block), occuring **Entity count** times, for each [Entity link block](#Entity-link-block).

### 1.3.6.2 - Replication entity variable block
| Position | Byte count | Data type | Description              |
| -------- | ---------- | --------- | ------------------------ |
| 0        | 2          | ushort    | Entity variable link ID. |
| 2        | 1          | uchar     | Entity ID count.         |
| 3        | ...        | ...       | ...                      |

Followed by [Replication entity variable data block](#Replication-entity-variable-data-block), occuring **Entity ID count** times.

### 1.3.6.3 - Replication entity variable data block
| Position | Type                    | Byte count | Data type | Description              |
| -------- | ----------------------- | ---------- | --------- | ------------------------ |
| 0        | [Entity ID](#Entity-ID) | 1          | uchar     | [Entity ID](#Entity-ID). |
| 1        |                         | 1          | uchar     | Data size.               |
| 2        |                         | max 255    | uchar *   | Data.                    |

### 1.3.6.4 - Replication variable block
| Position | Byte count | Data type | Description        |
| -------- | ---------- | --------- | ------------------ |
| 0        | 2          | ushort    | Variable link ID.  |
| 1        | 1          | uchar     | Data size.         |
| 2        | max 255    | uchar *   | Data.              |

### 1.3.7 - Message block
| Position | Type                            | Byte count | Data type | Description                |
| -------- |-------------------------------- | ---------- | --------- | -------------------------- |
| 0        | [Packet flags](#Packet-flags) | 1          | uchar     | Packet properties.         |
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
| Packet no. | Sender | [Connection block type](#Connection-block-type) | Packet size |
| ---------- | -------| --------------------------------------------------- | ----------- |
| 1          | Client | 0x00                                                | 5           |
| 2          | Server | 0x01                                                | 13          |
| 3          | Client | (Acknowledgement of packet no. 2.)                  | 3           |

## 2.2 - Rejection
Rejection(0x02) packet includes an optional reason text.

| Packet no. | Sender | [Connection block type](#Connection-block-type) | Packet size |
| ---------- | -------| --------------------------------------------------- | ----------- |
| 1          | Client | 0x00                                                | 5           |
| 2          | Server | 0x02                                                | 5 - 231     |

# 3 - Disconnection
Disconnection packets can be sent by server or client at any time.  
Packet sent by server includes an optional reason text.

# 4 - Acknowledgement
All packets, flagged with the reliable flag, must be acknowledged by the receiver as soon as possible.
Unacknowledged packets should be resent after any amount of time if they are marked as "reliable".
Syncronization information could be used for determination of when the packets should be resent.
Connection should be considered as lost after **x** number of unacknowledged resends.

# 5 - Synchronization
Synchronization interval is determined by the server and shared with the clients in the acception packet at connection.
A total of 3 packets are used for syncronization, initialized by the server. The client answers the initial packet with time information and the server answers back.

# 6 - Linking
Maps entity and variable names to readable names, as strings.
Makes it possible for the client to verify incoming replication data.
No replication to the client is done before the linking is requested and finished by the client.

