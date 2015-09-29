//CLIENT
#include "main.h"


int decimal_binary(int n)  //Function to convert decimal to binary
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
int binary_decimal(int n) //Function to convert binary to decimal.
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

int verifying_input_data(int argc, char** argv, int *port)
{
    if(argc == 3)
    {
        *port = atoi(argv[1]);
        strcpy(hostname, argv[2]);
    }
    else if(argc == 2 && strcmp(argv[1], "--help") == 0)
    {
        printf("./client <port to connect> <host name>\n");
        return -1;
    }
    else if(argc == 2)
    {
        *port = atoi(argv[1]);
        
        printf("Type host name to connect: ");
        scanf("%s", hostname);
    }
    
    else if(argc == 1)
    {
        printf("Type port number to connect: ");
        scanf("%d", port);
        
        printf("Type host name to connect: ");
        scanf("%s", hostname);
    }
    else
    {
        printf("Too many arguments \n");
        return -1;
    }
}
int creating_socket(int *socket_fd, struct hostent **server, char *hostname)
{
    *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        printf("ERROR on opening socket \n");
        return -1;
    }
    
    *server = gethostbyname(hostname);
    if (*server == NULL) 
    {
        printf("ERROR, no such host \n");
        return -1;
    }
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)(*server)->h_addr_list[0], (char*)&serv_addr.sin_addr.s_addr, (*server)->h_length);
    serv_addr.sin_port = htons(port);
    memset(&serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));
    
    if(connect(*socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("ERROR on connecting \n");
        return -1;
    }
}
int file_opening_error_handling(int n_read, FILE **file)
{
    if (n_read < 1)
    {
        if (ferror(*file))
            printf("\n@@@@@@@ERROR READING@@@@@@@\n\n");
        return 1;
    }
    return 0;
}
void hamming_coding(int *my_matrix, int* return_matrix)
{
    int G[7][4]={
            1, 1, 0, 1,
            1, 0, 1, 1,
            1, 0, 0, 0,
            0, 1, 1, 1,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1};
    
    int i,j;
    for(i=0;i<7;i++)
    {
        for(j=0;j<4;j++)
        {
        return_matrix[i] = return_matrix[i] + G[i][j]*my_matrix[j];
        return_matrix[i] = return_matrix[i] % 2;
        }
    }
}
int sending_and_receiving(char *buffor, int *buffor_counter, int packet_size, char first_char_to_send, char second_char_to_send, int socket_fd)
{
    FILE *output = fopen("modified","a+b+w");
    if(output == NULL)
    {
        printf("ERROR on opening file \n");
        return 1;
    }
    
    int readed;
    char *buffik = (char*)malloc(sizeof(char)* packet_size);
    
    if(*buffor_counter < packet_size)
    {
        buffor[*buffor_counter] = first_char_to_send;
        (*buffor_counter)++;
    }
    else
    {
        write(socket_fd,buffor,packet_size);
        readed = read(socket_fd, buffik, packet_size);
        fwrite(buffik, 1, readed, output);
        buffor[0] = first_char_to_send;
        *buffor_counter = 1;
 
    }
    if(*buffor_counter == packet_size)
    {
        write(socket_fd, buffor, packet_size);
        readed = read(socket_fd, buffik, packet_size);
        fwrite(buffik, 1, readed, output);
        *buffor_counter = 0;
    }
    if(*buffor_counter < packet_size)
    {
        buffor[*buffor_counter] = second_char_to_send;
        (*buffor_counter)++;
    }
    else
    {
        write(socket_fd,buffor,packet_size);
        readed = read(socket_fd, buffik, packet_size);
        fwrite(buffik, 1, readed, output);
        buffor[0] = second_char_to_send;
        *buffor_counter = 1;
    }
    if(*buffor_counter == packet_size)
    {
        write(socket_fd, buffor, packet_size);
        readed = read(socket_fd, buffik, packet_size);
        fwrite(buffik, 1, readed, output);
        *buffor_counter = 0;
    }
    free(buffik);
    fclose(output);
}
int parity_check(int *my_matrix)
{
    int H[3][7]={
                1,0,1,0,1,0,1,
                0,1,1,0,0,1,1,
                0,0,0,1,1,1,1};
    char tab[4];
    tab[3]='\0';
    int i,j=0;
    int parity[3];
    for(i=0;i<3;i++)
        parity[i]=0;
    
    for(i=0;i<3;i++)
    {
        for(j=0;j<7;j++)
        {
            parity[i] = parity[i] + H[i][j]*my_matrix[j+1];
            parity[i] = parity[i] % 2;
        }
    }
    j=0;
    for(i=2;i>=0;i--)
    {
        if(parity[i]==1)
            tab[j]='1';
        else
            tab[j]='0';
        j++;    
    }
    parity[0]=binary_decimal(atoi(tab));
    if(parity[0] == 0)
    {
        //printf("No wrong bits, or the 1st one\n");//1st one bit is not important, its padding
        return 0;
    }
    else
        //printf("Wrong bit: %d\n", parity[0]+1);
    return parity[0] + 1;
    
}
int* int_to_int_array(int bin)
{
    int *array_int = (int*)malloc(8*sizeof(int));
    int i;
    for(i=7;i>=0;i--)
    {
        array_int[i] = bin % 10;
        bin = bin / 10;
    }
    return array_int;
}
void hamming_correcting(int* bin_array, int wrong_bit)
{
    if(bin_array[wrong_bit-1] == 1)
        bin_array[wrong_bit-1] = 0;
    else
        bin_array[wrong_bit-1] = 1;
}
int* hamming_decoding(int *matrix)
{   
    int i,j;
    int *tab = (int*)malloc(4*sizeof(int));
    for(i=0;i<4;i++)
        tab[i]= 0;
    
    int R[4][7]={
                0,0,1,0,0,0,0,
                0,0,0,0,1,0,0,
                0,0,0,0,0,1,0,
                0,0,0,0,0,0,1};
   // printf("Decoded: ");
    for(i=0;i<4;i++)
    {
        for(j=0;j<7;j++)
        {
            tab[i] = tab[i] + R[i][j]*matrix[j+1]; 
        }
        //printf("%d", tab[i]);
    }
    //printf("\n");
    free(matrix);
    return tab;
}
char* decoding_char(unsigned char ch)
{
    int wrong_bit;
    int i;
    int *bin_array;
    unsigned char *decoded = (unsigned char*)malloc(4*sizeof(unsigned char));
    //printf("Received modified dec hamming of one part: %d \n",(int)ch);
    bin_array = int_to_int_array(decimal_binary((int)ch));
    
    //printf("Received modified bin hamming of one part: ");
    //for(i=0;i<8;i++)
    //{
    //    printf("%d", bin_array[i]);
    //}
   // printf("\n");
    
    wrong_bit = parity_check(bin_array);
    if(wrong_bit > 0)
            hamming_correcting(bin_array, wrong_bit);
    bin_array = hamming_decoding(bin_array);
    
    for(i=0;i<4;i++)
    {
        if(bin_array[i] == 1)
            decoded[i] = '1';
        else
            decoded[i] = '0';
    }
    free(bin_array);
    return decoded;
}
void connecting_two_chars(char *ch1, char *ch2, FILE **output)
{
    int i;
    char return_char;
    char* connected = (char*)malloc(9*sizeof(char));
    connected[8] = '\0';
    for(i=0;i<4;i++)
    {
        connected[i] = ch1[i];
        connected[i+4] = ch2[i];
    }
    //printf("Received bin by connecting decoded hamming's: %s \n", connected);
    return_char = (char)binary_decimal(atoi(connected));
    //printf("Received char by connecting decoded hamming's: %c \n-----------------\n", return_char);
    fwrite(&return_char, 1, 1, *output);
    free(connected);

}
void int_bin_array_to_char_array(int size, int *from, char *to)
{
    for(i=0;i<size;i++)
    {
        if(from[i] == 1)
        {
            to[i] = '1';
        }
        else if(from[i] == 0)
        {
            to[i] = '0';
        }
    }  
}
int dividing_byte_to_2x4_bits(int byte, int *bin1, int *bin2)
{
    count1 = 3;
    count2 = 3;
    for(i=7;i>=0;i--)//Dividing byte for 2x4 bits
    {
        if(i >= 4)
        {
            bin2[count2] = byte % 10;
            (count2)--;
            byte = byte / 10;
        } 
        if(i < 4)
        {
            bin1[count1] = byte % 10;
            (count1)--;
            byte = byte / 10;
        }
    }
}
int reading_from_file()
{
    FILE *file = fopen("modified","r+b");    
    FILE *output = fopen("output", "a+w+b");
    if(file == NULL)
    {
        printf("ERROR on opening file \n");
        return 1;
    }
    int n_read;
    char tab[2];
    char *decoded_1; 
    char *decoded_2; 
    while(1)
    {
        n_read = fread(tab,1,2,file);
        if(n_read > 0 )
        {
            decoded_1 = decoding_char(tab[0]);
            decoded_2 = decoding_char(tab[1]);
            
            connecting_two_chars(decoded_1, decoded_2, &output);
            free(decoded_1);
            free(decoded_2);
        }
        if(file_opening_error_handling(n_read, &file) == 1)
            break;
    }
    fclose(output);
    fclose(file);
    unlink("modified");
}
int getting_configurable_parameters(int *packet_size, int *error_generation_code, int *correcting_code, int *parameter, FILE **file, char *filename)
{
    //MAXIMUM SIZE OF PACKET
    printf("Maximum size of packet(in bytes): ");
    scanf("%d", packet_size);
    if(*packet_size <= 0)
    {
        printf("Must be greater than 0 \n");
        return -1;
    }
    //ERROR GENERATION METHOD
    printf("Error generation method: \n1)None \n2)Modify every n-th bit \n3)Modify percentage of data\n");
    scanf("%d", error_generation_code);
    if(*error_generation_code == 2)
    {
        printf("Type n: ");
        scanf("%d", parameter);
        if(*parameter < 0)
        {
            printf("Wrong parameter\n");
            return -1;
        }
    }
    else if(*error_generation_code == 3)
    {
        printf("Type percentage: ");
        scanf("%d", parameter);
        if(*parameter < 0 || *parameter > 100)
        {
            printf("Wrong parameter\n");
            return -1;
        }
    }
    else if(*error_generation_code <= 0 || *error_generation_code > 3)
    {
        
        printf("Wrong error generation method\n");
        return -1;
    }
    //ERROR CORRECTING CODE
    printf("Choose type of error correcting code:\n1)Hamming code \n2)Repetition code\n3)Packets with CRC\n");
    scanf("%d", correcting_code);
    if(*correcting_code <= 0 || *correcting_code > 3)
    {
        printf("Wrong error correcting code \n");
        return -1; 
    }
    //FILE TO SEND
    printf("Enter name of the file to send: ");
    bzero(filename,50);
    scanf("%s", filename);
    
    *file = fopen(filename,"rb");
    if(*file == NULL)
    {
        printf("ERROR on opening file \n");
    }
    
}

