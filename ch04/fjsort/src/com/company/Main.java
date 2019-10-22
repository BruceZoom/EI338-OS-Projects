package com.company;

import java.util.*;
import java.util.concurrent.*;

public class Main {
    static final int SIZE = 10000;

    public static void main(String[] args) {
        ForkJoinPool pool = new ForkJoinPool();
        int[] array = new int[SIZE];

        // create SIZE random integers between 0 and 9
        Random rand = new Random();

        for (int i = 0; i < SIZE; i++) {
            array[i] = rand.nextInt(10000);
            System.out.print(array[i] + " ");
        }
        System.out.println();

        // use fork-join parallelism to sum the array
        MergeSortTask task = new MergeSortTask(0, SIZE, array);

        pool.invoke(task);

        for(int i=0; i<SIZE; i++){
            System.out.print(array[i] + " ");
        }
        System.out.println();
    }
}
