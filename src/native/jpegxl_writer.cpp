#include "dev_ferrand_chunky_jpegxl_JpegxlWriter.h"
#include "jxl/encode_cxx.h"
#include "jxl/thread_parallel_runner_cxx.h"
#include <vector>
#include <algorithm>


void swapRB(int* pixels, long size)
{
    for(long i = 0; i < size; ++i)
    {
        int cur = pixels[i];
        int r = (cur >> 16) & 0xFF;
        int g = (cur >> 8) & 0xFF;
        int b = cur & 0xFF;
        int a = (cur >> 24) & 0xFF;
        int swapped = (a << 24) | (b << 16) | (g << 8) | r;
        pixels[i] = swapped;
    }
}

jlong Java_dev_ferrand_chunky_jpegxl_JpegxlWriter_compressInMemory(JNIEnv* env, jclass, jint width, jint height, jintArray data, jfloat compression)
{
    JxlEncoderPtr enc = JxlEncoderMake(nullptr);
    
    // Destroyed by the encoder
    JxlEncoderOptions* options = JxlEncoderOptionsCreate(enc.get(), nullptr);
    JxlEncoderOptionsSetDistance(options, compression);
    // If compression is 0, user want lossless
    if(compression < 1e-5)
        JxlEncoderOptionsSetLossless(options, JXL_TRUE);

    auto runner = JxlThreadParallelRunnerMake(
        nullptr, JxlThreadParallelRunnerDefaultNumWorkerThreads());
    if (JXL_ENC_SUCCESS !=
        JxlEncoderSetParallelRunner(enc.get(), JxlThreadParallelRunner, runner.get())) {
        fprintf(stderr, "JxlEncoderSetParallelRunner failed\n");
        return 0;
    }

    JxlPixelFormat pixel_format = {4, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 4};

    JxlBasicInfo basic_info = {};
    basic_info.xsize = width;
    basic_info.ysize = height;
    basic_info.bits_per_sample = 8;
    basic_info.exponent_bits_per_sample = 0;
    basic_info.alpha_exponent_bits = 0;
    basic_info.alpha_bits = 8;
    basic_info.uses_original_profile = JXL_FALSE;
    if (JXL_ENC_SUCCESS != JxlEncoderSetBasicInfo(enc.get(), &basic_info)) {
        fprintf(stderr, "JxlEncoderSetBasicInfo failed\n");
        return 0;
    }

    JxlColorEncoding color_encoding = {};
    JxlColorEncodingSetToSRGB(&color_encoding, JXL_FALSE);
    if (JXL_ENC_SUCCESS != JxlEncoderSetColorEncoding(enc.get(), &color_encoding)) {
        fprintf(stderr, "JxlEncoderSetColorEncoding failed\n");
        return 0;
    }
    
    long pixelCount = env->GetArrayLength(data);
    jboolean isCopy = 0;
    int* rawData = env->GetIntArrayElements(data, &isCopy);
    swapRB(rawData, pixelCount);
    
    if (JXL_ENC_SUCCESS !=
        JxlEncoderAddImageFrame(options,
                                &pixel_format, (void*)rawData,
                                pixelCount * 4)) {
        fprintf(stderr, "JxlEncoderAddImageFrame failed\n");
        if(!isCopy)
            swapRB(rawData, pixelCount);
        env->ReleaseIntArrayElements(data, rawData, JNI_ABORT);
        return 0;
    }

    if(!isCopy)
        swapRB(rawData, pixelCount);
    env->ReleaseIntArrayElements(data, rawData, JNI_ABORT);
    
    auto compressed = std::make_unique<std::vector<uint8_t>>();

    compressed->resize(64);
    uint8_t* next_out = compressed->data();
    size_t avail_out = compressed->size() - (next_out - compressed->data());
    JxlEncoderStatus process_result = JXL_ENC_NEED_MORE_OUTPUT;
    while (process_result == JXL_ENC_NEED_MORE_OUTPUT) {
        process_result = JxlEncoderProcessOutput(enc.get(), &next_out, &avail_out);
        if (process_result == JXL_ENC_NEED_MORE_OUTPUT) {
            size_t offset = next_out - compressed->data();
            compressed->resize(compressed->size() * 2);
            next_out = compressed->data() + offset;
            avail_out = compressed->size() - offset;
        }
    }
    compressed->resize(next_out - compressed->data());
    if (JXL_ENC_SUCCESS != process_result) {
        fprintf(stderr, "JxlEncoderProcessOutput failed\n");
        return 0;
    }
    
    return reinterpret_cast<jlong>(compressed.release());
}


jint Java_dev_ferrand_chunky_jpegxl_JpegxlWriter_readSomeCompressed(JNIEnv* env, jclass, jlong ptr, jbyteArray buffer, jlong pos)
{
    auto compressed = reinterpret_cast<std::vector<uint8_t>*>(ptr);
    if(pos >= compressed->size())
        return 0;
    
    int bufferSize = env->GetArrayLength(buffer);
    auto rawBuffer = env->GetByteArrayElements(buffer, nullptr);
    int elemCount = std::min<long>(bufferSize, compressed->size() - pos);
    
    std::copy_n(compressed->data() + pos, elemCount, rawBuffer);
    
    env->ReleaseByteArrayElements(buffer, rawBuffer, 0);
    
    return elemCount;
}


void Java_dev_ferrand_chunky_jpegxl_JpegxlWriter_freeCompressed(JNIEnv*, jclass, jlong ptr)
{
    delete reinterpret_cast<std::vector<uint8_t>*>(ptr);
}
