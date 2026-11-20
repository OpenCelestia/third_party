/**
 * File:   graphic_buffer_default.c
 * Author: AWTK Develop Team
 * Brief:  graphic_buffer default
 *
 * Copyright (c) 2019 - 2022  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2019-10-31 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "base/bitmap.h"
#include "base/graphic_buffer.h"
#include "ak_mem.h"
#include "ak_common.h"
#include "ak_common_graphics.h"
#include "ak_tde.h"



static graphic_buffer_default_t* graphic_buffer_default_cast(graphic_buffer_t* buffer);

#define GRAPHIC_BUFFER_DEFAULT(buffer) graphic_buffer_default_cast(buffer)

static bool_t graphic_buffer_default_is_valid_for(graphic_buffer_t* buffer, bitmap_t* bitmap) {
  graphic_buffer_default_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL && bitmap != NULL, FALSE);
  if (bitmap->orientation == LCD_ORIENTATION_0 || bitmap->orientation == LCD_ORIENTATION_180) {
    return b->w == bitmap->w && b->h == bitmap->h;
  } else {
    return b->w == bitmap->h && b->h == bitmap->w;
  }
}

static uint8_t* graphic_buffer_default_lock_for_read(graphic_buffer_t* buffer) {
  graphic_buffer_default_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, NULL);

  return b->data;
}

static uint8_t* graphic_buffer_default_lock_for_write(graphic_buffer_t* buffer) {
  graphic_buffer_default_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, NULL);

  return b->data;
}

static ret_t graphic_buffer_default_unlock(graphic_buffer_t* buffer) {
  return RET_OK;
}

static ret_t graphic_buffer_default_attach(graphic_buffer_t* buffer, void* data, uint32_t w,
                                           uint32_t h) {
  graphic_buffer_default_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, RET_BAD_PARAMS);
  return_value_if_fail(b->data_head == NULL, RET_NOT_IMPL);

  b->w = w;
  b->h = h;
  b->data = data;

  return RET_OK;
}

static ret_t graphic_buffer_default_destroy(graphic_buffer_t* buffer) {
  graphic_buffer_default_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, RET_BAD_PARAMS);

#ifndef WITH_G2D
  TKMEM_FREE(b->data_head);
#else
  if( b->data_head != NULL ) {
    log_debug("b->data_head= %p b= %p\n", b->data_head, b );
    ak_mem_dma_free( b->data_head );
  }
#endif
  memset(b, 0x00, sizeof(*b));

  TKMEM_FREE(b);

  return RET_OK;
}

static uint32_t graphic_buffer_default_get_physical_width(graphic_buffer_t* buffer) {
  graphic_buffer_default_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, 0);

  return b->w;
}

static uint32_t graphic_buffer_default_get_physical_height(graphic_buffer_t* buffer) {
  graphic_buffer_default_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, 0);

  return b->h;
}

static uint32_t graphic_buffer_default_get_physical_line_length(graphic_buffer_t* buffer) {
  graphic_buffer_default_t* b = GRAPHIC_BUFFER_DEFAULT(buffer);
  return_value_if_fail(b != NULL, 0);

  return b->line_length;
}

static const graphic_buffer_vtable_t s_graphic_buffer_default_vtable = {
    .lock_for_read = graphic_buffer_default_lock_for_read,
    .lock_for_write = graphic_buffer_default_lock_for_write,
    .unlock = graphic_buffer_default_unlock,
    .attach = graphic_buffer_default_attach,
    .is_valid_for = graphic_buffer_default_is_valid_for,
    .get_width = graphic_buffer_default_get_physical_width,
    .get_height = graphic_buffer_default_get_physical_height,
    .get_line_length = graphic_buffer_default_get_physical_line_length,
    .destroy = graphic_buffer_default_destroy};

static graphic_buffer_t* graphic_buffer_default_create(uint32_t w, uint32_t h,
                                                       bitmap_format_t format,
                                                       uint32_t line_length) {
  uint32_t size = 0;
  uint8_t* data = NULL;
  graphic_buffer_default_t* buffer = NULL;
  uint32_t min_line_length = bitmap_get_bpp_of_format(format) * w;
  return_value_if_fail(w > 0 && h > 0, NULL);

  buffer = TKMEM_ZALLOC(graphic_buffer_default_t);
  return_value_if_fail(buffer != NULL, NULL);
  
  buffer->line_length = tk_max(min_line_length, line_length);
  size = buffer->line_length * h;
#ifndef WITH_G2D
  size = TK_ROUND_TO(size, BITMAP_ALIGN_SIZE) + BITMAP_ALIGN_SIZE;  
#else
  size = TK_ROUND_TO(size, BITMAP_ALIGN_SIZE);
#endif
  /* 这里会多申请32字节数组，为了后面做地址偏移，传给buff一个32字节对齐的地址
   * 因为这里我们改为了dma方式申请，所以地址肯定是32字节对齐的，所以可以不加BITMAP_ALIGN_SIZE，节省内存使用
   */

#ifndef WITH_G2D
  data = (uint8_t*)TKMEM_ALLOC(size);
#else
  data = ak_mem_dma_alloc( 1, size );
  log_debug("w= %u h= %u format= %d line_length= %u size= %u data= %p\n",
               w, h, format, line_length, size, data );
#endif
  if(data == NULL){ // 如果申请失败则释放超过1S未被用到的图片后重新申请
    image_manager_unload_unused(image_manager(), 1);
    data = ak_mem_dma_alloc( 1, size );
    log_debug("w= %u h= %u format= %d line_length= %u size= %u data= %p\n",
               w, h, format, line_length, size, data );
  }

  if (data != NULL) {
    buffer->data_head = data;

    memset(data, 0x00, size);
    while (((uint32_t)(data - (uint8_t*)NULL)) % BITMAP_ALIGN_SIZE) {
      data++;
    }
    buffer->w = w;
    buffer->h = h;
    buffer->data = data;
    buffer->graphic_buffer.vt = &s_graphic_buffer_default_vtable;
    return GRAPHIC_BUFFER(buffer);
  } else {
    TKMEM_FREE(buffer);
    return NULL;
  }
}

/*public functions*/
graphic_buffer_t* graphic_buffer_create_with_data(const uint8_t* data, uint32_t w, uint32_t h,
                                                  bitmap_format_t format) {
  graphic_buffer_default_t* buffer = NULL;

  buffer = TKMEM_ZALLOC(graphic_buffer_default_t);
  return_value_if_fail(buffer != NULL, NULL);

  buffer->w = w;
  buffer->h = h;
  buffer->data = (uint8_t*)data;
  buffer->line_length = bitmap_get_bpp_of_format(format) * w;
  buffer->graphic_buffer.vt = &s_graphic_buffer_default_vtable;

  return GRAPHIC_BUFFER(buffer);
}

ret_t graphic_buffer_create_for_bitmap(bitmap_t* bitmap) {
  uint32_t line_length = bitmap_get_line_length(bitmap);
  return_value_if_fail(bitmap != NULL && bitmap->buffer == NULL, RET_BAD_PARAMS);

  bitmap->buffer = graphic_buffer_default_create(bitmap->w, bitmap->h,
                                                 (bitmap_format_t)(bitmap->format), line_length);
  bitmap->should_free_data = bitmap->buffer != NULL;

  return bitmap->buffer != NULL ? RET_OK : RET_OOM;
}

static graphic_buffer_default_t* graphic_buffer_default_cast(graphic_buffer_t* buffer) {
  return_value_if_fail(buffer != NULL && buffer->vt == &s_graphic_buffer_default_vtable, NULL);

  return (graphic_buffer_default_t*)(buffer);
}
