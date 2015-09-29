//SERVER
#include "main.h"
void modifying_every_n_bit(char *ch, int n, int *modified_bit)
{
    int j;
    int done = 0;
    
    for(j=*modified_bit;j>=0;j=j-n)
    {
        done = 1;
        if((*ch) & (1 << j) ) 
            *ch &= ~(1 << j);
        else
            *ch |= (1 << j);
                        
        *modified_bit = j;
    }
    if(done == 1)
    {
        *modified_bit -=  n;
    }
                    
    if(*modified_bit < 0)
    {
        *modified_bit += 8;
    }                  
}

void modifying_percentage_of_data(char *ch, int percent)
{
    int randomizer;
    for(j=0; j<8; j++)
    {
        randomizer = rand() % 100 +1;
        if(randomizer <= percent)
        {
            if((*ch) & (1 << j) ) 
                *ch &= ~(1 << j);
            else
                *ch |= (1 << j);
        }
    }
}

int decimal_binary(int n)  //Function to convert decimal to binary.
{
    int rem, i=1, binary=0;
    while (n!=0)
    {
        rem=n%2;
        n/=2;
        binary+=rem*i;
        i*=10;
    }
    return binary;
}

int binary_decimal(int n) // Function to convert binary to decimal.
{
    int decimal=0, i=0, rem;
    while (n!=0)
    {
        rem = n%10;
        n/=10;
        decimal += rem* pow(2,i);
        ++i;
    }
    return decimal;
}

int receiving_bytes(int socket_fd)
{
    int bytes_received;
    int modified_bit = 7;
    if(bytesReceived = read(socket_fd, parameters,8) > 0) // reading parameters
    {
        int packet_size = (int)parameters[1];
        unsigned char *buff = (char*)malloc(packet_size*sizeof(char) + 1);
        unsigned char *buffer = (char*)malloc(packet_size*sizeof(char) + 1);
        buff[packet_size] = '\0';
        buffer[packet_size] = '\0';
        printf("\n@@@@@Reading from socket@@@@@\n\n");

        while((bytesReceived = read(socket_fd, buff, packet_size)) > 0)
        {
            buff[bytesReceived] = '\0';
            buffer[bytesReceived] = '\0';
            //if(bytesReceived > 0)
            //{
                //printf("------------\nBytes received: %d \n", (int)bytesReceived);////
                //for(i = 0 ; i < bytesReceived ; i++)
                    //printf(" %d", buff[i]);
                //printf("\n");
            //}
            //###MODIFYING AND RESENDING###
            if((int)parameters[2] == 1)//Modifying nothing
            {   
                memcpy(buffer, buff, bytesReceived);
            }
            else if((int)(parameters[2]) == 2 )//Modifying every n bit
            { 
                for(i=0;i<bytesReceived;i++)
                {
                    modifying_every_n_bit(&buff[i], parameters[3], &modified_bit);
                    buffer[i] = buff[i];
                }
            }
            else if((int)(parameters[2]) == 3)//Modifying percentage of data
            {
                for(i=0;i<bytesReceived;i++)
                {
                    modifying_percentage_of_data(&buff[i], parameters[3]);
                    buffer[i] = buff[i];
                }
            }
            if(bytesReceived > 0)
                //printf("Resending: %s\n", buffer);
            write(socket_fd, buffer, packet_size);
        }
        printf("\nResending done\nWaiting for another client\n");
        free(buffer);
        free(buff);
    }  
    if(bytes_received == -1)
    {
        printf("ERROR on reading \n");
        return -1;
    }
}

int verifying_input_data(int argc, char **argv, int *port)
{
     if(argc == 2)
        *port = atoi(argv[1]);
    else if(argc == 1)
    {
        printf("Type your port number: ");
        scanf("%d", port);
    }
    else
    {
        printf("Too many arguments \n");
        return -1;
    }
}

int creating_socket(int *socket_fd)
{
    *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        printf("ERROR on opening socket \n");
        return -1;
    }
    
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    memset(&serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));
    memset(&cli_addr.sin_zero, 0, sizeof(cli_addr.sin_zero));
    
}

int main(int argc, char** argv)
{   
    srand((unsigned)time(&t));
    
    if(verifying_input_data(argc, argv, &port) == -1)
        return -1;
  
    creating_socket(&socket_fd);
    
    if(bind(socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("ERROR on binding socket \n");
        return -1;
    }    
    
    listen(socket_fd, 5);
    socklen_t cli_len = sizeof(cli_addr);
    printf("###Server is now working and waiting for client###\n");
    while(1)
    {
        new_socket_fd = accept(socket_fd, (struct sockaddr*) &cli_addr, &cli_len);
        if(new_socket_fd < 0)
        {
            printf("ERROR on accepting client \n");
            return 1;
        }
        pid = fork();
        if(pid<0)
        {
            printf("ERROR on forking");
            return 1;
        }
        if(pid == 0)
        {
            if(receiving_bytes(new_socket_fd) == -1)
                return -1;
        }
        else
        {
            close(new_socket_fd);
        }
    }
    return 0;
}


