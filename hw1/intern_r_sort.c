#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum bool{false, true}bool;
const int max_item=100;
int max_record=10000;

//判斷參數
typedef struct parameter{
    bool d,k,c,r,n,t,s;
}Par;

// for sort array
typedef struct element{
    char *key;
    char **pointer;
}Elem;

char*** create_3d_array(int a, int b){
    int i,j,k;
    char ***tmp;
    tmp=(char***)malloc(a*sizeof(char**));
    for(i=0;i<a;i++){
        tmp[i]=(char**)malloc(b*sizeof(char*));
    }
    for(i=0;i<a;i++){
        for(j=0;j<b;j++)
            tmp[i][j]=NULL;
    }
    return tmp;
}

char*** double_record(char ***array,int a,int b){
    char*** tmp;
    int i,j,k;
    tmp=create_3d_array(2*a,b);
    for(i=0;i<a;i++)
        for(j=0;j<b;j++)
            tmp[i][j]=array[i][j];
    return tmp;
}

int time_pat(const void *a,const void *b){ //-t cmp function
    Elem newa=*(Elem*)a;
    Elem newb=*(Elem*)b;
    int year_a=0,month_a=0,day_a=0,year_b=0,month_b=0,day_b=0;
    char *delim=":/";
    char *tmp;

    tmp=strdup(newa.key);
    strtok(tmp,delim);
    year_a=atoi(strtok(NULL,delim));
    month_a=atoi(strtok(NULL,delim));
    day_a=atoi(strtok(NULL,delim));

    tmp=strdup(newb.key);
    strtok(tmp,delim);
    year_b=atoi(strtok(NULL,delim));
    month_b=atoi(strtok(NULL,delim));
    day_b=atoi(strtok(NULL,delim));

    if(year_a!=year_b)
        return year_a-year_b;
    if(month_a!=month_b)
        return month_a-month_b;
    return day_a-day_b;
}

int size_order_no_key(const void *a,const void *b){ //-s 但沒 -k
    Elem newa=*(Elem*)a;
    Elem newb=*(Elem*)b;
    int size_a=0,size_b=0;
    int i;
    
    for(i=0;newa.pointer[i]!=NULL;i++){
        size_a+=strlen(newa.pointer[i]);
    }

    for(i=0;newb.pointer[i]!=NULL;i++){
        size_b+=strlen(newb.pointer[i]);
    }

    return size_a-size_b;
}

int size_order_with_key(const void *a,const void *b){ //-s && -k
    Elem newa=*(Elem*)a;
    Elem newb=*(Elem*)b;
    if(newa.key==NULL&&newb.key==NULL)
        return 0;
    if(newa.key==NULL&&newb.key!=NULL)
        return -1;
    if(newa.key!=NULL&&newb.key==NULL)
        return 1;
    return strlen(newa.key)-strlen(newb.key);
}

int norm(const void *a,const void *b){ //normal
    return(strcmp((*(Elem*)a).key,(*(Elem*)b).key));
}

int case_insen(const void *a,const void *b){ //不管大小寫
    return(strcasecmp((*(Elem*)a).key,(*(Elem*)b).key));
}

int num_com(const void *a,const void *b){ //numerical comparison
    return(atoi((*(Elem*)a).key)-atoi((*(Elem*)b).key));
}

