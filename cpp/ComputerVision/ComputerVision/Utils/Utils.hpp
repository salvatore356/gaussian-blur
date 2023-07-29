//
//  Utils.hpp
//  ComputeVision
//
//  Created by Salvatore on 25/7/23.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <stdio.h>
#include <jpeglib.h>
#include "Image.hpp"

using namespace std;

optional<Image> readJPG(string filename) {
    
    struct jpeg_decompress_struct info;
    struct jpeg_error_mgr err;

    unsigned long int imgWidth, imgHeight;
    int numComponents;

    unsigned long int dwBufferBytes;
    unsigned char* lpData;

    unsigned char* lpRowBuffer[1];

    FILE* fHandle;

    fHandle = fopen(filename.c_str(), "rb");
    if(fHandle == NULL) {
        fprintf(stderr, "%s:%u: Failed to read file %s\n", __FILE__, __LINE__, filename.c_str());
        return nullopt;
    }

    info.err = jpeg_std_error(&err);
    jpeg_create_decompress(&info);

    jpeg_stdio_src(&info, fHandle);
    jpeg_read_header(&info, TRUE);

    jpeg_start_decompress(&info);
    imgWidth = info.output_width;
    imgHeight = info.output_height;
    numComponents = info.num_components;

    dwBufferBytes = imgWidth * imgHeight * numComponents; /* We only read RGB, not A */
    lpData = (unsigned char*)malloc(sizeof(unsigned char)*dwBufferBytes);

    Image img((int)imgWidth, (int)imgHeight, numComponents);

    /* Read scanline by scanline */
    while(info.output_scanline < info.output_height) {
        lpRowBuffer[0] = (unsigned char *)(&lpData[numComponents*info.output_width*info.output_scanline]);
        jpeg_read_scanlines(&info, lpRowBuffer, 1);
    }

    jpeg_finish_decompress(&info);
    jpeg_destroy_decompress(&info);
    fclose(fHandle);
    
    for(int r = 0; r < imgHeight; ++r) {
        for(int c = 0; c < imgWidth; ++c){
            
            for(int ch = 0; ch < numComponents; ++ch){

                int index = ((r * imgWidth + c) * numComponents) + ch;
                float channel = ((float)(lpData[index]) / 255.0);

                switch (ch) {
                    case 1:
                        img.at(r, c).g = channel;
                        break;
                    case 2:
                        img.at(r, c).b = channel;
                        break;
                    default:
                        img.at(r, c).r = channel;
                        break;
                }
            }
        }
    }

    return img;
}

inline int writeJPG(Image lpImage, string filename) {
    struct jpeg_compress_struct info;
    struct jpeg_error_mgr err;

    unsigned char* lpRowBuffer[1];

    FILE* fHandle;

    fHandle = fopen(filename.c_str(), "wb");
    if(fHandle == NULL) {
        #ifdef DEBUG
            fprintf(stderr, "%s:%u Failed to open output file %s\n", __FILE__, __LINE__, filename.c_str());
        #endif
        return 1;
    }

    info.err = jpeg_std_error(&err);
    jpeg_create_compress(&info);

    jpeg_stdio_dest(&info, fHandle);

    info.image_width = lpImage.width;
    info.image_height = lpImage.height;
    info.input_components = lpImage.channels;
    info.in_color_space = lpImage.channels == 1 ? JCS_GRAYSCALE : JCS_RGB;
    
    jpeg_set_defaults(&info);
    jpeg_set_quality(&info, 100, TRUE);

    jpeg_start_compress(&info, TRUE);
    unsigned char* lpData = new unsigned char[lpImage.width * lpImage.height * lpImage.channels];
    
    for(int ch = 0; ch < lpImage.channels; ++ch)
        for(int r = 0; r < lpImage.height; ++r)
            for(int c = 0; c < lpImage.width; ++c) {
                int index = ((r * lpImage.width + c) * lpImage.channels) + ch;
                switch (ch) {
                    case 2:
                        lpData[index] = (unsigned char)(255.0 * lpImage.at(r, c).b);
                        break;
                    case 1:
                        lpData[index] = (unsigned char)(255.0 * lpImage.at(r, c).g);
                        break;
                    default:
                        lpData[index] = (unsigned char)(255.0 * lpImage.at(r, c).r);
                        break;
                }
            }

    /* Write every scanline ... */
    while(info.next_scanline < info.image_height) {
        lpRowBuffer[0] = &(lpData[info.next_scanline * (lpImage.width * lpImage.channels)]);
        jpeg_write_scanlines(&info, lpRowBuffer, 1);
    }

    jpeg_finish_compress(&info);
    fclose(fHandle);

    jpeg_destroy_compress(&info);
    return 0;
}

#endif /* Utils_hpp */
