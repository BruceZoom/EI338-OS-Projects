#include <pthread.h>
#include <stdio.h>

#include <stdlib.h>

int matrix[9][9];

int res[27];
void *runner(void *param);

typedef struct
{
	int row;
	int col;
} parameter;

int main(int argc, char *argv[])
{
	pthread_t workers[27];
	pthread_attr_t attrs[27];
	parameter data[27];
	int i;
	
	// load matrix and initialize attributes
	FILE *fp = fopen("test.txt", "r");	
	for(i=0; i<81; i++)
	{
		if(i<27)
			pthread_attr_init(attrs+i);
		fscanf(fp, "%d", &matrix[i/9][i%9]);
	}
	fclose(fp);
	
	
	for(i=0; i<9; i++)
	{
		// parepare data and create thread for a column
		data[i].row = -1;
		data[i].col = i;
		pthread_create(workers+i, attrs+i, runner, data+i);
		
		// parepare data and create thread for a row
		data[i+9].row = i;
		data[i+9].col = -1;
		pthread_create(workers+i+9, attrs+i+9, runner, data+i+9);
		
		// parepare data and create thread for a subgrid
		data[i+18].row = (i / 3) * 3;
		data[i+18].col = (i % 3) * 3;
		pthread_create(workers+i+18, attrs+i+18, runner, data+i+18);
	}
	
	// join threads and combine results
	for(i=0; i<27; i++)
	{
		pthread_join(workers[i], NULL);
		//printf("%d", res[i]);
		res[0] = res[0] & res[i];
	}
	
	printf((res[0]?"Valid":"Invalid"));
	printf("\n");
	
	return 0;
}

void *runner(void *param)
{
	parameter *p = (parameter*)param;
	int i, total = 0;
	
	// check column
	if(p->row == -1)
	{
		for(i=0; i<9; i++)
		{
			total = total | (1 << matrix[i][p->col]);
			//printf("%d", matrix[i][p->col]);
		}
		i = p->col;
	}
	// check row
	else if(p->col == -1)
	{
		for(i=0; i<9; i++)
		{
			total = total | (1 << matrix[p->row][i]);
			//printf("%d", matrix[p->row][i]);
		}
		i = 9 + p->row;
	}
	// check subgrid
	else
	{
		for(i=0; i<9; i++)
		{
			total = total | (1 << matrix[p->row+i/3][p->col+i%3]);
			//printf("%d", matrix[p->row+i/3][p->col+i%3]);
		}
		i = 18 + p->row + p->col / 3;
	}
	//printf("\n%d %d: %d\n", p->row, p->col, total);
	//printf("%d\n", i);
	res[i] = (total == 1022);
}

