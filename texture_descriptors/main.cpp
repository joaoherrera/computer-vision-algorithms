#include <iostream>
#include <time.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "texture_descriptors/volumetric_bouligand_minkowski.hpp"

using namespace std;
using namespace cv;

/**
 * Create a random image.
 * @param width Width of the random image.
 * @param height Height of the random image.
**/
Mat create_random_image(int width, int height){
    Mat random_image = Mat(Size(width, height), CV_8UC1);

    srand((unsigned) time(NULL));

    // Assign random values
    for(int i=0; i<width; ++i)
        for(int j=0; j<height; ++j)
            random_image.at<uchar>(i, j) = rand() % 255;
    
    return random_image;
}

int main(int argc, char *argv[]){
    Mat image = create_random_image(5, 5);

    vector<int> texture_array = volumetric_bouligand_minkowski(image, 10);
}