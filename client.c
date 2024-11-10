#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include "include/constants.h"

#include "include/rsa.h"
#include "include/network.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        error("Please provide arguments!!");
    }
    mpz_t n, e;
    int sockfd, clientfd;
    struct sockaddr_in serv_addr;
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        error("Cannot create sockets!!!");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        error("Invalid address!!");
    }

    if (clientfd = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Connection failed!!!");
    }
    size_t initial_size = 1024;
    char *data = (u_char *)receive_string(sockfd, &initial_size, 'a');

    mpz_init(e);
    mpz_init(n);
    split_and_set(data, '-', n, e, initial_size);
    free(data);
    while (1){
    size_t output_length = 0;
    char *response = input("Enter your text: ", &output_length);
    send_encrypted_string(response, n, e, sockfd, output_length);

    puts("Sent");
    free(response);
    }
    close(clientfd);
    close(sockfd);

    mpz_clear(n);
    mpz_clear(e);
    return 0;
}