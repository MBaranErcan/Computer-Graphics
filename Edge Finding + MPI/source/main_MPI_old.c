/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "../include/pnmio.h"

void apply_filter(int *img_data, int *img_result, int width, int height, int local_start, int local_end);

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Error: Arguments not specified correctly.\n"
               "Usage: ./main <file_path_to_PMG>\n");
        return 1;
    }


    // Image data
    int width, height, img_colors, is_ascii;
    int *img_data;
    int *img_result;

    // Initialize MPI
    int comm_sz;    // Number of processes
    int my_rank;    // My process rank
    int rows_per_process;
    int local_height;

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

 Master process reads the image data

    if (my_rank == 0) {
        // 1. Open the PGM (Portable Gray Map) image
        // 1.1 Set the PMG filepath
        char *file_path = argv[1];
        FILE *f = fopen(file_path, "r");
        if (f == NULL)
        {
            printf("Error: Unable to open the PMG file.\n");
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

        // Next, we will distribute the image data to all processes
        rows_per_process = height / comm_sz;
        local_height = rows_per_process + 2; // Add 2 to include the border pixels

        // Send the image data to all processes
        for (int i = 1; i < comm_sz; i++)
        {
            int local_start = (i * rows_per_process - 1) < 0 ? 0 : (i * rows_per_process - 1);  // Start index of the local image data
            int local_end = (local_start + local_height) > height ? height : (local_start + local_height); // End index of the local image data

            // Send the local image data to the process
            MPI_Send(&local_start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&local_end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&img_data[local_start * width], local_height * width, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&width, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&height, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Master process will process the first part of the image
        int local_start = 0;
        int local_end = local_height;
        apply_filter(img_data, img_result, width, height, local_start, local_end);


        // Receive the processed image data from all processes
        for (int i = 1; i < comm_sz; i++)
        {
            int local_start;
            int local_end;
            MPI_Recv(&local_start, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&local_end, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&img_result[local_start * width], (local_end - local_start) * width, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        }


        // 3. Add <filename>"_edges.pgm" to the name and save the result. (First erase.pgm, then add _edges.pgm)
        char *output_file_path = (char*)malloc(strlen(file_path) + 10);
        strcpy(output_file_path, file_path);
        output_file_path[strlen(file_path) - 4] = '\0'; // Remove the ".pgm" extension
        strcat(output_file_path, "_edges.pgm"); // Add "_edges.pgm" to the filename
        FILE *f_out = fopen(output_file_path, "w");
        write_pgm_file(f_out, img_result, width, height, 1, 1, img_colors, width, 1);


    }
 Slave processes receive the image data and process it

    else {
        int local_start;
        int local_end;
        int *local_img_data;
        int local_width;
        int local_height;

        // Receive the local image data
        MPI_Recv(&local_start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&local_end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        local_height = local_end - local_start;
        local_img_data = (int*)malloc(width * local_height * sizeof(int));
        MPI_Recv(local_img_data, local_height * width, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&local_width, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&local_height, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        // Apply the filter to the local image data
        apply_filter(local_img_data, img_result, local_width, local_height, local_start, local_end);

        // Send the processed image data back to the master process
        MPI_Send(&local_start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&local_end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(img_result, local_height * width, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();


    // 4. Clean up
    free(img_data);
    free(img_result);


    return 0;
}

// Helper method to apply the edge detection filter
void apply_filter(int *img_data, int *img_result, int width, int height, int local_start, int local_end)
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
            // If the sum is negative, set it to 0. If it is positive, set it to 255.
            sum = sum < 0 ? 0 : sum;
            sum = sum > 255 ? 255 : sum;
            img_result[y * width + x] = sum;
        }
    }
}
*/
