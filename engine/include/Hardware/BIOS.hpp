#pragma once

#include <cstdlib>

namespace BIOS
{

	#if	defined	(__thumb__)
		#define	swi_call(x)	 asm volatile("swi\t" #x ::: "r0", "r1", "r2", "r3")
	#else
		#define	swi_call(x)	 asm volatile("swi\t" #x "<< 16" ::: "r0", "r1", "r2", "r3")
	#endif

	inline void SoftReset()
	{
		swi_call(0x0);
	}

	inline void RegisterRAMReset()
	{
		swi_call(0x1);
	}

	inline void Halt()
	{
		swi_call(0x2);
	}

	inline void Stop()
	{
		swi_call(0x3);
	}

	inline void InterruptWait()
	{
		swi_call(0x4);
	}

	inline void VBlankWait()
	{
		swi_call(0x5);
	}
	
	struct DivResult
	{
		std::int32_t Div;
		std::int32_t Mod;
		std::uint32_t AbsDiv;
	};
	inline DivResult Div(std::int32_t Numerator, std::int32_t Denominator)
	{
		register std::int32_t R0 asm("r0"){ Numerator };
		register std::int32_t R1 asm("r1"){ Denominator };
		register std::uint32_t R3 asm("r3");
		swi_call(0x6);
		return { R0, R1, R3 };
	}

	inline std::uint32_t Sqrt(std::uint32_t Num)
	{
		register std::uint32_t R0 asm("r0"){ Num };
		swi_call(0x8);
		return R0;
	}

	//TODO:
	// parameters to ATan should be 1 sign bit, 1 integral bit, 14 decimal bits
	// result is mapped between 0 and 2pi, but using the full range of the 
	inline std::uint16_t ATan(std::int16_t Tan)
	{
		[[maybe_unused]]register std::int16_t R0 asm("r0"){ Tan };
		swi_call(0x9);
		register std::uint16_t Result asm("r0");
		return Result;
	}
	inline std::int16_t ATan2(std::int16_t X, std::int16_t Y)
	{
		[[maybe_unused]]register std::int16_t R0 asm("r0"){ X };
		[[maybe_unused]]register std::int16_t R1 asm("r1"){ Y };
		swi_call(0xA);
		register std::uint16_t Result asm("r0");
		return Result;
	}

	//TODO: move the decompression functions to iwram?

	// Wram version is faster
	inline void LZ77UnCompWram(const void* Source, void* Destination)
	{
		[[maybe_unused]]register const void* R0 asm("r0"){ Source };
		[[maybe_unused]]register void* R1 asm("R1"){ Destination };
		swi_call(0x11);
	}

	inline void LZ77UnCompVram(const void* Source, void* Destination)
	{
		[[maybe_unused]]register const void* R0 asm("r0"){ Source };
		[[maybe_unused]]register void* R1 asm("R1"){ Destination };
		swi_call(0x12);
	}

	inline void HuffUnComp(const void* Source, void* Destination)
	{
		[[maybe_unused]]register const void* R0 asm("r0"){ Source };
		[[maybe_unused]]register void* R1 asm("R1"){ Destination };
		swi_call(0x13);
	}

	//TODO: compress sprite and background tiles first with LZ77 then Huffman
	//TODO: then use the reverse functions to uncompress

	//TODO: author more BIOS call functions as necessary
}