int hamming()
{
    int buffer_counter = 0;
    unsigned char *buffer = (char*)malloc(packet_size*sizeof(char)+1);
    buffer[packet_size] = '\0';
    int *bin1 = (int*)malloc(4*sizeof(int));
    int *bin2 = (int*)malloc(4*sizeof(int));
    
    printf("\n@@@@@@@Reading from file@@@@@@@\n\n");
    while(1)
    {
        for(i=0;i<7;i++)
        {
            hamming_matrix_1[i]=0;
            hamming_matrix_2[i]=0;//ZEROWANIE
        }
        int n_read = fread(&char_from_file,1,1,file);
        if(n_read > 0)
        {
            //printf("-----------------\nBytes read %d \n", n_read);
            //printf("Readed char: %c \n", char_from_file);
            readed = decimal_binary((int)char_from_file);
            //printf("In binary: %d \n",readed);
            dividing_byte_to_2x4_bits(readed, bin1, bin2);
            hamming_coding(bin1, hamming_matrix_1);
            hamming_coding(bin2, hamming_matrix_2);
            int_bin_array_to_char_array(7,hamming_matrix_1,hamming_1);
            int_bin_array_to_char_array(7,hamming_matrix_2,hamming_2);
            //printf("2 parts of char coded to hamming: %s %s \n", hamming_1, hamming_2);
            sending_and_receiving(buffer, &buffer_counter, packet_size, (char)(binary_decimal(atoi(hamming_1))), (char)(binary_decimal(atoi(hamming_2))), socket_fd);  
        }
        if (n_read < 1)
        {
            if (feof(file))
                printf("\n@@@@@@@End of file@@@@@@@\n\n");
            if (ferror(file))
                printf("\n@@@@@@@ERROR READING@@@@@@@\n\n");
            break;
        }
    }
    
    fclose(file);
    buffer[buffer_counter] = '\0';
    write(socket_fd, buffer, packet_size);
    read(socket_fd, buffer, packet_size);
    if(buffer_counter > 0)
    {
        buffer[buffer_counter] = '\0';
        FILE *modified = fopen("modified","a+b+w");
        fwrite(buffer, 1, buffer_counter, modified);
        fclose(modified);
    }
    
    free(buffer);
    free(bin1);
    free(bin2);
    reading_from_file();    
    printf("Received file saved in file: output\n");
    return 0;
}

