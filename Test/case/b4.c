#include <stdio.h>
int main()
{
    int k;
    int arr[10], mi;
    int i, j, temp;

    mi = 0;
    while (mi < 10) {
        arr[mi] = (10 - mi) * 2;
        mi = mi + 1;
    }
    for (k = 0; k < 10; k++) {
        printf("%d ", arr[k]);
    }
    printf("\n");
    arr[2] = 5;
    arr[7] = 16;
    arr[9] = 15;

    i = 1;
    while (i < 10) {
        temp = arr[i];
        j = i - 1;
        while ((j >= 0) && (arr[j] > temp)) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        if (j != i - 1) {
            arr[j + 1] = temp;
        }
        i = i + 1;
        for (k = 0; k < 10; k++) {
            printf("%d ", arr[k]);
        }
        printf("\n");
    }

    i = 0;
    while (i < 10) {
        printf("%d\n", arr[i]);
        i = i + 1;
    }
    return 0;
}
