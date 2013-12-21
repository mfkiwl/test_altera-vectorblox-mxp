/**@file*/
// VBXCOPYRIGHTTAG
#include "vbx_copyright.h"
VBXCOPYRIGHT(vbw_sobel_argb32_3x3)

#include "vbx.h"
#include "vbw_mtx_sobel.h"

/// Convert a row of aRGB pixels into luma values
/// Trashes v_temp
static void vbw_rgb2luma(vbx_uhalf_t *v_luma, vbx_uword_t *v_row_in, vbx_uhalf_t *v_temp, const int image_width)
{
	vbx_set_vl(image_width);

	// Move weighted B into v_luma
	vbx(SVWHU, VAND, v_temp, 0xFF,   v_row_in);
	vbx(SVHU,  VMUL, v_luma, 25,     v_temp);

	// Move weighted G into v_temp and add it to v_luma
	vbx(SVWHU, VAND, v_temp, 0xFF,   (vbx_uword_t*)(((vbx_ubyte_t *)v_row_in)+1));
	vbx(SVHU,  VMUL, v_temp, 129,    v_temp);
	vbx(VVHU,  VADD, v_luma, v_luma, v_temp);

	// Move weighted R into v_temp and add it to v_luma
	vbx(SVWHU, VAND, v_temp, 0xFF,   (vbx_uword_t*)(((vbx_ubyte_t *)v_row_in)+2));
	vbx(SVHU,  VMUL, v_temp, 66,     v_temp);
	vbx(VVHU,  VADD, v_luma, v_luma, v_temp);

	vbx(SVHU,  VADD, v_luma, 128,    v_luma); // for rounding
	vbx(SVHU,  VSHR, v_luma, 8,      v_luma);
}


/// Apply [1 2 1] low-pass filter to raw input row
/// NB: Last two output pixels are not meaningful
inline static void vbw_sobel_3x3_row(vbx_uhalf_t *lpf, vbx_uhalf_t *raw, const short image_width)
{
	vbx_set_vl(image_width-1);
	vbx(VVHU, VADD, lpf, raw, raw+1);
	vbx_set_vl(image_width-2);
	vbx(VVHU, VADD, lpf, lpf, lpf+1);
}


/** Luma Edge Detection
 *
 * @usage 3x3 Sobel edge detection with 32-bit aRGB image
 *
 * @param[in] input        32-bit aRGB input
 * @param[out] output       32-bit aRGB edge-intensity output
 * @param[in] image_width  input/output image width
 * @param[in] image_height input/output image height
 * @param[in] image_pitch  input/output image pitch
 *
 * @retval 0 if successful; -1 if out of scratchpad memory
 */
