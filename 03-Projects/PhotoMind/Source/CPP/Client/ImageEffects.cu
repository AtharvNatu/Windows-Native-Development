#include "ImageEffects.cuh"

//! Cuda Utils Function Definitions
bool CudaUtils::memAlloc(void **devPtr, size_t size)
{
    // Code
    cudaError_t result = cudaMalloc(devPtr, size);
    if (result != cudaSuccess)
        return false;
    return true;
}

bool CudaUtils::memSet(void *devPtr, int value, size_t count)
{
    // Code
    cudaError_t result = cudaMemset(devPtr, value, count);
    if (result != cudaSuccess)
        return false;
    return true;
}

bool CudaUtils::memCopy(void *dst, const void *src, size_t count, cudaMemcpyKind kind)
{
    // Code
    cudaError_t result = cudaMemcpy(dst, src, count, kind);
    if (result != cudaSuccess)
        return false;
    return true;
}

void CudaUtils::memFree(void **devPtr)
{
    // Code
    if (*devPtr)
    {
        if (cudaFree(*devPtr) == cudaSuccess)
            *devPtr = nullptr;
    }
}

void CudaUtils::convertImageToPixelArr(uchar_t *imageData, uchar3 *pixelArray, size_t size)
{
    // Code
    for (size_t i = 0; i < size; i++, imageData += 3)
    {
        pixelArray[i].x = imageData[0];
        pixelArray[i].y = imageData[1];
        pixelArray[i].z = imageData[2];
    }
}

void CudaUtils::convertPixelArrToImage(uchar3 *pixelArray, uchar_t *imageData, size_t size)
{
    // Code
    for (size_t i = 0; i < size; i++, imageData += 3)
    {
        imageData[0] = pixelArray[i].x;
        imageData[1] = pixelArray[i].y;
        imageData[2] = pixelArray[i].z;
    }
}

__global__ void cudaDesaturation(uchar3* inputImageData, uchar3* outputImageData, size_t imageSize)
{
    // Code
    int pixelId = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (pixelId < imageSize)
    {
        uchar grayscale = (uchar)(
                        (0.3f * (uchar)inputImageData[pixelId].z) +
                        (0.59f * (uchar)inputImageData[pixelId].y) +
                        (0.11f * (uchar)inputImageData[pixelId].x)
                    );
        
        outputImageData[pixelId].x = grayscale;
        outputImageData[pixelId].y = grayscale;
        outputImageData[pixelId].z = grayscale;
    }
}

__global__ void cudaDesaturation(unsigned char* inputImageData, unsigned char* outputImageData, size_t imageSize)
{
    // Code
    int pixelId = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (pixelId < imageSize)
    {
        int idx = pixelId * 3;

        uchar grayscale = (uchar)(
                        (0.3f * (uchar)inputImageData[idx + 0]) +
                        (0.59f * (uchar)inputImageData[idx + 1]) +
                        (0.11f * (uchar)inputImageData[idx + 2])
                    );
        
        outputImageData[idx + 0] = grayscale;
        outputImageData[idx + 1] = grayscale;
        outputImageData[idx + 2] = grayscale;
    }
}

__global__ void cudaSepia(uchar3* inputImageData, uchar3* outputImageData, size_t imageSize)
{
    // Code
    int pixelId = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (pixelId < imageSize)
    {
        float sepiaR = (
            (0.393f * static_cast<float>(inputImageData[pixelId].z)) + 
            (0.769f * static_cast<float>(inputImageData[pixelId].y)) + 
            (0.189f * static_cast<float>(inputImageData[pixelId].x))
        );
        if (sepiaR > 255.0f) sepiaR = 255.0f;

        float sepiaG = (
            (0.349f * static_cast<float>(inputImageData[pixelId].z)) + 
            (0.686f * static_cast<float>(inputImageData[pixelId].y)) + 
            (0.168f * static_cast<float>(inputImageData[pixelId].x))
        );
        if (sepiaG > 255.0f) sepiaG = 255.0f;
        
        float sepiaB = (
            (0.272f * static_cast<float>(inputImageData[pixelId].z)) + 
            (0.534f * static_cast<float>(inputImageData[pixelId].y)) + 
            (0.131f * static_cast<float>(inputImageData[pixelId].x))
        );
        if (sepiaB > 255.0f) sepiaB = 255.0f;
        
        outputImageData[pixelId].x = static_cast<uchar>(sepiaB);
        outputImageData[pixelId].y = static_cast<uchar>(sepiaG);
        outputImageData[pixelId].z = static_cast<uchar>(sepiaR);
    }
}

