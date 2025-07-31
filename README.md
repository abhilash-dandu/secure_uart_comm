Secure UART Communication (Python + C)
======================================

This project demonstrates a secure UART (Universal Asynchronous Receiver/Transmitter) communication system using Python and C. A Python script sends hashed messages via serial, and a C program verifies the messages using SHA-256.

Features
--------
- Secure UART data transfer
- SHA-256 hashing for message authentication
- Cross-language implementation (Python sender and C receiver)
- Works with `pty` or real serial devices like `/dev/ttyUSBx`
- Modular code for easy extension to full-duplex messaging

Project Structure
-----------------
secure_uart/
├── secure_uart_sender.py         # Python-based UART sender
├── secure_uart_receiver.c        # C-based UART receiver with hash verification
├── secure_uart_comm.png          # Communication architecture diagram
├── uart_receiver                 # Compiled C binary (ignored in .gitignore)
└── .gitignore

How It Works
------------
1. **Sender (Python)**
   Takes user input, computes SHA-256 hash, and sends message over UART as:
   `$message|hash\n`

2. **Receiver (C)**
   Listens on a UART device, parses message and hash, recomputes SHA-256, and verifies.

Usage
-----

1. Setup Virtual UART Ports (for simulation)
   ```bash
   sudo apt install socat
   socat -d -d pty,raw,echo=0 pty,raw,echo=0
   ```
   Copy the two pseudo-terminal devices shown (e.g., `/dev/pts/3`, `/dev/pts/4`).

2. Compile Receiver (C)
   ```bash
   gcc secure_uart_receiver.c -o uart_receiver -lcrypto
   ```
   Run receiver on one of the terminals:
   ```bash
   ./uart_receiver /dev/pts/4
   ```

3. Run Sender (Python)
   Make sure `pyserial` is installed:
   ```bash
   pip install pyserial
   ```
   Run sender script:
   ```bash
   python3 secure_uart_sender.py
   ```
   Enter messages when prompted, they will be sent with hashes.

Notes
-----
- Ensure both ends use the same baud rate and line settings.
- This is a simple simulation, but can be extended to real UART devices on embedded systems.
- For full-duplex messaging, bidirectional scripts can be implemented.

License
-------
MIT License. Use freely with attribution.
