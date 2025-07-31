import serial
import hashlib
import time

# === CONFIGURATION ===
SERIAL_PORT = "/dev/pts/2"  # Change to your connected PTY
BAUD_RATE = 9600
XOR_KEY = 0xAA  # Simple symmetric key for XOR encryption

# === ENCRYPTION ===
def xor_encrypt(data, key):
    return bytes([b ^ key for b in data])

def compute_sha256(data):
    return hashlib.sha256(data).hexdigest().encode()

def send_uart_message(message):
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    if not ser.is_open:
        ser.open()

    # Encrypt message
    message_bytes = message.encode()
    encrypted = xor_encrypt(message_bytes, XOR_KEY)

    # Hash the original (plaintext) message
    digest = compute_sha256(message_bytes)

    # Packet structure: [ENCRYPTED_MESSAGE][DELIMITER][HASH]\n
    packet = encrypted + b'|' + digest + b'\n'

    ser.write(packet)
    ser.flush()
    print(f"[TX] Sent: {message}")
    ser.close()

# === MAIN LOOP ===
if __name__ == "__main__":
    print("Secure UART Sender (Type 'exit' to quit)")
    while True:
        user_input = input("Enter message to send: ")
        if user_input.strip().lower() == "exit":
            print("Exiting...")
            break
        send_uart_message(user_input)
        time.sleep(0.5)

