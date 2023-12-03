# ts_depacketizer

Demonstrate reception algorithm of TS packets to form a data frame

## Features

Simulate frames with different styles:
- Ordered
- Shuffled
- Incomplete at start
- Incomplete at mid
- Incomplete at end

Uses a unique PID per frame

Simulate reception of the frames and:
- Detect missing packets (frame loss)
- Detect continuity errors (reordering needed)

## Usage

Modify the *generate_frames* function in *simulation.c* to add or remove different frames then:
```
make
./build/ts_depacketizer
```

Example output (more details can be displayed by setting *DEBUG* to *true* in *common.h*):
```
[Sizes] TsPacket: 204  Input TS packets: 52020  RX buffer: 416160
generate_frame - [pid: 0x11AA] - Generated a frame with 5 ts packets and style [Ordered]
generate_frame - [pid: 0x11AB] - Generated a frame with 4 ts packets and style [Shuffled IncompleteAtStart]
generate_frame - [pid: 0x11AC] - Generated a frame with 4 ts packets and style [Shuffled IncompleteAtMid]
generate_frame - [pid: 0x11AD] - Generated a frame with 4 ts packets and style [Shuffled IncompleteAtEnd]
generate_frame - [pid: 0x11AE] - Generated a frame with 5 ts packets and style [Shuffled]
generate_frame - [pid: 0x11AF] - Generated a frame with 5 ts packets and style [Ordered]
generate_frame - [pid: 0x11B0] - Generated a frame with 5 ts packets and style [Ordered]
generate_frame - [pid: 0x11B1] - Generated a frame with 5 ts packets and style [Ordered]
Total: 37 packets
do_on_ts_reception - [pid: 0x11AA] - Frame can be sent!
[pid: 0x11AB] - Frame lost!
[pid: 0x11AC] - Frame lost!
do_on_ts_reception - [pid: 0x11AE] - Reordering..
do_on_ts_reception - [pid: 0x11AE] - Frame can be sent!
[pid: 0x11AD] - Frame lost!
do_on_ts_reception - [pid: 0x11AF] - Frame can be sent!
do_on_ts_reception - [pid: 0x11B0] - Frame can be sent!
do_on_ts_reception - [pid: 0x11B1] - Frame can be sent!
```