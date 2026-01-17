#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>

struct Pixel {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

void RGPS(const char *filename) {
    int width, height, channels;

    // Load image using stb_image
    unsigned char *img_data = stbi_load(filename, &width, &height, &channels, 0);
    if (img_data == NULL) {
        printf("Error: Could not load image: %s\n", stbi_failure_reason());
        return;
    }

    // Ensure the image is in RGB format
    if (channels < 3) {
        printf("Error: Image does not have at least 3 channels (RGB).\n");
        stbi_image_free(img_data);
        return;
    }

    // Allocate memory for pixels
    struct Pixel **pixels = (struct Pixel **)malloc(height * sizeof(struct Pixel*));
    for (int i = 0; i < height; i++) {
        pixels[i] = (struct Pixel *)malloc(width * sizeof(struct Pixel));
    }

    // Convert loaded data into Pixel structure
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pixelIndex = (y * width + x) * channels; // Calculate index in img_data
            pixels[y][x].red = img_data[pixelIndex];        // Assuming img_data is in RGB format
            pixels[y][x].green = img_data[pixelIndex + 1];
            pixels[y][x].blue = img_data[pixelIndex + 2];
        }
    }

    FILE *file = fopen("Generated.c","w");
    if (file == NULL) {
    printf("Error: Could not open file.\n");
    return;
    }
    else {
    //CODE GENERATOR 
    fprintf(file,"#include <stdio.h> \n void Img(int x , int y){\n");
    fprintf(file,"    int Img[]={");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(file,"%d,%d,%d ,", pixels[i][j].red, pixels[i][j].green, pixels[i][j].blue);
        }
        fprintf(file,"\n");
        // for (int j = 0; j < width; j++) {
        //    fprintf(file,"\\033[D\\033[D"); 
        // }
        // fprintf(file,"\\033[B\");\n");
    }
    fprintf(file,"};");
    fprintf(file,"\n}\n\n void main(){\n    Img(0,0);\n}");
    fclose(file); 
    }
    // Print the RGB values of the pixels
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("Pixel at (%d,%d): R=%d, G=%d, B=%d\n",i, j, pixels[i][j].red, pixels[i][j].green, pixels[i][j].blue);
        }
    }
    // Free loaded image data
    stbi_image_free(img_data);

    // Free allocated memory
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

int main() {
    RGPS("image.bmp");
    return 0;
}
