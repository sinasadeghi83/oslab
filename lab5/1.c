#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    int hist[25] = {0};
    int counter = 0;

    srand(200);

    clock_t start_time = clock();

    for (int j = 0; j < 5000; j++)
    {
        counter = 0;
        for (int i = 0; i < 12; i++)
        {
            int random_num = rand() % 101;

            if (random_num >= 49)
            {
                counter++;
            }
            else
            {
                counter--;
            }
        }

        if (counter >= -12 && counter <= 12)
        {
            hist[counter + 12]++;
        }
    }

    clock_t end_time = clock();

    // Calculate elapsed time in seconds
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("counter value: %d\n", counter);
    printf("hist array:\n");
    for (int i = 0; i < 25; i++)
    {
        printf("hist[%d]: %d\n", i - 12, hist[i]);
    }

    printf("\nElapsed Time:%f\n", elapsed_time);

    return 0;
}
