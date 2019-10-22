package com.company;

import javafx.concurrent.Task;

import java.util.*;

public class QuickSortTask extends SortTask {
    public QuickSortTask(int begin, int end, int[] array) {
        super(begin, end, array);
    }

    @Override
    protected void Sort() {
        Random rand = new Random();
        int i, j, k;
        int tmp;

        k = rand.nextInt(end-begin) + begin;
        i = begin;
        j = end-1;
        Swap(k, begin);
        tmp = array[begin];
        while(i < j){
            while (i < j && tmp <= array[j])  j--;
            if(i < j) array[i] = array[j];
            while (i < j && tmp >= array[i])  i++;
            if(i < j) array[j] = array[i];
        }
        array[i] = tmp;

        QuickSortTask leftTask = new QuickSortTask(begin, i, array);
        QuickSortTask rightTask = new QuickSortTask(i+1, end, array);

        leftTask.fork();
        rightTask.fork();

        leftTask.join();
        rightTask.join();
    }
}
