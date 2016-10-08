#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main()
{
       int j,n,a,i,e,o,u;
       char str[50];
       a=e=i=o=u=0;
       pid_t pid;
       if ((pid= vfork ()) < 0) {
                perror ("FORK ERROR");
                exit(1);
       }
       if (pid == 0) {
                printf("Counting Number of Vowels using VFORK\n");
                printf("-------- ------ -- ------ ----- -----\n");
                printf("Enter the String: ");
                gets(str);
                _exit(1);
        }
        else {
                n=strlen(str);
                for(j=0;j<n;j++)
                {
                        if(str[j]=='a' || str[j]=='A') a++;
                        if(str[j]=='e' || str[j]=='E') e++;
                        if(str[j]=='i' || str[j]=='I') i++;
                        if(str[j]=='o' || str[j]=='O') o++;
                        if(str[j]=='u' || str[j]=='U') u++;
                }
                printf("Vowels Counting\n");
                printf("------ --------\n");
                printf("Number of A  : %d\n", a);
                printf("Number of E  : %d\n", e);
                printf("Number of I  : %d\n",i);
                printf("Number of O  : %d\n",o);
                printf("Number of U  : %d\n",u);
                printf("Total vowels : %d\n",a+e+i+o+u);
                exit(1);
        }
}