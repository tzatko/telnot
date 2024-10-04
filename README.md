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

This grants the `TelNot` executable the capability to bind to privileged ports (below 1024).

4. Run the server:
   ```
   ./TelNot
   ```

## Usage

Connect to the server using a Telnet client:
```
telnet localhost 23
```

## Deployment and Persistence

### on Ubuntu 24.04.1 LTS

1. Create a dedicated user for running TelNot:
   ```
   sudo adduser --system --no-create-home telnot
   ```

2. Set up the program in the correct location:
   ```
   sudo mkdir -p /opt/TelNot
   sudo cp TelNot /opt/TelNot/
   sudo chown -R telnot:nogroup /opt/TelNot
   sudo chmod 755 /opt/TelNot/TelNot
   ```

3. Set the necessary capability:
   ```
   sudo setcap cap_net_bind_service=+ep /opt/TelNot/TelNot
   ```

4. Create a systemd service file:
   ```
   sudo nano /etc/systemd/system/telnot.service
   ```
   Add the following content:
   ```
   [Unit]
   Description=TelNot Server
   After=network.target

   [Service]
   ExecStart=/opt/TelNot/TelNot
   User=telnot
   Restart=on-failure
   StandardOutput=journal
   StandardError=journal

   [Install]
   WantedBy=multi-user.target
   ```

5. Enable the service to start on boot:
   ```
   sudo systemctl enable telnot.service
   ```

6. Start the service:
   ```
   sudo systemctl start telnot.service
   ```

7. Check the service status:
   ```
   sudo systemctl status telnot.service
   ```

8. View logs for the service:
   ```
   sudo journalctl -u telnot.service
   ```

9. Adjust log verbosity (if needed):
   Edit the service file to add an environment variable:
   ```
   sudo nano /etc/systemd/system/telnot.service
   ```
   Add under the [Service] section:
   ```
   Environment="TELNOT_LOG_LEVEL=6"
   ```
   Reload the systemd configuration and restart the service:
   ```
   sudo systemctl daemon-reload
   sudo systemctl restart telnot.service
   ```
   Log levels: 0 (emergency) to 7 (debug). Default is usually 6 (informational).

### on other operating systems

To deploy TelNot on other operating systems, use the following prompt with an AI language model (like ChatGPT, Claude, or LLAMA):

```
I want to deploy the TelNot program on $YOUR_OS_AND_VERSION and make it persistent across reboots. If $YOUR_OS_AND_VERSION is not replaced with an actual OS and version, please start by guiding me on how to determine my OS and version.

Once the OS is known, please provide step-by-step instructions for:

1. Creating a dedicated user for running TelNot.
2. Setting up the program in the correct location (preferably /opt/TelNot/) with proper permissions.
3. Granting the necessary capability to bind to port 23 without root privileges, if applicable to this OS.
4. Creating a service or daemon for TelNot appropriate for this OS.
5. Enabling the service to start on boot.
6. Starting the service immediately.
7. Checking the service status.
8. Viewing logs for the service.
9. Adjusting log verbosity, if possible.
10. Basic security considerations for running a service on port 23 on this specific OS.

Program details:
- Name: TelNot
- Preferred location: /opt/TelNot/TelNot
- Required capability (if applicable): cap_net_bind_service
- Port: 23
- Preferred user to run as: telnot

Please include all necessary commands, file contents, and explanations for each step. Assume I have administrative or root privileges.
```

Follow the AI-generated instructions to deploy TelNot on your specific operating system.

## Contributing

### ANSI Art Only

- We want cool ANSI art. Nothing else.
- Must look good on modern terminals. No DOS codepage block art.
- Keep it family-friendly.
- Use `.ans` extension.
- Don't make it huge.

### How to Contribute

1. Fork.
2. Add your `.ans` file to `textfiles`.
3. Pull request. Keep it brief.

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

