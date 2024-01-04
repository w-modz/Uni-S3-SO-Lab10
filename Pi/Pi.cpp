// SO IS1 221A LAB10
// Wiktor Modzelewski
// mw53766@zut.edu.pl
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
// Funkcja pomocnicza do weryfikowania argumentow
bool is_number(const char* str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (isdigit(str[i]) == 0) return false;
    }
    return true;
}
// Funkcja pomocnicza do wyznaczania wiekszej wartosci
int min(int a, int b)
{
    if (a >= b)
    {
        return a;
    }
    else if (b > a)
    {
        return b;
    }
}

// Struktura pomocnicza dla watkow
struct ThreadData {
    int start;
    int end;
};

#define MAXTHREADS 100
#define MAXN 1000000
pthread_mutex_t mutex;
double global_sum = 0.0;

// Funkcja dla watkow do obliczania wybranych wyrazow ciagu
void* sumThread(void* parameters) {
    struct ThreadData* data = (struct ThreadData*)parameters;
    double sum = 0.0;

    printf("Thread %lu size=%d first==%d\n", pthread_self(), data->end - data->start, data->start);

    for (int i = data->start; i < data->end; ++i) {
        sum += pow(-1.0, (double)i) * (4.0 / ((2.0 * ((double)i + 1.0)) * ((2.0 * ((double)i + 1.0)) + 1.0) * ((2.0 * ((double)i + 1.0)) + 2.0)));
    }

    pthread_mutex_lock(&mutex);
    global_sum += sum;
    pthread_mutex_unlock(&mutex);

    printf("Thread %lu sum=%0.20f\n", pthread_self(), sum);

    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    // Weryfikowanie poprawnosci argumentow
    if (argc != 3)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }
    else if (!is_number(argv[1]) || atoi(argv[1]) >= MAXN || atoi(argv[1]) <= 1)
    {
        fprintf(stderr, "Invalid argument\n");
        return 1;
    }
    else if (!is_number(argv[2]) || atoi(argv[2]) <= 1 || atoi(argv[2]) >= min(MAXTHREADS, atoi(argv[1])))
    {
        fprintf(stderr, "Invalid argument\n");
        return 1;
    }
    // Obliczanie Pi poprzez watki
    int n = atoi(argv[1]);
    int w = atoi(argv[2]);


    pthread_t threads[w];
    struct ThreadData threadData[w];
    pthread_mutex_init(&mutex, NULL);

    int numberPerThread = n / w;
    int extraNumbers = n % w;

    struct timespec start_time;
    clock_gettime(CLOCK_REALTIME, &start_time);


    // Sprawdzenie podzielnosci liczby wyrazow przez liczbe watkow
    if (extraNumbers == 0)
    {
        for (int i = 0; i < w; i++)
        {
            threadData[i].start = 1;
            threadData[i].start = i * numberPerThread;
            threadData[i].end = (i + 1) * numberPerThread;
            pthread_create(&threads[i], NULL, sumThread, &threadData[i]);
        }
    }
    else
    {
        for (int i = 0; i < w; i++)
        {
            threadData[i].start = i * numberPerThread;
            threadData[i].end = (i + 1) * numberPerThread;
        }
        threadData[w - 1].end += extraNumbers;
        for (int i = 0; i < w; i++)
        {
            pthread_create(&threads[i], NULL, sumThread, &threadData[i]);
        }
    }
    for (int i = 0; i < w; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);

    double pi = 3.0 + global_sum;
    long long elapsed_time = end_time.tv_nsec - start_time.tv_nsec;
    // Wypisanie z konwersja z nanosekund do sekund
    printf("w/Threads PI=%0.20lf time=%0.6lf\n", pi, (double)elapsed_time / 1000000000.0);
    // Obliczanie Pi bez watkow
    struct timespec start_time2;
    clock_gettime(CLOCK_REALTIME, &start_time2);
    double sum = 0.0;

    for (int i = 0; i < n; ++i) {
        sum += pow(-1.0, (double)i) * (4.0 / ((2.0 * ((double)i + 1.0)) * ((2.0 * ((double)i + 1.0)) + 1.0) * ((2.0 * ((double)i + 1.0)) + 2.0)));
    }
    pi = 3.0 + sum;
    struct timespec end_time2;
    clock_gettime(CLOCK_REALTIME, &end_time2);
    elapsed_time = end_time2.tv_nsec - start_time2.tv_nsec;
    printf("wo/Threads PI=%0.20lf time=%0.6lf\n", pi, (double)elapsed_time / 1000000000.0);
    pthread_mutex_destroy(&mutex);
    return 0;
}

