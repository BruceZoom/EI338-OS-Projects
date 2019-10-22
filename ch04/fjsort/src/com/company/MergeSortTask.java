package com.company;

public class MergeSortTask extends SortTask {

    private int middle;

    public MergeSortTask(int begin, int end, int[] array) {
        super(begin, end, array);
        middle = (begin + end) / 2;
    }

    @Override
    protected void Sort() {
        MergeSortTask leftTask = new MergeSortTask(begin, middle, array);
        MergeSortTask rightTask = new MergeSortTask(middle, end, array);

        leftTask.fork();
        rightTask.fork();

        leftTask.join();
        rightTask.join();

        Merge();
    }

    private void Merge(){
        int i, j, k;
        int[] tmp = new int[end-begin+1];
        i = begin;
        j = middle;
        k = 0;
        while(i < middle || j < end){
            if(i >= middle)
                tmp[k++] = array[j++];
            else if(j >= end)
                tmp[k++] = array[i++];
            else if(array[i] > array[j])
                tmp[k++] = array[j++];
            else
                tmp[k++] = array[i++];
        }
        for(i=begin; i<end; i++){
            array[i] = tmp[i-begin];
        }
    }
}
