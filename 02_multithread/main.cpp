#include <stdio.h>
#include <iostream>
#include <math.h>
#include <random>
#include <pthread.h>

// structure needed to pass multiple arguments in thread
struct thread_args {
    double **a;
    double *b;
    int size;
    int thread_no;
    int threads_count;
};

void* calculate(void *thread_input_args) {
    thread_args *args = (struct thread_args *) thread_input_args;
    double **a = args->a;
    double *&b = args->b;
    int size = args->size;
    int thread_no = args->thread_no;
    int threads_count = args->threads_count;

    std::cout << "Thread " << pthread_self() << " is writing...\n";
    std::cout << size << " " << thread_no << " " << "\n";

    int count = 0;
    for (int i = 2; i < size - 2; i++) {
        for (int j = 2; j < size - 2; j++) {
            if (((size - 4) * i + j - 2) % threads_count == thread_no) {
                b[count] = a[i + 2][j] + a[i - 2][j] + a[i][j + 2] + a[i][j - 2] - 4 * a[i][j];
                count++;
            }
        }
    }
}

// ONLY FOR THIS TASK
void init_matrix(double **a, int size) {
	std::default_random_engine gen(0);
    std::uniform_real_distribution<> dist(1, 1);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            a[i][j] = floor(dist(gen));
        }
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%7.2f", a[i][j]);
        }
        printf("\n");
    }
}

void get_result(double **a, double **b, int size) {
    for (int i = 2; i < size - 2; i++){
        for (int j = 2; j < size - 2; j++){
            a[i][j] = b[j - 2][i - 2];
        }
    }

    printf("\n");
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%7.2f", a[i][j]);
        }
        printf("\n");
    }
}
// ONLY FOR THIS TASK

int main(int argc, char *argv[]) {
    int threads_count = atoi(argv[2]);
    int size = atoi(argv[1]);

    if (threads_count < size / 2 ) {
        threads_count = (size / 2) + 1;
        printf("Not enough threads to calculate, number set to %d\n", threads_count);
    }

    double **b = new double*[threads_count];
    for(int i = 0; i < threads_count; i++){
        b[i] = new double[2 * size];
    }
    double **a = new double*[size];
    for(int i = 0; i < size; i++){
        a[i] = new double[size];
    }
    init_matrix(a, size);

    // array that would store ID of a threads
    pthread_t threads[threads_count];
    pthread_t id;
    for (int i = 0; i < threads_count; i++) {
        // arguments for thread
        struct thread_args *args = (struct thread_args*) malloc(sizeof(struct thread_args));
        args->a = a;
        args->threads_count = threads_count;
        args->size = size;
        args->thread_no = i;
        args->b = b[i];

        // create thread
        // 1: var to save thread ID
        // 2: thread attributes
        // 3: function that would run new thread
        // 4: arguments for thread
        pthread_create(&id, NULL, &calculate, (void *)args);
        threads[i] = id;
    }

    void* result;
    for(int i = 0; i < threads_count; i++){
        // join threads
        // 1: thread ID
        // 2: var that would save result
        pthread_join(threads[i], &result);
    }

    get_result(a, b, size);
    return 0;
}
