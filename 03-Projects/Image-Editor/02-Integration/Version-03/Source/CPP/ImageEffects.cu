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
        if (cudaFree(*devPtr) == cudaSuccess);
            *devPtr = nullptr;
    }
}

void CudaUtils::convertImageToPixelArr(uchar_t *imageData, uchar3 *pixelArray, size_t size)
{
    // Code
    for (size_t i = 0; i < size; i++, imageData += 3)
    {
        pixelArray[i].x = imageData[2];
        pixelArray[i].y = imageData[1];
        pixelArray[i].z = imageData[0];
    }
}

void CudaUtils::convertPixelArrToImage(uchar3 *pixelArray, uchar_t *imageData, size_t size)
{
    // Code
    for (size_t i = 0; i < size; i++, imageData += 3)
    {
        imageData[2] = pixelArray[i].x;
        imageData[1] = pixelArray[i].y;
        imageData[0] = pixelArray[i].z;
    }
}

// Variable Declarations
    // cv::Mat cuda_sobel_input_image, cuda_sobel_output_image;

    // Code

    // Reading Input Image
    // cuda_sobel_input_image = cv::imread(cuda_sobel_input_file, cv::IMREAD_GRAYSCALE);
    // cuda_sobel_output_image = cuda_sobel_input_image.clone();

    // double result = sobel_operator(&cuda_sobel_input_image, &cuda_sobel_output_image);

    // cuda_sobel_output_image.convertTo(cuda_sobel_output_image, CV_8UC1);

    // cv::imwrite(cuda_sobel_output_file, cuda_sobel_output_image);

    // // Cleanup Code
    // sobel_cuda_cleanup();
    // cuda_sobel_output_image.release();
    // cuda_sobel_input_image.release();

//! Sobel Operator Function Definitions
SobelOperator::SobelOperator()
{
    device_input = nullptr;
    device_output = nullptr;
    host_kernel = nullptr;
    device_kernel = nullptr;

    cudaUtils = new CudaUtils();
}

SobelOperator::~SobelOperator()
{
    cudaUtils->memFree((void**)&device_kernel);
    cudaUtils->memFree((void**)&device_output);
    cudaUtils->memFree((void**)&device_input);

    if (host_kernel)
    {
        delete[] host_kernel;
        host_kernel = nullptr;
    }

    delete cudaUtils;
    cudaUtils = nullptr;
}

void SobelOperator::applySobelEdgeDetection(cv::Mat *input_image, cv::Mat *output_image)
{
    // Variable Declarations
    float kernel_sum = 0.0f;
    float sigma = 1.0f;

    // Code

    // Get Image Properties
    int image_width = input_image->cols;
    int image_height = input_image->rows;
    int image_size = image_height * image_width * sizeof(unsigned char);

    // Create Gaussian Kernel
    host_kernel = new float[CUDA_GAUSSIAN_KERNEL_SIZE * CUDA_GAUSSIAN_KERNEL_SIZE];
    int kernel_radius = CUDA_GAUSSIAN_KERNEL_SIZE / 2;

    for (int i = -kernel_radius; i <= kernel_radius; i++) 
    {
        for (int j = -kernel_radius; j <= kernel_radius; j++)
        {
            int index = (i + kernel_radius) * kernel_radius + (j + kernel_radius);
            host_kernel[index] = exp(-(i * i + j + j) / (2.0f * sigma * sigma));
            kernel_sum = kernel_sum + host_kernel[index];
        }
    }

    for (int i = 0; i < CUDA_GAUSSIAN_KERNEL_SIZE * CUDA_GAUSSIAN_KERNEL_SIZE; i++)
        host_kernel[i] = host_kernel[i] / kernel_sum;

    cudaUtils->memAlloc((void **)&device_input, image_size);
    cudaUtils->memAlloc((void **)&device_output, image_size);
    cudaUtils->memAlloc((void **)&device_kernel, CUDA_GAUSSIAN_KERNEL_SIZE * CUDA_GAUSSIAN_KERNEL_SIZE * sizeof(float));
    
    cudaUtils->memCopy(device_input, input_image->data, image_size, cudaMemcpyHostToDevice);
    cudaUtils->memCopy(device_kernel, host_kernel, CUDA_GAUSSIAN_KERNEL_SIZE * CUDA_GAUSSIAN_KERNEL_SIZE * sizeof(float), cudaMemcpyHostToDevice);

    // Kernel Configuration
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid(image_height, image_width);

    // CUDA Kernel
    gaussianBlurKernel<<<dimGrid, dimBlock>>>(device_input, device_output, image_width, image_height, device_kernel);
    sobelFilterKernel<<<dimGrid, dimBlock>>>(device_input, device_output, input_image->cols, input_image->rows);

    cudaUtils->memCopy(output_image->data, device_output, image_size, cudaMemcpyDeviceToHost);
}



