/**
 * File:   image_g2d.c
 * Author: AWTK Develop Team
 * Brief:  image 2d operations.
 *
 * Copyright (c) 2018 - 2022  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2018-06-14 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "base/g2d.h"
#include "blend/soft_g2d.h"
#include "blend/image_g2d.h"
#include "awtk_config.h"

#ifdef WITH_G2D

ret_t TK_WEAK g2d_fill_rect(bitmap_t* fb, const rect_t* dst, color_t c) {
  (void)fb;
  (void)dst;
  (void)c;
  return RET_NOT_IMPL;
}

ret_t TK_WEAK g2d_copy_image(bitmap_t* fb, bitmap_t* img, const rect_t* src, xy_t x, xy_t y) {
  (void)fb;
  (void)img;
  (void)x;
  (void)y;
  return RET_NOT_IMPL;
}

ret_t TK_WEAK g2d_rotate_image(bitmap_t* fb, bitmap_t* img, const rect_t* src,
                               lcd_orientation_t o) {
  (void)fb;
  (void)img;
  (void)src;
  (void)o;
  return RET_NOT_IMPL;
}

ret_t TK_WEAK g2d_blend_image(bitmap_t* fb, bitmap_t* img, const rect_t* dst, const rect_t* src,
                              uint8_t global_alpha) {
  (void)fb;
  (void)img;
  (void)dst;
  (void)src;
  (void)global_alpha;
  return RET_NOT_IMPL;
}

#ifdef WITH_FAST_LCD_PORTRAIT
ret_t TK_WEAK g2d_rotate_image_ex(bitmap_t* dst, bitmap_t* src, const rect_t* src_r, xy_t dx,
                                  xy_t dy, lcd_orientation_t o) {
  (void)dst;
  (void)src;
  (void)src_r;
  (void)dx;
  (void)dy;
  (void)o;
  return RET_NOT_IMPL;
}

ret_t TK_WEAK g2d_blend_image_rotate(bitmap_t* dst, bitmap_t* src, const rectf_t* dst_r,
                                     const rectf_t* src_r, uint8_t alpha, lcd_orientation_t o) {
  (void)dst;
  (void)src;
  (void)dst_r;
  (void)src_r;
  (void)alpha;
  (void)o;
  return RET_NOT_IMPL;
}
#endif

#endif

ret_t image_fill(bitmap_t* dst, const rect_t* dst_r, color_t c) {
  return_value_if_fail(dst != NULL && dst_r != NULL, RET_OK);

  assert(dst_r->x >= 0 && (dst_r->x + dst_r->w) <= bitmap_get_physical_width(dst));
  assert(dst_r->y >= 0 && (dst_r->y + dst_r->h) <= bitmap_get_physical_height(dst));

#ifdef WITH_G2D
  if (g2d_fill_rect(dst, dst_r, c) == RET_OK) {
    return RET_OK;
  }
#endif /*WITH_G2D*/
  return soft_fill_rect(dst, dst_r, c);
}

ret_t image_clear(bitmap_t* dst, const rect_t* dst_r, color_t c) {
  return_value_if_fail(dst != NULL && dst_r != NULL, RET_OK);

  assert(dst_r->x >= 0 && (dst_r->x + dst_r->w) <= bitmap_get_physical_width(dst));
  assert(dst_r->y >= 0 && (dst_r->y + dst_r->h) <= bitmap_get_physical_height(dst));

  return soft_clear_rect(dst, dst_r, c);
}

ret_t image_copy(bitmap_t* dst, bitmap_t* src, const rect_t* src_r, xy_t dx, xy_t dy) {
  return_value_if_fail(dst != NULL && src != NULL && src_r != NULL, RET_OK);

  assert(dx >= 0 && (dx + src_r->w) <= bitmap_get_physical_width(dst));
  assert(dy >= 0 && (dy + src_r->h) <= bitmap_get_physical_height(dst));
  log_debug("g2d copy,[%s] sfmt=%d sw=%d,sh=%d srw=%d srh=%d dname=%s dfmt=%d dw=%d,dh=%d dx=%d dy=%d\n",src->name, src->format, src->w, src->h,src_r->w, src_r->h,dst->name, dst->format, dst->w, dst->h,dx, dy);
#ifdef WITH_G2D
  if (g2d_copy_image(dst, src, src_r, dx, dy) == RET_OK) {
    return RET_OK;
  }
#endif /*WITH_G2D*/
  log_debug("soft copy\n");
  return soft_copy_image(dst, src, src_r, dx, dy);
}

ret_t image_rotate(bitmap_t* dst, bitmap_t* src, const rect_t* src_r, lcd_orientation_t o) {
  return_value_if_fail(dst != NULL && src != NULL && src_r != NULL, RET_OK);

  if (o == LCD_ORIENTATION_0 || o == LCD_ORIENTATION_180) {
    assert(src_r->w <= bitmap_get_physical_width(dst));
    assert(src_r->h <= bitmap_get_physical_height(dst));
  } else if (o == LCD_ORIENTATION_90 || o == LCD_ORIENTATION_270) {
    assert(src_r->w <= bitmap_get_physical_height(dst));
    assert(src_r->h <= bitmap_get_physical_width(dst));
  }

#ifdef WITH_G2D
  log_debug("g2d rotate,[%s]-w=%d,h=%d\n",src->name, src->w, src->h);	
  if (g2d_rotate_image(dst, src, src_r, o) == RET_OK) {
    return RET_OK;
  }
#endif /*WITH_G2D*/
log_debug("soft rotate\n");
  return soft_rotate_image(dst, src, src_r, o);
}

