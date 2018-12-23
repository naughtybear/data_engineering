#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>


typedef struct Ele{
    wchar_t *url;
    wchar_t *topic;
    wchar_t *content;
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
        if(A[pattern_len] <= error_accept) count += (error_accept - A[pattern_len] + 1);
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
        if(B[pattern_len] <= error_accept) count += (error_accept - B[pattern_len] + 1);
        if(B[last+1] <= error_accept) last++;
        else if(A[last] > error_accept) last--;
    }
    return count;
}

int main(int argc, char const *argv[])
{
    /* init */
    char out_file[20];
    char in_file[20][20];
    int in_file_cnt = 0;
    wchar_t *pattern[20];
    int error_accept = 0;
    const int MAX_DATA = 7000000;
    int total_data = 0, total_pattern = 0;
    element *input = (element*) malloc(sizeof(element)*600000);
    setlocale(LC_ALL, "");

    /*handle argv*/
    for(int i=1; i<argc; i++){
        if(strcmp("-o", argv[i]) == 0){
            i++;
            strcpy(out_file, argv[i]);
        }
        else if(strcmp("-p", argv[i]) == 0){
            i++;
            wchar_t buf[1000];
            wchar_t *tmp;
            mbstowcs(buf, argv[i], 1000); 
            wchar_t *delim = " ,\t", *ptr;
            pattern[total_pattern] = wcstok(buf, delim, &ptr);
            total_pattern++;
            while(tmp = wcstok(NULL, delim, &ptr)){
                pattern[total_pattern] = tmp;
                total_pattern++;
            }
        }
        else{
            strcpy(in_file[in_file_cnt], argv[i]);
            in_file_cnt++;
        }
    }
    wprintf(L"in file cnt:%d\n", in_file_cnt);

    for(int i=0; i<in_file_cnt; i++){
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
    
    FILE *fp = fopen("out2.txt", "w");
    for(int i=0; i<total_data; i++){
        if(input[i].url == NULL) continue;
        if(fuzzy_search(input[i].content, pattern, error_accept)){
            fwprintf(fp, L"%d\t%ls\t%ls\t%ls", i, input[i].url, input[i].topic, input[i].content);
        }
    }

    /*wchar_t text[20000] = L"試圖跳出傳統新聞的窠臼，誰說新聞只有記者可以寫？誰說讀者只能等待新聞被印出來？現在網路這麼發達，每個人都可以發聲，每個人都可能是新聞資訊的來源。";
    wchar_t pattern[20] = L"誰說讀者只要等著新聞被";

    cout << "enter text:\n";
    wcin >> text;
    cout << "enter pattern:\n";
    wcin >> pattern;*/
    

    //cout << fuzzy_search(text, pattern, error_accept) << "\n";
    //fuzzy_search(text, pattern, error_accept)?  cout<< "true\n" : cout << "false\n";

    return 0;
}
