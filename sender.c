#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

// #include "include/utils.h"
#include "include/rsa.h"
#include "include/network.h"
#include "include/constants.h"

// Server

void dump(char *buffer, int length)
{
    for (size_t i = 0; i < length; i++)
    {
        printf("%02x", buffer[i]);
    }
    puts("");
}
int main()
{
    int sockfd, new_sockfd;
    struct sockaddr_in host_addr, client_addr;
    socklen_t sin_size;
    int recv_length = 1, yes = 1;
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error("in socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        error("setting socket option SO_REUSEADDR");
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = 0;
    memset(&(host_addr.sin_zero), '\0', 8);
    if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
        error("binding to socket");
    if (listen(sockfd, 5) == -1)
        error("listening on socket");
    printf("Listening on port %d\n", PORT);
    // printf("Connected to %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    mpz_t d, e, n;
    mpz_init(d);
    mpz_init(e);
    mpz_init(n);
    u_char *KEY = generate_random_array(AES_KEY_SIZE);
    u_char *IV = generate_random_array(AES_IV_SIZE);

    // int sent = send(new_socketfd, n_char, strlen(n_char), 0);
    init_rsa(d, n, e);
    const int n_size = mpz_sizeinbase(n, 10) + 1;

    const int e_size = mpz_sizeinbase(e, 10) + 1;

    char n_char[n_size];
    char e_char[e_size];
    mpz_get_str(n_char, 10, n);
    mpz_get_str(e_char, 10, e);
    char sep = '-';
    strcat(n_char, &sep);

    strcat(n_char, e_char);
    strcat(n_char, "a");
    while (1)
    { // Accept loop.

        sin_size = sizeof(struct sockaddr_in);
        new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_sockfd == -1)
            error("accepting connection");
        printf("Connected to %s port %d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        send(new_sockfd, n_char, n_size + e_size + 2, 0);
        size_t message_len = sizeof(unsigned long long int);
        while (get_encrypted_string(d, n, new_sockfd, &message_len,inet_ntoa(client_addr.sin_addr))==1);
    }
        close(new_sockfd);


    puts("");

    close(sockfd);
    free(IV);
    free(KEY);
    mpz_clear(n);
    mpz_clear(e);
    mpz_clear(d);
    return 0;
}