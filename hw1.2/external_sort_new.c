#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <sys/time.h>
#include <unistd.h>

typedef enum bool{false, true}bool;
const int _MAX_RECORD = 1024*1024*1024;
const int _CHUNK_SIZE = 6*1024;
int outFileCnt = 0;

typedef struct parameter{
    bool d,k,c,r,n,t,s;
    char* rb,*rl,*key;
}Par;
Par par;

typedef struct heap_ele{
    char *element;
    int pos;
}heapEle;

int cmp(const void* a, const void* b)
{
    char* tmpa = *(char**)a;
    char* tmpb = *(char**)b;
    if(!par.k){
        if(tmpa == NULL && tmpb != NULL)
            return 1;
        else if(tmpa != NULL && tmpb == NULL)
            return -1;
        else if(tmpa == NULL && tmpb == NULL)
            return 0;

        if(par.n)
            return strcmp(tmpa,tmpb);
        else if(par.c)
            return(atoi(*(char**)a) - atoi(*(char**)b));
        else if(par.s)
            return strlen(tmpa) - strlen(tmpb);
        else
            return strcmp(tmpa,tmpb);
    }
    tmpa = strstr(tmpa, par.key);
    tmpb = strstr(tmpb, par.key);
    if(tmpa == NULL && tmpb != NULL)
        return -1;
    else if(tmpa != NULL && tmpb == NULL)
        return 1;
    else if(tmpa == NULL && tmpb == NULL)
        return 0;

    if(par.n)
        return strcmp(tmpa,tmpb);
    else if(par.c)
        return(atoi(*(char**)a) - atoi(*(char**)b));
    else if(par.s)
        return strlen(tmpa) - strlen(tmpb);
    else
        return strcmp(tmpa,tmpb);
}

void output_file(char **record, int recordCnt)
{

    qsort((void*)record, recordCnt, sizeof(char*), cmp);

    //printf("aaaaa\n");
    int i;
    FILE *fp;
    char filename[15];

    snprintf(filename,15,"out%d.txt",outFileCnt);
    fp = fopen(filename, "w");
    if(!par.r){
        for(i=0; i<recordCnt; i++){
            fprintf(fp,"%s",record[i]);
            free(record[i]);
            record[i] = NULL;
        }
    }
    else{
        for(i=recordCnt-1; i>=0; i--){
            fprintf(fp,"%s",record[i]);
            free(record[i]);
            record[i] = NULL;
        }
    }
    //printf("bbbb\n");
    fclose(fp);
    outFileCnt++;
}

int split_file(FILE *fp, char **record, char *buf, int buf_left, int judge, int max_buf_size)
{
    char *start = buf + buf_left;
    int buf_size, recordCnt = 0;

    while(judge == 1 || judge == 2 || (buf_size = (int)fread(start, 1, max_buf_size - buf_left, fp)) == max_buf_size - buf_left ){
        if(judge == 1){
            buf[buf_left] = '\0';
        }
        start = buf;
        buf[max_buf_size] = '\0';
        if(!par.d || (par.rb == NULL && par.rl)){
            char *end;
            while((end = strstr(start, "\n")) != NULL){
                int len = end - start + 1;
                
                record[recordCnt] = (char*)malloc(len+1);
                strncpy(record[recordCnt], start, len);
                record[recordCnt][len] = '\0';
                recordCnt++;
                start = end+1;
            }
        }
        else{
            int rblen = strlen(par.rb);
            start = strstr(buf, par.rb);
            //printf("bbb\n");
            if(par.rl == NULL){
                char *end;
                while((end = strstr(start+rblen, par.rb)) != NULL){
                    //printf("haha\n");
                    int len = end - start;
                    
                    record[recordCnt] = (char*)malloc(len+1);
                    strncpy(record[recordCnt], start, len);
                    record[recordCnt][len] = '\0';
                    recordCnt++;
                    start = end;
                }
            }
            else{
                char *end, *tmp;
                int rllen = strlen(par.rl);
                while((tmp = strstr(start, par.rb)) != NULL){
                    if((end = strstr(tmp, par.rl)) != NULL){
                        int len = end - tmp + rllen;

                        record[recordCnt] = (char*)malloc(len+1);
                        strncpy(record[recordCnt], tmp, len);
                        record[recordCnt][len] = '\0';
                        recordCnt++;
                        start = end+rllen;
                    }
                    else{
                        start = tmp;
                        break;
                    }
                }
                if(tmp == NULL)
                    start = buf + max_buf_size;
            }
        }
        if(judge == 1 || (judge == 2 && max_buf_size > buf_left + buf_size)){
            //judge == 1 代表第一次讀原始檔案所剩的data
            //judge == 2 代表讀分檔後的檔所剩的data
            int len = buf + max_buf_size - start;

            record[recordCnt] = (char*)malloc(len+1);
            strncpy(record[recordCnt], start, len);
            record[recordCnt][len] = '\0';
            recordCnt++;
            buf_left = 0;
        }

        if(buf + max_buf_size > start){
            buf_left = buf +max_buf_size -start;
            strncpy(buf, start, buf_left);
        }
        else
            buf_left = 0;
        
        if(judge == 2)
            return buf_left;
        output_file(record, recordCnt);
        printf("reccnt:%d\n",recordCnt);
        recordCnt = 0;
        if(judge == 1)
            return buf_left;
        start = buf + buf_left;
    }
    buf_left = buf_left +buf_size; //一個檔案最後不完整一個chunk， 算出left給下個檔案用
    //printf("buf_left:%d\n",buf_left);
    return buf_left;
}

