# 源码修改记录

## 支持申请平台DMA memory
1. 修改 libavutil/buffer.h，添加以下接口定义：
AVBufferRef *ak_buffer_allocz(size_t size);
<br/>

2. 修改 libavutil/buffer.c，添加以下代码：
```C
#include "ak_mem.h"
static void ak_buffer_default_free(void *opaque, uint8_t *data)
{
#ifdef AK_BASE_MemAlloc
    AK_BASE_MemDmaFree(data);
#else
    ak_mem_dma_free(data);
#endif
}

static AVBufferRef *ak_buffer_alloc(size_t size)
{
    AVBufferRef *ret = NULL;
    uint8_t    *data = NULL;

#ifdef AK_BASE_MemAlloc
    data = (uint8_t *)AK_BASE_MemDmaAlloc(0xFE, size);
#else
    data = (uint8_t *)ak_mem_dma_alloc(0xFE, size);
#endif

    if (!data)
    {
        printf("[%s][%d] failed to allocating dma memory.\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    ret = av_buffer_create(data, size, ak_buffer_default_free, NULL, 0);
    if (!ret)
    {
        printf("[%s][%d] failed to creating dma memory.\n", __FUNCTION__, __LINE__);
        ak_buffer_default_free(NULL, &data);
    }

    return ret;
}

AVBufferRef *ak_buffer_allocz(size_t size)
{
    AVBufferRef *ret = ak_buffer_alloc(size);
    if (!ret)
        return NULL;

    memset(ret->data, 0, size);
    return ret;
}
```
<br/>

3. 修改 libavcodec/get_buffer.c(V4.2.9为libavcodec/decode.c文件)文件中update_frame_pool接口实现，修改如下：
将原代码：
```C
                pool->pools[i] = av_buffer_pool_init(size[i] + 16 + STRIDE_ALIGN - 1,
                                                     CONFIG_MEMORY_POISONING ?
                                                        NULL :
                                                        av_buffer_allocz);
```
修改如下：
```C
#if CONFIG_PLATFORM_DMA
                pool->pools[i] = av_buffer_pool_init(size[i] + 16 + STRIDE_ALIGN - 1,
                                                     CONFIG_MEMORY_POISONING ?
                                                        NULL :
                                                        ak_buffer_allocz);
#else
                pool->pools[i] = av_buffer_pool_init(size[i] + 16 + STRIDE_ALIGN - 1,
                                                     CONFIG_MEMORY_POISONING ?
                                                        NULL :
                                                        av_buffer_allocz);
#endif
```

4. 添加configure参数如下：
```C
--extra-cflags="-I$(PLATFORM_PATH)/platform/include -DCONFIG_ANYKA_DMA=1"
```