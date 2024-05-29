/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "../include/pnmio.h"

void apply_filter(int *img_data, int *img_result, int width, int height, int img_colors);

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Error: Arguments not specified correctly.\n"
               "Usage: ./main <file_path_to_PMG>\n");
        return 1;
    }

    // Initialize MPI
    int comm_sz;    // Number of processes
    int my_rank;    // My process rank

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Image data
    int width, height, img_colors, is_ascii;
    int *img_data = NULL;
    int *img_result = NULL;
    int rows_per_process = 0;

    if (my_rank == 0) {
        // Master process reads the image data
        char *file_path = argv[1];
        FILE *f = fopen(file_path, "r");
        if (f == NULL) {
            printf("Error: Unable to open the PMG file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }

        read_pgm_header(f, &width, &height, &img_colors, &is_ascii);
        printf("Is ASCII: %d\n", is_ascii);

        img_data = (int*)malloc(width * height * sizeof(int));
        img_result = (int*)malloc(width * height * sizeof(int));

        read_pgm_data(f, img_data, is_ascii);
        fclose(f);

        rows_per_process = height / comm_sz;
    }

    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&img_colors, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&rows_per_process, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank != 0) {
        img_data = (int*)malloc(width * height * sizeof(int));
        img_result = (int*)malloc(width * height * sizeof(int));
    }

    int *local_img_data = (int*)malloc(rows_per_process * width * sizeof(int));
    int *local_img_result = (int*)malloc(rows_per_process * width * sizeof(int));

    // Scatter the image data to all processes
    MPI_Scatter(img_data, rows_per_process * width, MPI_INT, local_img_data, rows_per_process * width, MPI_INT, 0, MPI_COMM_WORLD);

    apply_filter(local_img_data, local_img_result, width, rows_per_process, img_colors);

    // Gather the image data from all processes
    MPI_Gather(local_img_result, rows_per_process * width, MPI_INT, img_data, rows_per_process * width, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        char *output_file_path = (char*)malloc(strlen(argv[1]) + 10);
        strcpy(output_file_path, argv[1]);
        output_file_path[strlen(argv[1]) - 4] = '\0';
        strcat(output_file_path, "_edges.pgm");

        FILE *f_out = fopen(output_file_path, "w");
        write_pgm_file(f_out, img_data, width, height, 1, 1, img_colors, width, 1);

        fclose(f_out);
        free(output_file_path);
        free(img_data);
        free(img_result);
    }

    free(local_img_data);
    free(local_img_result);

    MPI_Finalize();

    return 0;
}

void apply_filter(int *img_data, int *img_result, int width, int height, int img_colors) {
    int filter[3][3] = {{-1, 0, 1},
                        {-2, 0, 2},
                        {-1, 0, 1}};

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sum = 0;
            for (int filter_y = 0; filter_y < 3; filter_y++) {
                for (int filter_x = 0; filter_x < 3; filter_x++) {
                    int image_x = x + filter_x - 1;
                    int image_y = y + filter_y - 1;
                    int image_value = img_data[image_y * width + image_x];
                    sum += image_value * filter[filter_y][filter_x];
                }
            }
            sum = sum < 0 ? 0 : sum;
            sum = sum > img_colors ? img_colors : sum;
            img_result[y * width + x] = sum;
        }
    }
}
*/
