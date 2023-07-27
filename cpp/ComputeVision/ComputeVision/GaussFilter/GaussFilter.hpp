//
//  GaussFilter.hpp
//  ComputeVision
//
//  Created by Salvatore on 27/7/23.
//

#ifndef GaussFilter_hpp
#define GaussFilter_hpp

#include <stdio.h>

#include "../Utils/Constants.hpp"
#include "../Utils/Image.hpp"

inline float gaussianWeight(float squaredL2Norm, float sigma) {
    float sigmaSquared = sigma * sigma;
    float normalizationFactor = 1.0 / (sqrt(2.0 * PI) * sigmaSquared);
    return normalizationFactor * exp(-squaredL2Norm / (2.0 * sigmaSquared));
}

inline Pixel applyBlurToPixel(Image &img, int y, int x, int radius){
    
    Pixel blurredPixel;
    float totalWeight = 0.0;
    int squaredL2Norm;
    
    for (int dx = -radius; dx <= radius; ++dx) {

        int xx = x + dx;
        if (xx < 0 || xx >= img.width) continue;
        
        for (int dy = -radius; dy <= radius; ++dy) {

            int yy = y + dy;
            if (yy < 0 || yy >= img.height) continue;

            squaredL2Norm = dx * dx + dy * dy;
            float weight = gaussianWeight(squaredL2Norm, radius);

            totalWeight += weight;
            blurredPixel = blurredPixel + (img.at(yy, xx) * weight);
        }
        
    }
    blurredPixel = blurredPixel * (1.0 / totalWeight);
    return blurredPixel;
}

// Function to apply Gaussian blur on an image
inline Image applyGaussianBlur(Image& image, int kernelSize ) {

    int radius = ceil((float)kernelSize/2.0);
    
    Image blurredImage(image.height, image.width, image.channels);

    for (int y = 0; y < image.height; ++y){
        for (int x = 0; x < image.width; ++x){
            blurredImage.at(y, x) = applyBlurToPixel(image, y, x, radius);
        }
            
    }
    return blurredImage;
}
#endif /* GaussFilter_hpp */
