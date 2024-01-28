#include "image.h"
#include <stdlib.h>
#include <string.h>


image_t *image_alloc(int width, int height, int format)
{
    image_t *img = image_create(width, height, format);
    if(!img)
        return NULL;

    if(IMAGE_IS_RGB16(format))
        img->stride = ((width + 1) >> 1) * 4;
    else if(IMAGE_IS_P8(format))
        img->stride = width;
    else if(IMAGE_IS_P4(format))
        img->stride = ((width + 1) >> 1);

    void *data = malloc(height * img->stride);
    if(!data) {
        image_free(img);
        return NULL;
    }

    img->data = data;
    img->flags |= IMAGE_FLAGS_DATA_ALLOC;
    return img;
}

bool image_alloc_palette(image_t *img, int size)
{
    if(!img || !IMAGE_IS_INDEXED(img->format))
        return false;
    if(img->flags & IMAGE_FLAGS_PALETTE_ALLOC)
        free(img->palette);

    if(IMAGE_IS_P8(img->format)) {
        size = (size <= 0) ? 256 : min(size, 256);
    }
    if(IMAGE_IS_P4(img->format)) {
        size = 16;
    }

    img->palette = calloc(size, 2);
    img->color_count = 0;
    img->flags &= ~IMAGE_FLAGS_PALETTE_ALLOC;

    if(!img->palette)
        return false;

    memset(img->palette, 0, 2*size);
    img->color_count = size;
    img->flags |= IMAGE_FLAGS_PALETTE_ALLOC;
    return true;
}

int image_alpha(int format)
{
    switch(format) {
    case IMAGE_RGB565A:
        return 0x0001;
    case IMAGE_P8_RGB565A:
        return -128;
    case IMAGE_P4_RGB565A:
        return 0;
    default:
        /* A value that cannot be found in any pixel of any format */
        return 0x10000;
    }
}


void image_clear(image_t *img)
{
    if(!IMAGE_IS_ALPHA(img->format))
        return;

    image_fill(img, image_alpha(img->format));
}

void image_copy(image_t const *src, image_t *dst, bool copy_alpha)
{
    if(!image_target(src, dst, DATA_RW))
        return;
    if(!IMAGE_IS_ALPHA(src->format))
        copy_alpha = true;

    /* Clip the input to match the size of the output */
    int w = min(src->width, dst->width);
    int h = min(src->height, dst->height);
    if(w <= 0 || h <= 0)
        return;

    void *src_px = src->data;
    void *dst_px = dst->data;
    int src_alpha = image_alpha_2(src->format, copy_alpha);
    int dst_alpha = image_alpha_2(dst->format, copy_alpha);

    if(IMAGE_IS_RGB16(src->format) && IMAGE_IS_RGB16(dst->format)) {
        do {
            for(int x = 0; x < w; x++) {
                int px = ((uint16_t *)src_px)[x];
                if(px != src_alpha) {
                    /* Don't copy opaque pixels of value 0x0001 into an RGB565A
                       array. We can use -= which is faster (subc) without
                       changing the visuals because dst_alpha != 0. */
                    ((uint16_t *)dst_px)[x] = px - (px == dst_alpha);
                }
            }
            src_px += src->stride;
            dst_px += dst->stride;
        } while(--h > 0);
    }
    else if(IMAGE_IS_P8(src->format) && IMAGE_IS_RGB16(dst->format)) {
        uint16_t *palette = src->palette + 128;

        do {
            for(int x = 0; x < w; x++) {
                int px = ((int8_t *)src_px)[x];
                if(px != src_alpha) {
                    px = palette[px];
                    ((uint16_t *)dst_px)[x] = px - (px == dst_alpha);
                }
            }
            src_px += src->stride;
            dst_px += dst->stride;
        } while(--h > 0);
    }
    else if(IMAGE_IS_P8(src->format) && IMAGE_IS_P8(dst->format)) {
        do {
            for(int x = 0; x < w; x++) {
                int px = ((int8_t *)src_px)[x];
                if(px != src_alpha)
                    ((int8_t *)dst_px)[x] = px;
            }
            src_px += src->stride;
            dst_px += dst->stride;
        } while(--h > 0);
    }
    else if(IMAGE_IS_P8(src->format) && IMAGE_IS_P4(dst->format)) {
        do {
            for(int x = 0; x < w; x++) {
                int px = ((int8_t *)src_px)[x];
                if(px != src_alpha) {
                    uint8_t *cell = dst_px + (x >> 1);
                    if(x & 1)
                        *cell = (*cell & 0xf0) | (px & 0x0f);
                    else
                        *cell = (*cell & 0x0f) | (px << 4);
                }
            }
            src_px += src->stride;
            dst_px += dst->stride;
        } while(--h > 0);
    }
    else if(IMAGE_IS_P4(src->format) && IMAGE_IS_P4(dst->format)) {
        do {
            for(int x = 0; x < w; x++) {
                int px = ((uint8_t *)src_px)[x >> 1];
                px = (x & 1) ? (px & 0x0f) : (px >> 4);
                if(px != src_alpha) {
                    uint8_t *cell = dst_px + (x >> 1);
                    if(x & 1)
                        *cell = (*cell & 0xf0) | (px & 0x0f);
                    else
                        *cell = (*cell & 0x0f) | (px << 4);
                }
            }
            src_px += src->stride;
            dst_px += dst->stride;
        } while(--h > 0);
    }
    else if(IMAGE_IS_P4(src->format) && IMAGE_IS_P8(dst->format)) {
        do {
            for(int x = 0; x < w; x++) {
                int px = ((uint8_t *)src_px)[x >> 1];
                px = (x & 1) ? (px & 0x0f) : (px >> 4);
                if(px != src_alpha)
                    ((int8_t *)dst_px)[x] = px;
            }
            src_px += src->stride;
            dst_px += dst->stride;
        } while(--h > 0);
    }
    else if(IMAGE_IS_P4(src->format) && IMAGE_IS_RGB16(dst->format)) {
        do {
            for(int x = 0; x < w; x++) {
                int px = ((uint8_t *)src_px)[x >> 1];
                px = (x & 1) ? (px & 0x0f) : (px >> 4);
                if(px != src_alpha) {
                    px = src->palette[px];
                    ((uint16_t *)dst_px)[x] = px - (px == dst_alpha);
                }
            }
            src_px += src->stride;
            dst_px += dst->stride;
        } while(--h > 0);
    }
}

