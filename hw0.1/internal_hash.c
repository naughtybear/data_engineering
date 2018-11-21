#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <locale.h>
#include <sys/time.h>
#include <unistd.h>

const int prime=(1<<28);

typedef struct Node{
    unsigned int offset,next,cnt;
}node;

typedef struct Parameter{
    unsigned int *table;
    node *key;
    char *data_start;
    unsigned int key_current;
    int offset;
    unsigned int key_size, data_size;
}par;

unsigned int time33(char *string)
{
    unsigned int hash = 5381,i;
    
    for(i = 0; i < strlen(string); i++){
        hash = (((hash << 5) + hash) + string[i])%prime;
    }

    return hash;
}

void hash_insert(par *parameter, char* string)
{
    unsigned int hash = time33(string);
    unsigned int key_pos = parameter->table[hash];
    int i;

    while(1){
        if(key_pos > parameter->key_size){
            printf("hash:%d\n",hash);
            exit(0);
        }
        if(key_pos == 0){
            int len = strlen(string);

            parameter->key[parameter->key_current].next = parameter->table[hash];
            parameter->table[hash] = parameter->key_current;
            parameter->key[parameter->key_current].cnt = 1;
            /*if(parameter->offset+len > parameter->data_size){
                printf("memory leak\n");
                exit(0);
            }*/
            parameter->key[parameter->key_current].offset = parameter->offset;
            strcpy(parameter->data_start+parameter->offset, string);
            parameter->offset = parameter->offset+len;
            *(parameter->offset+parameter->data_start) = '\0';
            parameter->offset++;
            parameter->key_current++;

            return;
        }
        char *buf = parameter->key[key_pos].offset+parameter->data_start;
        if(strcmp(buf, string) == 0){
           // printf("same\n");
            parameter->key[key_pos].cnt++;
            return;
        }
        key_pos = parameter->key[key_pos].next;
    }
}

unsigned int hash_find(par *parameter, char* string)
{
    unsigned int hash = time33(string);
    unsigned int key_pos = parameter->table[hash];

    while(1){
        if(key_pos == 0)
            return 0;
        char *buf = parameter->key[key_pos].offset+parameter->data_start;
        if(strcmp(buf, string) == 0){
            return parameter->key[key_pos].cnt;
        }
        key_pos = parameter->key[key_pos].next;
    }
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int i;
    unsigned int hash_size = 1<<28, key_size = 1<<28, data_size = 1<<30;
    char buf[100000];
    par parameter;
    struct timeval start;
    struct timeval end;

    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], "-m") == 0){
            long long memory_size;
            i++;
            memory_size = atoll(argv[i]);
            hash_size = (unsigned int)(memory_size/20);
            key_size = (unsigned int)(memory_size/20);
            data_size = (unsigned int)(memory_size/5);
        }
        else if(strcmp(argv[i], "-s") == 0){
            i++;
            key_size = (unsigned int)atoll(argv[i]);
        }
        else if(strcmp(argv[i], "-h") == 0){
            i++;
            hash_size = (unsigned int)atoll(argv[i]);
        }
    }

    fp = fopen("content.txt","r");
    parameter.data_start = (char*)malloc(data_size);
    parameter.offset = 0;
    parameter.key = (node*)malloc(key_size*sizeof (node));
    parameter.key_current = 1;
    parameter.key_size = key_size;
    parameter.table = (int*)malloc(hash_size*sizeof(int));
    for(i=0; i<hash_size; i++){
        parameter.table[i] = 0;
        parameter.key[i].next = parameter.key[i].offset = parameter.key[i].cnt= 0;
    }
    int cnt=0;
    gettimeofday(&start,NULL);
    while(fgets(buf,100000,fp)!=NULL){
        hash_insert(&parameter, buf);
    }
    gettimeofday(&end,NULL);
    printf("buid hash time: %dmin %ds\n",(int)(end.tv_sec-start.tv_sec)/60,(int)(end.tv_sec-start.tv_sec)%60);
    printf("please enter sentence\n");
    while(fgets(buf,100000,stdin)!=NULL){
        printf("count:%d\n", hash_find(&parameter, buf));
    }
    //printf("%u\n", hash_size);
    return 0;
}