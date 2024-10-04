# TelNot

TelNot is a playful server application that mimics a Telnet server with a twist. It's designed to amuse tech enthusiasts and frustrate potential intruders.

## Demo

![TelNot Demo](telnot-demo.gif)

This animation demonstrates how it looks from the client's side - showing how it sends colorful ANSI text files byte-by-byte with random delays. Yes, it is a feature, not a bug.

## Features

- Simulates a Telnet server
- Sends colorful ANSI text files byte-by-byte with random delays
- Supports both Linux (epoll) and macOS/BSD (kqueue) systems
- Uses non-blocking I/O and an event-driven architecture

## Setup

1. Clone the repository:
   ```
   git clone https://github.com/tzatko/telnot.git
   ```
2. Navigate to the TelNot directory:
   ```
   cd telnot
   ```
3. Compile the project:
   ```
   make
   ```

### Running on Linux without root privileges

To allow TelNot to bind to port 23 without running as root, you can use the `setcap` command to add the necessary capability:

```
sudo setcap cap_net_bind_service=+ep ./TelNot
```


## Contributing

### ANSI & ASCII Art

- We want cool ANSI & ASCII art
- Must look good on modern terminals. DOS codepage block art doesn't work quete right.

### Credit

Currently using art from [NNBnh's ansi project](https://github.com/NNBnh/ansi). If you're copying, say so.

## License

This project is released under the "Friend Meetup License" (FML):

1. You can do whatever you want with this code.
2. If you find it useful, meet up with a friend you haven't seen in a while.
3. Share a drink (beer, coffee, tea, water - your choice) and good conversation.
4. Remember, the point is human connection, not the beverage.

## Disclaimer

- You live at your own risk.
- Running stuff on port 23 might be risky. If you don't know why, maybe don't do it.
- This is a toy. Treat it like one. Don't use it for anything important.
- If this breaks your system, loses your data, or summons ancient eldritch horrors, that's on you.
- The universe doesn't owe you anything. Nor does it care.
- YOLO, but try not to be stupid about it.
- There is no god - Zeus told me. In person.
- "Don't be a dick." — Buddha