__global__ void cudaColorInversion(uchar3* inputImageData, uchar3* outputImageData, size_t imageSize)
{
    // Code
    int pixelId = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (pixelId < imageSize)
    {
        outputImageData[pixelId].x = 255 - inputImageData[pixelId].x;
        outputImageData[pixelId].y = 255 - inputImageData[pixelId].y;
        outputImageData[pixelId].z = 255 - inputImageData[pixelId].z;
    }
}

__global__ void cudaGaussianBlur(unsigned char* inputData, unsigned char* outputData, int width, int height, float *kernel)
{
    // Code
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < width && y < height)
    {
        float blur_pixel = 0.0f;
        int kernelRadius = CUDA_GAUSSIAN_KERNEL_SIZE / 2;

        for (int i = -kernelRadius; i <= kernelRadius; i++)
        {
            for (int j = -kernelRadius; j <= kernelRadius; j++)
            {
                int x_offset = x + i;
                int y_offset = y + j;

                if (x_offset >= 0 && x_offset < width && y_offset >= 0 && y_offset < height)
                {
                    int input_index = y_offset * width + x_offset;
                    int kernel_index = (i + kernelRadius) * CUDA_GAUSSIAN_KERNEL_SIZE + (j + kernelRadius);
                    blur_pixel = blur_pixel + static_cast<float>(inputData[input_index]) * kernel[kernel_index];
                }
            }
        }

        outputData[y * width + x] = static_cast<unsigned char>(blur_pixel);
    }
}


int applyDesaturationCUDA1(cv::Mat& image)
{
    uchar3* hostData = nullptr;
    uchar3* deviceInput = nullptr;
    uchar3* deviceOutput = nullptr;

    // Code
    size_t imageSize = image.size().height * image.size().width;

    hostData = new uchar3[imageSize];

    CudaUtils *cudaUtils = new CudaUtils();

    cudaUtils->convertImageToPixelArr(image.data, hostData, imageSize);

    if (!cudaUtils->memAlloc((void**)&deviceInput, imageSize * sizeof(uchar3)))
        return -1;

    if (!cudaUtils->memAlloc((void**)&deviceOutput, imageSize * sizeof(uchar3)))
        return -2;

    if (!cudaUtils->memCopy(deviceInput, hostData, imageSize * sizeof(uchar3), cudaMemcpyHostToDevice))
        return -3;

    //* CUDA Kernel Configuration
    dim3 BLOCKS((imageSize + (THREADS_PER_BLOCK - 1)) / THREADS_PER_BLOCK);

    cudaDesaturation<<<BLOCKS, THREADS_PER_BLOCK>>>(deviceInput, deviceOutput, imageSize);
    cudaDeviceSynchronize();

    if (!cudaUtils->memCopy(hostData, deviceOutput, imageSize * sizeof(uchar3), cudaMemcpyDeviceToHost))
        return -4;

    cudaUtils->convertPixelArrToImage(hostData, image.data, imageSize);

    cudaUtils->memFree((void**)&deviceOutput);
    cudaUtils->memFree((void**)&deviceInput);

    delete cudaUtils;
    cudaUtils = nullptr;

    delete[] hostData;
    hostData = nullptr;

    return 0;
}

