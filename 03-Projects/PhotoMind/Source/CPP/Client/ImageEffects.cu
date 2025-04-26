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

#include <fstream>

void logMessage(const std::string& message) {
    FILE* logFile = fopen("log.txt", "a+");
    if (logFile) {
        fprintf(logFile, "%s\n", message.c_str());
        fclose(logFile);
    }
}

// void applyDesatCUDA(cv::Mat& image)
// {
//     uchar3* hostImage = nullptr;
//     uchar3* deviceInput = nullptr;
//     uchar3* deviceOutput = nullptr;

//     size_t imageSize = image.size().height * image.size().width;
//     hostImage = new uchar3[imageSize];

//     CudaUtils *cudaUtils = new CudaUtils();

//     bool isBGR = false;
//     if (image.channels() == 3 && image.type() == CV_8UC3) {
//         isBGR = true;
//     }

//     logMessage("Checking image format...");
//     if (isBGR) {
//         logMessage("Image is in BGR format. Proceeding with conversion.");
//         cudaUtils->convertImageToPixelArr(image.data, hostImage, imageSize);
//     } else {
//         logMessage("Image is not in BGR format. Converting...");
//         cv::Mat tempImage;
//         cv::cvtColor(image, tempImage, cv::COLOR_RGB2BGR);
//         cudaUtils->convertImageToPixelArr(tempImage.data, hostImage, imageSize);
//     }

//     if (!cudaMalloc((void**)&deviceInput, imageSize * sizeof(uchar3)) ||
//         !cudaMalloc((void**)&deviceOutput, imageSize * sizeof(uchar3))) {
//         logMessage("CUDA memory allocation failed!");
//         return;
//     }
//     logMessage("Memory allocated successfully.");

//     if (!cudaMemcpy(deviceInput, hostImage, imageSize * sizeof(uchar3), cudaMemcpyHostToDevice)) {
//         logMessage("CUDA memory copy to device failed!");
//         return;
//     }
//     logMessage("Memory copied to device successfully.");

//     dim3 BLOCKS((imageSize + (THREADS_PER_BLOCK - 1)) / THREADS_PER_BLOCK);
//     logMessage("Launching CUDA kernel...");

//     cudaDesaturation<<<BLOCKS, THREADS_PER_BLOCK>>>(deviceInput, deviceOutput, imageSize);
//     cudaError_t err = cudaGetLastError();
//     if (err != cudaSuccess) {
//         logMessage("Kernel launch failed: " + std::string(cudaGetErrorString(err)));
//         return;
//     }

//     cudaDeviceSynchronize();
//     logMessage("CUDA kernel execution completed.");

//     cudaError_t err1 = cudaMemcpy(deviceOutput, hostImage, imageSize * sizeof(uchar3), cudaMemcpyDeviceToHost);
//     if (err1 != cudaSuccess) {
//         logMessage("CUDA memory copy from device failed! Error: " + std::string(cudaGetErrorString(err)));
//         return;
//     }
//     logMessage("Memory copied from device successfully.");

//     cudaUtils->convertPixelArrToImage(hostImage, image.data, imageSize);

//     if (!cv::imwrite("output_desaturated.png", image)) {
//         logMessage("Error writing the image!");
//         return;
//     }
//     logMessage("Desaturated image saved successfully.");

//     cudaUtils->memFree((void**)&deviceOutput);
//     cudaUtils->memFree((void**)&deviceInput);

//     delete cudaUtils;
//     delete[] hostImage;
// }


int applyDesatCUDA(cv::Mat& image)
{
    uchar3* hostImage = nullptr;
    uchar3* outputData = nullptr;
    uchar3* deviceInput = nullptr;
    uchar3* deviceOutput = nullptr;
    cudaError_t result;

    size_t imageSize = image.size().height * image.size().width;

    hostImage = new uchar3[imageSize];
    outputData = new uchar3[imageSize];

    CudaUtils *cudaUtils = new CudaUtils();

    cudaUtils->convertImageToPixelArr(image.data, hostImage, imageSize);

    if (cudaMalloc((void**)&deviceInput, imageSize * sizeof(uchar3)) != cudaSuccess)
        return -1;

    if (cudaMalloc((void**)&deviceOutput, imageSize * sizeof(uchar3)) != cudaSuccess)
        return -2;

    if (cudaMemcpy(deviceInput, hostImage, imageSize * sizeof(uchar3), cudaMemcpyHostToDevice) != cudaSuccess)
        return -3;


    //* CUDA Kernel Configuration
    dim3 BLOCKS((imageSize + (THREADS_PER_BLOCK - 1)) / THREADS_PER_BLOCK);

    cudaDesaturation<<<BLOCKS, THREADS_PER_BLOCK>>>(deviceInput, deviceOutput, imageSize);
    cudaDeviceSynchronize();

    if (cudaMemcpy(outputData, deviceOutput, imageSize * sizeof(uchar3), cudaMemcpyDeviceToHost) != cudaSuccess)
        return -4;

    cudaUtils->convertPixelArrToImage(outputData, image.data, imageSize);

    cudaFree((void**)&deviceOutput);
    cudaFree((void**)&deviceInput);

    delete cudaUtils;
    cudaUtils = nullptr;

    delete[] outputData;
    outputData = nullptr;

    delete[] hostImage;
    hostImage = nullptr;

    return 0;
}

