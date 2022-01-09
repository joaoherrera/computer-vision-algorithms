#include "volumetric_bouligand_minkowski.hpp"

/**
 * Get the maximum gray intensity of a given gray scale image.
 * @param gray_scale_image A gray scale image.
 * @return the maximum gray intensity of a gray scale image. 
*/
uint get_maximum_gray_intensity(Mat gray_scale_image){
    uint max_gray_value = 0;
    uint pixel_value;

    // Get maximum gray intensity
    for(int i=0; i<gray_scale_image.rows; ++i){
        for(int j=0; j<gray_scale_image.cols; ++j){
            pixel_value = (uint) gray_scale_image.at<uchar>(i, j);

            if(pixel_value > max_gray_value)
                max_gray_value = pixel_value;
        }
    }

    return max_gray_value;
}

/**
 *  Compute a tridimensional (volumetric) euclidian map. The volumetric euclidian map is created
 *  from a gray scale image, where the X and Y coordinates are preserved and the Z coordinate is 
 *  composed by the gray instensities. The voxels locations (x, y, z) corresponding to the gray
 *  values are filled with 0 and the remaining positions are filled with the euclidian distance
 *  between their coordinates (x', y', z') and the nearest voxel (x, y, z).
 *  @param basins_image A gray scale image with values in the range 0 - 255.
 *  @param offset The margin size in X, Y and Z.
 *  @returns the tridimensional euclidian map of the image 'basins_image'.
*/
Mat tridimensional_euclidian_map(Mat basins_image, int offset){
    uint pixel_value;
    uint max_gray_value = get_maximum_gray_intensity(basins_image);

    // We use the offset to assure that all spheres will be complete. Otherwise it will be limited
    // by the borders of the image.

    int volumetric_map_rows = basins_image.rows + (2 * offset);
    int volumetric_map_cols = basins_image.cols + (2 * offset);
    int volumetric_map_channels = max_gray_value + (2 * offset);

    int map_dimension[3] = {volumetric_map_cols, volumetric_map_rows, volumetric_map_channels};
    Mat edm = Mat(3, map_dimension, CV_32FC1);

    int vx, vy, vz;

    // Initialize volumetric map
    // (x, y, gray intensity) voxels will be 0 and the others are 1.

    for(int k=0; k<volumetric_map_channels; ++k)
        for(int i=0; i<volumetric_map_rows; ++i)
            for(int j=0; j<volumetric_map_cols; ++j)
                edm.at<float>(i, j, k) = 1;    
    
    for(int i=0; i<basins_image.rows; ++i){
        for(int j=0; j<basins_image.cols; ++j){
            pixel_value = (uint) basins_image.at<uchar>(i, j);

            vx = j + offset;
            vy = i + offset;
            vz = pixel_value + offset;
            
            edm.at<float>(vy, vx, vz) = 0;
        }
    }

    // Compute Euclidian Distance Transform and get the minimum distance between a voxel 
    // with value 1 and a voxel with value 0.
    float distance, min_distance;
    uint dx, dy, dz;

    for(int k=0; k<volumetric_map_channels; ++k){
        for(int i=0; i<volumetric_map_rows; ++i){
            for(int j=0; j<volumetric_map_cols; ++j){

                if(edm.at<float>(i, j, k) == 0.0)
                    continue;

                min_distance = max_gray_value;

                for(int a=0; a<basins_image.rows; ++a){
                    for(int b=0; b<basins_image.cols; ++b){
                        dx = pow(b - j + offset, 2);
                        dy = pow(a - i + offset, 2);
                        dz = pow(((uint) basins_image.at<uchar>(a, b)) - k + offset, 2);

                        distance = sqrt(dx + dy + dz);

                        if(distance < min_distance)
                            min_distance = distance;
                    }
                }
                edm.at<float>(i, j, k) = min_distance;
            }
        }
    }

    return edm;
}

/**
 * Run the Volumetric Bouligand-Minkowski texture descriptor.
 * @param grayscale_image A grayscale image.
 * @param max_radius The maximum euclidian distance value to consider when creating the 
 * feature array.
 * @return a vector with the cumulative sums among the euclidian distances <= 'max_radius'.
*/
vector<int> volumetric_bouligand_minkowski(Mat grayscale_image, int max_radius){
    Mat edt = tridimensional_euclidian_map(grayscale_image, max_radius);

    int map_width, map_height, map_depth;

    map_width = edt.size[0];
    map_height = edt.size[1];
    map_depth = edt.size[2];

    float *edt_array = (float*) edt.data;

    // To achieve a better performance (of time), the values of the EDT are usually sorted.
    // Using a map assures that the values will be in an asceding order.
    map<float, int> distances_occurrences;
    char key[10];

    // Get the occurrences of each existing value in the array.
    // It is very import to truncate keys to avoid inserting multiple instances 
    // of a given distance into different buckets due to insignificant decimal places.
    for(int i=0; i<map_width * map_height * map_depth; ++i){
        sprintf(key, "%.2f", edt_array[i]);
        distances_occurrences[atof(key)] += 1;
    }

    vector<int> distances_acc;
    int cumsum = 0;

    for(auto i = distances_occurrences.begin(); i != distances_occurrences.end(); ++i){
        if(i->first <= max_radius){
            cumsum += i->second;
            distances_acc.push_back(cumsum);
        }
        else
            break;
    }

    return distances_acc;
}