ret_t image_blend(bitmap_t* dst, bitmap_t* src, const rectf_t* dst_r, const rectf_t* src_r,
                  uint8_t global_alpha) {
  return_value_if_fail(dst != NULL && src != NULL && dst_r != NULL && src_r != NULL,
                       RET_BAD_PARAMS);

int i_ret ;
  assert(dst_r->x >= 0 && (dst_r->x + dst_r->w) <= bitmap_get_physical_width(dst));
  assert(dst_r->y >= 0 && (dst_r->y + dst_r->h) <= bitmap_get_physical_height(dst));
  log_debug("blend name=%s fmt=%d w=%d h=%d sw=%d sh=%d dw=%d dh=%d\n",src->name, src->format, src->w, src->h,tk_roundi(src_r->w), tk_roundi(src_r->h), tk_roundi(dst_r->w), tk_roundi(dst_r->h));
#ifdef WITH_G2D
  // 因为37e的tde不支持对带有alpha的数据做全局透明度设置，所以有Alpha通道且global_alpha不等于255的操作要使用awtk的软件功能实现
  if(global_alpha < TK_OPACITY_ALPHA && (src->format < BITMAP_FMT_RGB565))
    goto soft_blend;
  // 因为37e的tde不支持缩放带有alpha的数据，所以有Alpha通道且需要缩放的操作要使用awtk的软件功能实现
  //
#ifdef CHIP_TYPE_AK37E
  if ( (src->format > BITMAP_FMT_ARGB8888) || (src_r->w == dst_r->w && src_r->h  == dst_r->h)){
#endif
#ifdef CHIP_TYPE_AK37D   // 37D不支持ARGB格式的数据图层混合操作
  if ( src->format > BITMAP_FMT_ARGB8888){
#endif
	rect_t tmp_src_r = rect_from_rectf(src_r);
    rect_t tmp_dst_r = rect_from_rectf(dst_r); 
     if((i_ret= g2d_blend_image(dst, src, (const rect_t*)&tmp_dst_r, (const rect_t*)&tmp_src_r, global_alpha) )== RET_OK ){ 
    //DEBUG_PRINT( COLOR_MODE_NORMAL , COLOR_BACK_BLACK , COLOR_FRONT_BLUE, "i_ret= %d src->pv_layer= %p\n",i_ret, src->pv_layer )
 	  log_debug("g2d blend ok\n");
	  return RET_OK;
    }
    log_debug("g2d blend err=%d\n",i_ret);
  } 
  
#endif /*WITH_G2D*/
soft_blend:
  log_debug("soft blend\n");
  return soft_blend_image(dst, src, dst_r, src_r, global_alpha);
}

#ifdef WITH_FAST_LCD_PORTRAIT
ret_t image_rotate_ex(bitmap_t* dst, bitmap_t* src, const rect_t* src_r, xy_t dx, xy_t dy,
                      lcd_orientation_t o) {
  return_value_if_fail(dst != NULL && src != NULL && src_r != NULL, RET_OK);

  if (o == LCD_ORIENTATION_0 || o == LCD_ORIENTATION_180) {
    assert(src_r->w <= bitmap_get_physical_width(dst));
    assert(src_r->h <= bitmap_get_physical_height(dst));
  } else if (o == LCD_ORIENTATION_90 || o == LCD_ORIENTATION_270) {
    assert(src_r->w <= bitmap_get_physical_height(dst));
    assert(src_r->h <= bitmap_get_physical_width(dst));
  }

#ifdef WITH_G2D
  if (g2d_rotate_image_ex(dst, src, src_r, dx, dy, o) == RET_OK) {
    return RET_OK;
  }
#endif /*WITH_G2D*/

  return soft_rotate_image_ex(dst, src, src_r, dx, dy, o);
}

ret_t image_rotate_blend(bitmap_t* dst, bitmap_t* src, const rectf_t* dst_r, const rectf_t* src_r,
                         uint8_t global_alpha, lcd_orientation_t o) {
  return_value_if_fail(dst != NULL && src != NULL && dst_r != NULL && src_r != NULL,
                       RET_BAD_PARAMS);
  if (o == LCD_ORIENTATION_90 || o == LCD_ORIENTATION_270) {
    assert(dst_r->x >= 0 && (dst_r->x + dst_r->w) <= bitmap_get_physical_height(dst));
    assert(dst_r->y >= 0 && (dst_r->y + dst_r->h) <= bitmap_get_physical_width(dst));
  } else {
    assert(dst_r->x >= 0 && (dst_r->x + dst_r->w) <= bitmap_get_physical_width(dst));
    assert(dst_r->y >= 0 && (dst_r->y + dst_r->h) <= bitmap_get_physical_height(dst));
  }

#ifdef WITH_G2D
  if (src_r->w == dst_r->w && src_r->h == dst_r->h) {
    if (g2d_blend_image_rotate(dst, src, dst_r, src_r, global_alpha, o) == RET_OK) {
      return RET_OK;
    }
  }
#endif /*WITH_G2D*/

  return soft_blend_image_rotate(dst, src, dst_r, src_r, global_alpha, o);
}
#else

ret_t image_rotate_ex(bitmap_t* dst, bitmap_t* src, const rect_t* src_r, xy_t dx, xy_t dy,
                      lcd_orientation_t o) {
  (void)dst;
  (void)src;
  (void)src_r;
  (void)dx;
  (void)dy;
  (void)o;
  return RET_NOT_IMPL;
}

ret_t image_rotate_blend(bitmap_t* dst, bitmap_t* src, const rectf_t* dst_r, const rectf_t* src_r,
                         uint8_t global_alpha, lcd_orientation_t o) {
  (void)dst;
  (void)src;
  (void)dst_r;
  (void)src_r;
  (void)global_alpha;
  (void)o;
  return RET_NOT_IMPL;
}
#endif