// // Variable Declarations
// cv::Mat cuda_canny_input_image, cuda_canny_output_image;
// cv::String cuda_canny_input_file, cuda_canny_output_file;

// // Code
// cuda_canny_input_file = input_file;
// filesystem::path output_path = filesystem::path(input_file).filename();
// string output_file_name = output_path.string();

// #if (OS == 1)
//     cuda_canny_output_file = output_file + "\\Canny_CUDA_" + output_file_name;
// #elif (OS == 2)
//     cuda_canny_output_file = output_file + "/Canny_CUDA_" + output_file_name;
// #endif

// cuda_canny_input_image = cv::imread(cuda_canny_input_file, cv::IMREAD_GRAYSCALE);
// cuda_canny_output_image = cuda_canny_input_image.clone();

// double result = canny_operator(cuda_canny_input_image.data, cuda_canny_output_image.data, cuda_canny_input_image.cols, cuda_canny_input_image.rows);

// cuda_canny_output_image.convertTo(cuda_canny_output_image, CV_8UC1);

// cv::imwrite(cuda_canny_output_file, cuda_canny_output_image);

// canny_cuda_cleanup();
// cuda_canny_output_image.release();
// cuda_canny_input_image.release();

CannyOperator::CannyOperator()
{
    input_pixels = nullptr;
    output_pixels = nullptr;
    segment_pixels = nullptr, 
    final_result = nullptr;
    gradient_pixels = nullptr;
    max_pixels = nullptr;
    gaussian_kernel_gpu = nullptr;
    sobel_kernel_x_gpu = nullptr;
    sobel_kernel_y_gpu = nullptr;

    cudaUtils = new CudaUtils();
}

CannyOperator::~CannyOperator()
{
    cudaUtils->memFree((void**)&final_result);
    cudaUtils->memFree((void**)&sobel_kernel_y_gpu);
    cudaUtils->memFree((void**)&sobel_kernel_x_gpu);
    cudaUtils->memFree((void**)&gaussian_kernel_gpu);
    cudaUtils->memFree((void**)&segment_pixels);
    cudaUtils->memFree((void**)&max_pixels);
    cudaUtils->memFree((void**)&gradient_pixels);
    cudaUtils->memFree((void**)&output_pixels);
    cudaUtils->memFree((void**)&input_pixels);

    delete cudaUtils;
    cudaUtils = nullptr;
}

