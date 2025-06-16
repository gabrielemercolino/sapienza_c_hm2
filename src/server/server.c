#include "args.h"
#include "socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "decrypt.h"
#include "common/thread_pool.h"


#include <time.h>

void handle_client(void *arg) {
    ClientSocket *client_socket = (ClientSocket *) arg;

    if (read_message(client_socket) < 0) {
        close_client_socket(client_socket);
        free(client_socket);
        return;
    }
    ///qui decifrazione, mancano argomenti
    char *decrypt_text=decrypt_message(client_socket->buffer, client_socket->length, client_socket->key, client_socket->threads );


    //write file with padding. Puo essere estratto nella funzione separata


    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H:%M:%S", tm_info);

    char*fname=strcat(client_socket->file_prefix, buffer);


    char *fpath=strcat("../../resources/", fname);
    FILE *fout=fopen("../../resources/","w");
    if (fout==NULL) {
        perror("fopen fail");
    }
    fputs(decrypt_text, fout);
    fclose(fout);
    free(decrypt_text);
    free(fname);
    free(client_socket);



    // Send acknowledgment back to the client
    int b_write = send_ack(client_socket);
    if (b_write < 0) {
        close_client_socket(client_socket);
        free(client_socket);
        return;
    }
    close_client_socket(client_socket);
    free(client_socket);

}





int main(int argc, char *argv[]) {
    ServerConfig config = {0};
    SPAResult result = server_parse_args(argc, argv, &config);

    if (result == ONLY_HELP) {
        server_print_usage(argv[0]);
        return 0;
    }

    if (result != OK) {
        fprintf(stderr, "%s\n", server_pa_result_to_string(result));
        server_print_usage(argv[0]);
        return 1;
    }

    printf("Hello form server\n");
    printf("Config:\n  -t: %lu\n  -p: '%s'\n  -c: %d\n", config.threads,
           config.file_prefix, config.max_connections);

    // Create socket
    ServerSocket *server_socket = create_server_socket("INADDR_ANY", 8080, config.max_connections);
    ThreadPool *pool = create_thread_pool(config.max_connections);

    while (1) {
        printf("Waiting for a connection...\n");
        ClientSocket *client_socket = accept_client_connection(server_socket);
        if (!client_socket || client_socket->fd < 0) continue;

        //inizio E

        ClientSocket *client_arg = malloc(sizeof(ClientSocket));
        client_socket->threads=config.threads;
        client_socket->file_prefix=config.file_prefix;


        TPTaskResult r = thread_pool_try_do(pool, handle_client, client_arg);
        if (r != OK) {
            printf("Thread pool pieno. Connessione rifiutata.\n");
            close_client_socket(client_arg);
            free(client_arg);
            continue;
        }
        //fine E


    }

    printf("Chiusura server...\n");
    thread_pool_join(pool);
    thread_pool_free(pool);
    return 0;
}
