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

class SobelOperator
{
    private:
        unsigned char *device_input, *device_output;
        float *host_kernel, *device_kernel;
        CudaUtils *cudaUtils = nullptr;

    public:
        SobelOperator();
        ~SobelOperator();

        void applySobelEdgeDetection(cv::Mat *input_image, cv::Mat *output_image);
};

class CannyOperator
{
    private:
        uchar_t *input_pixels = nullptr, *output_pixels = nullptr, *segment_pixels = nullptr, *final_result = nullptr;
        double *gradient_pixels = nullptr, *max_pixels = nullptr, *gaussian_kernel_gpu = nullptr;
        schar_t* sobel_kernel_x_gpu = nullptr, *sobel_kernel_y_gpu = nullptr;
        CudaUtils *cudaUtils = nullptr;

    public:
        CannyOperator();
        ~CannyOperator();

        void applyCannyEdgeDetection(uchar_t *input_image_data, uchar_t *output_image_data, int image_width, int image_height);
};

//! CUDA Kernel Prototypes
__global__ void gaussianBlurKernel(
    unsigned char *cuda_sobel_input_image, 
    unsigned char *cuda_sobel_output_image, 
    int width, 
    int height, 
    float *kernel
);

__global__ void sobelFilterKernel(
    unsigned char *cuda_sobel_input_image, 
    unsigned char *cuda_sobel_output_image, 
    unsigned int image_width, 
    unsigned int image_height
);

__global__ void gaussianBlur(
    const uchar_t* input_data, 
    uchar_t* output_data, 
    int image_width, 
    int image_height, 
    double* gaussian_kernel
);

__global__ void sobelFilter(
    double* gradient_pixels, 
    uchar_t* segment_pixels, 
    const uchar_t* input_data, 
    int image_width, 
    int image_height, 
    schar_t* sobel_kernel_x, 
    schar_t* sobel_kernel_y
);

__global__ void nonMaxSuppression(
    double* max_pixels, 
    double* gradient_pixels, 
    uchar_t* segment_pixels, 
    int image_width, 
    int image_height
);

__global__ void doubleThreshold(
    uchar_t* out, 
    double* max_pixels, 
    int strong_threshold, 
    int weak_threshold,
    int image_width, 
    int image_height
);

__global__ void edgeHysteresis(
    uchar_t* out, 
    uchar_t* in, 
    int image_width, 
    int image_height
);

