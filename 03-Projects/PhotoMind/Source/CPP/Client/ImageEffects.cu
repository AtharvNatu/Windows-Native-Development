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

__global__ void cudaDesaturation(uchar3* inputData, uchar3* outputData, size_t imageSize)
{
    // Code
    int pixelId = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (pixelId < imageSize)
    {
        uchar grayscale = (uchar)(
                        (0.3f * (uchar)inputData[pixelId].z) +
                        (0.59f * (uchar)inputData[pixelId].y) +
                        (0.11f * (uchar)inputData[pixelId].x)
                    );
        
        outputData[pixelId].x = grayscale;
        outputData[pixelId].y = grayscale;
        outputData[pixelId].z = grayscale;
    }
}

__global__ void cudaDesaturation(unsigned char* inputData, unsigned char* outputData, int imageWidth, int imageHeight)
{
    // Code
    int xColumn = blockIdx.x * blockDim.x + threadIdx.x;
    int yRow = blockIdx.y * blockDim.y + threadIdx.y;

    if (xColumn < imageWidth && yRow < imageHeight)
    {
        int pixelId = (yRow * imageWidth + xColumn) * 3;

        uchar grayscale = (uchar)(
                        (0.3f * (uchar)inputData[pixelId + 0]) +
                        (0.59f * (uchar)inputData[pixelId + 1]) +
                        (0.11f * (uchar)inputData[pixelId + 2])
                    );
        
        outputData[pixelId + 0] = grayscale;
        outputData[pixelId + 1] = grayscale;
        outputData[pixelId + 2] = grayscale;
    }
}

__global__ void cudaSepia(unsigned char* inputData, unsigned char* outputData, int imageWidth, int imageHeight)
{
    // Code
    int xColumn = blockIdx.x * blockDim.x + threadIdx.x;
    int yRow = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (xColumn < imageWidth && yRow < imageHeight)
    {
        int pixelId = (yRow * imageWidth + xColumn) * 3;

        unsigned char pixelBlue = inputData[pixelId + 0];
        unsigned char pixelGreen = inputData[pixelId + 1];
        unsigned char pixelRed = inputData[pixelId + 2];

        float sepiaR = (
            (0.393f * pixelRed) + 
            (0.769f * pixelBlue) + 
            (0.189f * pixelGreen)
        );
        if (sepiaR > 255.0f) sepiaR = 255.0f;

        float sepiaG = (
            (0.349f * pixelRed) + 
            (0.686f * pixelBlue) + 
            (0.168f * pixelGreen)
        );
        if (sepiaG > 255.0f) sepiaG = 255.0f;
        
        float sepiaB = (
            (0.272f * pixelRed) + 
            (0.534f * pixelBlue) + 
            (0.131f * pixelGreen)
        );
        if (sepiaB > 255.0f) sepiaB = 255.0f;
        
        outputData[pixelId + 0] = static_cast<unsigned char>(sepiaB);
        outputData[pixelId + 1] = static_cast<unsigned char>(sepiaG);
        outputData[pixelId + 2] = static_cast<unsigned char>(sepiaR);
    }
}

__global__ void cudaColorInversion(unsigned char* inputData, unsigned char* outputData, int imageWidth, int imageHeight)
{
    // Code
    int xColumn = blockIdx.x * blockDim.x + threadIdx.x;
    int yRow = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (xColumn < imageWidth && yRow < imageHeight)
    {
        int pixelId = (yRow * imageWidth + xColumn) * 3;

        outputData[pixelId + 0] = 255 - inputData[pixelId + 0];
        outputData[pixelId + 1] = 255 - inputData[pixelId + 1];
        outputData[pixelId + 2] = 255 - inputData[pixelId + 2];
    }
}

