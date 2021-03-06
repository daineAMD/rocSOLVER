/* ************************************************************************
 * Copyright (c) 2019-2021 Advanced Micro Devices, Inc.
 * ************************************************************************ */

#include "roclapack_potf2.hpp"

template <typename T, typename U>
rocblas_status rocsolver_potf2_impl(rocblas_handle handle,
                                    const rocblas_fill uplo,
                                    const rocblas_int n,
                                    U A,
                                    const rocblas_int lda,
                                    rocblas_int* info)
{
    ROCSOLVER_ENTER_TOP("potf2", "--uplo", uplo, "-n", n, "--lda", lda);

    if(!handle)
        return rocblas_status_invalid_handle;

    // argument checking
    rocblas_status st = rocsolver_potf2_potrf_argCheck(handle, uplo, n, lda, A, info);
    if(st != rocblas_status_continue)
        return st;

    // working with unshifted arrays
    rocblas_int shiftA = 0;

    // normal (non-batched non-strided) execution
    rocblas_stride strideA = 0;
    rocblas_int batch_count = 1;

    // memory workspace sizes:
    // size for constants in rocblas calls
    size_t size_scalars;
    // size of reusable workspace
    size_t size_work;
    // size to store pivots in intermediate computations
    size_t size_pivots;
    rocsolver_potf2_getMemorySize<T>(n, batch_count, &size_scalars, &size_work, &size_pivots);

    if(rocblas_is_device_memory_size_query(handle))
        return rocblas_set_optimal_device_memory_size(handle, size_scalars, size_work, size_pivots);

    // memory workspace allocation
    void *scalars, *work, *pivots;
    rocblas_device_malloc mem(handle, size_scalars, size_work, size_pivots);

    if(!mem)
        return rocblas_status_memory_error;

    scalars = mem[0];
    work = mem[1];
    pivots = mem[2];
    if(size_scalars > 0)
        init_scalars(handle, (T*)scalars);

    // execution
    return rocsolver_potf2_template<T>(handle, uplo, n, A, shiftA, lda, strideA, info, batch_count,
                                       (T*)scalars, (T*)work, (T*)pivots);
}

/*
 * ===========================================================================
 *    C wrapper
 * ===========================================================================
 */

extern "C" {

rocblas_status rocsolver_spotf2(rocblas_handle handle,
                                const rocblas_fill uplo,
                                const rocblas_int n,
                                float* A,
                                const rocblas_int lda,
                                rocblas_int* info)
{
    return rocsolver_potf2_impl<float>(handle, uplo, n, A, lda, info);
}

rocblas_status rocsolver_dpotf2(rocblas_handle handle,
                                const rocblas_fill uplo,
                                const rocblas_int n,
                                double* A,
                                const rocblas_int lda,
                                rocblas_int* info)
{
    return rocsolver_potf2_impl<double>(handle, uplo, n, A, lda, info);
}

rocblas_status rocsolver_cpotf2(rocblas_handle handle,
                                const rocblas_fill uplo,
                                const rocblas_int n,
                                rocblas_float_complex* A,
                                const rocblas_int lda,
                                rocblas_int* info)
{
    return rocsolver_potf2_impl<rocblas_float_complex>(handle, uplo, n, A, lda, info);
}

rocblas_status rocsolver_zpotf2(rocblas_handle handle,
                                const rocblas_fill uplo,
                                const rocblas_int n,
                                rocblas_double_complex* A,
                                const rocblas_int lda,
                                rocblas_int* info)
{
    return rocsolver_potf2_impl<rocblas_double_complex>(handle, uplo, n, A, lda, info);
}
}
