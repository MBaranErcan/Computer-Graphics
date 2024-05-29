/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h> // In this, we are going to try broadcasting the image data to all the processes.

#include "../include/pnmio.h"

void apply_filter(int *img_data, int *img_result, int width, int height, int local_start, int local_end, int img_colors);

int main(int argc, char *argv[]) {
    if(argc < 2)
    {
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
    int local_start = 0, local_end = 0;

    // Master process reads the image data
    if (my_rank == 0) {
        // 1. Open the PGM (Portable Gray Map) image
        // 1.1 Set the PMG filepath
        char *file_path = argv[1];
        FILE *f = fopen(file_path, "r");
        if (f == NULL)
        {
            printf("Error: Unable to open the PMG file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }

        // 1.2 Read the PGM image header
        read_pgm_header(f, &width, &height, &img_colors, &is_ascii);
        printf("Is ASCII: %d\n", is_ascii);

        // Allocate memory for the image input and the result
        img_data = (int*)malloc(width * height * sizeof(int));
        img_result = (int*)malloc(width * height * sizeof(int));

        // 1.3 Read the PGM image data
        read_pgm_data(f, img_data, is_ascii); // Store the image data in the img_data array

        // Close the file
        fclose(f);

    }
    // Broadcast the image data to all the processes
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&img_colors, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate memory for the image in all other processes
    if (my_rank != 0) {
        img_data = (int*)malloc(width * height * sizeof(int));
        img_result = (int*)malloc(width * height * sizeof(int));
    }
    MPI_Barrier(MPI_COMM_WORLD);
    // Broadcast the image data to all the processes
    MPI_Bcast(img_data, width * height, MPI_INT, 0, MPI_COMM_WORLD);

    // Now that each process has the image data, we can proceed with the edge detection
    rows_per_process = height / comm_sz;
    local_start = (my_rank * rows_per_process - 1) < 0 ? 0 : (my_rank * rows_per_process - 1);  // Start index of the local image data
    local_end = (local_start + rows_per_process + 2) > height ? height : (local_start + rows_per_process + 2); // End index of the local image data

    // Process the image data
    apply_filter(img_data, img_result, width, height, local_start, local_end, img_colors);

    // Master process receives the results from all the processes
    if (my_rank == 0) {
        // Master process will process the first part of the image
        for (int i = 1; i < comm_sz; i++)
        {
            int local_start = (i * rows_per_process - 1) < 0 ? 0 : (i * rows_per_process - 1);  // Start index of the local image data
            int local_end = (local_start + rows_per_process + 2) > height ? height : (local_start + rows_per_process + 2); // End index of the local image data

            // Receive the local image data from the process
            MPI_Recv(&img_result[local_start * width], (local_end - local_start) * width, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        }

        // Save the result
        char *output_file_path = (char*)malloc(strlen(argv[1]) + 10);
        strcpy(output_file_path, argv[1]);
        output_file_path[strlen(argv[1]) - 4] = '\0'; // Remove the ".pgm" extension
        strcat(output_file_path, "_edges.pgm"); // Add "_edges.pgm" to the filename
        FILE *f_out = fopen(output_file_path, "w");
        write_pgm_file(f_out, img_result, width, height, 1, 1, img_colors, width, 1);

        // Clean up
        free(img_data);
        free(img_result);
    } else {
        // Send the local image data to the master process
        MPI_Send(&img_result[local_start * width], (local_end - local_start) * width, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    free(img_data);
    free(img_result);

    MPI_Finalize();

    return 0;
}

// Helper method to apply the edge detection filter
void apply_filter(int *img_data, int *img_result, int width, int height, int local_start, int local_end, int img_colors)
{
    // 2. Find the horizontal edges of the image
    // 2.1 Define the edge detection filter.
    // The filter used here is a 3x3 Sobel filter
    int filter[3][3] = {{-1, 0, 1},
                        {-2, 0, 2},
                        {-1, 0, 1}};

    // 2.2 Apply the edge detection filter to the image
    for (int y = local_start; y < local_end; y++) {  // Iterate over the height, skip border pixels
        for (int x = 1; x < width - 1; x++) {   // Iterate over the width, skip border pixels
            int sum = 0;    // Sum to store the convolution result. Higher values indicate edges. Lower values indicate smooth areas.
            for (int filter_y = 0; filter_y < 3; filter_y++) {  // We are applying a 3x3 filter
                for (int filter_x = 0; filter_x < 3; filter_x++) {
                    int image_x = x + filter_x - 1;
                    int image_y = y + filter_y - 1;
                    int image_value = img_data[image_y * width + image_x];
                    sum += image_value * filter[filter_y][filter_x];
                }
            }
            // If the sum is negative, set it to 0. If it is positive, set it to max pixel value.
            sum = sum < 0 ? 0 : sum;
            sum = sum > img_colors ? img_colors : sum;
            img_result[y * width + x] = sum;
        }
    }
}
*/