int applyDesaturationCUDA(cv::Mat& image)
{
    unsigned char* deviceInput = nullptr;
    unsigned char* deviceOutput = nullptr;

    // Code
    size_t imagePixels = image.size().height * image.size().width;
    size_t dataSize = imagePixels * 3;

    CudaUtils *cudaUtils = new CudaUtils();

    if (!cudaUtils->memAlloc((void**)&deviceInput, dataSize))
        return -1;

    if (!cudaUtils->memAlloc((void**)&deviceOutput, dataSize))
        return -2;

    if (!cudaUtils->memCopy(deviceInput, image.data, dataSize, cudaMemcpyHostToDevice))
        return -3;

    //* CUDA Kernel Configuration
    dim3 BLOCKS((imagePixels + (THREADS_PER_BLOCK - 1)) / THREADS_PER_BLOCK);

    cudaDesaturation<<<BLOCKS, THREADS_PER_BLOCK>>>(deviceInput, deviceOutput, imagePixels);
    cudaDeviceSynchronize();

    if (!cudaUtils->memCopy(image.data, deviceOutput, dataSize, cudaMemcpyDeviceToHost))
        return -4;

    cudaUtils->memFree((void**)&deviceOutput);
    cudaUtils->memFree((void**)&deviceInput);

    delete cudaUtils;
    cudaUtils = nullptr;

    return 0;
}

int applySepiaCUDA(cv::Mat& image)
{
    uchar3* hostData = nullptr;
    uchar3* deviceInput = nullptr;
    uchar3* deviceOutput = nullptr;

    // Code
    size_t imageSize = image.size().height * image.size().width;

    hostData = new uchar3[imageSize];

    CudaUtils *cudaUtils = new CudaUtils();

    cudaUtils->convertImageToPixelArr(image.data, hostData, imageSize);

    if (!cudaUtils->memAlloc((void**)&deviceInput, imageSize * sizeof(uchar3)))
        return -1;

    if (!cudaUtils->memAlloc((void**)&deviceOutput, imageSize * sizeof(uchar3)))
        return -2;

    if (!cudaUtils->memCopy(deviceInput, hostData, imageSize * sizeof(uchar3), cudaMemcpyHostToDevice))
        return -3;

    //* CUDA Kernel Configuration
    dim3 BLOCKS((imageSize + (THREADS_PER_BLOCK - 1)) / THREADS_PER_BLOCK);

    cudaSepia<<<BLOCKS, THREADS_PER_BLOCK>>>(deviceInput, deviceOutput, imageSize);
    cudaDeviceSynchronize();

    if (!cudaUtils->memCopy(hostData, deviceOutput, imageSize * sizeof(uchar3), cudaMemcpyDeviceToHost))
        return -4;

    cudaUtils->convertPixelArrToImage(hostData, image.data, imageSize);

    cudaUtils->memFree((void**)&deviceOutput);
    cudaUtils->memFree((void**)&deviceInput);

    delete cudaUtils;
    cudaUtils = nullptr;

    delete[] hostData;
    hostData = nullptr;

    return 0;
}

int applyColorInversionCUDA(cv::Mat& image)
{
    uchar3* hostData = nullptr;
    uchar3* deviceInput = nullptr;
    uchar3* deviceOutput = nullptr;

    // Code
    size_t imageSize = image.size().height * image.size().width;

    hostData = new uchar3[imageSize];

    CudaUtils *cudaUtils = new CudaUtils();

    cudaUtils->convertImageToPixelArr(image.data, hostData, imageSize);

    if (!cudaUtils->memAlloc((void**)&deviceInput, imageSize * sizeof(uchar3)))
        return -1;

    if (!cudaUtils->memAlloc((void**)&deviceOutput, imageSize * sizeof(uchar3)))
        return -2;

    if (!cudaUtils->memCopy(deviceInput, hostData, imageSize * sizeof(uchar3), cudaMemcpyHostToDevice))
        return -3;

    //* CUDA Kernel Configuration
    dim3 BLOCKS((imageSize + (THREADS_PER_BLOCK - 1)) / THREADS_PER_BLOCK);

    cudaColorInversion<<<BLOCKS, THREADS_PER_BLOCK>>>(deviceInput, deviceOutput, imageSize);
    cudaDeviceSynchronize();

    if (!cudaUtils->memCopy(hostData, deviceOutput, imageSize * sizeof(uchar3), cudaMemcpyDeviceToHost))
        return -4;

    cudaUtils->convertPixelArrToImage(hostData, image.data, imageSize);

    cudaUtils->memFree((void**)&deviceOutput);
    cudaUtils->memFree((void**)&deviceInput);

    delete cudaUtils;
    cudaUtils = nullptr;

    delete[] hostData;
    hostData = nullptr;

    return 0;
}

