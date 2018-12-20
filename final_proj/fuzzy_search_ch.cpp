#include <bits/stdc++.h>

using namespace std;

typedef struct Ele{
    wstring url,topic,content;
}element;

int min(int a, int b, int c)
{
    int min_num = a;
    if(min_num > b)
        min_num = b;
    if(min_num > c)
        min_num = c;
    return min_num;
}

bool fuzzy_search(const wchar_t *text, wchar_t *pattern, int error_accept)
{
    int text_len = wcslen(text);
    int pattern_len = wcslen(pattern);
    int A[pattern_len+1], B[pattern_len+1];
    int last = 1;

    for(int i=0; i<=pattern_len; i++)
        A[i] = B[i] = i;
    for(int i=0; i<text_len; i++){
        A[0] = 0;
        //printf("test:%d %d %d", A[j-1], B[j]+1, B[j-1]+(text[i]!=pattern[j]));
        for(int j=1; j<=last+1; j++){
            //printf("A:j:%d\n",j);
            //wprintf(L"%lc\n", text[i]);
           // wprintf(L"%lc\n",pattern[j]);
            //printf("test1:%d %d %d\n", A[j-1]+1, B[j]+1, B[j-1]);
            A[j] = min(A[j-1]+1, B[j]+1, B[j-1]+(text[i]!=pattern[j-1]));
            //printf("test2:%d\n", A[j]);
        }
        /*cout << "A:\n";
        for(int j=1; j<=last+1; j++){
           cout << A[j];
        }
        cout << endl;
        cout << "pos:" << i << endl;*/
        //printf("A[pattern_len]:%d", A[pattern_len]);
        if(A[pattern_len] <= error_accept) return true;
        if(A[last+1] <= error_accept) last++;
        else if(A[last] > error_accept) last--;
        /*cout << "++++++++++++++last:" << last << "\n";*/
        i++;
        if(i == text_len){
            printf("haha\n");
            return false;
        }
        B[0] = 0;
        for(int j=1; j<=last+1; j++){
            //printf("B:j:%d\n",j);
            //printf("test1:%d %d %d\n",B[j-1]+1, A[j]+1, A[j-1]+(text[i]!=pattern[j]));
           // wprintf(L"%lc\n", text[i]);
           // wprintf(L"%lc\n",pattern[j]);

            B[j] = min(B[j-1]+1, A[j]+1, A[j-1]+(text[i]!=pattern[j-1]));
            //printf("test2:%d\n", B[j]);
        }
        /*cout << "B:\n";
        for(int j=1; j<=last+1; j++){
           cout << B[j];
        }
        cout << endl;
        cout << "pos:" << i << endl;*/
        //printf("B[pattern_len]:%d", B[pattern_len]);
        if(B[pattern_len] <= error_accept) return true;
        if(B[last+1] <= error_accept) last++;
        else if(A[last] > error_accept) last--;
        //cout << "-------------last:" << last << "\n";
    }
    //printf("qqq\n");
    return false;
}

int main(int argc, char const *argv[])
{
    /* init */
    char out_file[20];
    char in_file[20][20];
    int in_file_cnt = 0;
   // wchar_t pattern[40];
    int error_accept = 0;
    const int MAX_DATA = 7000000;
    int total_data = 0;
    wstring buf;
    //element *input = new element[7000000];
    setlocale(LC_ALL, "");

    /*handle argv*/
    for(int i=1; i<argc; i++){
        cout << argv[i] << endl;
        if(strcmp("-o", argv[i]) == 0){
            i++;
            strcpy(out_file, argv[i]);
        }
       /* else if(strcmp("-p", argv[i]) == 0){
            i++;
            mbstowcs(pattern, argv[i], 40); 
        }*/
        else if(strcmp("-e",argv[i]) == 0){
            i++;
            error_accept = atoi(argv[i]);
        }
        else{
            strcpy(in_file[in_file_cnt], argv[i]);
            in_file_cnt++;
        }
    }
    /*cout << "in file count:" << in_file_cnt << endl;
    for(int i=0; i<700000; i++)
        input = NULL;

    for(int i=0; i<in_file_cnt; i++){
        wifstream fin;
        fin.open(in_file[i], ios::in);
        while(getline(fin, buf)){
            cout << total_data << endl;
            getline(fin, input[total_data].url);
            cout << "dadd\n";
            getline(fin, input[total_data].topic);
            getline(fin, buf);
            getline(fin, input[total_data].content);
            total_data++;
        }
    }

    cout << "total data:" << total_data << endl;

    wofstream fout("out.txt");
    for(int i=0; i<total_data; i++){
        if(fuzzy_search(input[i].content.c_str(), pattern, error_accept)){
            fout << input[i].url << input[i].topic << input[i].content;
        }
    }*/

    wchar_t text[20000] = L"試圖跳出傳統新聞的窠臼，誰說新聞只有記者可以寫？誰說讀者只能等待新聞被印出來？現在網路這麼發達，每個人都可以發聲，每個人都可能是新聞資訊的來源。";
    wchar_t pattern[20] = L"誰說讀者只要等著新聞被";

    cout << "enter text:\n";
    wcin >> text;
    cout << "enter pattern:\n";
    wcin >> pattern;
    

    //cout << fuzzy_search(text, pattern, error_accept) << "\n";
    fuzzy_search(text, pattern, error_accept)?  cout<< "true\n" : cout << "false\n";

    return 0;
}
