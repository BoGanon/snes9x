#ifndef __PREFETCH_H__
#define __PREFETCH_H__

#define prefetch_load 0

#ifdef __ASSEMBLY__

	.macro __pref hint addr
	pref \hint,\addr
	.endm

	.macro pref_load addr
	__pref prefetch_load, \addr
	.endm
#endif

static inline void prefetch(const void *addr)
{
	__asm__ __volatile__(
		"pref	%0, (%1)\n\t"
	:
	: "i" (prefetch_load), "r" (addr));
}

#endif /*__PREFETCH_H__*/
