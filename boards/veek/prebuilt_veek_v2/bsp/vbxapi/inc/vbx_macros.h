/**
 * @file
 * @defgroup VBX_macros
 * @brief VBX macros
 *
 * @ingroup VBXapi
 */
/**@{*/
//VBXCOPYRIGHTTAG

#ifndef __VBX_MACROS_H
#define __VBX_MACROS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h> // for proper exit() prototype
#include <stdio.h>  // for printf() and putchar()

#include "vbx_extern.h"
#include "vbx_common.h"


// ---------------------------------

/** VBX start for macros ( do{...}while(0) --> VBX_S{...}VBX_E ) */
#define VBX_S do
/** VBX end for macros ( do{...}while(0) --> VBX_S{...}VBX_E ) */
#define VBX_E while(0)

// ---------------------------------

#define VBX_REG_MXPCPU       16

#if VBX_USE_GLOBAL_MXP_PTR
#define VBX_GET_THIS_MXP() (vbx_mxp_ptr)
#else
#define VBX_GET_THIS_MXP() \
	({ int __t__; vbx_get_reg( VBX_REG_MXPCPU, &__t__ ); (vbx_mxp_t*)__t__; })
#endif

//#define VBX_IS_VPTR(PTR)
//	({ vbx_void_t *__vptr__ = (vbx_void_t *)(PTR);
///           ( (VBX_SCRATCHPAD_ADDR<=__vptr__) && (__vptr__<VBX_SCRATCHPAD_END) );
//         })
//
//The alternative macros below only work if you can guarantee scratchpad
// start address is aligned to the scratchpad size.
//#define VPTR_MASK    (~(VECTOR_MEMORY_SIZE*1024-1))
//#define IS_VPTR(PTR) ( (((vbx_void_t *)PTR)&(VPTR_MASK))==(VBX_SCRATCHPAD_ADDR) )

#define VBX_PAD_UP(BYTES,ALIGNMENT) \
	({ \
		uint32_t __mask__ = ((uint32_t)(ALIGNMENT))-1; \
		( ((uint32_t)(BYTES)) + __mask__) & ~__mask__; \
	})

#define VBX_PAD_DN(BYTES,ALIGNMENT) \
	({ \
		uint32_t __mask__ = ((uint32_t)(ALIGNMENT))-1; \
		((uint32_t)(BYTES))  &  ~__mask__; \
	})

#define VBX_IS_MISALIGNED(LENGTH,ALIGNMENT)	((((uint32_t)(LENGTH))&((uint32_t)(ALIGNMENT)-1))?1:0)
#define VBX_IS_ALIGNED(LENGTH,ALIGNMENT)	(!VBX_IS_MISALIGNED((LENGTH),(ALIGNMENT)))

// ---------------------------------

#define VBX_PADDING() \
({ \
	int padding; \
	vbx_mxp_t *this_mxp = VBX_GET_THIS_MXP(); \
	padding = max( VBX_CPU_DCACHE_LINE_SIZE, this_mxp->dma_alignment_bytes ); \
})

// ---------------------------------

#define VBX_DEBUG_FUNC1(fname,...) \
	VBX_SKIP_ALL_CHECKS ? \
		fname##_nodebug(__VA_ARGS__) : \
		fname##_debug(__LINE__,__FILE__,__VA_ARGS__)

#define VBX_DEBUG_FUNC0(fname)\
	VBX_SKIP_ALL_CHECKS ? \
		fname##_nodebug() : \
		fname##_debug(__LINE__,__FILE__)

/** Malloc in scratchpad.
 *
 * @param[in] amount -- number of bytes to allocate
 */
#define vbx_sp_malloc(amount)      ( VBX_DEBUG_FUNC1(vbx_sp_malloc,amount) )

/** Set new position of scratchpad.
 *
 * @param[in] new_sp -- new address pointing to top of scratchpad
 */
#define vbx_sp_set(new_sp)         VBX_S{ VBX_DEBUG_FUNC1( vbx_sp_set, new_sp ); }VBX_E

/** Free entire scratchpad.
 *
 * Use @ref vbx_sp_push and @ref vbx_sp_pop for partial allocating/free
 */
#define vbx_sp_free()              VBX_S{ VBX_DEBUG_FUNC0( vbx_sp_free );        }VBX_E

/** Push current scratchpad address to stack
 *
 * Use with @ref vbx_sp_pop for partial freeing of scratchpad memory
 */
#define vbx_sp_push()              VBX_S{ VBX_DEBUG_FUNC0( vbx_sp_push );        }VBX_E

/** Pop current scratchpad address to stack
 *
 * Use with @ref vbx_sp_push for partial freeing of scratchpad memory
 */
#define vbx_sp_pop()               VBX_S{ VBX_DEBUG_FUNC0( vbx_sp_pop  );        }VBX_E

/** Malloc in scratchpad *aligned*.
 *
 * @param[in] amount -- number of bytes to allocate
 */
#define vbx_shared_alloca(amount)  ( VBX_DEBUG_FUNC1( vbx_shared_alloca, amount, alloca(amount+2*VBX_PADDING()) ) )


// ---------------------------------

#define VBX_PRINTF(...) \
	VBX_S{ \
		if( VBX_DEBUG_LEVEL ) { \
			printf( __VA_ARGS__ ); \
		} \
	}VBX_E

#define VBX_FATAL(LINE,FNAME,ERR) \
	VBX_S { \
		printf( "FATAL ERROR: line %d of %s.\n", LINE, FNAME ); \
		if( VBX_DEBUG_LEVEL > 1 ) { \
			printf("Exiting.\n"); \
			putchar(4); \
			exit(ERR); \
		} \
	} VBX_E

#define VBX_EXIT(ERR)  \
	VBX_FATAL(__LINE__,__FILE__,ERR)


#ifdef __cplusplus
}
#endif

#endif //__VBX_MACROS_H
/**@}*/