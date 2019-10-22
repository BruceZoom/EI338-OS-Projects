#include <pthread.h>
#include <stdio.h>

#include <stdlib.h>

int *data;
int *res;
void *sorting(void *param);
void *merging(void *param);

typedef struct{
	int low;
	int high;
} parameter;

int main()
{
	pthread_t workers[2];
	pthread_attr_t attrs[3];
	parameter p[2];
	int i;
	int n, m;
	
	// input and initialize
	scanf("%d", &n);
	m = n / 2;
	data = (int*)malloc(sizeof(int) * n);
	res = (int*)malloc(sizeof(int) * n);
	for(i=0; i<n; i++)
		scanf("%d", &data[i]);
		
	for(i=0; i<3; i++)
		pthread_attr_init(attrs+i);
	
	// create and join sorting theads
	p[0].low = 0;
	p[0].high = m;
	pthread_create(workers, attrs, sorting, p);
	p[1].low = m;
	p[1].high = n;
	pthread_create(workers+1, attrs+1, sorting, p+1);
	
	pthread_join(workers[0], NULL);
	pthread_join(workers[1], NULL);
	
	// create and join merge thread
	p[0].low = 0;
	p[0].high = n;
	pthread_create(workers, attrs+2, merging, p);
	pthread_join(workers[0], NULL);

	// output
	for(i=0; i<n; i++)
		printf("%d ", res[i]);
	printf("\n");

	free(data);
	free(res);
	return 0;
}

// selection sort for data[low:high]
void *sorting(void *param)
{
	parameter *p = (parameter*)param;
	int i, j, k, tmp;
	for(i=p->low; i<p->high-1; i++)
	{
		k = i;
		for(j=i+1; j<p->high; j++)
		{
			if(data[k] > data[j])
			{
				k = j;
			}
		}
		tmp = data[i];
		data[i] = data[k];
		data[k] = tmp;
	}
}

// merge data[low:high] into res[low:high]
void *merging(void *param)
{
	parameter *p = (parameter*)param;
	int i, j, k, m;
	m = (p->low + p->high) / 2;
	i = p->low;
	j = m;
	k = i;
	while(i < m || j < p->high)
	{
		if(i >= m)
			res[k++] = data[j++];
		else if(j >= p->high)
			res[k++] = data[i++];
		else if(data[i] > data[j])
			res[k++] = data[j++];
		else
			res[k++] = data[i++];
	}
}





