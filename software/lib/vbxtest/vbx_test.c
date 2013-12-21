/* VECTORBLOX MXP SOFTWARE DEVELOPMENT KIT
 * 
 * Copyright (C) 2012-2013 VectorBlox Computing Inc., Vancouver, British Columbia, Canada.
 * All rights reserved.
 * 
 * THIS BETA VERSION SOFTWARE IS PROVIDED ONLY TO PARTIES BOUND UNDER A SIGNED
 * NON-DISCLOSURE AGREEMENT. THIS SOFTWARE IS FOR THE PURPOSES OF EVALUATION ONLY.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, OF THIS BETA VERSION SOFTWARE IS STRICTLY PROHIBITED.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL VECTORBLOX COMPUTING INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This agreement shall be governed in all respects by the laws of the Province
 * of British Columbia and by the laws of Canada.
 *
 * This file is part of the VectorBlox MXP Software Development Kit.
 *
 */


#include "vbx_copyright.h"
VBXCOPYRIGHT( vbx_test )

#include <stdio.h>
#include <math.h>

#include "vbx_test.h"

///////////////////////////////////////////////////////////////////////////
// Based on: http://www.cs.tut.fi/~jkorpela/c/eng.html
// Return a string representing value in engineering notation.
// digits specifies the desired number of significant digits.
// NOTE: "The return value is a pointer to a data area which will be
// overwritten by the next call to the same function. Therefore,
// do not call eng twice in one statement."
char *vbx_eng(double value, int digits)
{
	int expof10;
	static char result[100];
	char *res = result;

	if (value < 0.) {
		*res++ = '-';
		value = -value;
	}
	if (value == 0.) {
		return "0.0";
	}

	expof10 = (int) log10(value);
	if (expof10 > 0)
		expof10 = (expof10/3)*3;
	else
		expof10 = (-expof10+3)/3*(-3);

	value *= pow(10,-expof10);

	if (value >= 1000.) {
		value /= 1000.0;
		expof10 += 3;
	} else if (value >= 100.0) {
		digits -= 2;
	} else if(value >= 10.0) {
		digits -= 1;
	}

	if (expof10 == 0) {
		sprintf(res, "%.*f", digits-1, value);
	} else {
		sprintf(res, "%.*fe%d", digits-1, value, expof10);
	}
	return result;
}

///////////////////////////////////////////////////////////////////////////
void vbx_mxp_print_params()
{
	vbx_mxp_t *this_mxp = VBX_GET_THIS_MXP();
	printf("MXP Parameters:\n");
	printf("vector_lanes = %d\n", this_mxp->vector_lanes);
	printf("core_freq = %s\n", vbx_eng((double) this_mxp->core_freq, 4));
	printf("scratchpad_size = %d\n", this_mxp->scratchpad_size);
	printf("dma_alignment_bytes = %d\n", this_mxp->dma_alignment_bytes);
	printf("fxp_word_frac_bits = %d\n", this_mxp->fxp_word_frac_bits);
	printf("fxp_half_frac_bits = %d\n", this_mxp->fxp_half_frac_bits);
	printf("fxp_byte_frac_bits = %d\n", this_mxp->fxp_byte_frac_bits);
}

///////////////////////////////////////////////////////////////////////////
static double vbx_print_metrics(vbx_timestamp_t time_start,
								vbx_timestamp_t time_stop,
								double cycles_divisor,
								char *divisor_str,
								char *metrics_str,
								double speedup_dividend)
{
	if (time_stop < time_start) {
		printf("Error: %s stop time (%llu) is less than start time (%llu)!\n",
		       metrics_str, (unsigned long long) time_stop,
		       (unsigned long long) time_start);
		printf("Skipping metrics computation.\n");
		return 0.0;
		// VBX_EXIT(-1);
	}

	vbx_timestamp_t cycles = time_stop - time_start;
	vbx_timestamp_t mxp_cycles = vbx_mxp_cycles(cycles);
	double seconds = ((double) cycles) / ((double) vbx_timestamp_freq());

	printf("\n");
	printf("%s time in seconds: %s\n", metrics_str, vbx_eng(seconds, 4));
	printf("%s time in cycles: %llu\n", metrics_str,
	       (unsigned long long) mxp_cycles);
	if (cycles_divisor > 0.0) {
		printf("%s time in cycles per %s: %s\n", metrics_str, divisor_str,
		       vbx_eng(mxp_cycles/cycles_divisor, 4));
	}
	if (speedup_dividend > 0.0) {
		printf("%s speedup: %s\n", metrics_str,
		       vbx_eng(speedup_dividend/seconds, 4));
	}
	return seconds;
}