int vbw_sobel_argb32_3x3(unsigned *output, unsigned *input, const short image_width, const short image_height, const short image_pitch, const short renorm)
//int vbw_sobel_argb32_3x3(unsigned *input, unsigned *output, const short image_width, const short image_height, const short image_pitch)
{
	int y;

	vbx_uword_t *v_row_in, *v_row_in_nxt;
	vbx_uhalf_t *v_luma_top, *v_luma_mid, *v_luma_bot;
	vbx_uword_t *v_row_out;

	vbx_uhalf_t *v_sobel_row_top, *v_sobel_row_mid, *v_sobel_row_bot;
	vbx_uhalf_t *v_gradient_x, *v_gradient_y;
	vbx_uhalf_t *v_tmp;

	void *tmp_ptr;

	// Allocate space in scratchpad for vectors
	if ((v_row_in        = (vbx_uword_t *)vbx_sp_malloc(image_width*sizeof(vbx_uword_t))) == NULL
        ||  (v_row_in_nxt    = (vbx_uword_t *)vbx_sp_malloc(image_width*sizeof(vbx_uword_t))) == NULL
        ||  (v_luma_top      = (vbx_uhalf_t *)vbx_sp_malloc(image_width*sizeof(vbx_uhalf_t))) == NULL
        ||  (v_luma_mid      = (vbx_uhalf_t *)vbx_sp_malloc(image_width*sizeof(vbx_uhalf_t))) == NULL
        ||  (v_luma_bot      = (vbx_uhalf_t *)vbx_sp_malloc(image_width*sizeof(vbx_uhalf_t))) == NULL
        ||  (v_sobel_row_top = (vbx_uhalf_t *)vbx_sp_malloc(image_width*sizeof(vbx_uhalf_t))) == NULL
        ||  (v_sobel_row_mid = (vbx_uhalf_t *)vbx_sp_malloc(image_width*sizeof(vbx_uhalf_t))) == NULL
        ||  (v_sobel_row_bot = (vbx_uhalf_t *)vbx_sp_malloc(image_width*sizeof(vbx_uhalf_t))) == NULL
        ||  (v_row_out       = (vbx_uword_t *)vbx_sp_malloc(image_width*sizeof(vbx_uword_t))) == NULL) {
		printf("Out of scratchpad memory\n");
		return -1;
	}

	// Re-use v_sobel_row_bot as v_tmp
	v_tmp = v_sobel_row_bot;

	// Transfer the first 3 input rows and interleave first 2 rgb2luma and first 2 sobel row calculations
	vbx_dma_to_vector_aligned(v_row_in,     input,                 image_width*sizeof(vbx_uword_t)); // 1st input row
	vbx_dma_to_vector_aligned(v_row_in_nxt, input +   image_pitch, image_width*sizeof(vbx_uword_t)); // 2nd input row
	vbw_rgb2luma(v_luma_top, v_row_in, v_tmp, image_width);                                          // 1st luma row
	vbw_sobel_3x3_row(v_sobel_row_top, v_luma_top, image_width);                                     // 1st partial sobel row
	vbx_dma_to_vector_aligned(v_row_in,     input + 2*image_pitch, image_width*sizeof(vbx_uword_t)); // 3rd input row
	vbw_rgb2luma(v_luma_mid, v_row_in_nxt, v_tmp, image_width);                                      // 2nd luma row
	vbw_sobel_3x3_row(v_sobel_row_mid, v_luma_mid, image_width);                                     // 2nd partial sobel row

	// Set top output row to 0
	vbx_set_vl(image_width);
	vbx(SVWU, VMOV, v_row_out, 0, 0);
	vbx_dma_to_host_aligned(output, v_row_out, image_width*sizeof(vbx_uword_t));

	// Calculate edges
	for (y = 0; y < image_height-(FILTER_HEIGHT-1); y++) {
		// Transfer the next input row while processing
		vbx_dma_to_vector_aligned(v_row_in_nxt, input + (y+FILTER_HEIGHT)*image_pitch, image_width*sizeof(vbx_uword_t));

		// Re-use v_sobel_row_bot as v_tmp
		v_tmp = v_sobel_row_bot;

		// Convert aRGB input to luma
		vbw_rgb2luma(v_luma_bot, v_row_in, v_tmp, image_width);

		// Done with v_row_in; re-use for v_gradient_x and v_gradient_y (be careful!)
		v_gradient_x = (vbx_uhalf_t *)v_row_in;
		v_gradient_y = (vbx_uhalf_t *)v_row_in + image_width;

		// Calculate gradient_x
		// Apply [1 2 1]T matrix to all columns
		vbx_set_vl(image_width);
		vbx(SVHU, VSHL, v_gradient_x, 1,          v_luma_mid); // multiply by 2
		vbx(VVHU, VADD, v_tmp,        v_luma_top, v_luma_bot);
		vbx(VVHU, VADD, v_tmp,        v_tmp,      v_gradient_x);
		// For each column, calculate absolute difference with 2nd column to the right
		vbx_set_vl(image_width-2);
		vbx(VVH, VABSDIFF, (vbx_half_t*)v_gradient_x, (vbx_half_t*)v_tmp, (vbx_half_t*)v_tmp+2);

		// Calculate gradient_y
		// Apply [1 2 1] matrix to last row in window and calculate absolute difference with pre-computed first row
		vbw_sobel_3x3_row(v_sobel_row_bot, v_luma_bot, image_width);
		vbx(VVH, VABSDIFF, (vbx_half_t*)v_gradient_y, (vbx_half_t*)v_sobel_row_top, (vbx_half_t*)v_sobel_row_bot);

		// Re-use v_sobel_row_top as v_tmp
		v_tmp = v_sobel_row_top;

		// sum of absoute gradients
		vbx_set_vl(image_width-2);
		vbx(VVHU, VADD, v_tmp, v_gradient_x,  v_gradient_y);
		vbx(SVHU, VSHR, v_tmp, renorm, v_tmp);

		// Threshold
		vbx(SVHU, VSUB,     v_gradient_y, 255, v_tmp);
		vbx(SVHU, VCMV_LTZ, v_tmp,        255, v_gradient_y);

		// Copy the result to the low byte of the output row
		// Trick to copy the low byte (b) to the middle two bytes as well
		// Note that first and last columns are 0
		vbx_set_vl(image_width-2);
		vbx(SVHWU, VMULLO, v_row_out+1, 0x00010101, v_tmp);

		// DMA the result to the output
		vbx_dma_to_host_aligned(output+(y+1)*image_pitch, v_row_out, image_width*sizeof(vbx_uword_t));

		// Swap input row buffers
		tmp_ptr      = (void *)v_row_in;
		v_row_in     = v_row_in_nxt;
		v_row_in_nxt = (vbx_uword_t *)tmp_ptr;

		// Rotate luma buffers
		tmp_ptr      = (void *)v_luma_top;
		v_luma_top   = v_luma_mid;
		v_luma_mid   = v_luma_bot;
		v_luma_bot   = (vbx_uhalf_t *)tmp_ptr;

		// Rotate v_sobel_row buffers (for gradient_y)
		tmp_ptr         = (void *)v_sobel_row_top;
		v_sobel_row_top = v_sobel_row_mid;
		v_sobel_row_mid = v_sobel_row_bot;
		v_sobel_row_bot = (vbx_uhalf_t *)tmp_ptr;
	}

	// Set bottom row to 0
	vbx_set_vl(image_width);
	vbx(SVWU, VMOV, v_row_out, 0, 0);
	vbx_dma_to_host_aligned(output+(image_height-1)*image_pitch, v_row_out, image_width*sizeof(vbx_uword_t));

	vbx_sync();
	vbx_sp_free();

	return 0;
}