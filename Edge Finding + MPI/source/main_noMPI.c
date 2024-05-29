#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/pnmio.h"

void apply_filter(int *img_data, int *img_result, int width, int height, int local_start, int local_end, int img_colors);

int main(int argc, char *argv[])
{
/*    if(argc < 2)
    {
        printf("Error: Arguments not specified correctly.\n"
               "Usage: ./main <file_path_to_PMG>\n");
        return 1;
    }*/


    int width, height, img_colors, is_ascii;
    int *img_data;
    int *img_result;

    // 1. Open the PGM (Portable Gray Map) image
    // 1.1 Set the PMG filepath
//    char *file_path = argv[1];
    char *file_path = "../images/car.pgm";
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

    // 2. Find the horizontal edges of the image

    // 2.1 Apply the edge detection filter to the image
    apply_filter(img_data, img_result, width, height, 1, height - 1, img_colors);



    // 3. Add <filename>"_edges.pgm" to the name and save the result. (First erase.pgm, then add _edges.pgm)
    char *output_file_path = (char*)malloc(strlen(file_path) + 10);
    strcpy(output_file_path, file_path);
    output_file_path[strlen(file_path) - 4] = '\0'; // Remove the ".pgm" extension
    strcat(output_file_path, "_edges.pgm"); // Add "_edges.pgm" to the filename
    FILE *f_out = fopen(output_file_path, "w");
    write_pgm_file(f_out, img_result, width, height, 1, 1, img_colors, width, 1);


    // 4. Clean up
    free(img_data);
    free(img_result);

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
            // If the sum is negative, set it to 0. If it is positive, set it to 255.
            sum = sum < 0 ? 0 : sum;
            sum = sum > img_colors ? img_colors : sum;
            img_result[y * width + x] = sum;
        }
    }
}
