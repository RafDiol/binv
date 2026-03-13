#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define BUFFER_SIZE 16

#define SUCCESSFUL_EXECUTION 0
#define MEMORY_ALLOCATION_ERROR 1
#define IO_ERROR 2
#define ARGUMENT_PARSING_ERROR 3

#define ASCII_FLAG 1 << 0

double sStrToDouble(char* str, short* flag){
    errno = 0;
    char* endptr;
    double value = strtod(str, &endptr);
    if(endptr == str || errno == ERANGE || *endptr != '\0'){
        *flag = 1;
        errno = 0;
    }
    return value;
}

int main(int argc, char** argv){
    char* filepath = NULL;
    unsigned char flags = 0;
    size_t bufferSize = BUFFER_SIZE;
    if(argc > 1){
        for(int i = 1; i < argc; i++){
            if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0){
                printf("Usage ./binv -f <file>\n\n");
                printf("Options:\n");
                printf("%-25s %-30s\n", "--file or -f", "Indicates the file to view");
                printf("%-25s %-30s\n", "--ascii or -a", "Shows ascii representation");
                printf("%-25s %-30s\n", "--buffer-size or -b", "Sets the buffer size meaning the bytes per line");
                return SUCCESSFUL_EXECUTION;
            }
            if(strcmp(argv[i], "--ascii") == 0 || strcmp(argv[i], "-a") == 0){
                flags |= ASCII_FLAG;
            }
            if(strcmp(argv[i], "--buffer-size") == 0 || strcmp(argv[i], "-b") == 0){
                if(i + 1 >= argc){
                    fprintf(stderr, "Error: no buffer size parameter provided\n");
                    return ARGUMENT_PARSING_ERROR;
                }
                i++;
                short flag = 0;
                bufferSize = (size_t)sStrToDouble(argv[i], &flag);
                if(flag != 0){
                    return ARGUMENT_PARSING_ERROR;
                }
            }
            if(strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0){
                if(i + 1 >= argc){
                    fprintf(stderr, "Error: no file parameter provided\n");
                    return ARGUMENT_PARSING_ERROR;
                }
                i++;
                filepath = argv[i];
            }
        }
    } else {
        filepath = malloc(sizeof(char) * 256);
        if(filepath == NULL){
            fprintf(stderr, "Error: memory allocation failed\n");
            return MEMORY_ALLOCATION_ERROR;
        }
        printf("Filepath: ");
        fgets(filepath, 256, stdin);
        filepath[strcspn(filepath, "\n")] = '\0';
    }

    if(filepath == NULL){
        fprintf(stderr, "Error: no file provided\n");
        return ARGUMENT_PARSING_ERROR;
    }

    FILE* file = fopen(filepath, "rb");
    if(file == NULL){
        fprintf(stderr, "Error: unable to open file \"%s\"\n", filepath);
        return IO_ERROR;
    }

    unsigned char* buffer = malloc(sizeof(unsigned char) * bufferSize);
    if(buffer == NULL){
        fprintf(stderr, "Error: memory allocation failed\n");
        return MEMORY_ALLOCATION_ERROR;
    }

    size_t bytesRead;
    size_t offset = 0;
    while((bytesRead = fread(buffer, 1, bufferSize, file)) > 0){
        printf("%08zx:\t", offset);
        for(size_t i = 0; i < bytesRead; i++){
            printf("%02x ", buffer[i]);
        }

        if(bytesRead < bufferSize){
            size_t diff = bufferSize - bytesRead;
            for(size_t i = 0; i < diff; i++){
                printf("   ");
            }
        }

        if(flags & ASCII_FLAG){ 
            printf("\t");
            for(size_t i = 0; i < bytesRead; i++){
                char c = buffer[i];
                if(!(c >= 32 && c < 126)){
                    c = '.';
                }
                printf("%c", c);
            }
        }
        printf("\n");
        offset += bytesRead;
    }

    fclose(file);

    return SUCCESSFUL_EXECUTION;
}