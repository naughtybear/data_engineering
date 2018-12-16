#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int i, j, error_accept;
    char input_file[20];
    char text[10000], pattern[100];
    int pattern_len, text_len;
    
    printf("enter error accept:\n");
    scanf("%d", &error_accept);
    printf("enter text:\n");
    scanf("%s", input_file);
    printf("enter pattern:\n");
    scanf("%s", pattern);

    FILE *fp = fopen(input_file, "r");
    fgets(text, 10000, fp);
    
    int mask[26];
    for(i=0; i<26; i++)
        mask[i] = 0;
    pattern_len = strlen(pattern);
    text_len = strlen(text);
    for(i=0; i<pattern_len; i++){
        for(j=0; j<26; j++)
            mask[j] <<= 1;
        mask[pattern[i]-'a']+=1;
    }

    int dp[error_accept][pattern_len];
    return 0;
}