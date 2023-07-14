#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <filesystem>

using namespace std;

int main(int argc, char ** argv) {
    if (argc < 2) {
        cout << "Please supply PPM image as argument." << endl;
        return 1;
    }
    if (!filesystem::exists(argv[1])) {
        cout << "File " << argv[1] << " doesn't exist." << endl;
        return 1;
    }


    // initialize variable
    int constant;
    uint32_t width, height, i, j, j_width;
    int tmp_res[3];
    uint32_t hist[5];
    unsigned char * memblock;
    unsigned char * result;
    char * tmp = new char[4];
    // initialize histogram values
    for (i = 0; i < 5; ++i)
        hist[i] = 0;


    // load file
    ifstream file(argv[1]);
    file >> tmp >> width >> height >> constant;
    file.get();
    j_width= 3*width; // 3 channels of 1 byte size -> 1 line = 3*width
    memblock = new unsigned char [(height+1)*j_width];
    result = new unsigned char [j_width];
    file.read((char*)memblock, height*j_width);
    file.close();

    // init output file
    ofstream output("output.ppm");
    output << "P6" << endl << width << endl << height << endl << 255 << endl;

    // write first line of image - it will not be changed by convolution
    output.write((char *)memblock, j_width);

    // cached lines
    unsigned char * cache_top; 
    unsigned char * cache_mid;
    unsigned char * cache_bot;
    // calculate grayscale pixel values in first line and add them to histogram
    for (i = 0; i < j_width; i = i + 3) {
        int y = (2126*memblock[i] + 7152*memblock[i+1] + 722*memblock[i+2] + 5000)/510000;
        hist[y - (y==5)]++;
    }

    // init cached lines
    cache_top = memblock;
    cache_mid = memblock + j_width;
    cache_bot = memblock + 2*j_width;

    // for each image line
    for (j = 1; j < height-1; ++j) {
        uint32_t size = j*j_width;
        // copy first pixel - it will not change
        result[0] = cache_mid[0];
        result[1] = cache_mid[1];
        result[2] = cache_mid[2];
        // add its grayscale value to histogram
        int y = (2126*cache_mid[0] + 7152*cache_mid[1] + 722*cache_mid[2] + 5000)/510000;
        hist[y - (y==5)]++;
        // calculate convolution for all pixel in image line except first and last
        for (i = 3; i < j_width-3; i += 3) {
            // subtract pixel on left
            tmp_res[0] = 0 - cache_mid[i-3];
            tmp_res[1] = 0 - cache_mid[i-2];
            tmp_res[2] = 0 - cache_mid[i-1];
            // add 5 times actual pixel
            tmp_res[0] = tmp_res[0] + 5*cache_mid[i];
            tmp_res[1] = tmp_res[1] + 5*cache_mid[i+1];
            tmp_res[2] = tmp_res[2] + 5*cache_mid[i+2];
            // subtract pixel on right
            tmp_res[0] = tmp_res[0] - cache_mid[i+3];
            tmp_res[1] = tmp_res[1] - cache_mid[i+4];
            tmp_res[2] = tmp_res[2] - cache_mid[i+5];
            // subtract pixel on top
            tmp_res[0] = tmp_res[0] - cache_top[i];
            tmp_res[1] = tmp_res[1] - cache_top[i+1];
            tmp_res[2] = tmp_res[2] - cache_top[i+2];
            // subtract pixel on bottom
            tmp_res[0] = tmp_res[0] - cache_bot[i];
            tmp_res[1] = tmp_res[1] - cache_bot[i+1];
            tmp_res[2] = tmp_res[2] - cache_bot[i+2];
            // check overflow and underflow
            result[i] = tmp_res[0] < 0 ? 0 : tmp_res[0] > 255 ? 255 : tmp_res[0];
            result[i +1] = tmp_res[1] < 0 ? 0 : tmp_res[1] > 255 ? 255 : tmp_res[1];
            result[i+2] = tmp_res[2] < 0 ? 0 : tmp_res[2] > 255 ? 255 : tmp_res[2];
            // save grayscale pixel value to histogram
            y = (2126*result[i] + 7152*result[i+1] + 722*result[i+2] + 5000)/510000;
            hist[y - (y==5)]++;

        }
        // copy last pixel and add its grayscale value to histogram
        size = size + j_width;
        y = (2126*memblock[size - 3] + 7152*memblock[size - 2] + 722*memblock[size - 1] + 5000)/510000;
        hist[y - (y==5)]++;
        result[j_width - 3] = memblock[size - 3];
        result[j_width - 2] = memblock[size - 2];
        result[j_width - 1] = memblock[size - 1];

        // write result image line into output
        output.write((char *)result, j_width);

        // new values for cache - move cache window by 1 line
        cache_top = cache_mid;
        cache_mid = cache_bot;
        cache_bot = cache_mid + j_width;
    }

    // calculate grayscale histogram values of last line
    int f = (height-1)*j_width;
    for (i = 0; i < j_width; i = i + 3) {
        int y = (2126*memblock[f + i] + 7152*memblock[f + i+1] + 722*memblock[f + i+2] + 5000)/510000;
        hist[y - (y==5)]++;
    }

    // write last line of image
    output.write((char *)&memblock[f], j_width);
    output.close();

    // write histogram values
    ofstream outputhist("output.txt");
    outputhist << hist[0] << " " << hist[1] << " " << hist[2] << " " << hist[3] << " " << hist[4];
    outputhist.close();

    // free memory
    delete[] memblock;
    delete[] result;
    delete[] tmp;
    return 0;
}
