#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>


typedef struct Ele{
    wchar_t *url;
    wchar_t *topic;
    wchar_t *content;
    int point;
}element;

typedef enum bool{false, true}bool;

int min(int a, int b, int c)
{
    int min_num = a;
    if(min_num > b)
        min_num = b;
    if(min_num > c)
        min_num = c;
    return min_num;
}

int cmp_element(const void* a, const void* b)
{
    return (*(element*)b).point - (*(element*)a).point;
}

int fuzzy_search(const wchar_t *text, wchar_t *pattern, int error_accept)
{
    int text_len = wcslen(text);
    int pattern_len = wcslen(pattern);
    int A[pattern_len+1], B[pattern_len+1];
    int last = 1;
    int count = 0;

    for(int i=0; i<=pattern_len; i++)
        A[i] = B[i] = i;
    for(int i=0; i<text_len; i++){
        A[0] = 0;
        for(int j=1; j<=last+1; j++){
            A[j] = min(A[j-1]+1, B[j]+1, B[j-1]+(text[i]!=pattern[j-1]));
        }
        if(A[pattern_len] <= error_accept){
            if(A[pattern_len] == 0)
                count += pattern_len * 2;
            else
                count += pattern_len - A[pattern_len];
        }
        if(A[last+1] <= error_accept) last++;
        else if(A[last] > error_accept) last--;
        i++;
        if(i == text_len){
            return count;
        }
        B[0] = 0;
        for(int j=1; j<=last+1; j++){
            B[j] = min(B[j-1]+1, A[j]+1, A[j-1]+(text[i]!=pattern[j-1]));
        }
        if(B[pattern_len] <= error_accept){
            if(B[pattern_len] == 0)
                count += pattern_len * 2;
            else
                count += pattern_len - B[pattern_len];
        }
        if(B[last+1] <= error_accept) last++;
        else if(A[last] > error_accept) last--;
    }
    return count;
}

int main(int argc, char const *argv[])
{
    /* init */
    int i, j;
    char out_file[20] = "out2.txt";
    char in_file[20][20];
    int in_file_cnt = 0;
    wchar_t *pattern[20];
    int error_accept = 0;
    const int MAX_DATA = 7000000;
    int total_data = 0, total_pattern = 0;
    element *input = (element*) malloc(sizeof(element)*600000);
    setlocale(LC_ALL, "");

    /*handle argv*/
    //./dp_search ettoday0.rec -p "+英雄聯盟, -選手"
    for(i=1; i<argc; i++){
        if(strcmp("-o", argv[i]) == 0){
            i++;
            strcpy(out_file, argv[i]);
        }
        else if(strcmp("-p", argv[i]) == 0){
            i++;
            wchar_t buf[1000];
            wchar_t *tmp;
            mbstowcs(buf, argv[i], 1000); 
            wchar_t *delim = L" ,\t", *ptr;
            pattern[total_pattern] = wcsdup(wcstok(buf, delim, &ptr));
            //wprintf(L"%ls\n %d", pattern[total_pattern], total_pattern);
            total_pattern++;
            while(tmp = wcstok(NULL, delim, &ptr)){
                wprintf(L"%ls\n", tmp);
                pattern[total_pattern] = wcsdup(tmp);
                total_pattern++;
            }
        }
        else{
            strcpy(in_file[in_file_cnt], argv[i]);
            in_file_cnt++;
        }
    }
    wprintf(L"in file cnt:%d\n", in_file_cnt);

    for(i=0; i<in_file_cnt; i++){
        FILE *fp = fopen(in_file[i], "r");
        int len;
        wchar_t buf[10000];
        wchar_t *tmp;
        if(fp == NULL) return -1;

        while(fgetws(buf, 10000, fp)){
            //wprintf(L"buf:%ls\n", buf);
            //printf("adsada\n");
            if(wcsncmp(buf, L"@GAISRec:", 9) !=0) continue;
            fgetws(buf, 10000, fp);
            //wprintf(L"buf:%ls\n", buf);
            len = wcslen(buf);
            buf[len-1] = L'\0';
            tmp = buf; //去＠U:
            tmp += 3;
            input[total_data].url = malloc(sizeof(wchar_t)*(len-3));
            wcsncpy(input[total_data].url, tmp, len-3);
            //input[total_data].url = wcsdup(tmp);
            fgetws(buf, 10000, fp);
            len = wcslen(buf);
            buf[len-1] = L'\0';
            tmp = buf; //去＠T:
            tmp += 3;
            input[total_data].topic = malloc(sizeof(wchar_t)*(len-3));
            wcsncpy(input[total_data].topic, tmp, len-3);
            fgetws(buf, 10000, fp); // 去@B:
            fgetws(buf, 10000, fp);
            input[total_data].content = wcsdup(buf);
            total_data++;
            //getchar();
        }
        fclose(fp);
    }

    wprintf(L"total data:%d\n", total_data);

    for(i=0; i<total_pattern; i++)
        wprintf(L"pattern:%ls\n", pattern[i]);
    
    for(j=0; j<total_data; j++)
        input[i].point = 0;
    for(i=0; i<total_pattern; i++){
        for(j=0; j<total_data; j++){
            int num, topic_point;
            if(input[j].url == NULL) continue;
            if(pattern[i][0] == L'-'){
                //wprintf(L"1\n");
                wchar_t *true_pattern = pattern[i] + 1;
                topic_point = fuzzy_search(input[j].topic, true_pattern, 0);
                if((num = fuzzy_search(input[j].content, true_pattern, 0)) || topic_point){
                    free(input[j].url);
                    input[j].url = NULL;
                    free(input[j].topic);
                    input[j].topic = NULL;
                    free(input[j].content);
                    input[j].content = NULL;
                    input[j].point = 0;
                    continue;
                }
                input[j].point += num + 5*topic_point;
            }
            else if(pattern[i][0] == L'+'){
               // wprintf(L"2\n");
                wchar_t *true_pattern = pattern[i] + 1;
                topic_point = fuzzy_search(input[j].topic, true_pattern, 0);
                if(!(num = fuzzy_search(input[j].content, true_pattern, 0)) && !topic_point){
                    free(input[j].url);
                    input[j].url = NULL;
                    free(input[j].topic);
                    input[j].topic = NULL;
                    free(input[j].content);
                    input[j].content = NULL;
                    input[j].point = 0;
                    continue;
                }
                input[j].point += num + 5*topic_point;
            }
            else{
                //wprintf(L"3\n");
                topic_point = fuzzy_search(input[j].topic, pattern[i], wcslen(pattern[i])/5+1);
                if(!(num = fuzzy_search(input[j].content, pattern[i], wcslen(pattern[i])/5+1)) && !topic_point){
                    //wprintf(L"3\n");
                    free(input[j].url);
                    input[j].url = NULL;
                    free(input[j].topic);
                    input[j].topic = NULL;
                    free(input[j].content);
                    input[j].content = NULL;
                    input[j].point = 0;
                    continue;
                }
                input[j].point += num + 5*topic_point;
            }
        }
    }
    wprintf(L"preprocess finish\n");
    qsort(input, 600000, sizeof(element), cmp_element);
    wprintf(L"sort finish\n");
    FILE *fp = fopen(out_file, "w");
    for(i=0; i<total_data; i++){
        if(input[i].url == NULL) break;
        //wprintf(L"haha\n");
        fwprintf(fp, L"%d\t%ls\t%ls\t%ls", input[i].point, input[i].url, input[i].topic, input[i].content);
    }
    fclose(fp);

    return 0;
}
