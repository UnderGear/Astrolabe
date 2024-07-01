#pragma once

namespace BIOS
{

	#if	defined	( __thumb__ )
		#define	swi_call(x)	 asm volatile("swi\t" #x ::: "r0", "r1", "r2", "r3")
	#else
		#define	swi_call(x)	 asm volatile("swi\t" #x "<< 16" ::: "r0", "r1", "r2", "r3")
	#endif

	inline void SoftReset()
	{
		swi_call(0x0);
	}

	inline void VBlankWait()
	{
		swi_call(0x5);
	}

	//TODO: author more BIOS call functions as necessary
}
