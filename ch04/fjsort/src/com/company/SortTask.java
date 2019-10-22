package com.company;

import java.util.concurrent.RecursiveAction;

public abstract class SortTask<T extends Comparable<T>> extends RecursiveAction {
    private static final int THRESHOLD = 100;

    protected int begin;
    protected int end;
    protected T[] array;

    public SortTask(int begin, int end, T[] array) {
        this.begin = begin;
        this.end = end;
        this.array = array;
    }

    @Override
    protected void compute() {
        if (end - begin <= THRESHOLD) {
            // selection sort if the input size is small enough
            SelectionSort();
        } else {
            // special sort if the input size is large
            Sort();
        }
    }

    // special sort
    protected abstract void Sort();

    // simple selection sort
    protected void SelectionSort() {
        int k;
        for (int i = begin; i < end - 1; i++) {
            k = i;
            for (int j = i + 1; j < end; j++) {
                if (array[k].compareTo(array[j]) > 0) {
                    k = j;
                }
            }
            Swap(i, k);
        }
    }

    protected void Swap(int i, int j) {
        T tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
    }
}
