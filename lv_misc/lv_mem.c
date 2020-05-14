/**
 * @file lv_mem.c
 * General and portable implementation of malloc and free.
 * The dynamic memory monitoring is also supported.
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#include "lv_mem.h"
#include "lv_math.h"
#include <string.h>

#if LV_MEM_CUSTOM != 0
#include LV_MEM_CUSTOM_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/
#define LV_MEM_ADD_JUNK     0   /*Add memory junk on alloc (0xaa) and free(0xbb) (just for testing purposes)*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initiaiize the dyn_mem module (work memory and other variables)
 */
void lv_mem_init(void)
{
}

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory
 */
void * lv_mem_alloc(uint32_t size)
{
    return LV_MEM_CUSTOM_ALLOC(size);
}

/**
 * Free an allocated data
 * @param data pointer to an allocated memory
 */
void lv_mem_free(const void * data)
{
	/*Use custom, user defined free function*/
    LV_MEM_CUSTOM_FREE(data);
}

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data pointer to an allocated memory.
 * Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory
 */
void * lv_mem_realloc(void * data_p, uint32_t new_size)
{
	/* Avoid reallocating the same amount of memory */
	uint32_t old_size = lv_mem_get_size(data_p);
	if (old_size == new_size) return data_p;

	/* Allocate new memory */
	void * new_p = lv_mem_alloc(new_size);

	/* Copy the data */
	uint32_t copy_size = LV_MATH_MIN(new_size, old_size);
	if (copy_size != 0) {
		memcpy(new_p, data_p, copy_size);
		lv_mem_free(data_p);
	}

	return new_p;
}

/**
 * Join the adjacent free memory blocks
 */
void lv_mem_defrag(void)
{
}

/**
 * Give information about the work memory of dynamic allocation
 * @param mon_p pointer to a dm_mon_p variable,
 *              the result of the analysis will be stored here
 */
void lv_mem_monitor(lv_mem_monitor_t * mon_p)
{
    memset(mon_p, 0, sizeof(lv_mem_monitor_t));
}

/**
 * Give the size of an allocated memory
 * @param data pointer to an allocated memory
 * @return the size of data memory in bytes
 */
__attribute__((naked)) uint32_t lv_mem_get_size(const void * data)
{
	__asm volatile(
		"	brf			r0, .lv_mem_get_size_exit\n"
		"	ldsz		r0, r0\n"
		"	ret\n"
		".lv_mem_get_size_exit:\n"
		"	ldc			r0, 0\n"
		"	ret\n"
	);
}
