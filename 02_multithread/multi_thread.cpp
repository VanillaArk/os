#include <iostream>
#include <pthread.h>
#include <random>

struct thread_args {
    double** arr;
    int size;
    int thread;
    int threadsNumber;
};

void* check_symmetry(void* input) {
    bool mismatch = false;
    thread_args *in = (thread_args*)input;

    double** a = in->arr;
    int size = in->size;
    int thread = in->thread;
    int threads_number = in->threadsNumber;
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

void init_matrix(double **matr, int size, bool random) {
    if (random) {
        std::default_random_engine gen(0);
        std::uniform_real_distribution<> dist(0, 1);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                matr[i][j] = dist(gen);
            }
        }
    } else {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                matr[i][j] = i + j;
            }
        }
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%7.3f", matr[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return 1;
    }

    int threads_count = atoi(argv[1]);
    int size = atoi(argv[2]);
    bool random = false;
    if (argc == 4) {
        random = (bool) atoi(argv[3]);
    }

    double **matr = new double *[size];
    for (int i = 0; i < size; i++) {
        matr[i] = new double[size];
    }
    init_matrix(matr, size, random);

    thread_args args = (thread_args){matr, size, 0, threads_count};

    pthread_t threads[threads_count];
    pthread_t id;
    for (int i = 0; i < threads_count; i++) {
        args.thread = i;
        pthread_create(&id, NULL, &check_symmetry, &args);
        threads[i] = id;
    }

    bool isSymmetric = true;
    void* result;
    for (int i = 0; i < threads_count; i++) {
        pthread_join(threads[i], &result);
        isSymmetric &= (bool)result;
    }
    printf("\nIs symmetric: %d\n", isSymmetric);
}
