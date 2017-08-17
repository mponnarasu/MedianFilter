/******************************************************************************
* Copyright 2017
* Author: Ponnarasu M.
* No guarantees, warrantees, or promises, implied or otherwise.
* May be used for any purpose provided copyright notice remains intact.
*
*****************************************************************************/

#include <omp.h>      //OpenMP library functions
#include <stdio.h>    // Printf 
#include <stdlib.h>   // rand

/****************************************************************************
*  Configuration                                                            *
****************************************************************************/
/* Operating data size */
#define MAX_DATA_SIZE 1024U

/****************************************************************************
*  Internal Macros                                                          *
****************************************************************************/


/****************************************************************************
*  Global Data                                                              *
****************************************************************************/
int input[MAX_DATA_SIZE][MAX_DATA_SIZE];  // input raw data
int output[MAX_DATA_SIZE][MAX_DATA_SIZE]; // Filtered output data


/******************************************************************************
*
* Function Name: void print_buffer(int buf[][MAX_DATA_SIZE])
*
* Description:
* Prints a two dimentional array of size 
*
* Parameters: 
* int buf[][MAX_DATA_SIZE]
* int size: size of buffer to print         
*
* Returns: void 
*
*****************************************************************************/
void print_buffer(int buf[][MAX_DATA_SIZE], int size)
{
    int i,j;
    for(i=0;i<size;i++)
    {
        for(j=0;j<size;j++)
        {
            printf("%d\t",buf[i][j]);
        }
        printf("\n");
    }
}

/******************************************************************************
*
* Function Name: void read_buffer(void)
*
* Description:
* Reads input buffer and prepates the output buffer
*
* Parameters: void
*
* Returns: void 
*
*****************************************************************************/
void read_buffer(int size)
{
    int x,y;  
    for(x=0;x<size;x++)
    {
        for(y=0;y<size;y++)
        {
            input[x][y]=rand()%12;
            if(x==0 || y ==0 || x == (size-1) || y == (size-1))
            output[x][y]=input[x][y];

        }
    }
    printf("The input matrix is: \n");
    print_buffer(&input[0],size);
    printf("The initial output matrix is: \n");
    print_buffer(&output[0],size);
}


/******************************************************************************
*
* Function Name: int main(void)
*
* Description:
* Main function computing the median of give data of size 
*
* Parameters: void
*
* Returns: int 
*
*****************************************************************************/
int main(void)
{

    double starttime;
    unsigned int size;
    unsigned int window_size;
    do
    {   
        printf("Enter Image size (<= %d ): ",MAX_DATA_SIZE);
        scanf("%d",&size);
    }while(size > MAX_DATA_SIZE);
    do
    {
        printf("Please enter suitable window size: ");
        scanf("%d",&window_size);
    }while((window_size >= size) || ((window_size%2) == 0));

    /* read input buffer and prepare the output buffer */   
    read_buffer(size);
    /* record current time stamp for execution time measurement */
    starttime = omp_get_wtime();

#pragma omp parallel shared(input,output)
  {   
    int num_threads;
    int x, y, thread_id;
    thread_id=omp_get_thread_num();
    if(thread_id==0)
    {
        num_threads=omp_get_num_threads();
        printf("The number of threads are:%d\n",num_threads);
    }
    #pragma omp for schedule(static) collapse(2)
    for(x=0;x<(size-(window_size-1));x++) // row index
    {
        for(y=0;y<(size-(window_size-1));y++) // col index
        {
        unsigned int row_pos = x + (unsigned int)(window_size/2);
        unsigned int col_pos = y + (unsigned int)(window_size/2);
        unsigned int window_size_2d = window_size * window_size;
        int median_Window[window_size_2d]; // Temporary array to find median 
        printf("Thread %d working on element [%d][%d] \n",thread_id,row_pos,col_pos);
            int idx=0, i, j;
            /* Extract median_Window */
            for(i=0;i<window_size;i++)
            {
                for(j=0;j<window_size;j++)
                {
                    median_Window[idx]=input[i+x][j+y];
                    idx++;
                }
            }
            /* sort the median_Window contents */
            for(i=(window_size_2d-1);i>1;i--)
            {
                for(j=0;j<i;j++)
                {
                    if(median_Window[j]> median_Window[j+1])
                    {
                        int temp = median_Window[j];
                        median_Window[j] = median_Window[j+1];
                        median_Window[j+1] = temp;
                    }
                }
            }
        /* Pick the medain and update in output*/
        output[(row_pos)][col_pos]=median_Window[(unsigned int)(window_size_2d/2U)];
        }
    }
  }
/* Compute and print execution time */
printf("Execution time = %lf ms\n",1000*(omp_get_wtime() - starttime ));
/* print the result */
printf("The resultant matrix is: \n");
print_buffer(&output[0],size);
return 0;
}