void restoring_data()
{
    FILE *file = fopen("repeated","a+b+w+r");
    FILE *output = fopen("output","a+b+w+r");
    char tab[3];
    int count = 0;
    while(1)
    {
        n_read = fread(&char_from_file,1,1,file);
        if(n_read > 0)
        {
            tab[count] = char_from_file;
            count++;
            if(count == 3)
            {
                for(i=7;i>=0;i--)
                {
                    if((tab[0] & (1 << i)) & (tab[1] & (1 << i)))
                        out |= (1 << i);
                    else 
                    {
                        if((tab[0] & (1 << i)) & (tab[2] & (1 << i)))
                            out |= (1 << i);
                        else
                            out &= ~(1 << i);
                    }   
                }
                count = 0;
                fwrite(&out, 1, 1, output);
            }
        }  

        if(file_opening_error_handling(n_read, &file) == 1)
            break;
    }
    fclose(file);
    fclose(output);
    printf("Received file saved in file: output\n");
    unlink("repeated");
}
int repetition_code()
{
    FILE *repeated = fopen("repeated","a+b+w+r");
    
    unsigned char *buffer = (char*)malloc(packet_size*sizeof(char)+1);
    unsigned char *buffik = (char*)malloc(packet_size*sizeof(char)+1);
    buffer[packet_size] = '\0';
    buffik[packet_size] = '\0';
    int buffer_counter = 0;

    printf("\n@@@@@@@Readed from file@@@@@@@\n\n");
    while(1) // SENDING AND READING
    {
        n_read = fread(&char_from_file,1,1,file);
        if(n_read > 0)
        {
            for(i=0;i<3;i++)
            {
                if(buffer_counter < packet_size)
                {
                    buffer[buffer_counter] = char_from_file;
                    buffer_counter++;
                }
                else
                {
                    buffer[buffer_counter] = '\0';
                    write(socket_fd, buffer, packet_size);
                    readed = read(socket_fd, buffik, packet_size);
                    fwrite(buffik, 1, readed, repeated);
                    buffer[0] = char_from_file;
                    buffer_counter = 1;
                }
                if(buffer_counter == packet_size)
                {
                    buffer[buffer_counter] = '\0';
                    write(socket_fd, buffer, packet_size);
                    readed = read(socket_fd, buffik, packet_size);
                    fwrite(buffik, 1, readed, repeated);
                    buffer_counter = 0;
                }
            }
        }
        
         if(file_opening_error_handling(n_read, &file) == 1)
            break;
    }

    if(buffer_counter > 0)
    {
        buffer[buffer_counter] = '\0';
        write(socket_fd, buffer, buffer_counter);
        readed = read(socket_fd, buffik, buffer_counter);
        buffik[readed] = '\0';
        fwrite(buffik, 1, readed, repeated);
    }
    
    fclose(file);
    fclose(repeated);
    restoring_data();
    free(buffer);
    free(buffik);
}
unsigned long int crc32_tab[256] =
{
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
  0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
  0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
  0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
  0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
  0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
  0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
  0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
  0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
  0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
  0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
  0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
  0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
  0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
  0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
  0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
  0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
  0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
  0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
  0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
  0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
  0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
  0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
  0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
  0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
  0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
  0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
  0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
  0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
  0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
  0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
  0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
  0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
  0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
  0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
  0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
  0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
  0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
  0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
  0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
  0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
  0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
  0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};