int tree_cmp(heapEle *win_tree,int idx1,int idx2)
{
    return cmp(&(win_tree[idx1].element),&(win_tree[idx2].element));
}

void min_tree_build(heapEle *win_tree,int heapSize)
{
    int start = heapSize,i;
    heapEle loser[heapSize];
    
    for(i = heapSize/2; i<heapSize;i++){
        loser[i].element = win_tree[i].element;
        loser[i].pos = win_tree[i].pos;
    }
    for(i = heapSize-1; i > 1;i-=2){
        if(tree_cmp(loser, i, i-1)>0){
            win_tree[i/2] = loser[i];
            loser[i/2] = loser[i-1];
        }
        else{
            win_tree[i/2] = loser[i-1];
            loser[i/2] = loser[i];   
        }
    }
    win_tree[0] = loser[1];
}

heapEle get_min(heapEle *win_tree, int heapSize, char ***array, int *now_line, int *buf_left , int max_size, FILE **fp)
{
    heapEle returnval = win_tree[0];
    int current = returnval.pos+(heapSize/2);
    char buf[60000];
    
    if(returnval.element == NULL){
        return returnval;
    }
    
    now_line[returnval.pos]++;
    if(array[returnval.pos][now_line[returnval.pos]] == NULL){
        now_line[returnval.pos] = 0;
        buf_left[returnval.pos] = split_file(fp[returnval.pos], array[returnval.pos], buf, buf_left[returnval.pos], 2, max_size);
        if(buf_left[returnval.pos] == 0){
            array[returnval.pos][0]=NULL;
        }
    }
    win_tree[current].element = array[returnval.pos][now_line[returnval.pos]];
    win_tree[0] = win_tree[current];
    current/2;
    while(current){
        if(tree_cmp(win_tree,current, 0)<0){
            heapEle tmp;
            tmp = win_tree[current];
            win_tree[current] = win_tree[0];
            win_tree[0] = tmp;
        }
        current/=2;
    }
    return returnval;
}

