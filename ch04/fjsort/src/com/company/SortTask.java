package com.company;

import java.util.concurrent.RecursiveAction;

public abstract class SortTask extends RecursiveAction {
    private static final int THRESHOLD = 100;

    protected int begin;
    protected int end;
    protected int[] array;    // later change to object

    public SortTask(int begin, int end, int[] array){
        this.begin = begin;
        this.end = end;
        this.array = array;
    }

    @Override
    protected void compute(){
        if(end - begin <= THRESHOLD){
            SelectionSort();
        }
        else{
            Sort();
        }
    }

    protected abstract void Sort();

    protected void SelectionSort(){
        int k, tmp;
        for (int i = begin; i<end-1; i++){
            k = i;
            for (int j=i+1; j<end; j++){
                if(array[k] > array[j]){
                    k = j;
                }
            }
            Swap(i, k);
        }
    }

    protected void Swap(int i, int j){
        int tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
    }
}