image_t *image_copy_alloc(image_t const *src, int new_format)
{
    if(!image_valid(src))
        return NULL;

    image_t *dst = image_alloc(src->width, src->height, new_format);
    if(!dst)
        return NULL;
    if(!image_copy_palette(src, dst, -1)) {
        image_free(dst);
        return NULL;
    }

    image_copy(src, dst, true);
    return dst;
}


bool image_copy_palette(image_t const *src, image_t *dst, int size)
{
    if(!image_valid(src) || !dst)
        return false;
    if(!IMAGE_IS_INDEXED(dst->format))
        return true;

    if(size < 0)
        size = src->color_count;
    if(!image_alloc_palette(dst, size))
        return false;

    int N = min(src->color_count, dst->color_count);
    memcpy(dst->palette, src->palette, 2*N);
    return true;
}

image_t *image_create(int width, int height, int format)
{
    if(!IMAGE_IS_RGB16(format) && !IMAGE_IS_P8(format) && !IMAGE_IS_P4(format))
        return NULL;
    if(width <= 0 || width > 0xffff || height <= 0 || height > 0xffff)
        return NULL;

    image_t *img = malloc(sizeof *img);
    if(!img)
        return NULL;

    img->format = format;
    img->flags = 0;
    img->color_count = 0;
    img->width = width;
    img->height = height;
    img->stride = 0;
    img->data = NULL;
    img->palette = NULL;

    return img;
}


image_t *image_create_vram(void)
{
    image_t *img = image_create(DWIDTH, DHEIGHT, IMAGE_RGB565);
    if(!img)
        return NULL;

    img->stride = 2 * DWIDTH;
    img->data = gint_vram;
    return img;
}


int image_data_size(image_t const *img)
{
    return img->stride * img->height;
}

int image_decode_pixel(image_t const *img, int pixel)
{
    if(IMAGE_IS_RGB16(img->format))
        return pixel;
    else if(IMAGE_IS_P8(img->format))
        return img->palette[pixel+128];
    else if(IMAGE_IS_P4(img->format))
        return img->palette[pixel];
    return -1;
}


