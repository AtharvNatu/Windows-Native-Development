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

enum CUDA_EFFECT
{
    DESATURATION_CUDA,
    SEPIA_CUDA,
    COLOR_INVERSION_CUDA,
    PIXELATION_CUDA,
    GAUSSIAN_BLUR_CUDA
};

enum CUDA_STATUS
{
    MEM_ALLOC_ERROR = -1,
    MEM_COPY_HOST_TO_DEVICE_ERROR = -2,
    MEM_COPY_DEVICE_TO_HOST_ERROR = -3,
    SUCCESS = 0
};

class CudaUtils
{
    public:
    // Function Declarations
    bool memAlloc(void **devPtr, size_t size);
    bool memSet(void *devPtr, int value, size_t count);
    bool memCopy(void *dst, const void *src, size_t count, cudaMemcpyKind kind);
    void memFree(void **devPtr);
};


void applyCUDAEffect(cv::Mat&, int, int&);

int applyGaussianBlurCUDA(cv::Mat&);
void applyPixCUDA(cv::Mat&);
