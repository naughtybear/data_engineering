#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <sys/time.h>
#include <unistd.h>

typedef enum bool{false, true}bool;
const int _MAX_RECORD = 4*1024*1024;
const int _ORIGION_ARRAY_SIZE = 64;
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

char* double_array(char *input,int *arraySize)
{
    char *tmp;
    
    tmp = (char*)malloc((*arraySize)*2*sizeof(char));
    strncpy(tmp,input, (*arraySize));
    *arraySize = (*arraySize)*2;
    //printf("a\n");
    free(input);
    input = NULL;
    //printf("b\n");
    return tmp;
}

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

void output_file(char **record)
{

    qsort((void*)record, _MAX_RECORD, sizeof(char*), cmp);

    //printf("aaaaa\n");
    int i;
    FILE *fp;
    char filename[15];

    snprintf(filename,15,"out%d.txt",outFileCnt);
    fp = fopen(filename, "w");
    if(!par.r){
        for(i=0; i<_MAX_RECORD; i++){
            fprintf(fp,"%s",record[i]);
            free(record[i]);
            record[i] = NULL;
        }
    }
    else{
        for(i=_MAX_RECORD-1; i>=0; i--){
            fprintf(fp,"%s",record[i]);
            free(record[i]);
            record[i] = NULL;
        }
    }
    //printf("bbbb\n");
    fclose(fp);
    outFileCnt++;
}

