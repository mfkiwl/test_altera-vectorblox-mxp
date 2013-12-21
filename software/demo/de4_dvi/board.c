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
VBXCOPYRIGHT( board_de4 )

#include "board.h"

// map switch value to mode number.
int slide_switches()
{
	int r = IORD_ALTERA_AVALON_PIO_DATA(SLIDE_SWITCH_BASE);
	switch (r) {

		case SWITCH_VECTOR_SPLIT:
			return MODE_VECTOR_SPLIT;
		case SWITCH_SCALAR_SPLIT:
			return MODE_SCALAR_SPLIT;
		case SWITCH_VECTOR_MULTI:
			return MODE_VECTOR_MULTI;
		case SWITCH_SCALAR_MULTI:
			return MODE_SCALAR_MULTI;
		case SWITCH_VECTOR_FACE_DETECT:
			return MODE_VECTOR_FACE_DETECT;
		case SWITCH_SCALAR_FACE_DETECT:
			return MODE_SCALAR_FACE_DETECT;
		case SWITCH_VECTOR_EDGE_DETECT:
			return MODE_VECTOR_EDGE_DETECT;
		case SWITCH_SCALAR_EDGE_DETECT:
			return MODE_SCALAR_EDGE_DETECT;
		case SWITCH_VECTOR_MOTEST:
			return MODE_VECTOR_MOTEST;
		case SWITCH_SCALAR_MOTEST:
			return MODE_SCALAR_MOTEST;
		case SWITCH_VECTOR_MANDEL:
			return MODE_VECTOR_MANDEL;
		case SWITCH_SCALAR_MANDEL:
			return MODE_SCALAR_MANDEL;
        case SWITCH_VECTOR_REPULSION:
			return MODE_VECTOR_REPULSION;
        case SWITCH_SCALAR_REPULSION:
			return MODE_SCALAR_REPULSION;

		default:
			return MODE_PASSTHRU;
	}
}

int application_mode(demo_t *pDemo, int local_mode)
{
	return slide_switches();
}

int escape_application(demo_t *pDemo, int local_mode)
{
    if(application_mode( pDemo, local_mode) != local_mode){
        return 1;
    }
    return 0;
}

void board_init()
{
	input_mirroring_enable();
}

// Bit 6 of LED PIO doubles as DVI input mirrorring enable.
void input_mirroring_enable()
{
	IOWR_ALTERA_AVALON_PIO_DATA(LED_BASE, IORD_ALTERA_AVALON_PIO_DATA(LED_BASE) | 0x40);
}

void input_mirroring_disable()
{
	IOWR_ALTERA_AVALON_PIO_DATA(LED_BASE, IORD_ALTERA_AVALON_PIO_DATA(LED_BASE) & ~0x40);
}

