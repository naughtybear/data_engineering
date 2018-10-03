#include <stdio.h>

#include <stdlib.h>

#include <time.h>

void main()

{

 int a;
 FILE *fp;
 srand(time(NULL));
 fp=fopen("num.txt","w");
 int i;
 for(i=0;i<500000;i++){
 a=(rand());
 
 fprintf(fp,"%d\n",a);
 }
}