int main(int argc, char* argv[])
{
    Par par; //parameter
    int i=0,j=0,k=0;
    char* rb,*rl,*key;
    char* pt=NULL;
    char file[10][100],buf[60000];
    int file_count=0;
    FILE *fp_in,*fp_out;
    char ***record;
    int record_count=0;
    
    //init
    par.c=par.d=par.k=par.n=par.r=false;
    rb=rl=NULL;
    
    //處理命令列參數
    for(i=1;i<argc;i++){
        if(argv[i][0]=='-'&&argv[i][2]=='\0'){
            if(argv[i][1]=='d'){
                par.d=true;
                if(strncmp(argv[i+1],"rb:",3)==0){
                    pt=argv[i+1]+3;
                    rb=strdup(pt);
                    i+=1;
                    if(strncmp(argv[i+1],"rl:",3)==0){
                        pt=argv[i+1]+3;
                        rl=strdup(pt);
                        i++;
                    } 
                }
                else if(strncmp(argv[i+1],"rl:",3)==0){
                    pt=argv[i+1]+3;
                    rl=strdup(pt);
                    i+=1;
                    if(strncmp(argv[i+1],"rb:",3)==0){
                        pt=argv[i+1]+3;
                        rb=strdup(pt);
                        i++;
                    }
                } 
            }
            else if(argv[i][1]=='k'){
                par.k=true;
                key=strdup(argv[i+1]);
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
            strcpy(file[file_count],argv[i]);
            file_count++;
        }
    }

    //讀檔並將data分成record
    record=create_3d_array(max_record,max_item);
    for(i=0;i<file_count;i++){
        fp_in=fopen(file[i],"r");
        if(fp_in==NULL){
            printf("open failed\n");
            exit(0);
        }
        //一個record只有一行
        if(!par.d||(rb==NULL&&rl==NULL)){
            while(fgets(buf,60000,fp_in)!=NULL){
                if(buf[0]==' '||buf[0]=='\t'){
                    char *tmp;
                    tmp=(char*)malloc(60000*sizeof(char));
                    strcpy(tmp,record[record_count-1][0]);
                    strcat(tmp,buf);
                    free(record[record_count-1][0]);
                    record[record_count-1][0]=tmp;
                }
                else{
                    record[record_count][0]=strdup(buf);
                    record_count++;
                }
                if(record_count==max_record-1){
                    record=double_record(record,max_record,max_item);
                    max_record*=2;
                }
            }
        }
        //一個record不只一行
        else{
            //state=0表示不在record區域 state=1表示在record區域
            int state=0,item_count=0;
            while(fgets(buf,60000,fp_in)!=NULL){
                if(strncmp(rb,buf,strlen(rb))==0&&buf[strlen(rb)]=='\n'){
                    if(state==1){
                        record_count++;
                        item_count=0;
                    }
                    state=1;
                    continue;
                }
                if(rl!=NULL&&strncmp(rl,buf,strlen(rl))==0&&buf[strlen(rl)]=='\n'){
                    state=0;
                    record_count++;
                    item_count=0;
                }
                if(state==1){
                    if(buf[0]==' '||buf[0]=='\t'){
                        char *tmp;
                        tmp=(char*)malloc(60000*sizeof(char));
                        strcpy(tmp,record[record_count][item_count-1]);
                        strcat(tmp,buf);
                        free(record[record_count][item_count-1]);
                        record[record_count][item_count-1]=tmp;
                    }
                    else{
                        record[record_count][item_count]=strdup(buf);
                        item_count++;
                    }
                }
                if(record_count==max_record-1){
                    char ***tmp;
                    tmp=double_record(record,max_record,max_item);
                    record=tmp;
                    max_record*=2;
                }
                //printf("%d\n",record_count);
            }
        }
    }

    //將record放入要sort的structure
    Elem sort_array[record_count];
    for(i=0;i<record_count;i++){
        sort_array[i].pointer=record[i];
        if(!par.k){
            sort_array[i].key=record[i][0];
        }
        else{
            bool flag=false;
            //printf("%s",record[i][0]);
            for(j=0;record[i][j]!=NULL;j++){
                //printf("%s\n",key);
                if(strncmp(key,record[i][j],strlen(key))==0
                    &&record[i][j][strlen(key)]==':'){
                    sort_array[i].key=record[i][j];
                    //printf("%s",sort_array[i].key);
                    flag=true;
                }
            }
            if(flag=false){
                sort_array[i].key=NULL;
            }
        }
    }

    if(par.c&&!par.n){
        qsort(sort_array,record_count,sizeof(sort_array[0]),case_insen);
    }
    else if(par.n){
        qsort(sort_array,record_count,sizeof(sort_array[0]),num_com);
    }
    else if(par.s && !par.k){
        qsort(sort_array,record_count,sizeof(sort_array[0]),size_order_no_key);
    }
    else if(par.s && par.k){
        qsort(sort_array,record_count,sizeof(sort_array[0]),size_order_with_key);
    }
    else if(par.t){
        qsort(sort_array,record_count,sizeof(sort_array[0]),time_pat);
    }
    else{
        qsort(sort_array,record_count,sizeof(sort_array[0]),norm);
    }
    for(i=0;i<record_count;i++){
        record[i]=sort_array[i].pointer;
    }
    fp_out=fopen("out.txt","w");
    if(par.r){
        for(i=record_count-1;i>=0;i--){
            for(j=0;record[i][j]!=NULL;j++)
                fprintf(fp_out,"%s",record[i][j]);
            //fprintf(fp_out,"%s",record[i][0]);
        }
    }
    else{
        for(i=0;i<record_count;i++){
            for(j=0;record[i][j]!=NULL;j++)
                fprintf(fp_out,"%s",record[i][j]);
        }
    }
    return 0;
}