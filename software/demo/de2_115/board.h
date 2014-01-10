/* VECTORBLOX MXP SOFTWARE DEVELOPMENT KIT
 *
 * Copyright (C) 2012-2014 VectorBlox Computing Inc., Vancouver, British Columbia, Canada.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of VectorBlox Computing Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
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


#ifndef __BOARD_H
#define __BOARD_H

// for all mxp demos
#include "vbx.h"
#include "vbx_port.h"

// for all altera demos
#include "../common/altera_demo.h"

// board specific includes/defines
#include "altera_avalon_pio_regs.h"
#define BOARD "de2_115"
#define SCALAR_CPU "Nios"


#define VFR_PIXELS_PER_WORD 1
#define IMAGE_WIDTH  800
#define IMAGE_HEIGHT 600

// SW[2] selects between scalar and vector modes.
#define SWITCH_VECTOR_MANDEL      0
#define SWITCH_SCALAR_MANDEL      (0x4 | SWITCH_VECTOR_MANDEL)
#define SWITCH_VECTOR_REPULSION   1
#define SWITCH_SCALAR_REPULSION   (0x4 | SWITCH_VECTOR_REPULSION)
#define SWITCH_VECTOR_MANDEL2     2
#define SWITCH_SCALAR_MANDEL2     (0x4 | SWITCH_VECTOR_MANDEL2)
#define SWITCH_VECTOR_SPLIT       3
#define SWITCH_SCALAR_SPLIT       (0x4 | SWITCH_VECTOR_SPLIT)

// map switch value to mode number.
int slide_switches();
int application_mode(demo_t *pDemo, int local_mode);
int escape_application(demo_t *pDemo, int local_mode);
void board_init();

#endif// __BOARD_H
