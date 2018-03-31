#include <stdio.h>
#include <stdlib.h>

void selectionSort(int *nums, int length);  // 选择排序
void insertionSort(int *nums, int length);  // 插入排序
void shellSort(int *nums, int length, int h, int k);  // 希尔排序
void mergeSort(int *nums, int length);  // 归并排序

void printArray(int *nums, int length) {
    printf("[");
    for (int i = 0; i < length; i++) {
        printf("%d", nums[i]);
        if (i != length - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

int main(int argc, char const *argv[]) {
    int nums[10] = {2, 3, 1, 5, 6, 2, 0, 3, 4, 8};
    mergeSort(nums, 10);
    printArray(nums, 10);
    return 0;
}

/*
 * 选择排序
 */
void selectionSort(int *nums, int length) {
    int i = 0;
    for (i = 0; i < length; i++) {
        int min = i;
        int j = i + 1;
        for (j = i + 1; j < length; j++) {
            if (nums[j] < nums[min]) {
                min = j;
            }
        }
        if (min != i) {
            int temp = nums[min];
            nums[min] = nums[i];
            nums[i] = temp;
        }
    }
}

/*
 * 插入排序
 */
void insertionSort(int *nums, int length) {
    int i = 0;
    for (i = 0; i < length; i++) {
        int num = nums[i];
        int j;
        for (j = i; j > 0 && nums[j - 1] > num; j--) {
            nums[j] = nums[j - 1];
        }
        nums[j] = num;
    }
}

/*
 * 希尔排序
 */
void shellSort(int *nums, int length, int h, int k) {
    while (h >= 1) {
        int i = 0;
        for (i = 0; i < length; i += h) {
            int num = nums[i];
            int j;
            for (j = i; j >= h && nums[j - h] > num; j -= h) {
                nums[j] = nums[j - h];
            }
            nums[j] = num;
        }
        h /= k;
    }
}

/*
 * 归并排序
 */
void mergeApart(int *nums, int low, int high) {
    if (low == high) return;

    int middle = (low + high) / 2;
    mergeApart(nums, low, middle);
    mergeApart(nums, middle + 1, high);

    int *temp = (int *)malloc(sizeof(int) * (high - low + 1));
    int i;
    for (i = 0; i < high - low + 1; i++) {
        temp[i] = nums[low + i];
    }
    int p_1 = 0;
    int p_2 = middle - low + 1;
    for (i = low; i <= high; i++) {
        if (p_1 > middle - low) nums[i] = temp[p_2++];
        else if (p_2 > high - low) nums[i] = temp[p_1++];
        else if (temp[p_1] < temp[p_2]) nums[i] = temp[p_1++];
        else nums[i] = temp[p_2++];
    }
    free(temp);
    temp = NULL;
}

void mergeSort(int *nums, int length) {
    mergeApart(nums, 0, length - 1);
}
