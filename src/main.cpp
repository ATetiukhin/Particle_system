#include <cstdio>
#include <cstdlib>

const int N = 16;


void run(char * string, int * vector, int csize, int isize);


int main()
{
    char a[N] = "Hello \0\0\0\0\0\0";
    int b[N] = { 15, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    const int csize = N * sizeof(char);
    const int isize = N * sizeof(int);

    printf("%s\n", a);
    run(a, b, csize, isize);
    printf("%s", a);


    printf("%s\n", a);
    return EXIT_SUCCESS;
}