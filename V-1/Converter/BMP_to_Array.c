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

    unsigned char *img_data = stbi_load(filename, &width, &height, &channels, 0);
    if (img_data == NULL) {
        printf("Error: Could not load image: %s\n", stbi_failure_reason());
        return;
    }

    if (channels < 3) {
        printf("Error: Image does not have at least 3 channels (RGB).\n");
        stbi_image_free(img_data);
        return;
    }

    struct Pixel **pixels = (struct Pixel **)malloc(height * sizeof(struct Pixel*));
    for (int i = 0; i < height; i++) {
        pixels[i] = (struct Pixel *)malloc(width * sizeof(struct Pixel));
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pixelIndex = (y * width + x) * channels; 
            pixels[y][x].red = img_data[pixelIndex];        
            pixels[y][x].green = img_data[pixelIndex + 1];
            pixels[y][x].blue = img_data[pixelIndex + 2];
        }
    }

    FILE *file = fopen("Generated.hpp","w");
    if (file == NULL) {
    printf("Error: Could not open file.\n");
    return;
    }
    else {

    fprintf(file,"    int Img[]={");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(file,"%d,%d,%d ,", pixels[i][j].red, pixels[i][j].green, pixels[i][j].blue);
        }
        fprintf(file,"\n");
    }
    fprintf(file,"};");
    fclose(file); 
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("Pixel at (%d,%d): R=%d, G=%d, B=%d\n",i, j, pixels[i][j].red, pixels[i][j].green, pixels[i][j].blue);
        }
    }
    stbi_image_free(img_data);

    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

int main() {
    RGPS("image.bmp");
    return 0;
}
