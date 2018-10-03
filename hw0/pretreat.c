#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int sentence_count=17000000;
int count_total=0;
void cut_sentence(char tmp)
{
    char *input[5];
    char *per="。 ",*com="，",*que="？",*exc="！";
    int i;
    char *pt;
    char buf[60000];
    FILE *fp, *fp_top, *fp_b;
    int count_chi;

    for(i=0;i<4;i++){
        input[i]=malloc(10000);
    }
    input[4]=malloc(150000);
    if(tmp=='0'){
        fp = fopen("ettoday0.rec","r");
        fp_top = fopen("topic.txt","w");
        fp_b = fopen("content.txt","w");
        //fp_url = fopen("url.txt", "w");
    }
    else{
        strcpy(buf,"ettoday");
        buf[7]=tmp;
        strcat(buf,".rec\0");
        fp = fopen(buf,"r");
        fp_top = fopen("topic.txt","a");
        fp_b = fopen("content.txt","a");
        //fp_url = fopen("url.txt", "a");
    }
    
    
    //fgets(input[0],80000,fp);
    while(fgets(input[0],10000,fp)!=NULL){
        fgets(input[1],10000,fp);
        fgets(input[2],10000,fp);
        fgets(input[3],10000,fp);
        fgets(input[4],150000,fp);
        pt=input[1];
        pt+=3;
        if(pt[0]=='h')
            fprintf(fp_top,"%s",pt);
        pt = input[2];
        pt+=3;
        //if(*pt<0&&*(pt+1)<0&&*(pt+2)<0)
            fprintf(fp_top,"%s",pt);
        pt = input[4];
        while(1){
            if(*pt==' ')
                pt++;
            else
                break;
        }
        //printf("%s",input[2]);
        memset(buf,'\0',60000);
        count_chi = 0;
        while(1){
            if(*pt=='\0')
                break;
            if(*pt<0){
                if(strncmp(pt,per,3)==0
                ||strncmp(pt,com,3)==0
                ||strncmp(pt,que,3)==0
                ||strncmp(pt,exc,3)==0){
                    //printf("2");
                    if(count_chi>5&&buf[0]<0&&buf[1]<0&&buf[2]<0&&strncmp(buf,"一",3)>=0){
                        //printf("%d\n",(int)strlen(buf));
                        fprintf(fp_b,"%s %6d\n",buf,count_total);
                    }
                    memset(buf,'\0',60000);
                    //printf("qq\n");
                    //fprintf(fp_b,"\n");
                    count_chi=0;
                }
                else{
                    strncat(buf,pt,3);
                    count_chi++;
                    //printf("%s\n",buf);
                    //fwrite(pt, 1, 3, fp_b);
                }
                pt+=3;
            }
            else if(*pt>0){
                if(count_chi==0&&*pt==' '){
                    pt++;
                    continue;
                }
                if(*pt=='\n'){
                    if(count_chi>5)
                        fprintf(fp_b,"%s %6d\n",buf,count_total);
                    memset(buf,'\0',60000);
                    count_chi=0;
                }
                strncat(buf,pt,1);
                //fwrite(pt, 1, 1, fp_b);
                pt++;
            }
            //printf("%s\n",buf);
        }
        //printf("%s",input[4]);
        count_total++;
    }
    fclose(fp);
    fclose(fp_b);
    fclose(fp_top);
    for(int i=0;i<5;i++)
        free(input[i]);
}

int compareString(const void *a, const void *b)
{
    return strcmp(*(char **)a, *(char **)b);
}

void read_and_sort()
{
    FILE *fp, *fp_out;
    char **input;
    char buf[60000];
    int count=0;
    int i;
    int judge=0;
    
    input = (char**)malloc(sentence_count*sizeof(char *)); 
    printf("malloc success\n");
    fp = fopen("content.txt", "r");
    fp_out = fopen("final.txt", "w");
    if(fp!=NULL&&fp_out!=NULL)
        printf("open success\n");
    else
        printf("failed\n");
    //fgets(buf,50000,fp);
    //printf("%s",buf);
    while(fgets(buf,60000,fp)!=NULL){
        //printf("dup %d\n",count);
        if(strncmp(buf,"一",3)>=0&&strncmp(buf,"龜",3)<=0){
            input[count]=strdup(buf);
            count++;
        }
        //if(count%100==0)
            //printf("%d",count);
    }
    qsort( (void*) input , count , sizeof( char* ) , compareString );
    for(int i=0;i<count;i++){
        fprintf(fp_out,"%s",input[i]);
    }
    free(input);
    fclose(fp);
    fclose(fp_out);
}

int main(int argc, char const *argv[])
{
    int i=0;
    for(i=0;i<6;i++){
        cut_sentence('0'+i);
    }
    //printf("cut success\n");
    read_and_sort();
    return 0;
}
