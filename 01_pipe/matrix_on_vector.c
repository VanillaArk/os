#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define READ_PIPE_END  0
#define WRITE_PIPE_END 1

void parent_work(int pipe_d[2], double *matrix, size_t size) {
    close(pipe_d[READ_PIPE_END]); // close reading end of pipe
    for(int i = 0; i < size; i++) {
        write(pipe_d[WRITE_PIPE_END], &matrix[i * size], sizeof(double) * size); // write data to writing end of pipe
    }
}

void child_work(int pipe_d[2], double *vector, size_t size) {
    close(pipe_d[WRITE_PIPE_END]); // close writing end of pipe
    double *result = (double *)calloc(size, sizeof(double));
    double *matrix_string = (double *)calloc(size, sizeof(double));

    int i = 0;
    int n;
    printf("Result: \n");
    while( n = read(pipe_d[READ_PIPE_END], matrix_string, sizeof(double) * size)) { // read data from reading end of pipe
        double computed = 0;
        for (int j = 0; j < size; j++) {
            computed += matrix_string[j] * vector[j];
        }
        result[i] = computed;
        printf(" %.1f ", computed);
        i++;
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    //size_t size = 3;
    size_t size = (size_t) atoi(argv[1]);
    
    int pipe_file_descriptor[2];
    pid_t process_id;

    printf("Vector: \n");
    double *vector = (double *)calloc(size, sizeof(double));
    for (int i = 0; i < size; i++) {
        vector[i] = (i * 2) % 10 + 1;
        printf("%.1f ", vector[i]);
    }
    printf("\n\n");

    printf("Matrix: \n");
    double *matrix = (double *)calloc(size * size, sizeof(double));
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[j + size * i] = (i + j) % 10 + 1;
            printf("%.1f ", matrix[j + size * i]);
        }
        printf("\n");
    }
    printf("\n");

    if (pipe(pipe_file_descriptor) < 0) {
        perror("Error when pipe");
    }

    // Create new child process. It would be copy of parent process
    // >0 - return to PARENT_PROCESS Child_Process_ID
    // =0 - return to CHILD_PROCESS
    if ((process_id = fork()) < 0) { 
        perror("Error when fork");
    }
    // parent process
    else if (process_id > 0) {
        parent_work(pipe_file_descriptor, matrix, size);
    }
    // child process
    else {
        child_work(pipe_file_descriptor, vector, size);
    }
    exit(0);
}