void image_fill(image_t *img, int value)
{
    if(!image_target(img, img, NOT_P4, DATA_RW))
        return;

    void *img_px = img->data;

    if(IMAGE_IS_RGB16(img->format)) {
        for(int y = 0; y < img->height; y++) {
            for(int x = 0; x < img->width; x++) {
                ((uint16_t *)img_px)[x] = value;
            }
            img_px += img->stride;
        }
    }
    else if(IMAGE_IS_P8(img->format)) {
        for(int y = 0; y < img->height; y++) {
            for(int x = 0; x < img->width; x++) {
                ((int8_t *)img_px)[x] = value;
            }
            img_px += img->stride;
        }
    }
}


void image_free(image_t *img)
{
    if(!img || mmu_is_rom(img))
        return;

    if(img->flags & IMAGE_FLAGS_DATA_ALLOC)
        free(img->data);
    if(img->flags & IMAGE_FLAGS_PALETTE_ALLOC)
        free(img->palette);

    free(img);
}


int image_get_pixel(image_t const *img, int x, int y)
{
    if((unsigned)x >= img->width || (unsigned)y >= img->height)
        return 0;

    void *data = img->data + y * img->stride;
    uint8_t *data_u8 = data;
    uint16_t *data_u16 = data;

    if(IMAGE_IS_RGB16(img->format)) {
        return data_u16[x];
    }
    else if(IMAGE_IS_P8(img->format)) {
        return (int8_t)data_u8[x];
    }
    else if(IMAGE_IS_P4(img->format)) {
        if(x & 1)
            return data_u8[x >> 1] & 0x0f;
        else
            return data_u8[x >> 1] >> 4;
    }
    return 0;
}


void image_set_palette(image_t *img, uint16_t *palette, int size, bool owns)
{
    if(!img || !IMAGE_IS_INDEXED(img->format))
        return;
    if(img->flags & IMAGE_FLAGS_PALETTE_ALLOC)
        free(img->palette);

    img->palette = palette;
    img->color_count = size;

    if(owns)
        img->flags |= IMAGE_FLAGS_PALETTE_ALLOC;
    else
        img->flags &= ~IMAGE_FLAGS_PALETTE_ALLOC;
}

void image_set_pixel(image_t const *img, int x, int y, int value)
{
    if(!image_valid(img))
        return;
    if((unsigned)x >= img->width || (unsigned)y >= img->height)
        return;

    void *data = img->data + y * img->stride;
    uint8_t *data_u8 = data;
    uint16_t *data_u16 = data;

    if(IMAGE_IS_RGB16(img->format)) {
        data_u16[x] = value;
    }
    else if(IMAGE_IS_P8(img->format)) {
        data_u8[x] = value;
    }
    else if(IMAGE_IS_P4(img->format)) {
        if(x & 1)
            data_u8[x >> 1] = (data_u8[x >> 1] & 0xf0) | (value & 0x0f);
        else
            data_u8[x >> 1] = (data_u8[x >> 1] & 0x0f) | (value << 4);
    }
}


image_t *image_sub(image_t const *src, int left, int top, int w, int h,
    image_t *dst)
{
    if(!dst)
        dst = &image_sub_default;
    if(w < 0)
        w = src->width - left;
    if(h < 0)
        h = src->height - top;

    struct gint_image_box box = { 0, 0, w, h, left, top };
    struct dwindow in_window = { 0, 0, w, h };
    if(!image_valid(src) || IMAGE_IS_P4(src->format) ||
       !gint_image_clip_input(src, &box, &in_window)) {
        memset(dst, 0, sizeof *dst);
        return dst;
    }

    int const ro_flags = IMAGE_FLAGS_DATA_RO | IMAGE_FLAGS_PALETTE_RO;
    dst->format = src->format;
    dst->flags = src->flags & ro_flags;
    dst->color_count = src->color_count;
    dst->width = box.w;
    dst->height = box.h;
    dst->stride = src->stride;

    if(IMAGE_IS_RGB16(src->format))
        dst->data = src->data + box.top * src->stride + 2 * box.left;
    else if(IMAGE_IS_P8(src->format))
        dst->data = src->data + box.top * src->stride + box.left;

    dst->palette = src->palette;
    return dst;
}


bool image_valid(image_t const *img)
{
    if(!img)
        return false;

    if(IMAGE_IS_RGB16(img->format)) {
        return (img->data != NULL);
    }
    if(IMAGE_IS_P8(img->format) || IMAGE_IS_P4(img->format)) {
        return (img->data != NULL) && (img->palette != NULL) &&
            (img->color_count != 0);
    }

    /* Invalid format */
    return false;
}
