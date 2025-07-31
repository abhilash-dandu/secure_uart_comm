#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <openssl/sha.h>

#define BUF_SIZE 1024
#define XOR_KEY 0xAA

void xor_decrypt(char *data, int len) {
    for (int i = 0; i < len; i++) {
        data[i] ^= XOR_KEY;
    }
}

void sha256_hash(const char *data, size_t len, char *output) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)data, len, hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <serial_device>\n", argv[0]);
        return 1;
    }

    int serial_port = open(argv[1], O_RDONLY | O_NOCTTY);
    if (serial_port < 0) {
        perror("Unable to open serial port");
        return 1;
    }

    struct termios options;
    tcgetattr(serial_port, &options);
    cfsetispeed(&options, B9600);
    options.c_cflag |= CREAD | CLOCAL;
    tcsetattr(serial_port, TCSANOW, &options);

    char buffer[BUF_SIZE];
    int idx = 0;
    printf("Secure UART Receiver Listening on %s...\n", argv[1]);

    while (1) {
        char c;
        int n = read(serial_port, &c, 1);
        if (n > 0) {
            if (c == '\n') {
                buffer[idx] = '\0';

                // Split encrypted data and hash
                char *sep = strchr(buffer, '|');
                if (sep == NULL) {
                    printf("[ERROR] Malformed message\n");
                    idx = 0;
                    continue;
                }

                *sep = '\0';
                char *encrypted_data = buffer;
                char *received_hash = sep + 1;

                int encrypted_len = strlen(encrypted_data);
                char decrypted_msg[BUF_SIZE];
                memcpy(decrypted_msg, encrypted_data, encrypted_len);
                xor_decrypt(decrypted_msg, encrypted_len);
                decrypted_msg[encrypted_len] = '\0';

                char calc_hash[65];
                sha256_hash(decrypted_msg, encrypted_len, calc_hash);

                printf("[RX] Encrypted: %s\n", encrypted_data);
                printf("[RX] Decrypted: %s\n", decrypted_msg);
                printf("[RX] Received Hash: %s\n", received_hash);
                printf("[RX] Computed Hash: %s\n", calc_hash);

                if (strncmp(received_hash, calc_hash, 64) == 0) {
                    printf("[✔] Hash Match: Message Integrity Verified\n\n");
                } else {
                    printf("[✘] Hash Mismatch: Message May Be Tampered\n\n");
                }

                idx = 0;
            } else {
                buffer[idx++] = c;
            }
        }
    }

    close(serial_port);
    return 0;
}

