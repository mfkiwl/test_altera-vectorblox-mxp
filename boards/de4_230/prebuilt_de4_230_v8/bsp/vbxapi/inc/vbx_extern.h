/**
 * @file
 * @defgroup VBX_extern
 * @brief VBX Extern
 *
 * @ingroup VBXapi 
 */
/**@{*/
//VBXCOPYRIGHTTAG

#ifndef __VBX_EXTERN_H
#define __VBX_EXTERN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vbx_types.h"


#ifndef VBX_SKIP_ALL_CHECKS
#define VBX_SKIP_ALL_CHECKS  1 /*(mxp_cpu->skip_all_checks)*/
#endif

#ifndef VBX_DEBUG_LEVEL
//Set below 4 for running tests, should be higher for debugging
#define VBX_DEBUG_LEVEL      3 /*(mxp_cpu->debug_level)*/
#endif

#ifndef VBX_SAFE_TO_CLOBBER_SOURCE
#define VBX_SAFE_TO_CLOBBER_SOURCE  0   /* some algorithms operate faster if they can clobber source operand memory */
#endif

///////////////////////////////////////////////////////////////////////////
#define VBX_DEBUG_MALLOC 0
#define VBX_DEBUG_NO_SPSTACK 0
#define VBX_DEBUG_VBXLIB 0
#define VBX_USE_GLOBAL_MXP_PTR 1
#define VBX_USE_AXI_INSTR_PORT_NORMAL_MEMORY 0
#define VBX_USE_AXI_INSTR_PORT_DEVICE_MEMORY 1
#define VBX_USE_AXI_INSTR_PORT_ADDR_INCR 0
#define VBX_USE_AXI_INSTR_PORT_VST 1
#define VBX_USE_A9_PMU_TIMER 1
///////////////////////////////////////////////////////////////////////////

#if VBX_USE_GLOBAL_MXP_PTR
extern vbx_mxp_t* vbx_mxp_ptr;
#endif

#ifdef __cplusplus
}
#endif

#endif //__VBX_EXTERN_H
/**@}*/