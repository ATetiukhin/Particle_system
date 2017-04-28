const int blocksize = 16;

__global__ void hello(char *a, int *b)
{
    a[threadIdx.x] += b[threadIdx.x];
}


void run(char * string, int * vector, int csize, int isize)
{
    char *ad;
    int *bd;


    cudaMalloc((void**)&ad, csize);
    cudaMalloc((void**)&bd, isize);
    cudaMemcpy(ad, string, csize, cudaMemcpyHostToDevice);
    cudaMemcpy(bd, vector, isize, cudaMemcpyHostToDevice);

    dim3 dimBlock(blocksize, 1);
    dim3 dimGrid(1, 1);
    hello<<<dimGrid, dimBlock>>>(ad, bd);
    cudaMemcpy(string, ad, csize, cudaMemcpyDeviceToHost);
    cudaFree(ad);
    cudaFree(bd);
}
