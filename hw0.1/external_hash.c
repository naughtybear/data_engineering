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
    char *filename;
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
            FILE *fp = fopen(parameter->filename, "a");
            fwrite(string, 1, len, fp);
            fwrite("\0", 1, 1, fp);
            parameter->offset = parameter->offset+len+1;
            parameter->key_current++;
            fclose(fp);
            return;
        }
        FILE *fp = fopen(parameter->filename, "r");
        char buff[100000];
        fseek(fp, parameter->key[key_pos].offset, SEEK_SET);
        fgets(buff, 100000, fp);
        if(strcmp(buff, string) == 0){
           // printf("same\n");
            parameter->key[key_pos].cnt++;
            fclose(fp);
            return;
        }
        fclose(fp);
        key_pos = parameter->key[key_pos].next;
    }
}

unsigned int hash_find(par *parameter, char* string)
{
    unsigned int hash = time33(string);
    unsigned int key_pos = parameter->table[hash];
    FILE *fp = fopen(parameter->filename, "r");

    while(1){
        if(key_pos == 0)
            return 0;
        char buff[100000];
        fseek(fp, parameter->key[key_pos].offset, SEEK_SET);
        fgets(buff, 100000, fp);
        if(strcmp(buff, string) == 0){
           // printf("same\n");
            fclose(fp);
            return parameter->key[key_pos].cnt;
        }
        key_pos = parameter->key[key_pos].next;
    }
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int i;
    unsigned int hash_size = 1<<28, key_size = 1<<28;
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

    parameter.filename = "data.txt";
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
    for(int i=0 ;i<10;i++){
        fp = fopen("content.txt","r");
        while(fgets(buf,100000,fp)!=NULL){
            hash_insert(&parameter, buf);
        }
        fclose(fp);
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