int crc()
{   
    FILE *output = fopen("output","a+b+w+r");
    char *buffer = (char*)malloc(sizeof(char)* packet_size + 1);
    char *buff = (char*)malloc(sizeof(char)* packet_size + 1);
    buff[packet_size] = '\0';
    buffer[packet_size] = '\0';
    while(1)
    {
        n_read = fread(buff, 1, packet_size, file);
        if(n_read > 0)
        {
            write(socket_fd, buff, packet_size);
            readed = read(socket_fd, buffer, packet_size);
            fwrite(buffer, 1, readed, output);
            buffer[packet_size] = '\0';
            buff[packet_size] = '\0';
            for(i=0;i<packet_size;i++)
                sent_crc = (sent_crc >> 8) ^ crc32_tab[(sent_crc & 0xff) ^ buff[i]];
            for(i=0;i<packet_size;i++)
                received_crc = (received_crc >> 8) ^ crc32_tab[(received_crc & 0xff) ^ buffer[i]];    
            
            for(i=0;i<packet_size;i++)
            {
                buff[i] = 0;
                buffer[i] = 0;
            }
        }
        
        if(file_opening_error_handling(n_read, &file) == 1)
            break;
    }
    fclose(output);
    fclose(file);
    sent_crc ^= 0xffffffff;
    received_crc ^= 0xffffffff;
    printf("Sent CRC     = %lx\n", sent_crc);
    printf("Received CRC = %lx\n", received_crc);
    if(sent_crc == received_crc)
        printf("Transmission fully correct \n");
    else
        printf("Errors occured in transmission \n");
    printf("Received file saved in file: output\n");
    free(buffer);
    free(buff);
}