int read_record_withd(FILE *fp,char **record,int recordCnt)
{
    char buf[_ORIGION_ARRAY_SIZE];
    bool judge = false;
    char *oneRecord = (char*)malloc(_ORIGION_ARRAY_SIZE);
    int arrayCurSize = 0, arrayMaxSize = _ORIGION_ARRAY_SIZE;
    
    oneRecord[0] = '\0';
    while(fgets(buf,_ORIGION_ARRAY_SIZE,fp) != NULL){
        //printf("%d\n",recordCnt);
        if(judge == false){
            char *start_pos = strstr(buf, par.rb);
            char *end_pos;
            
            while(start_pos != NULL){
                strncat(oneRecord, start_pos, strlen(par.rb));
                arrayCurSize+=strlen(par.rb);
                start_pos = start_pos + strlen(par.rb);
                if(par.rl != NULL){
                    end_pos = strstr(start_pos, par.rl);
                    if(end_pos!=NULL){
                        int len = end_pos - start_pos;
                        
                        if(len+arrayCurSize >= arrayMaxSize){
                            oneRecord = double_array(oneRecord, &arrayMaxSize);
                        }
                        strncat(oneRecord, start_pos, len);
                        arrayCurSize = len+arrayCurSize;
                        start_pos = strstr(end_pos+strlen(par.rl), par.rb);
                        judge = false;
                        record[recordCnt] = oneRecord;
                        oneRecord = (char*)malloc(_ORIGION_ARRAY_SIZE);
                        oneRecord[0] = '\0';
                        arrayMaxSize = _ORIGION_ARRAY_SIZE; 
                        arrayCurSize = 0;
                        recordCnt++;
                        if(recordCnt == _MAX_RECORD){
                            output_file(record);
                            recordCnt = 0;
                        }
                    }
                    else{
                        int len = strlen(start_pos);

                        if(len+arrayCurSize >=arrayMaxSize){
                            oneRecord = double_array(oneRecord, &arrayMaxSize);
                        }
                        strncat(oneRecord, start_pos, len);
                        arrayCurSize = len+arrayCurSize;
                        start_pos = NULL;
                        judge = true;
                    }
                }
                else{
                    end_pos = strstr(start_pos, par.rb);
                    if(end_pos!=NULL){
                        int len = end_pos - start_pos;
                        
                        if(len+arrayCurSize >= arrayMaxSize){
                            oneRecord = double_array(oneRecord, &arrayMaxSize);
                        }
                        strncat(oneRecord, start_pos, len);
                        arrayCurSize = len+arrayCurSize;
                        start_pos = end_pos;
                        judge = true;
                        record[recordCnt] = oneRecord;
                        oneRecord = (char*)malloc(_ORIGION_ARRAY_SIZE);
                        oneRecord[0] = '\0';
                        arrayMaxSize = _ORIGION_ARRAY_SIZE;
                        arrayCurSize = 0;
                        recordCnt++;
                        if(recordCnt == _MAX_RECORD){
                            output_file(record);
                            recordCnt = 0;
                        }
                    }
                    else{
                        int len = strlen(start_pos);
                        if(len+arrayCurSize >=arrayMaxSize){
                            oneRecord = double_array(oneRecord, &arrayMaxSize);
                        }
                        strncat(oneRecord, start_pos, len);
                        arrayCurSize = len+arrayCurSize;
                        start_pos = NULL;
                        judge = true;
                    }
                }
            }
        }
        // judge == true
        else{
            if(par.rl != NULL){
                char *start_pos;
                char *end_pos = strstr(buf, par.rl);
                
                if(end_pos!= NULL){
                    int len = end_pos - buf;
                    if(len + arrayCurSize >= arrayMaxSize){
                        oneRecord = double_array(oneRecord, &arrayMaxSize);
                    }
                    strncat(oneRecord, buf, len);
                    //printf("%s",oneRecord);
                    //printf("\n-----------------------------\n");
                    arrayCurSize = len + arrayCurSize;
                    start_pos = strstr(end_pos+strlen(par.rl), par.rb);
                    judge = false;
                    record[recordCnt] = oneRecord;
                    oneRecord = (char*)malloc(_ORIGION_ARRAY_SIZE);
                    oneRecord[0] = '\0';
                    arrayMaxSize = _ORIGION_ARRAY_SIZE; 
                    arrayCurSize = 0;
                    recordCnt++;
                    if(recordCnt == _MAX_RECORD){
                        output_file(record);
                        recordCnt = 0;
                    }

                    // same as line 148 to 172
                    while(start_pos != NULL){
                        strncat(oneRecord, start_pos, strlen(par.rb));
                        arrayCurSize+=strlen(par.rb);
                        start_pos = start_pos + strlen(par.rb);
                        end_pos = strstr(start_pos, par.rl);
                        if(end_pos!=NULL){
                            int len = end_pos - start_pos;
                            
                            if(len+arrayCurSize >= arrayMaxSize){
                                oneRecord = double_array(oneRecord, &arrayMaxSize);
                            }
                            strncat(oneRecord, start_pos, len);
                            arrayCurSize = len+arrayCurSize;
                            start_pos = strstr(end_pos+strlen(par.rl), par.rb);
                            judge = false;
                            record[recordCnt] = oneRecord;
                            oneRecord = (char*)malloc(_ORIGION_ARRAY_SIZE);
                            oneRecord[0] = '\0';
                            arrayMaxSize = _ORIGION_ARRAY_SIZE; 
                            arrayCurSize = 0;
                            recordCnt++;
                            if(recordCnt == _MAX_RECORD){
                                output_file(record);
                                recordCnt = 0;
                            }
                        }
                        else{
                            int len = strlen(start_pos);

                            if(len+arrayCurSize >=arrayMaxSize){
                                oneRecord = double_array(oneRecord, &arrayMaxSize);
                            }
                            strncat(oneRecord, start_pos, len);
                            arrayCurSize = len+arrayCurSize;
                            start_pos = NULL;
                            judge = true;
                        }
                    }
                }
                // end_pos ==NULL
                else{
                    int len = strlen(buf); 
                    
                    if(len + arrayCurSize >= arrayMaxSize){
                        oneRecord = double_array(oneRecord, &arrayMaxSize);
                    }
                    strcat(oneRecord, buf);
                    arrayCurSize = len + arrayCurSize;
                }
            }
            // rl == NULL
            else{
                char *start_pos;
                char *end_pos = strstr(buf, par.rb);
                
                if(end_pos != NULL){
                    int len = end_pos - buf;
                    if(len + arrayCurSize >= arrayMaxSize){
                        oneRecord = double_array(oneRecord, &arrayMaxSize);
                    }
                    strncat(oneRecord, buf, len);
                    arrayCurSize = len + arrayCurSize;
                    start_pos = strstr(end_pos+strlen(par.rb), par.rb);
                    judge = true;
                    record[recordCnt] = oneRecord;
                    oneRecord = (char*)malloc(_ORIGION_ARRAY_SIZE);
                    oneRecord[0] = '\0';
                    arrayMaxSize = _ORIGION_ARRAY_SIZE; 
                    arrayCurSize = 0;
                    recordCnt++;
                    if(recordCnt == _MAX_RECORD){
                        output_file(record);
                        recordCnt = 0;
                    }
                    strncat(oneRecord, end_pos, strlen(par.rb));
                    arrayCurSize+=strlen(par.rb);
                    //same as 185 to 218
                    while(start_pos !=NULL){
                        strncat(oneRecord, start_pos, strlen(par.rb));
                        arrayCurSize+=strlen(par.rb);
                        start_pos = start_pos + strlen(par.rb);
                        end_pos = strstr(start_pos, par.rb);
                        if(end_pos!=NULL){
                            int len = end_pos - start_pos;
                            
                            if(len+arrayCurSize >= arrayMaxSize){
                                oneRecord = double_array(oneRecord, &arrayMaxSize);
                            }
                            strncat(oneRecord, start_pos, len);
                            arrayCurSize = len+arrayCurSize;
                            start_pos = end_pos;
                            judge = true;
                            record[recordCnt] = oneRecord;
                            oneRecord = (char*)malloc(_ORIGION_ARRAY_SIZE);
                            oneRecord[0] = '\0';
                            arrayMaxSize = _ORIGION_ARRAY_SIZE;
                            arrayCurSize = 0;
                            recordCnt++;
                            if(recordCnt == _MAX_RECORD){
                                output_file(record);
                                recordCnt = 0;
                            }
                        }
                        else{
                            int len = strlen(start_pos);

                            if(len+arrayCurSize >=arrayMaxSize){
                                oneRecord = double_array(oneRecord, &arrayMaxSize);
                            }
                            strncat(oneRecord, start_pos, len);
                            arrayCurSize = len+arrayCurSize;
                            start_pos = NULL;
                            judge = true;
                        }
                    }
                }
                // end_pos == null
                else{
                    int len = strlen(buf); 
                    
                    if(len + arrayCurSize >= arrayMaxSize){
                        oneRecord = double_array(oneRecord, &arrayMaxSize);
                    }
                    strcat(oneRecord, buf);
                    arrayCurSize = len + arrayCurSize;
                }
            }
        }
    }
    record[recordCnt] = oneRecord;
    recordCnt++;
    return recordCnt;
}