void CannyOperator::applyCannyEdgeDetection(uchar_t *input_image_data, uchar_t *output_image_data, int image_width, int image_height)
{
    // Variable Declarations
	const double gaussian_kernel[9] = 
    {
		1, 2, 1,
		2, 4, 2,
		1, 2, 1
	};

	const schar_t sobel_kernel_x[] = 
    {   
        -1, 0, 1,
		-2, 0, 2,
		-1, 0, 1 
    };

	const schar_t sobel_kernel_y[] = 
    {    
        1, 2, 1,
		0, 0, 0,
		-1,-2,-1 
    };

	const int NUM_BLOCKS = (image_height * image_width) / THREADS_PER_BLOCK;

	cudaUtils->memAlloc((void**)&input_pixels, sizeof(uchar_t) * image_height * image_width);
	cudaUtils->memAlloc((void**)&output_pixels, sizeof(uchar_t) * image_height * image_width);
	cudaUtils->memAlloc((void**)&gradient_pixels, sizeof(double) * image_height * image_width);
	cudaUtils->memAlloc((void**)&final_result, sizeof(uchar_t) * image_height * image_width);
	cudaUtils->memAlloc((void**)&max_pixels, sizeof(double) * image_height * image_width);
	cudaUtils->memAlloc((void**)&segment_pixels, sizeof(uchar_t) * image_height * image_width);
	cudaUtils->memAlloc((void**)&gaussian_kernel_gpu, sizeof(double) * CV_GAUSSIAN_KERNEL_SIZE * CV_GAUSSIAN_KERNEL_SIZE);
	cudaUtils->memAlloc((void**)&sobel_kernel_x_gpu, sizeof(schar_t) * 3 * 3);
	cudaUtils->memAlloc((void**)&sobel_kernel_y_gpu, sizeof(schar_t) * 3 * 3);

	cudaUtils->memCopy(input_pixels, input_image_data, image_height * image_width * sizeof(uchar_t), cudaMemcpyHostToDevice);
	cudaUtils->memCopy(gaussian_kernel_gpu, gaussian_kernel, sizeof(double) * CV_GAUSSIAN_KERNEL_SIZE * CV_GAUSSIAN_KERNEL_SIZE, cudaMemcpyHostToDevice);
	cudaUtils->memCopy(sobel_kernel_x_gpu, sobel_kernel_x, sizeof(schar_t) * CV_GAUSSIAN_KERNEL_SIZE * CV_GAUSSIAN_KERNEL_SIZE, cudaMemcpyHostToDevice);
	cudaUtils->memCopy(sobel_kernel_y_gpu, sobel_kernel_y, sizeof(schar_t) * CV_GAUSSIAN_KERNEL_SIZE * CV_GAUSSIAN_KERNEL_SIZE, cudaMemcpyHostToDevice);

    // CUDA Kernel Call
    gaussianBlur<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>(input_pixels, output_pixels, image_width, image_height, gaussian_kernel_gpu);
    sobelFilter<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>(gradient_pixels, segment_pixels, output_pixels, image_width, image_height, sobel_kernel_x_gpu, sobel_kernel_y_gpu);
    
    cudaUtils->memCopy(max_pixels, gradient_pixels, image_height * image_width * sizeof(double), cudaMemcpyDeviceToDevice);
    
    nonMaxSuppression<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>(max_pixels, gradient_pixels, segment_pixels, image_width, image_height);
    doubleThreshold<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>(output_pixels, max_pixels, CUDA_THRESHOLD * CUDA_RATIO, CUDA_THRESHOLD, image_width, image_height);
    
    cudaUtils->memCopy(final_result, output_pixels, image_height * image_width * sizeof(uchar_t), cudaMemcpyDeviceToDevice);
    
    edgeHysteresis<<<NUM_BLOCKS, THREADS_PER_BLOCK>>>(final_result, output_pixels, image_width, image_height);

	cudaUtils->memCopy(output_image_data, final_result, image_width * image_height * sizeof(uchar_t), cudaMemcpyDeviceToHost);
}



// CUDA Kernels
__global__ void gaussianBlurKernel(
    unsigned char *cuda_sobel_input_image, 
    unsigned char *cuda_sobel_output_image, 
    int width, 
    int height, 
    float *kernel
)
{
    // Code
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < width && y < height)
    {
        float blur_pixel = 0.0f;
        int kernel_radius = CUDA_GAUSSIAN_KERNEL_SIZE / 2;

        for (int i = -kernel_radius; i <= kernel_radius; i++)
        {
            for (int j = -kernel_radius; j <= kernel_radius; j++)
            {
                int x_offset = x + i;
                int y_offset = y + j;

                if (x_offset >= 0 && x_offset < width && y_offset >= 0 && y_offset < height)
                {
                    int input_index = y_offset * width + x_offset;
                    int kernel_index = (i + kernel_radius) * CUDA_GAUSSIAN_KERNEL_SIZE + (j + kernel_radius);
                    blur_pixel = blur_pixel + static_cast<float>(cuda_sobel_input_image[input_index]) * kernel[kernel_index];
                }
            }
        }

        cuda_sobel_output_image[y * width + x] = static_cast<unsigned char>(blur_pixel);
    }
}

