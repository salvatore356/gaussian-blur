//
//  main.cpp
//  ComputeVision
//
//  Created by Salvatore on 25/7/23.
//

#include <iostream>
#include <format>

#include "Utils/Utils.hpp"
#include "GaussFilter/GaussFilter.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    auto img = readJPG("donut.jpg");
    int kernelSize = 7;
    
    auto blurred = applyGaussianBlur(*img, kernelSize);
    
    string outputPath = "donut-kernel-" + to_string(kernelSize) + ".jpg";
    writeJPG(blurred, outputPath);
    return 0;
    
    
}