int applyGaussianBlurCUDA(cv::Mat& image)
{
    // Variable Declarations
    float* hostKernel = nullptr;
    float* deviceKernel = nullptr;

    uchar* deviceInput = nullptr;
    uchar* deviceOutput = nullptr;

    float kernelSum = 0.0f, sigma = 1.0f;

    // Code

    //* Get Image Properties
    int imageWidth = image.cols;
    int imageHeight = image.rows;
    size_t imageSize = imageWidth * imageHeight * sizeof(uchar);

    //* Create Gaussian Kernel
    hostKernel = new float[CUDA_GAUSSIAN_KERNEL_SIZE * CUDA_GAUSSIAN_KERNEL_SIZE];
    int kernelRadius = CUDA_GAUSSIAN_KERNEL_SIZE / 2;

    for (int i = -kernelRadius; i <= kernelRadius; i++) 
    {
        for (int j = -kernelRadius; j <= kernelRadius; j++)
        {
            int index = (i + kernelRadius) * kernelRadius + (j + kernelRadius);
            hostKernel[index] = exp(-(i * i + j + j) / (2.0f * sigma * sigma));
            kernelSum = kernelSum + hostKernel[index];
        }
    }
   
    for (int i = 0; i < CUDA_GAUSSIAN_KERNEL_SIZE * CUDA_GAUSSIAN_KERNEL_SIZE; i++)
        hostKernel[i] = hostKernel[i] / kernelSum;
    
    int kernelSize = CUDA_GAUSSIAN_KERNEL_SIZE * CUDA_GAUSSIAN_KERNEL_SIZE * sizeof(float);

    CudaUtils *cudaUtils = new CudaUtils();

    

    // if (!cudaUtils->memAlloc((void**)&deviceInput, imageSize));
    //     return -1;
    // if (!cudaUtils->memAlloc((void**)&deviceOutput, imageSize));
    //     return -2;
    // if (!cudaUtils->memAlloc((void**)&deviceKernel, kernelSize));
    //     return -3;

    if (cudaMalloc((void**)&deviceInput, imageSize) != cudaSuccess)
        return -1;
    if (cudaMalloc((void**)&deviceOutput, imageSize) != cudaSuccess)
        return -2;
    if (cudaMalloc((void**)&deviceKernel, kernelSize) != cudaSuccess)
        return -3;

    // if (!cudaUtils->memCopy(deviceInput, image.data, imageSize, cudaMemcpyHostToDevice))
    //     return -4;
    // if (!cudaUtils->memCopy(deviceKernel, hostKernel, kernelSize, cudaMemcpyHostToDevice))
    //     return -5;

    if (cudaMemcpy(deviceInput, image.data, imageSize, cudaMemcpyHostToDevice) != cudaSuccess)
        return -4;
    if (cudaMemcpy(deviceKernel, hostKernel, kernelSize, cudaMemcpyHostToDevice) != cudaSuccess)
        return -5;

     // Kernel Configuration
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid(imageHeight, imageWidth);

    cudaGaussianBlur<<<dimGrid, dimBlock>>>(deviceInput, deviceOutput, imageWidth, imageHeight, deviceKernel);
    cudaDeviceSynchronize();

    // if (!cudaUtils->memCopy(image.data, deviceOutput, imageSize, cudaMemcpyDeviceToHost))
    //     return -6;
    if (cudaMemcpy(image.data, deviceOutput, imageSize, cudaMemcpyDeviceToHost) != cudaSuccess)
        return -6;

    cudaUtils->memFree((void**)&deviceKernel);
    cudaUtils->memFree((void**)&deviceOutput);
    cudaUtils->memFree((void**)&deviceInput);

    delete cudaUtils;
    cudaUtils = nullptr;

    delete[] hostKernel;
    hostKernel = nullptr;

    return 0;
}