///////////////////////////////////////////////////////////////////////////
double vbx_print_scalar_time_per(vbx_timestamp_t time_start,
								 vbx_timestamp_t time_stop,
								 double cycles_divisor,
								 char *divisor_str)
{
	return vbx_print_metrics(time_start,
	                         time_stop,
	                         cycles_divisor,
	                         divisor_str,
	                         "Scalar",
	                         0.0);
}

///////////////////////////////////////////////////////////////////////////
double vbx_print_scalar_time(vbx_timestamp_t time_start,
							 vbx_timestamp_t time_stop)
{
	return vbx_print_scalar_time_per(time_start,
	                                 time_stop,
	                                 0.0,
	                                 "");
}

///////////////////////////////////////////////////////////////////////////
double vbx_print_vector_time_per(vbx_timestamp_t time_start,
								 vbx_timestamp_t time_stop,
								 double cycles_divisor,
								 char *divisor_str,
								 double scalar_time)
{
	return vbx_print_metrics(time_start,
	                         time_stop,
	                         cycles_divisor,
	                         divisor_str,
	                         "Vector",
	                         scalar_time);
}

///////////////////////////////////////////////////////////////////////////
double vbx_print_vector_time(vbx_timestamp_t time_start,
							 vbx_timestamp_t time_stop,
							 double scalar_time)
{
	return vbx_print_vector_time_per(time_start,
	                                 time_stop,
	                                 0.0,
	                                 "",
	                                 scalar_time);
}

///////////////////////////////////////////////////////////////////////////
int lfsr_32( int previous_value )
{
	return (((previous_value>>31)^(previous_value>>21)^(previous_value>>1)^(previous_value>>0))&0x1)|((previous_value)<<1);
}