void statistics(char *filename)
{
    int n_read1, n_read2;
    FILE *source = fopen(filename, "r+b");
    FILE *output = fopen("output", "r+b");
    char char_from_source;
    char char_from_output;
    float good = 0;
    float all=0;
    while(1)
    {
        n_read1 = fread(&char_from_source, 1, 1, source);
        n_read2 = fread(&char_from_output, 1, 1, output);
        if(n_read1 > 0)
        {
            all++;
            if(char_from_source == char_from_output)
                good++;
        }
        if(file_opening_error_handling(n_read1, &source) == 1)
            break;
    }
    fclose(source);
    fclose(output);
    float stats = (good / all) * 100;
    printf("good %f\nall %1f \n", good, all);
    printf("output file good at %f percent \n", stats);
    
}

int main(int argc, char** argv) 
{
    unlink("repeated");
    unlink("output");
    if(verifying_input_data(argc, argv, &port) < 0)
        return -1;
    if(creating_socket(&socket_fd, &server, hostname) < 0)
        return -1;
    if(getting_configurable_parameters(&packet_size, &error_generation_code, &correcting_code, &parameter, &file, filename) < 0)
        return -1;
    
    parameters[0] = (uint16_t)correcting_code;
    parameters[1] = (uint16_t)packet_size;
    parameters[2] = (uint16_t)error_generation_code;
    parameters[3] = (uint16_t)parameter;
    write(socket_fd,parameters,8);// Sending initial parameters
    
    if(correcting_code == 1) //HAMMING CODE
        hamming();
    else if(correcting_code == 2)//REPETITION CODE
        repetition_code();
    else if(correcting_code == 3)//CRC
        crc();
    statistics(filename);
    return 0; 
}

