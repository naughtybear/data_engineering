#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

typedef enum bool{false, true}bool;
const int _MAX_RECORD = 8*1024*1024;
const int _ORIGION_ARRAY_SIZE = 64;
int outFileCnt = 0;

typedef struct parameter{
    bool d,k,c,r,n,t,s;
    char* rb,*rl,*key;
}Par;
Par par;

char* double_array(char *input,int *arraySize)
{
    char *tmp;
    
    tmp = (char*)malloc((*arraySize)*2*sizeof(char));
    strncpy(tmp,input, (*arraySize));
    *arraySize = (*arraySize)*2;
    //printf("a\n");
    free(input);
    //printf("b\n");
    return tmp;
}

int norm_cmp(const void* a, const void* b)
{
    if(!par.k)
        return(strcmp(*(char**)a, *(char**)b));
    char* tmpa = *(char**)a;
    char* tmpb = *(char**)b;
    tmpa = strstr(tmpa, par.key);
    tmpb = strstr(tmpb, par.key);
    if(tmpa == NULL && tmpb != NULL)
        return -1;
    else if(tmpa != NULL && tmpb == NULL)
        return 1;
    else if(tmpa == NULL && tmpb == NULL)
        return 0;
    return strcmp(tmpa,tmpb);
}

int caseins_cmp(const void* a, const void* b)
{
    if(!par.k)
        return(strcasecmp(*(char**)a, *(char**)b));
    char* tmpa = *(char**)a;
    char* tmpb = *(char**)b;
    tmpa = strstr(tmpa, par.key);
    tmpb = strstr(tmpb, par.key);
    if(tmpa == NULL && tmpb != NULL)
        return -1;
    else if(tmpa != NULL && tmpb == NULL)
        return 1;
    else if(tmpa == NULL && tmpb == NULL)
        return 0;
    return strcasecmp(tmpa,tmpb);
}

int num_cmp(const void* a, const void* b)
{
    if(!par.k)
        return(atoi(*(char**)a) - atoi(*(char**)b));
    char* tmpa = *(char**)a;
    char* tmpb = *(char**)b;
    tmpa = strstr(tmpa, par.key);
    tmpb = strstr(tmpb, par.key);
    if(tmpa == NULL && tmpb != NULL)
        return -1;
    else if(tmpa != NULL && tmpb == NULL)
        return 1;
    else if(tmpa == NULL && tmpb == NULL)
        return 0;
    return atoi(tmpa) - atoi(tmpb);
}

int size_cmp(const void* a, const void* b)
{
    if(!par.k)
        return(strlen(*(char**)a) - strlen(*(char**)b));
    char* tmpa = *(char**)a;
    char* tmpb = *(char**)b;
    tmpa = strstr(tmpa, par.key);
    tmpb = strstr(tmpb, par.key);
    if(tmpa == NULL && tmpb != NULL)
        return -1;
    else if(tmpa != NULL && tmpb == NULL)
        return 1;
    else if(tmpa == NULL && tmpb == NULL)
        return 0;
    return strlen(tmpa) - strlen(tmpb);
}

void output_file(char **record)
{
    //printf("ccc\n");
    if(par.n)
        qsort((void*)record, _MAX_RECORD, sizeof(char*), num_cmp);
    else if(par.c)
        qsort((void*)record, _MAX_RECORD, sizeof(char*), caseins_cmp);
    else if(par.s)
        qsort((void*)record, _MAX_RECORD, sizeof(char*), size_cmp);
    else
        qsort((void*)record, _MAX_RECORD, sizeof(char*), norm_cmp);

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
        }
    }
    else{
        for(i=_MAX_RECORD-1; i>=0; i--){
            fprintf(fp,"%s",record[i]);
            free(record[i]);
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

                    // same as line 46 to 77
                    while(start_pos != NULL){
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
                    //same as 79 to 106
                    while(start_pos !=NULL){
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
        while(fgets(buf, 60000, fp[i])!=NULL && real_max_line[i]!=max_line){
            array[i][real_max_line[i]]=strdup(buf);
            real_max_line[i]++;
        }
    }
    int judge = outFileCnt;
    while(judge){
        //printf("a\n");
        int best_idx=-1;
        for(i=0;i<outFileCnt;i++){
            if(real_max_line[i]==0)
                continue;
            if(best_idx == -1){
                best_idx = i;
                continue;
            }
            if(!par.r){
                if(par.n){
                    if(num_cmp(&array[best_idx][now_line[best_idx]],&array[i][now_line[i]])>0)
                        best_idx=i;
                }
                else if(par.c){
                    if(caseins_cmp(&array[best_idx][now_line[best_idx]],&array[i][now_line[i]])>0)
                        best_idx=i;
                }
                else if(par.s){
                    if(size_cmp(&array[best_idx][now_line[best_idx]],&array[i][now_line[i]])>0)
                        best_idx=i;
                }
                else{
                    if(norm_cmp(&array[best_idx][now_line[best_idx]],&array[i][now_line[i]])>0)
                        best_idx=i;
                }
            }
            else{
                if(par.n){
                    if(num_cmp(&array[best_idx][now_line[best_idx]],&array[i][now_line[i]])<0)
                        best_idx=i;
                }
                else if(par.c){
                    if(caseins_cmp(&array[best_idx][now_line[best_idx]],&array[i][now_line[i]])<0)
                        best_idx=i;
                }
                else if(par.s){
                    if(size_cmp(&array[best_idx][now_line[best_idx]],&array[i][now_line[i]])<0)
                        best_idx=i;
                }
                else{
                    if(norm_cmp(&array[best_idx][now_line[best_idx]],&array[i][now_line[i]])<0)
                        best_idx=i;
                }
            }
        }
        fprintf(fp_out,"%s",array[best_idx][now_line[best_idx]]);
        now_line[best_idx]++;
        if(now_line[best_idx] == real_max_line[best_idx]){
            real_max_line[best_idx] = now_line[best_idx] = 0;
            while(fgets(buf, 60000, fp[best_idx])!=NULL && real_max_line[best_idx]!=max_line){
                free(array[best_idx][real_max_line[best_idx]]);
                array[best_idx][real_max_line[best_idx]]=strdup(buf);
                real_max_line[best_idx]++;
            }
            if(real_max_line[best_idx]==0)
                judge--;
        }
    }
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
    
    //init
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
    if(par.n)
        qsort((void*)record, recordCnt, sizeof(char*), num_cmp);
    else if(par.c)
        qsort((void*)record, recordCnt, sizeof(char*), caseins_cmp);
    else if(par.s)
        qsort((void*)record, recordCnt, sizeof(char*), size_cmp);
    else
        qsort((void*)record, recordCnt, sizeof(char*), norm_cmp);
    if(!par.r){
        for(i=0; i<recordCnt; i++){
            //printf("11\n");
            fprintf(fp_out,"%s",record[i]);
            free(record[i]);
        }
    }
    else{
        for(i=recordCnt-1; i>=0; i--){
            fprintf(fp_out,"%s",record[i]);
            free(record[i]);
        }
    }
    fclose(fp_out);
    outFileCnt++;
    exter_sort();
    //printf("%s",record[7]);
    return 0;
}