__global__ void sobelFilterKernel(
    unsigned char *cuda_sobel_input_image, 
    unsigned char *cuda_sobel_output_image, 
    unsigned int image_width, 
    unsigned int image_height
)
{
    // Variable Declarations
    int sobel_x[CUDA_SOBEL_KERNEL_SIZE][CUDA_SOBEL_KERNEL_SIZE] = 
    {
        { -1, 0, 1 },
        { -2, 0, 2 },
        { -1, 0, 1 }
    };

    int sobel_y[CUDA_SOBEL_KERNEL_SIZE][CUDA_SOBEL_KERNEL_SIZE] = 
    {
        { -1, -2, -1 },
        {  0,  0,  0 },
        {  1,  2,  1 }
    };

    // Code
    int num_rows = blockIdx.x * blockDim.x + threadIdx.x;
    int num_columns = blockIdx.y * blockDim.y + threadIdx.y;

    int index = (num_rows * image_width) + num_columns;

    if ((num_columns < (image_width - 1)) && (num_rows < (image_height - 1)))
    {
        float gradient_x =  (cuda_sobel_input_image[index] * sobel_x[0][0]) + (cuda_sobel_input_image[index + 1] * sobel_x[0][1]) + (cuda_sobel_input_image[index + 2] * sobel_x[0][2]) +
                            (cuda_sobel_input_image[index] * sobel_x[1][0]) + (cuda_sobel_input_image[index + 1] * sobel_x[1][1]) + (cuda_sobel_input_image[index + 2] * sobel_x[1][2]) +
                            (cuda_sobel_input_image[index] * sobel_x[2][0]) + (cuda_sobel_input_image[index + 1] * sobel_x[2][1]) + (cuda_sobel_input_image[index + 2] * sobel_x[2][2]);

        float gradient_y =  (cuda_sobel_input_image[index] * sobel_y[0][0]) + (cuda_sobel_input_image[index + 1] * sobel_y[0][1]) + (cuda_sobel_input_image[index + 2] * sobel_y[0][2]) +
                            (cuda_sobel_input_image[index] * sobel_y[1][0]) + (cuda_sobel_input_image[index + 1] * sobel_y[1][1]) + (cuda_sobel_input_image[index + 2] * sobel_y[1][2]) +
                            (cuda_sobel_input_image[index] * sobel_y[2][0]) + (cuda_sobel_input_image[index + 1] * sobel_y[2][1]) + (cuda_sobel_input_image[index + 2] * sobel_y[2][2]);

        float gradient = sqrtf(gradient_x * gradient_x + gradient_y * gradient_y);

        if (gradient > 255)
            gradient = 255;

        if (gradient < 0)
            gradient = 0;

        __syncthreads();

        cuda_sobel_output_image[index] = gradient;
    }
}

__global__ void gaussianBlur(
    const uchar_t* input_data, 
    uchar_t* output_data, 
    int image_width, 
    int image_height, 
    double* gaussian_kernel
)
{
    // Code
	int id = blockIdx.x * blockDim.x + threadIdx.x;

	const int offset_xy = ((CV_GAUSSIAN_KERNEL_SIZE - 1) / 2);

	if ((id >= 0 && id < image_height * image_width))
    {
        double kernelSum = 0;
        double blurPixel = 0;

        for (int i = 0; i < CV_GAUSSIAN_KERNEL_SIZE; i++) 
        {
            for (int j = 0; j < CV_GAUSSIAN_KERNEL_SIZE; j++) 
            {
                if (((id + ((i - offset_xy) * image_width) + j - offset_xy) >= 0) && 
                    ((id + ((i - offset_xy) * image_width) + j - offset_xy) <= image_height * image_width - 1) && 
                    (((id % image_width) + j - offset_xy) >= 0) && 
                    (((id % image_width) + j - offset_xy) <= (image_width - 1))) 
                    {
                        blurPixel = blurPixel + gaussian_kernel[i * CV_GAUSSIAN_KERNEL_SIZE + j] * input_data[id + ((i - offset_xy) * image_width) + j - offset_xy];
                        kernelSum = kernelSum + gaussian_kernel[i * CV_GAUSSIAN_KERNEL_SIZE + j];
                    }
            }
        }
        
        output_data[id] = (uchar_t)(blurPixel / kernelSum);
    }
}

