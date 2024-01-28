//---
// gint:image - Image manipulation and rendering
//
// Note: this module is currently only available on fx-CG.
//
// This header provides image manipulation functions. This mainly consists of a
// reference-based image format, various access and modification functions, and
// a number of high-performance transformations and rendering effects. If you
// find yourself limited by rendering time, note that RAM writing speed is
// often the bottleneck, and image rendering is much faster in Azur (which is
// what the renderer was initially designed for).
//
// This module supports 3 bit depths: full-color 16-bit (RGB565), indexed 8-bit
// (P8) and indexed 4-bit (P4). All three have an "alpha" variation where one
// color is treated as transparent, leading to 6 total formats.
//
// The image renderers support so-called *dynamic effects*, which are image
// transformations performed on-the-fly while rendering, without generating an
// intermediate image. They comprise straightforward transformations that
// achieve similar performance to straight rendering and can be combined to
// some extent, which makes them reliable whenever applicable.
//
// For images of the RGB16 and P8 bit depths, the module supports a rich API
// with image subsurfaces and a fairly large sets of geometric and color
// transforms, including some in-place. P4 is not supported in most of these
// functions because the dense bit packing is both impractical and slower for
// these applications.
//---

#ifndef GINT_IMAGE
#define GINT_IMAGE

#ifdef __cplusplus
extern "C" {
#endif


//---
// Image structures
//---

/* Image formats. Note that transparency really only indicates the default
   rendering method, as a transparent background can always be added or removed
   by a dynamic effect on any image. */
enum {
    IMAGE_RGB565      = 0,  /* RGB565 without alpha */
    IMAGE_RGB565A     = 1,  /* RGB565 with one transparent color */
    IMAGE_P8_RGB565   = 4,  /* 8-bit palette, all opaque colors */
    IMAGE_P8_RGB565A  = 5,  /* 8-bit with one transparent color */
    IMAGE_P4_RGB565   = 6,  /* 4-bit palette, all opaque colors */
    IMAGE_P4_RGB565A  = 3,  /* 4-bit with one transparent color */

    IMAGE_DEPRECATED_P8 = 2,
};

/* Quick macros to compare formats by storage size */
#define IMAGE_IS_RGB16(format) \
    ((format) == IMAGE_RGB565 || (format) == IMAGE_RGB565A)
#define IMAGE_IS_P8(format) \
    ((format) == IMAGE_P8_RGB565 || (format) == IMAGE_P8_RGB565A)
#define IMAGE_IS_P4(format) \
    ((format) == IMAGE_P4_RGB565 || (format) == IMAGE_P4_RGB565A)
/* Check whether image format has an alpha color */
#define IMAGE_IS_ALPHA(format) \
    ((format) == IMAGE_RGB565A || \
     (format) == IMAGE_P8_RGB565A || \
     (format) == IMAGE_P4_RGB565A)
/* Check whether image format uses a palette */
#define IMAGE_IS_INDEXED(format) \
    (IMAGE_IS_P8(format) || IMAGE_IS_P4(format))

/* Image flags. These are used for memory management, mostly. */
enum {
    IMAGE_FLAGS_DATA_RO         = 0x01,  /* Data is read-only */
    IMAGE_FLAGS_PALETTE_RO      = 0x02,  /* Palette is read-only */
    IMAGE_FLAGS_DATA_ALLOC      = 0x04,  /* Data is malloc()'d */
    IMAGE_FLAGS_PALETTE_ALLOC   = 0x08,  /* Palette is malloc()'d */
};

#ifndef unit8_t
typedef unsigned char uint8_t;
#endif
#ifndef int16_t
typedef short int16_t;
#endif
#ifndef uint16_t
typedef unsigned short uint16_t;
#endif
#ifndef bool
typedef int bool;
#define true 1
#define false 0
#endif

typedef struct
{
    /* Color format, one of the IMAGE_* values defined above */
    uint8_t format;
    /* Additional flags, a combination of IMAGE_FLAGS_* values */
    uint8_t flags;
    /* Number of colors in the palette; this includes alpha for transparent
       images, as alpha is always the first entry.
       RGB16: 0
       P8: Ranges between 1 and 256
       P4: 16 */
    int16_t color_count;
    /* Full width and height, in pixels */
    uint16_t width;
    uint16_t height;
    /* Byte stride between lines */
    int stride;

    /* Pixel data in row-major order, left to right.
       - RGB16: 2 bytes per entry, each row padded to 4 bytes for alignment.
         Each 2-byte value is an RGB565 color.
       - P8: 1 signed byte per entry. Each byte is a palette index shifted by
         128 (to access the color, use palette[<value>+128]).
       - P4: 4 bits per entry, each row padded to a full byte. Each entry is a
         direct palette index between 0 and 15. */
    void *data;

    /* For P8 and P4, color palette. The number of entries allocated in the
       array is equal to the color_count attribute. */
    uint16_t *palette;

} image_t;

/* Dynamic effects: these transformations can be applied on images while
   rendering. Not all effects can be combined; unless specified otherwise:
   - HFLIP and VFLIP can both be added regardless of any other effect
   - At most one color effect can be applied */
enum {
    /* Value 0x01 is reserved, because it is DIMAGE_NOCLIP, which although
       part of the old API still needs to be supported. */

    /* [Any]: Skip clipping the command against the source image */
    IMAGE_NOCLIP_INPUT   = 0x04,
    /* [Any]: Skip clipping the command against the output VRAM */
    IMAGE_NOCLIP_OUTPUT  = 0x08,
    /* [Any]: Skip clipping both */
    IMAGE_NOCLIP         = IMAGE_NOCLIP_INPUT | IMAGE_NOCLIP_OUTPUT,

    // Geometric effects. These values should remain at exactly bit 8 and
    // following, or change gint_image_mkcmd() along with it.

    /* [Any]: Flip image vertically */
    IMAGE_VFLIP          = 0x0100,
    /* [Any]: Flip image horizontally */
    IMAGE_HFLIP          = 0x0200,

    // Color effects

    /* [RGB565, P8_RGB565, P4_RGB565]: Make a color transparent
       Adds one argument:
       * Color to clear (RGB16: 16-bit value; P8/P4: palette index) */
    IMAGE_CLEARBG        = 0x10,
    /* [RGB565, P8_RGB565, P4_RGB565]: Turn a color into another
       Adds two arguments:
       * Color to replace (RGB16: 16-bit value; P8/P4: palette index)
       * Replacement color (16-bit value) */
    IMAGE_SWAPCOLOR      = 0x20,
    /* [RGB565A, P8_RGB565A, P4_RGB565A]: Add a background
        Adds one argument:
        * Background color (16-bit value) */
    IMAGE_ADDBG          = 0x40,
    /* [RGB565A, P8_RGB565A, P4_RGB565A]: Dye all non-transparent pixels
       Adds one argument:
       * Dye color (16-bit value) */
    IMAGE_DYE            = 0x80,
};

//---
// Image creation and destruction
//---

/* image_alloc(): Create a new (uninitialized) image

   This function allocates a new image of the specified dimensions and format.
   It always allocates a new data array; if you need to reuse a data array, use
   the lower-level image_create() or image_create_sub().

   The first parameters [width] and [height] specify the dimensions of the new
   image in pixels. The [format] should be one of the IMAGE_* formats, for
   example IMAGE_RGB565A or IMAGE_P4_RGB565.

   This function does not specify or initialize the palette of the new image;
   use image_set_palette(), image_alloc_palette() or image_copy_palette()
   after calling this function.

   The returned image structure must be freed with image_free() after use.

   @width         Width of the new image
   @height        Height of the new image
   @format        Pixel format; one of the IMAGE_* formats defined above */
image_t *image_alloc(int width, int height, int format);

/* image_set_palette(): Specify an external palette for an image

   This function sets the image's palette to the provided address. The number
   of entries allocated must be specified in size. It is also the caller's
   responsibility to ensure that the palette covers all the indices used in the
   image data.

   The old palette, if owned by the image, is freed. If [owns=true] the
   palette's ownership is given to the image, otherwise it is kept external. */
void image_set_palette(image_t *img, uint16_t *palette, int size, bool owns);

/* image_alloc_palette(): Allocate a new palette for an image

   This function allocates a new palette for an image. The number of entries is
   specified in size; for P8 it can vary between 1 and 256, for P4 it is
   ignored (P4 images always have 16 colors).

   The old palette, if owned by the image, is freed. The entries of the new
   palette are all initialized to 0. If size is -1, the format's default
   palette size is used. Returns true on success. */
bool image_alloc_palette(image_t *img, int size);

/* image_copy_palette(): Copy another image's palette

   This function allocates a new palette for an image, and initializes it with
   a copy of another image's palette. For P8 the palette can be resized by
   specifying a value other than -1 as the size; by default, the source image's
   palette size is used (within the limits of the new format). Retuns true on
   success. */
bool image_copy_palette(image_t const *src, image_t *dst, int size);

/* image_create(): Create a bare image with no data/palette

   This function allocates a new image structure but without data or palette.
   The [data] and [palette] members are NULL, [color_count] and [stride] are 0.

   This function is useful to create images that reuse externally-provided
   information. It is intended that the user of this function sets the [data]
   and [stride] fields themselves, along with the IMAGE_FLAGS_DATA_ALLOC flag
   if the image should own its data.

   The [palette] and [color_count] members can be set with image_set_palette(),
   image_alloc_palette(), image_copy_palette(), or manually.

   The returned image structure must be freed with image_free() after use. */
image_t *image_create(int width, int height, int format);

/* image_create_vram(): Create a reference to gint_vram

   This function creates a new RGB565 image that references gint_vram. Using
   this image as target for transformation functions can effectively render
   transformed images to VRAM.

   The value of gint_vram is captured when this function is called, and does
   not update after dupdate() when triple-buffering is used. The user should
   account for this option. (Using this function twice then replacing one of
   the [data] pointers is allowed.)

   The VRAM image owns no data but it does own its own structure so it must
   still be freed with image_free() after use. */
image_t *image_create_vram(void);

/* image_free(): Free and image and the data it owns

   This function frees the provided image structure and the data that it owns.
   Images converted by fxconv should not be freed; nonetheless, this functions
   distinguishes them and should work. Images are not expected to be created on
   the stack.

   If the image has the IMAGE_FLAGS_DATA_ALLOC flag, the data pointer is also
   freed. Similarly, the image has the IMAGE_FLAGS_PALETTE_ALLOC flag, the
   palette is freed. Make sure to not free images when references to them still
   exist, as this could cause the reference's pointers to become dangling. */
void image_free(image_t *img);

//---
// Basic image access and information
//---

/* image_valid(): Check if an image is valid
   An image is considered valid if it has a valid profile, a non-NULL data
   pointer, and for palette formats a valid palette pointer. */
bool image_valid(image_t const *img);

/* image_alpha(): Get the alpha value for an image format

   This function returns the alpha value for any specific image format:
   * RGB16: 0x0001
   * P8: -128 (0x80)
   * P4: 0
   For non-transparent formats, it returns a value that is different from all
   valid pixel values of the format, which means it is always safe to compare a
   pixel value to the image_alpha() of the format. */
int image_alpha(int format);

/* image_get_pixel(): Read a pixel from the data array

   This function reads a pixel from the image's data array at position (x,y).
   It returns the pixel's value, which is either a full-color value (RGB16) or
   a possibly-negative palette index (P8/P4). See the description of the [data]
   field of image_t for more details. The value of the pixel can be decoded
   into a 16-bit color either manually or by using the image_decode_pixel()
   function.

   Note that reading large amounts of image data with this function will be
   slow; if you need reasonable performance, consider iterating on the data
   array manually. */
int image_get_pixel(image_t const *img, int x, int y);

/* image_decode_pixel(): Decode a pixel value

   This function decodes a pixel's value obtained from the data array (for
   instance with image_get_pixel()). For RGB16 formats this does nothing, but
   for palette formats this accesses the palette at a suitable position.

   Note that reading large amounts of data with this function will be slow; if
   you need reasonable performance, consider inlining the format-specific
   method or iterating on the data array manually. */
int image_decode_pixel(image_t const *img, int pixel);

/* image_data_size(): Compute the size of the [data] array
   This function returns the size of the data array, in bytes. This can be used
   to duplicate it. Note that for sub-images this is a subsection of another
   image's data array, and might be much larger than the sub-image. */
int image_data_size(image_t const *img);

//---
// Basic image modifications
//---

/* image_set_pixel(): Set a pixel in the data array

   This function writes a pixel into the image's data array at position (x,y).
   The pixel value must be of the proper format, as specified in the definition
   of the [data] field of image_t.

   Formats: RGB16, P8, P4 */
void image_set_pixel(image_t const *img, int x, int y, int value);

/* image_copy(): Convert and copy an image

   This function copies an image into another image while converting certain
   formats. Unlike transforms, this function does clip, so there are no
   conditions on the size of the target.

   If [copy_alpha] is true, transparent pixels are copied verbatim, which
   effectively replaces the top-left corner of [dst] with [src]. If it's false,
   transparent pixels of [src] are skipped, effectively rendering [src] over
   the top-left corner of [src].

   This function converts between all formats except from RGB16 to P8/P4, since
   this requires generating a palette (which is a complex endeavour).
   Conversions from P8/P4 to RGB16 simply decode the palette. Conversions
   between P8/P4 preserve the contents but renumber the palette entries. From
   P4 to P8, the image is always preserved. From P8 to P4, the image is only
   preserved if it has less than 16 colors (this is intended to allow P4 images
   to be converted to P8 for edition by this library, and then back to P4). The
   following table summarizes the conversions:

     Source format →      RGB16           P8                P4
     Target format ↓  +-----------+----------------+------------------+
               RGB16  |    Copy     Decode palette    Decode palette  |
                  P8  |     -            Copy        Enlarge palette  |
                  P4  |     -       Narrow palette         Copy       |
                      +-----------+----------------+------------------+

   Note that conversions to RGB16 are not lossless because RGB565, P8 and P4
   can represent any color; if a color equal to image_alpha(IMAGE_RGB565A) is
   found during conversion, this function transforms it slightly to look
   similar instead of erroneously generating a transparent pixel.

   Formats: RGB16 → RGB16, P8 → Anything, P4 → Anything
   Size requirement: none (clipping is performed)
   Supports in-place: No (useless) */
void image_copy(image_t const *src, image_t *dst, bool copy_alpha);

/* image_copy_alloc(): Convert and copy into a new image
   This function is similar to image_copy(), but it allocates a target image of
   the desired format before copying. */
image_t *image_copy_alloc(image_t const *src, int new_format);

/* image_fill(): Fill an image with a single pixel value */
void image_fill(image_t *img, int value);

/* image_clear(): Fill a transparent image with its transparent value */
void image_clear(image_t *img);



#ifdef __cplusplus
}
#endif

#endif 