int read_record_withoutd(FILE *fp,char **record,int recordCnt)
{
    char buf[_ORIGION_ARRAY_SIZE];
    bool judge = false;
    char *oneRecord = (char*)malloc(_ORIGION_ARRAY_SIZE);
    int arrayCurSize = 0, arrayMaxSize = _ORIGION_ARRAY_SIZE;

    oneRecord[0] = '\0';
    while(fgets(buf, _ORIGION_ARRAY_SIZE, fp) != NULL){
        int len = strlen(buf);
        if(len + arrayCurSize >= arrayMaxSize){
            oneRecord = double_array(oneRecord, &arrayMaxSize);
        }
        strncat(oneRecord, buf, len+1);
        arrayCurSize = len + arrayCurSize;
        if(len != _ORIGION_ARRAY_SIZE-1){
            record[recordCnt] = oneRecord;
            oneRecord = (char*)malloc(_ORIGION_ARRAY_SIZE);
            oneRecord[0] = '\0';
            arrayMaxSize = _ORIGION_ARRAY_SIZE;
            arrayCurSize = 0;
            recordCnt++;
            if(recordCnt == _MAX_RECORD){
                output_file(record);
                recordCnt = 0;
            }
        }
        //printf("%d\n",recordCnt);
    }
    return recordCnt;
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

heapEle get_min(heapEle *win_tree, int heapSize, char ***array, int *now_line, int *real_max_line , int max_line, FILE **fp)
{
    heapEle returnval = win_tree[0];
    int current = returnval.pos+(heapSize/2);
    char buf[60000];
    
    if(returnval.element == NULL){
        return returnval;
    }
    
    now_line[returnval.pos]++;
    if(now_line[returnval.pos]==(real_max_line[returnval.pos])){
        real_max_line[returnval.pos] = now_line[returnval.pos] = 0;
        while(real_max_line[returnval.pos]!=max_line && fgets(buf, 60000, fp[returnval.pos])!=NULL ){
            free(array[returnval.pos][real_max_line[returnval.pos]]);
            array[returnval.pos][real_max_line[returnval.pos]]=strdup(buf);
            real_max_line[returnval.pos]++;
        }
        if(real_max_line[returnval.pos]==0){
            array[returnval.pos][real_max_line[returnval.pos]]=NULL;
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
    int max_line = _MAX_RECORD/outFileCnt;
    int real_max_line[outFileCnt];
    int now_line[outFileCnt];
    FILE *fp[outFileCnt],*fp_out;
    char ***array=(char***)malloc(outFileCnt*sizeof(char**));

    fp_out = fopen("total_out.txt", "w");
    if(fp_out==NULL){
        printf("open fail\n");
        exit(0);
    }
    for(int i=0;i<outFileCnt;i++){
        now_line[i] = real_max_line[i] =0;
        array[i] = (char**)malloc(max_line*sizeof(char*));
        snprintf(filename,15,"out%d.txt",i);
        fp[i] = fopen(filename, "r");
        while(real_max_line[i]!=max_line && fgets(buf, 60000, fp[i])!=NULL){
            array[i][real_max_line[i]]=strdup(buf);
            real_max_line[i]++;
        }
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
    result = get_min(win_tree,heapSize, array, now_line, real_max_line, max_line, fp);
    while(result.element != NULL){
        fprintf(fp_out,"%s",result.element);
        result = get_min(win_tree,heapSize, array, now_line, real_max_line, max_line, fp);
    }
    fclose(fp_out);
}

int main(int argc,char *argv[])
{
    int i=0,j=0,k=0;
    //char* rb,*rl,*key;
    char* pt=NULL;
    char file[20][100];
    int fileCnt = 0,recordCnt = 0;
    char **record = (char**)malloc(_MAX_RECORD*sizeof(char*));
    FILE *fp,*fp_out;
    struct timeval start;
    struct timeval end;
    
    //init
    gettimeofday(&start,NULL);
    setlocale(LC_ALL, "");
    par.c=par.d=par.k=par.n=par.r=false;
    par.rb=par.rl=NULL;
    
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
    printf("%s\n",file[0]);
    // read file
    for(i = 0;i < fileCnt;i++){
        fp = fopen(file[i], "r");
        if(fp == NULL){
            printf("open fail\n");
            exit(0);
        }
        printf("111\n");
        if(!par.d || (par.rb == NULL && par.rl == NULL))
            recordCnt = read_record_withoutd(fp, record, recordCnt);
        else
            recordCnt = read_record_withd(fp, record, recordCnt);
        fclose(fp);
    }
    char filename[15];
    snprintf(filename,15,"out%d.txt",outFileCnt);
    fp_out = fopen(filename, "w"); 
    qsort((void*)record, recordCnt, sizeof(char*), cmp);
    if(!par.r){
        for(i=0; i<recordCnt; i++){
            //printf("11\n");
            fprintf(fp_out,"%s",record[i]);
            free(record[i]);
            record[i] = NULL; 
        }
    }
    else{
        for(i=recordCnt-1; i>=0; i--){
            fprintf(fp_out,"%s",record[i]);
            free(record[i]);
            record[i] = NULL;
        }
    }
    fclose(fp_out);
    outFileCnt++;
    gettimeofday(&end,NULL);
    printf("split file time: %dmin %ds\n",(int)(end.tv_sec-start.tv_sec)/60,(int)(end.tv_sec-start.tv_sec)%60);
    start = end;
    exter_sort();
    gettimeofday(&end,NULL);
    printf("external sort time: %dmin %ds\n",(int)(end.tv_sec-start.tv_sec)/60,(int)(end.tv_sec-start.tv_sec)%60);
    //printf("%s",record[7]);
    return 0;
}