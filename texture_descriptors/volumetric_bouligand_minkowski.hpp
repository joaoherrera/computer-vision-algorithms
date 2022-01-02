#ifndef __TEXTURE_DESCRIPTORS_H__
#define __TEXTURE_DESCRIPTORS_H__

#include <stdio.h>
#include <math.h>
#include <opencv4/opencv2/core.hpp>
#include <unordered_map>

using namespace std;
using namespace cv;

uint get_maximum_gray_intensity(Mat gray_scale_image);

Mat tridimensional_euclidian_map(Mat basins_image, int offset=10);
int* volumetric_bouligand_minkowski(Mat grayscale_image, int max_radius);

#endif