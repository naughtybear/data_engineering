#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void binprintf(int v)
{
    unsigned int mask=1<<((sizeof(int)<<3)-1);
    while(mask) {
        printf("%d", (v&mask ? 1 : 0));
        mask >>= 1;
    }
}

int main(int argc, char* argv[])
{
    int i, j, error_accept=1;
    char input_file[20] = "text.txt";
    char text[10000], pattern[100] = "computear";
    int pattern_len, text_len;
    
    /*printf("enter error accept:\n");
    scanf("%d", &error_accept);
    printf("enter text:\n");
    scanf("%s", input_file);
    printf("enter pattern:\n");
    scanf("%s", pattern);*/

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
        mask[pattern[i]-'a'] += 1;
    }

    for(i=0; i<26; i++){
        printf("%c:", i+'a');
        binprintf(mask[i]);
        printf("\n");
    }

    int M[error_accept+2];
    int in_mask = 1<<(pattern_len-1);
    for(i=0; i<error_accept+2; i++)
        M[i] = 0;
    printf("0:");
    binprintf(in_mask);
    printf("\n");

    printf("\n");
    printf("\n");
    printf("-----------------------------------\n");
    for(i=0; i<text_len; i++){
        printf("current:%d\n", i);
        int last = 0;
        for(j=1; j<error_accept+2; j++){
            //printf("haha\n");
            int tmp = M[j];
            M[j] = M[j] >> 1 | in_mask;
            M[j] = M[j] & mask[text[i]-'a'] | last | last>>1 | M[j-1]>>1;
            if(M[j]&1){
                printf("\n");
                printf("pos:%d\n", i - pattern_len);
            }
            printf("%d:", j);
            binprintf(M[j]);
            printf("\n");
            last = tmp;
        }
        printf("-----------------------------------\n");
    }
    return 0;
}