__global__ void sobelFilter(
    double* gradient_pixels, 
    uchar_t* segment_pixels, 
    const uchar_t* input_data, 
    int image_width, 
    int image_height, 
    schar_t* sobel_kernel_x, 
    schar_t* sobel_kernel_y
) 
{
    // Code
	int id = blockIdx.x * blockDim.x + threadIdx.x;

	if ((id >= 0 && id < image_height * image_width))
    {
        int offset_xy = 1;
        double convolve_X = 0.0;
        double convolve_Y = 0.0;
        int k = 0;
        int segment = 0;

        int x = id % image_width;
        int y = id / image_width;
        
        if (x < offset_xy || x >= image_width - offset_xy || y < offset_xy || y >= image_height - offset_xy)
            return;
        
        int src_id = x + (y * image_width);

        for (int ky = -offset_xy; ky <= offset_xy; ky++) {
            for (int kx = -offset_xy; kx <= offset_xy; kx++) {
                convolve_X += input_data[src_id + (kx + (ky * image_width))] * sobel_kernel_x[k];
                convolve_Y += input_data[src_id + (kx + (ky * image_width))] * sobel_kernel_y[k];
                k++;
            }
        }

        if (convolve_X == 0.0 || convolve_Y == 0.0) 
        {
            gradient_pixels[src_id] = 0;
        }
        else 
        {
            gradient_pixels[src_id] = ((std::sqrt((convolve_X * convolve_X) + (convolve_Y * convolve_Y))));
            double theta = std::atan2(convolve_Y, convolve_X);
            theta = theta * (360.0 / (2.0 * M_PI));

            if ((theta <= 22.5 && theta >= -22.5) || (theta <= -157.5) || (theta >= 157.5))
                segment = 1;
            else if ((theta > 22.5 && theta <= 67.5) || (theta > -157.5 && theta <= -112.5))
                segment = 2;
            else if ((theta > 67.5 && theta <= 112.5) || (theta >= -112.5 && theta < -67.5))
                segment = 3;
            else if ((theta >= -67.5 && theta < -22.5) || (theta > 112.5 && theta < 157.5))
                segment = 4;
        }

        segment_pixels[src_id] = (uchar_t)segment;
    }
}

__global__ void nonMaxSuppression(
    double* max_pixels, 
    double* gradient_pixels, 
    uchar_t* segment_pixels, 
    int image_width, 
    int image_height
) 
{
    // Code
	int id = blockIdx.x * blockDim.x + threadIdx.x;

	if ((id >= 0 && id < image_height * image_width))
    {
        switch (segment_pixels[id]) 
        {
            case 1:
                if (segment_pixels[id - 1] >= gradient_pixels[id] || gradient_pixels[id + 1] > gradient_pixels[id])
                    max_pixels[id] = 0;
            break;

            case 2:
                if (gradient_pixels[id - (image_width - 1)] >= gradient_pixels[id] || gradient_pixels[id + (image_width - 1)] > gradient_pixels[id])
                    max_pixels[id] = 0;
            break;

            case 3:
                if (gradient_pixels[id - (image_width)] >= gradient_pixels[id] || gradient_pixels[id + (image_width)] > gradient_pixels[id])
                    max_pixels[id] = 0;
            break;

            case 4:
                if (gradient_pixels[id - (image_width + 1)] >= gradient_pixels[id] || gradient_pixels[id + (image_width + 1)] > gradient_pixels[id])
                    max_pixels[id] = 0;
            break;

            default:
                max_pixels[id] = 0;
            break;
        }
    }
}

__global__ void doubleThreshold(
    uchar_t* out, 
    double* max_pixels, 
    int strong_threshold, 
    int weak_threshold,
    int image_width, 
    int image_height
) 
{
    // Code
	int id = blockIdx.x * blockDim.x + threadIdx.x;

	if ((id >= 0 && id < image_height * image_width))
    {
        if (max_pixels[id] > strong_threshold)
		    out[id] = 255;
        else if (max_pixels[id] > weak_threshold)
            out[id] = 100;
        else
            out[id] = 0;
    }
}

__global__ void edgeHysteresis(
    uchar_t* out, 
    uchar_t* in, 
    int image_width, 
    int image_height
) 
{
    // Code
	int id = blockIdx.x * blockDim.x + threadIdx.x;

	if ((id >= 0 && id < image_height * image_width))
    {
        if (in[id] == 100) 
        {
            if (in[id - 1] == 255 || in[id + 1] == 255 ||
                in[id - image_width] == 255 || in[id + image_width] == 255 ||
                in[id - image_width - 1] == 255 || in[id - image_width + 1] == 255 ||
                in[id + image_width - 1] == 255 || in[id + image_width + 1] == 255)
                {
                    out[id] = 255;
                }
                
            else
                out[id] = 0;
        }
	}
}
