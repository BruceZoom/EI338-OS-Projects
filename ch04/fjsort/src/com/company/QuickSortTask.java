package com.company;

import javafx.concurrent.Task;

import java.util.*;

// performs quick sort to sort an array
public class QuickSortTask<T extends Comparable<T>> extends SortTask<T> {
    public QuickSortTask(int begin, int end, T[] array) {
        super(begin, end, array);
    }

    @Override
    // special sort: quick sort
    protected void Sort() {
        // find pivot
        int m = Pivot();

        // quick sort two sub-arrays
        QuickSortTask<T> leftTask = new QuickSortTask<T>(begin, m, array);
        QuickSortTask<T> rightTask = new QuickSortTask<T>(m + 1, end, array);

        leftTask.fork();
        rightTask.fork();

        leftTask.join();
        rightTask.join();
    }

    // find pivot and adjust
    private int Pivot() {
        Random rand = new Random();
        int i, j, k;
        T tmp;

        // randomly select pivot
        k = rand.nextInt(end - begin) + begin;
        i = begin;
        j = end - 1;
        Swap(k, begin);
        tmp = array[begin];

        // adjust the array based on selected pivot
        while (i < j) {
            while (i < j && tmp.compareTo(array[j]) <= 0) j--;
            if (i < j) array[i] = array[j];
            while (i < j && tmp.compareTo(array[i]) >= 0) i++;
            if (i < j) array[j] = array[i];
        }
        array[i] = tmp;

        return i;
    }
}
