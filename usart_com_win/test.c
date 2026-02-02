#include <stdio.h>

int main(void)
{
    int temp;
    scanf("%d", &temp);
    char ch;
    while(getchar()!='\n');
    scanf("%c", &ch);
    printf("%d\n", temp);
    printf("%c\n", ch);

    return 0;
}