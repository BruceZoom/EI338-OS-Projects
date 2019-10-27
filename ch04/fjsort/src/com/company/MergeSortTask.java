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

        // merge into one array
        while (i < middle || j < end) {
            if (i >= middle)
                array[k++] = tmp[j++];
            else if (j >= end)
                array[k++] = tmp[i++];
            else if (tmp[i].compareTo(tmp[j]) > 0)
                array[k++] = tmp[j++];
            else
                array[k++] = tmp[i++];
        }
    }
}