__global__ void cudaGaussianBlur(unsigned char* inputData, unsigned char* outputData, float *kernel, int imageWidth, int imageHeight)
{
    // Code
    int xColumn = blockIdx.x * blockDim.x + threadIdx.x;
    int yRow = blockIdx.y * blockDim.y + threadIdx.y;

    if (xColumn < imageWidth && yRow < imageHeight)
    {
        int kernelRadius = CUDA_GAUSSIAN_KERNEL_SIZE / 2;
        float blurB = 0.0f, blurG = 0.0f, blurR = 0.0f;

        for (int i = -kernelRadius; i <= kernelRadius; i++)
        {
            for (int j = -kernelRadius; j <= kernelRadius; j++)
            {
                int xOffset = xColumn + i;
                int yOffset = yRow + j;

                if ((xOffset >= 0 && xOffset < imageWidth) && (yOffset >= 0 && yOffset < imageHeight))
                {
                   int pixelIndex = (yOffset * imageWidth + xOffset) * 3;
                   int kernelIndex = (i + kernelRadius) * CUDA_GAUSSIAN_KERNEL_SIZE + (j + kernelRadius);
                    
                   blurB += static_cast<float>(inputData[pixelIndex + 0]) * kernel[kernelIndex];
                   blurG += static_cast<float>(inputData[pixelIndex + 1]) * kernel[kernelIndex];
                   blurR += static_cast<float>(inputData[pixelIndex + 2]) * kernel[kernelIndex];
                }
            }
        }

        int outputIndex = (yRow * imageWidth + xColumn) * 3;
        outputData[outputIndex + 0] = static_cast<unsigned char>(blurB);
        outputData[outputIndex + 1] = static_cast<unsigned char>(blurG);
        outputData[outputIndex + 2] = static_cast<unsigned char>(blurR);
    }
}


void applyCUDAEffect(cv::Mat& image, int effectType, int& status)
{
    // Variable Declarations
    unsigned char* deviceInput = nullptr;
    unsigned char* deviceOutput = nullptr;

    //* Gaussian Blur Related
    float* hostKernel = nullptr;
    float* deviceKernel = nullptr;
    float kernelSum = 0.0f, sigma = 1.0f;
    int kernelSize;

    // Code
    int imageWidth = image.cols;
    int imageHeight = image.rows;
    size_t imagePixels = imageWidth * imageHeight;
    size_t dataSize = imagePixels * 3;

    CudaUtils *cudaUtils = new CudaUtils();

    if (!cudaUtils->memAlloc((void**)&deviceInput, dataSize))
        status = MEM_ALLOC_ERROR;

    if (!cudaUtils->memAlloc((void**)&deviceOutput, dataSize))
        status = MEM_ALLOC_ERROR;

    if (effectType == GAUSSIAN_BLUR_CUDA)
    {
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
        
        kernelSize = CUDA_GAUSSIAN_KERNEL_SIZE * CUDA_GAUSSIAN_KERNEL_SIZE * sizeof(float);
        
        if (!cudaUtils->memAlloc((void**)&deviceKernel, kernelSize))
            status = MEM_ALLOC_ERROR;
    }

    if (!cudaUtils->memCopy(deviceInput, image.data, dataSize, cudaMemcpyHostToDevice))
        status = MEM_COPY_HOST_TO_DEVICE_ERROR;

    if (effectType == GAUSSIAN_BLUR_CUDA)
    {
        if (!cudaUtils->memCopy(deviceKernel, hostKernel, kernelSize, cudaMemcpyHostToDevice))
            status = MEM_COPY_HOST_TO_DEVICE_ERROR;
    }

    //* CUDA Kernel Configuration
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid(imageHeight, imageWidth);

    switch(effectType)
    {
        case DESATURATION_CUDA:
            cudaDesaturation<<<dimGrid, dimBlock>>>(deviceInput, deviceOutput, imageWidth, imageHeight);
        break;

        case SEPIA_CUDA:
            cudaSepia<<<dimGrid, dimBlock>>>(deviceInput, deviceOutput, imageWidth, imageHeight);
        break;

        case COLOR_INVERSION_CUDA:
            cudaColorInversion<<<dimGrid, dimBlock>>>(deviceInput, deviceOutput, imageWidth, imageHeight);
        break;

        case GAUSSIAN_BLUR_CUDA:
            cudaGaussianBlur<<<dimGrid, dimBlock>>>(deviceInput, deviceOutput, deviceKernel, imageWidth, imageHeight);
        break;
    }

    if (!cudaUtils->memCopy(image.data, deviceOutput, dataSize, cudaMemcpyDeviceToHost))
        status = MEM_COPY_DEVICE_TO_HOST_ERROR;

    if (effectType == GAUSSIAN_BLUR_CUDA)
        cudaUtils->memFree((void**)&deviceKernel);

    cudaUtils->memFree((void**)&deviceOutput);
    cudaUtils->memFree((void**)&deviceInput);

    if (effectType == GAUSSIAN_BLUR_CUDA)
    {
        delete[] hostKernel;
        hostKernel = nullptr;
    }

    delete cudaUtils;
    cudaUtils = nullptr;

    status = SUCCESS;
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
    size_t imageSize = imageWidth * imageHeight * sizeof(uchar) * 3;

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

    cudaGaussianBlur<<<dimGrid, dimBlock>>>(deviceInput, deviceOutput, deviceKernel, imageWidth, imageHeight);
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


