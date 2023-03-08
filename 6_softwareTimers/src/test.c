#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
    char arr1[] = "Array1";
    char arr2[] = "Array2";

    char *parr1 = NULL;
    char *parr2 = NULL;

    parr1 = arr2;
    parr2 = arr1;

    printf("%s\n",parr1);
    printf("%s\n",parr2);

    return 0;
}