#include "args.h"
#include "get_text.h"
#include "socket.h"

#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encryption.h"

int main(int argc, char *argv[]) {
    ClientConfig config = {0};
    CPAResult result = client_parse_args(argc, argv, &config);

    if (result == ONLY_HELP) {
        client_print_usage(argv[0]);
        return 0;
    }

    if (result != OK) {
        fprintf(stderr, "%s\n", client_pa_result_to_string(result));
        client_print_usage(argv[0]);
        return 1;
    }

    printf("Hello from client\n");
    printf("Config:\n  -k: %lu\n  -t: %lu\n  -f: '%s'\n  -a: '%s'\n  -p: %hu\n",
           config.key, config.threads, config.file_path, config.server_ip,
           config.server_port);

    // Create socket
    ClientSocket *client_socket = create_socket(config.server_ip, config.server_port);
    if (client_socket->fd < 0) {
        close_socket(client_socket);
        return 1;
    }

    // Get file text
    // char *text = get_text(config.file_path);
    // size_t length = strlen(text);


    /* encryption */


    //inizio parte E
    size_t *out_len=malloc(sizeof(size_t));
    char *ciphertext=encrypt_file(config.file_path, config.key, out_len, config.threads);




    //fine parte E

    size_t length = strlen(ciphertext);


    // Send message to the server
    int b_send = send_message(client_socket, length, ciphertext, config.key);
    if (b_send < 0) {
        close_socket(client_socket);
        return 1;
    }

    // Receive ack from the server
    char ack_buffer[64];
    int b_read = receive_ack(client_socket, ack_buffer, 64);
    if (b_read < 0) {
        close_socket(client_socket);
        return 1;
    }

    // Close connection
    close_socket(client_socket);
    return 0;
}