void test_inc_array_byte( int8_t *d, int size, int seed, int increase )
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = seed;
		seed += increase;
	}
}
void test_init_array_byte( int8_t *d, int size, int seed )
{
	int i;

	d[0]  = seed;
	for(i = 1; i < size; i++){
		d[i] = (((d[i-1]>>7)^(d[i-1]>>5)^(d[i-1]>>4)^(d[i-1]>>3))&0x1)|(((d[i-1])<<1)&0xFE);
	}
}
void test_init_matrix_byte( int8_t *d, int height, int width, int seed)
{
	int i,j;
	int lfsr = seed;

	for(j=0; j<height; j++) {
		for(i=0; i<width; i++) {
			d[j*width+i] = lfsr & 0xFF;
			lfsr = lfsr_32(lfsr);
		}
	}
}
void test_zero_array_byte( int8_t *d, int size)
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = 0;
	}
}
void test_copy_array_byte( int8_t *out, int8_t *in, int size)
{
	int i;
	for(i = 0; i < size; i++){
		out[i] = in[i];
	}
}
void test_print_hex_array_byte( int8_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%04X ", d[i] );
	}
	printf("\n");
}
void test_print_hex_matrix_byte( int8_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%04X ", d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
void test_print_array_byte( int8_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%d\t", d[i] );
	}
	printf("\n");
}
void test_print_matrix_byte( int8_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%d\t", d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
int test_verify_array_byte( int8_t *scalar_out, int8_t *vector_out, int size )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i] != scalar_out[i] ) {
			printf( "\nFail at sample %d.\nScalar: %d\nVector: %d\n", i, scalar_out[i], vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}
int test_range_array_byte( int8_t *scalar_out, int8_t *vector_out, int size, int range )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i]+range < scalar_out[i] || vector_out[i]-range > scalar_out[i]) {
			printf( "\nFail at sample %d.\nScalar: %d\nVector: %d\n", i, scalar_out[i], vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}

void test_inc_array_half( int16_t *d, int size, int seed, int increase )
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = seed++;
		seed += increase;
	}
}
void test_init_array_half( int16_t *d, int size, int seed )
{
	int i;

	d[0]  = seed;
	for(i = 1; i < size; i++){
		d[i] = (((d[i-1]>>7)^(d[i-1]>>5)^(d[i-1]>>4)^(d[i-1]>>3))&0x1)|(((d[i-1])<<1)&0xFE);
	}
}
void test_init_matrix_half( int16_t *d, int height, int width, int seed)
{
	int i,j;
	int lfsr = seed;

	for(j=0; j<height; j++) {
		for(i=0; i<width; i++) {
			d[j*width+i] = lfsr & 0xFF;
			lfsr = lfsr_32(lfsr);
		}
	}
}
void test_zero_array_half( int16_t *d, int size)
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = 0;
	}
}
void test_copy_array_half( int16_t *out, int16_t *in, int size)
{
	int i;
	for(i = 0; i < size; i++){
		out[i] = in[i];
	}
}
void test_print_hex_array_half( int16_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%04X ", d[i] );
	}
	printf("\n");
}
void test_print_hex_matrix_half( int16_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%04X ", d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
void test_print_array_half( int16_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%d\t ", d[i] );
	}
	printf("\n");
}
void test_print_matrix_half( int16_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%d\t ", d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
int test_verify_array_half( int16_t *scalar_out, int16_t *vector_out, int size )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i] != scalar_out[i] ) {
			printf( "\nFail at sample %d.\nScalar: %d\nVector: %d\n", i, scalar_out[i], vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}
int test_range_array_half( int16_t *scalar_out, int16_t *vector_out, int size, int range )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i]+range < scalar_out[i] || vector_out[i]-range > scalar_out[i]) {
			printf( "\nFail at sample %d.\nScalar: %d\nVector: %d\n", i, scalar_out[i], vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}

void test_inc_array_word( int32_t *d, int size, int seed, int increase )
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = seed++;
		seed += increase;
	}
}
void test_init_array_word( int32_t *d, int size, int seed )
{
	int i;

	d[0]  = seed;
	for(i = 1; i < size; i++){
		d[i] = (((d[i-1]>>7)^(d[i-1]>>5)^(d[i-1]>>4)^(d[i-1]>>3))&0x1)|(((d[i-1])<<1)&0xFE);
	}
}
void test_init_matrix_word( int32_t *d, int height, int width, int seed)
{
	int i,j;
	int lfsr = seed;

	for(j=0; j<height; j++) {
		for(i=0; i<width; i++) {
			d[j*width+i] = lfsr & 0xFF;
			lfsr = lfsr_32(lfsr);
		}
	}
}
void test_zero_array_word( int32_t *d, int size)
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = 0;
	}
}
void test_copy_array_word( int32_t *out, int32_t *in, int size)
{
	int i;
	for(i = 0; i < size; i++){
		out[i] = in[i];
	}
}
void test_print_hex_array_word( int32_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%04X ", (unsigned int)d[i] );
	}
	printf("\n");
}
void test_print_hex_matrix_word( int32_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%04X ", (unsigned int)d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
void test_print_array_word( int32_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%d\t", (unsigned int)d[i] );
	}
	printf("\n");
}
void test_print_matrix_word( int32_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%d\t", (unsigned int)d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
int test_verify_array_word( int32_t *scalar_out, int32_t *vector_out, int size )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i] != scalar_out[i] ) {
			printf( "\nFail at sample %d.\nScalar: %d\nVector: %d\n", i, (int)scalar_out[i], (int)vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}
int test_range_array_word( int32_t *scalar_out, int32_t *vector_out, int size, int range )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i]+range < scalar_out[i] || vector_out[i]-range > scalar_out[i]) {
			printf( "\nFail at sample %d.\nScalar: %d\nVector: %d\n", i, (int)scalar_out[i], (int)vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}

void test_inc_array_ubyte( uint8_t *d, int size, int seed, int increase )
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = seed++;
		seed += increase;
	}
}
void test_init_array_ubyte( uint8_t *d, int size, int seed )
{
	int i;

	d[0]  = seed;
	for(i = 1; i < size; i++){
		d[i] = (((d[i-1]>>7)^(d[i-1]>>5)^(d[i-1]>>4)^(d[i-1]>>3))&0x1)|(((d[i-1])<<1)&0xFE);
	}
}
void test_init_matrix_ubyte( uint8_t *d, int height, int width, int seed)
{
	int i,j;
	int lfsr = seed;

	for(j=0; j<height; j++) {
		for(i=0; i<width; i++) {
			d[j*width+i] = lfsr & 0xFF;
			lfsr = lfsr_32(lfsr);
		}
	}
}
void test_zero_array_ubyte( uint8_t *d, int size)
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = 0;
	}
}
void test_copy_array_ubyte( uint8_t *out, uint8_t *in, int size)
{
	int i;
	for(i = 0; i < size; i++){
		out[i] = in[i];
	}
}
void test_print_hex_array_ubyte( uint8_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%04X ", d[i] );
	}
	printf("\n");
}
void test_print_hex_matrix_ubyte( uint8_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%04X ", d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
void test_print_array_ubyte( uint8_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%u\t", d[i] );
	}
	printf("\n");
}
void test_print_matrix_ubyte( uint8_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%u\t", d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
int test_verify_array_ubyte( uint8_t *scalar_out, uint8_t *vector_out, int size )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i] != scalar_out[i] ) {
			printf( "\nFail at sample %d.\nScalar: %u\nVector: %u\n", i, scalar_out[i], vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}
int test_range_array_ubyte( uint8_t *scalar_out, uint8_t *vector_out, int size, int range )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i]+range < scalar_out[i] || vector_out[i]-range > scalar_out[i]) {
			printf( "\nFail at sample %d.\nScalar: %u\nVector: %u\n", i, scalar_out[i], vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}

void test_inc_array_uhalf( uint16_t *d, int size, int seed, int increase )
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = seed++;
		seed += increase;
	}
}
void test_init_array_uhalf( uint16_t *d, int size, int seed )
{
	int i;

	d[0]  = seed;
	for(i = 1; i < size; i++){
		d[i] = (((d[i-1]>>7)^(d[i-1]>>5)^(d[i-1]>>4)^(d[i-1]>>3))&0x1)|(((d[i-1])<<1)&0xFE);
	}
}
void test_init_matrix_uhalf( uint16_t *d, int height, int width, int seed)
{
	int i,j;
	int lfsr = seed;

	for(j=0; j<height; j++) {
		for(i=0; i<width; i++) {
			d[j*width+i] = lfsr & 0xFF;
			lfsr = lfsr_32(lfsr);
		}
	}
}
void test_zero_array_uhalf( uint16_t *d, int size)
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = 0;
	}
}
void test_copy_array_uhalf( uint16_t *out, uint16_t *in, int size)
{
	int i;
	for(i = 0; i < size; i++){
		out[i] = in[i];
	}
}
void test_print_hex_array_uhalf( uint16_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%04X ", d[i] );
	}
	printf("\n");
}
void test_print_hex_matrix_uhalf( uint16_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%04X ", d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
void test_print_array_uhalf( uint16_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%u\t", d[i] );
	}
	printf("\n");
}
void test_print_matrix_uhalf( uint16_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%u\t", d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
int test_verify_array_uhalf( uint16_t *scalar_out, uint16_t *vector_out, int size )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i] != scalar_out[i] ) {
			printf( "\nFail at sample %d.\nScalar: %u\nVector: %u\n", i, scalar_out[i], vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}
int test_range_array_uhalf( uint16_t *scalar_out, uint16_t *vector_out, int size, int range )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i]+range < scalar_out[i] || vector_out[i]-range > scalar_out[i]) {
			printf( "\nFail at sample %d.\nScalar: %u\nVector: %u\n", i, scalar_out[i], vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}

void test_inc_array_uword( uint32_t *d, int size, int seed, int increase )
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = seed++;
		seed += increase;
	}
}
void test_init_array_uword( uint32_t *d, int size, int seed )
{
	int i;

	d[0]  = seed;
	for(i = 1; i < size; i++){
		d[i] = (((d[i-1]>>7)^(d[i-1]>>5)^(d[i-1]>>4)^(d[i-1]>>3))&0x1)|(((d[i-1])<<1)&0xFE);
	}
}
void test_init_matrix_uword( uint32_t *d, int height, int width, int seed)
{
	int i,j;
	int lfsr = seed;

	for(j=0; j<height; j++) {
		for(i=0; i<width; i++) {
			d[j*width+i] = lfsr & 0xFF;
			lfsr = lfsr_32(lfsr);
		}
	}
}
void test_zero_array_uword( uint32_t *d, int size)
{
	int i;
	for(i = 0; i < size; i++){
		d[i] = 0;
	}
}
void test_copy_array_uword( uint32_t *out, uint32_t *in, int size)
{
	int i;
	for(i = 0; i < size; i++){
		out[i] = in[i];
	}
}
void test_print_hex_array_uword( uint32_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%04X ", (unsigned int)d[i] );
	}
	printf("\n");
}
void test_print_hex_matrix_uword( uint32_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%04X ", (unsigned int)d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
void test_print_array_uword( uint32_t *d, int size )
{
	int i;
	for( i = 0; i < size; i++ ) {
		printf( "%u\t", (unsigned int)d[i] );
	}
	printf("\n");
}
void test_print_matrix_uword( uint32_t *d, int row, int col, int width )
{
	int i, j;
	for( j = 0; j < row; j++ ) {
		for( i = 0; i < col; i++ ) {
			printf( "%u\t", (unsigned int)d[j*width+i] );
		}
		printf("\n");
	}
	printf("\n");
}
int test_verify_array_uword( uint32_t *scalar_out, uint32_t *vector_out, int size )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i] != scalar_out[i] ) {
			printf( "\nFail at sample %d.\nScalar: %u\nVector: %u\n", i, (unsigned int)scalar_out[i], (unsigned int)vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}
int test_range_array_uword( uint32_t *scalar_out, uint32_t *vector_out, int size, int range )
{
	int i;
	int errors = 0;
	for( i = 0; i < size; i++ ) {
		if( vector_out[i]+range < scalar_out[i] || vector_out[i]-range > scalar_out[i]) {
			printf( "\nFail at sample %d.\nScalar: %u\nVector: %u\n", i, (unsigned int)scalar_out[i], (unsigned int)vector_out[i] );
			errors += 1;
			break;
		}
	}
	return errors;
}

///////////////////////////////////////////////////////////////////////////
#if __NIOS2__
int vbx_test_init()
{
	return 0;
}
///////////////////////////////////////////////////////////////////////////
#elif __MICROBLAZE__ || __ARM_ARCH_7A__

#include "xparameters.h"
#include "xil_types.h"
#include "xil_cache.h"
#include "vectorblox_mxp.h"

vbx_mxp_t vbx_test_mxp_inst;

#if XPAR_XTMRCTR_NUM_INSTANCES && !(__ARM_ARCH_7A__ && VBX_USE_A9_PMU_TIMER)
#include "xtmrctr.h"
XTmrCtr vbx_test_tmr_inst;
#endif

///////////////////////////////////////////////////////////////////////////
#if __ARM_ARCH_7A__
#include "xpseudo_asm.h"
#include "xil_mmu.h"

extern u32 MMUTable;

///////////////////////////////////////////////////////////////////////////
// For API compatibility, we want to be able to access main memory in an
// uncached manner from the ARM CPU by e.g. setting bit 31 of the address
// (like on Nios). We can accomplish this with the ARM MMU's translation
// table.
// The function below is for the Zynq Cortex-A9 on the ZedBoard, running the
// standalone BSP.
// It maps (virtual) address range 0x8000_0000 to 0x9fff_ffff (512 MB) to
// DDR physical address range 0x0-0x1fff_ffff and sets memory attributes to
// strongly-ordered. (Was experiencing unexpected behaviour when attrs set to
// normal, non-cacheable.)
// Based in Xil_SetTlbAttributes() in xil_mmu.c.
int vbx_zynq_remap_ddr_uncached()
{
	u32 addr;
	// Section descriptor bits 19:0 for
	// normal, outer and inner noncacheable memory region:
	//   S=b1 TEX=b001 AP=b11, Domain=b0000, C=b0, B=b0 => 0x11c02
	//   S=b1 TEX=b001 AP=b11, Domain=b1111, C=b0, B=b0 => 0x11de2
	// shareable device:
	//   S=b0 TEX=b000 AP=b11, C=b0, B=b1 => 0xC06
	// strongly-ordered:
	//   S=b0 TEX=b000 AP=b11, C=b0, B=b0 => 0xC02
	// (Short-descriptor translation table format, FigB3-4, p. B3-1325 of
	// ARM Architecture Reference Manual DDI0406C.b)
	// u32 attrib = 0x11c02;
	u32 attrib = 0xc02;
	u32 *ptr;
	u32 section;

	// printf("MMUTable = 0x%08x\n", (unsigned int) &MMUTable);

	mtcp(XREG_CP15_INVAL_UTLB_UNLOCKED, 0);
	dsb();

	// One descriptor per 1 MB region; iterate over 512 descriptors covering
	// 512MB in range 0x8000_0000 to 0x9fff_ffff.
	for (addr = 0x80000000; addr < 0xa0000000; addr += 0x100000) {
		// Index into translation table
        section = addr / 0x100000;
        ptr = &MMUTable + section;
        // Map to physical addresses in range 0x0 to 0x1fff_ffff,
        // i.e. clear bit 31.
        *ptr = (addr & 0x7FF00000) | attrib;
	}
	dsb();

	mtcp(XREG_CP15_INVAL_UTLB_UNLOCKED, 0);
	/* Invalidate all branch predictors */
	mtcp(XREG_CP15_INVAL_BRANCH_ARRAY, 0);

	dsb(); /* ensure completion of the BP and TLB invalidation */
    isb(); /* synchronize context on this processor */

    return XST_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
int vbx_zynq_set_instr_port_normal_uncached()
{
	// Change memory attributes of 1MB region starting at base address of
	// AXI instruction port.
	// normal, outer and inner noncacheable memory region:
	//   S=b1 TEX=b001 AP=b11, Domain=b0000, C=b0, B=b0 => 0x11c02
	Xil_SetTlbAttributes(XPAR_VECTORBLOX_MXP_ARM_0_S_AXI_INSTR_BASEADDR,
	                     0x11c02);
	return XST_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
int vbx_zynq_set_instr_port_device_memory()
{
	// Change memory attributes of 1MB region starting at base address of
	// AXI instruction port.
	// shareable device:
	//   S=b0 TEX=b000 AP=b11, C=b0, B=b1 => 0xC06
	Xil_SetTlbAttributes(XPAR_VECTORBLOX_MXP_ARM_0_S_AXI_INSTR_BASEADDR,
	                     0xc06);
	return XST_SUCCESS;
}

#endif // __ARM_ARCH_7A__

///////////////////////////////////////////////////////////////////////////
int vbx_test_init()
{
	int status;

#if __MICROBLAZE__
	u16 mxp_dev_id     = XPAR_VECTORBLOX_MXP_0_DEVICE_ID;
#else
	u16 mxp_dev_id     = XPAR_VECTORBLOX_MXP_ARM_0_DEVICE_ID;
#endif

#if (__ARM_ARCH_7A__ && VBX_USE_A9_PMU_TIMER)
	u32 tmrctr_freq_hz = XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/2;
#else
	u16 tmrctr_dev_id  = XPAR_TMRCTR_0_DEVICE_ID;
	u32 tmrctr_freq_hz = XPAR_TMRCTR_0_CLOCK_FREQ_HZ;
#endif

#ifdef XPAR_MICROBLAZE_USE_ICACHE
	Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
	Xil_DCacheEnable();
#endif
	// On Zynq's ARM Cortex-A9, the caches are enabled by the standalone BSP's
	// boot code, so we don't have to re-enable them here.
#if __ARM_ARCH_7A__
	// On Zynq, redirect accesses in the range 0x8000_0000-0x9fff_ffff
	// to be uncached access to DDR in the range 0x0-0x1fff_ffff.
#if 1
	vbx_zynq_remap_ddr_uncached();
#endif
#if VBX_USE_AXI_INSTR_PORT_NORMAL_MEMORY
	vbx_zynq_set_instr_port_normal_uncached();
#elif VBX_USE_AXI_INSTR_PORT_DEVICE_MEMORY
	vbx_zynq_set_instr_port_device_memory();
#endif
#endif

	status = VectorBlox_MXP_Initialize(&vbx_test_mxp_inst, mxp_dev_id);
	if (status != XST_SUCCESS) {
		VBX_PRINTF("ERROR: VectorBlox_MXP_Initialize failed.\n");
		VBX_FATAL(__LINE__, __FILE__, -1);
		// return XST_FAILURE;
	}

#if (__ARM_ARCH_7A__ && VBX_USE_A9_PMU_TIMER)
	vbx_timestamp_init(tmrctr_freq_hz);
#else
	status = XTmrCtr_Initialize(&vbx_test_tmr_inst, tmrctr_dev_id);
	if (status != XST_SUCCESS) {
		VBX_PRINTF("ERROR: XtmrCtr_Initialize failed.\n");
		VBX_FATAL(__LINE__, __FILE__, -1);
		// return XST_FAILURE;
	}

	vbx_timestamp_init(&vbx_test_tmr_inst, tmrctr_freq_hz);
#endif

	return XST_SUCCESS;
}

#endif // __MICROBLAZE__ || __ARM_ARCH_7A__

///////////////////////////////////////////////////////////////////////////
#if __MICROBLAZE__ && defined(USE_ZYNQ_UART)

// On Zynq, use PS7 UART for stdout.

#include "xil_io.h"

#define PS7_UART_BASEADDR 0xE0001000

/* Write to memory location or register */
#define X_mWriteReg(BASE_ADDRESS, RegOffset, data) \
	*(unsigned int *)(BASE_ADDRESS + RegOffset) = ((unsigned int) data);
/* Read from memory location or register */
#define X_mReadReg(BASE_ADDRESS, RegOffset) \
	*(unsigned int *)(BASE_ADDRESS + RegOffset);

#define XUartChanged_IsTransmitFull(BaseAddress) \
	((Xil_In32((BaseAddress) + 0x2C) & 0x10) == 0x10)

/************************************************************************/
void XUartChanged_SendByte(u32 BaseAddress, u8 Data)
{
	// Wait until there is space in TX FIFO
	while (XUartChanged_IsTransmitFull(BaseAddress));

	// Write the byte into the TX FIFO
	X_mWriteReg(BaseAddress, 0x30, Data);
}

/************************************************************************/
void outbyte(char c) {
	 XUartChanged_SendByte(PS7_UART_BASEADDR, c);
}

#endif // __MICROBLAZE__ && defined(USE_ZYNQ_UART)
///////////////////////////////////////////////////////////////////////////