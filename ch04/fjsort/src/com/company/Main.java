package com.company;

import java.util.*;
import java.util.concurrent.*;

public class Main {
    static final int SIZE = 10000;

    private static Integer[] array;

    public static void main(String[] args) {
        array = new Integer[SIZE];

        MergeSortTask<Integer> mergeSortTask = new MergeSortTask<Integer>(0, SIZE, array);
        QuickSortTask<Integer> quickSortTask = new QuickSortTask<Integer>(0, SIZE, array);

        System.out.println("Merge Sort");
        Test(mergeSortTask);
        System.out.println("\nQuick Sort");
        Test(quickSortTask);
    }

    private static void Test(SortTask task) {
        ForkJoinPool pool = new ForkJoinPool();
        Random rand = new Random();

        System.out.print("Before sort: ");
        for (int i = 0; i < SIZE; i++) {
            array[i] = rand.nextInt(2 * SIZE);
            System.out.print(array[i] + " ");
        }
        System.out.println();

        pool.invoke(task);

        System.out.print("After sort: ");
        for (int i = 0; i < SIZE; i++) {
            System.out.print(array[i] + " ");
        }
        System.out.println();
    }
}
