#include <stdio.h>
int get_m2nd(int arr[20], int m)
{
    int m1st, m2nd;
    int mi;
    if (arr[0] < arr[1]) {
        m1st = arr[0];
        m2nd = arr[1];
    } else {
        m1st = arr[1];
        m2nd = arr[0];
    }
    mi = 2;
    while (mi < 20) {
        if (arr[mi] < m1st) {
            m2nd = m1st;
            m1st = arr[mi];
        } else if (arr[mi] < m2nd) {
            m2nd = arr[mi];
        }
        mi = mi + 1;
    }
    return m2nd;
}

int main()
{
    int num[20];
    int i = 0;
    while (i < 20) {
        int ti = i * i + i * 2 
    }
    printf("%d\n", get_m2nd(num, 20));
    return 0;
}
