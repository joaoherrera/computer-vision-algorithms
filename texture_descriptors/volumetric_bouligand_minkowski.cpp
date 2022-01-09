#include "volumetric_bouligand_minkowski.hpp"

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

vector<int> volumetric_bouligand_minkowski(Mat grayscale_image, int max_radius){
    Mat edt = tridimensional_euclidian_map(grayscale_image, max_radius);

    int map_width, map_height, map_depth;

    map_width = edt.size[0];
    map_height = edt.size[1];
    map_depth = edt.size[2];

    // Sort map values.
    float *edt_array = (float*) edt.data;
    sort(edt_array, edt_array + (map_width * map_height * map_depth));

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