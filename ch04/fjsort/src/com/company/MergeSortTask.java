package com.company;

// performs merge sort to sort an array
public class MergeSortTask<T extends Comparable<T>> extends SortTask<T> {

    private int middle;

    public MergeSortTask(int begin, int end, T[] array) {
        super(begin, end, array);
        middle = (begin + end) / 2;
    }

    @Override
    // special sort: merge sort
    protected void Sort() {
        // merge sort two sub-arrays
        MergeSortTask<T> leftTask = new MergeSortTask<T>(begin, middle, array);
        MergeSortTask<T> rightTask = new MergeSortTask<T>(middle, end, array);

        leftTask.fork();
        rightTask.fork();

        leftTask.join();
        rightTask.join();

        // merge
        Merge();
    }

    // merge sorted array
    private void Merge() {
        int i, j, k;
        T[] tmp = array.clone();
        i = begin;
        j = middle;
        k = begin;

        // merge into a temporary array
        while (i < middle || j < end) {
            if (i >= middle)
                tmp[k++] = array[j++];
            else if (j >= end)
                tmp[k++] = array[i++];
            else if (array[i].compareTo(array[j]) > 0)
                tmp[k++] = array[j++];
            else
                tmp[k++] = array[i++];
        }

        // overwrite to the original array
        for (i = begin; i < end; i++) {
            array[i] = tmp[i];
        }
    }
}
