#include <iostream>
#include <pthread.h>
#include <random>

// structure needed to pass multiple arguments in thread
struct thread_args {
    double** matrix;
    int size;
    int thread_no;
    int threads_count;
};

void* check_symmetry(void* input) {
    bool mismatch = false;
    thread_args *in = (thread_args*)input;

    double** a = in->matrix;
    int size = in->size;
    int thread = in->thread_no;
    int threads_number = in->threads_count;
    std::cout << "Thread " << pthread_self() << " is writing...\n";
    std::cout << size << " " << thread << " " << a << "\n";

    for (int i = 0; i < size && !mismatch; i++) {
        for (int j = 0; j < size && !mismatch; j++) {
            if ((i + j) % threads_number == thread) {
                if (a[i][j] != a[j][i]) mismatch = true;
            }
        }
    }
    return (void*)!mismatch;
}

double** init_matrix(int size, bool random) {
    double **matrix = new double *[size];
    for (int i = 0; i < size; i++) {
        matrix[i] = new double[size];
    }

    if (random) {
        std::default_random_engine gen(0);
        std::uniform_real_distribution<> dist(0, 1);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                matrix[i][j] = dist(gen);
            }
        }
    } else {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                matrix[i][j] = i + j;
            }
        }
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%7.3f", matrix[i][j]);
        }
        printf("\n");
    }
    return matrix;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return 1;
    }

    int size = atoi(argv[2]);
    int threads_count = atoi(argv[1]);
    if (threads_count < size / 2 ) {
        threads_count = size / 2 + 1;
        printf("Not enough threads to calculate, number set to %d\n", threads_count);
    }

    bool random = false;
    if (argc == 4) {
        random = (bool) atoi(argv[3]);
    }
    double **matrix = init_matrix(size, random);

    // array that would store ID of a threads
    pthread_t threads[threads_count];
    pthread_t id;
    for (int i = 0; i < threads_count; i++) {
        // arguments for thread
        struct thread_args *args = (struct thread_args*) malloc(sizeof(struct thread_args));
        args->matrix = matrix;
        args->size = size;
        args->thread_no = i;
        args->threads_count = threads_count;

        // create thread
        // 1: var to save thread ID
        // 2: ..
        // 3: function that would run new thread
        // 4: arguments for thread
        pthread_create(&id, NULL, &check_symmetry, (void*)args);
        threads[i] = id;
    }

    bool isSymmetric = true;
    void* result;
    for (int i = 0; i < threads_count; i++) {
        // join threads
        // 1: thread ID
        // 2: var that would save result
        pthread_join(threads[i], &result);
        isSymmetric &= (bool)result;
    }
    printf("\nIs symmetric: %d\n", isSymmetric);
}