void exter_sort()
{
    int i,j;
    char filename[15];
    char buf[60000];
    int max_size = _CHUNK_SIZE/outFileCnt;
    int buf_left[outFileCnt];
    int now_line[outFileCnt];
    FILE *fp[outFileCnt],*fp_out;
    char ***array=(char***)malloc(outFileCnt*sizeof(char**));

    fp_out = fopen("total_out.txt", "w");
    if(fp_out==NULL){
        printf("open fail\n");
        exit(0);
    }
    for(int i=0;i<outFileCnt;i++){
        now_line[i] = 0;
        array[i] = (char**)malloc(max_size*sizeof(char*));
        snprintf(filename,15,"out%d.txt",i);
        fp[i] = fopen(filename, "r");
        buf_left[i] = split_file(fp[i], array[i], buf, buf_left[i], 2, max_size);
    }

    int heapSize=1;
    while(heapSize/2-1<outFileCnt)
        heapSize*=2;
    heapEle win_tree[heapSize],result;
    int start = heapSize/2;

    for(i = 0;i<outFileCnt;i++){
        heapEle tmp;
        tmp.element = array[i][0];
        tmp.pos = i;
        win_tree[i+start] = tmp;
    }

    for(i=start + outFileCnt-1; i<heapSize;i++){
        heapEle tmp;
        tmp.element = NULL;
        tmp.pos = i-start;
        win_tree[i] = tmp;
    }

    min_tree_build(win_tree, heapSize);
    result = get_min(win_tree,heapSize, array, now_line, buf_left, max_size, fp);
    while(result.element != NULL){
        fprintf(fp_out,"%s",result.element);
        result = get_min(win_tree,heapSize, array, now_line, buf_left, max_size, fp);
    }
    fclose(fp_out);
}

int main(int argc,char *argv[])
{
    int i=0,j=0,k=0;
    char* pt=NULL;
    char file[20][100];
    int fileCnt = 0,recordCnt = 0, buf_left = 0;
    char **record = (char**)malloc(_MAX_RECORD*sizeof(char*));
    FILE *fp,*fp_out;
    struct timeval start;
    struct timeval end;
    char *buf;
    
    //init
    gettimeofday(&start,NULL);
    par.c=par.d=par.k=par.n=par.r=false;
    par.rb=par.rl=NULL;
    buf = (char*)malloc(_CHUNK_SIZE+1);
    
    //處理命令列參數
    for(i=1;i<argc;i++){
        if(argv[i][0]=='-'&&argv[i][2]=='\0'){
            if(argv[i][1]=='d'){
                par.d=true;
                if(strncmp(argv[i+1],"rb:",3)==0){
                    pt=argv[i+1]+3;
                    par.rb=strdup(pt);
                    i+=1;
                    if(strncmp(argv[i+1],"rl:",3)==0){
                        pt=argv[i+1]+3;
                        par.rl=strdup(pt);
                        i++;
                    } 
                }
                else if(strncmp(argv[i+1],"rl:",3)==0){
                    pt=argv[i+1]+3;
                    par.rl=strdup(pt);
                    i+=1;
                    if(strncmp(argv[i+1],"rb:",3)==0){
                        pt=argv[i+1]+3;
                        par.rb=strdup(pt);
                        i++;
                    }
                } 
            }
            else if(argv[i][1]=='k'){
                par.k=true;
                par.key=strdup(argv[i+1]);
                i++;
            }
            else if(argv[i][1]=='c'){
                par.c=true;
            }
            else if(argv[i][1]=='r'){
                par.r=true;
            }
            else if(argv[i][1]=='n'){
                par.n=true;
            }
            else if(argv[i][1]=='t'){
                par.t=true;
            }
            else if(argv[i][1]=='s'){
                par.s=true;
            } 
        }
        else{
            strcpy(file[fileCnt],argv[i]);
            fileCnt++;
        }
    }
    //printf("%s\n",file[0]);
    // read file
    for(i = 0;i < fileCnt;i++){
        fp = fopen(file[i], "r");
        if(fp == NULL){
            printf("open fail\n");
            exit(0);
        }
        printf("open:%s\n",file[i]);
        buf_left = split_file(fp, record, buf, buf_left, 0, _CHUNK_SIZE);
        if(i != fileCnt-1)
            fclose(fp);
    }
    split_file(fp, record, buf, buf_left, 1, _CHUNK_SIZE);
    fclose(fp);
    gettimeofday(&end,NULL);
    printf("split file time: %dmin %ds\n",(int)(end.tv_sec-start.tv_sec)/60,(int)(end.tv_sec-start.tv_sec)%60);
    start = end;
    exter_sort();
    gettimeofday(&end,NULL);
    printf("external sort time: %dmin %ds\n",(int)(end.tv_sec-start.tv_sec)/60,(int)(end.tv_sec-start.tv_sec)%60);
    //printf("%s",record[7]);
    return 0;
}