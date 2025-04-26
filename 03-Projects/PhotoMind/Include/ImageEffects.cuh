// Headers
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <cuda.h>
#include <cuda_runtime.h>

#define _USE_MATH_DEFINES
#include <math.h>

using std::string;

typedef unsigned char uchar_t;
typedef signed char schar_t;

// Macros
#define CUDA_GAUSSIAN_KERNEL_SIZE   3
#define CV_GAUSSIAN_KERNEL_SIZE     3

#define CUDA_SOBEL_KERNEL_SIZE      5
#define CV_SOBEL_KERNEL_SIZE        5

#define CV_THRESHOLD                40
#define CUDA_THRESHOLD              40

#define CV_RATIO                    3
#define CUDA_RATIO                  3

#define THREADS_PER_BLOCK           1024
#define BLOCK_SIZE                  32

class CudaUtils
{
    public:
    // Function Declarations
    bool memAlloc(void **devPtr, size_t size);
    bool memSet(void *devPtr, int value, size_t count);
    bool memCopy(void *dst, const void *src, size_t count, cudaMemcpyKind kind);
    void memFree(void **devPtr);
    void convertImageToPixelArr(uchar_t *imageData, uchar3 *pixelArray, size_t size);
    void convertPixelArrToImage(uchar3 *pixelArray, uchar_t *imageData, size_t size);
};


int applyDesatCUDA(cv::Mat&);
void applySepiaCUDA(cv::Mat&);
void applyNegCUDA(cv::Mat&);
void applyPixCUDA(cv::Mat&);
void applyBlurCUDA(cv::Mat&);
