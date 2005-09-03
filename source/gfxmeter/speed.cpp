#include "../include/G3DAll.h"

typedef	void (*DELAY_FUNC)(uint32 uiMS);

int64 GetCyclesDifference(DELAY_FUNC DelayFunction, uint32 uiParameter) {
	unsigned int edx1, eax1;
	unsigned int edx2, eax2;
		

#   define rdtsc _asm _emit 0x0f _asm _emit 0x31
	__try {
		_asm {

            // Get the count before calling the known delay function
			rdtsc
			mov         esi, eax
			mov         edi, edx

            // call the delay function
            push        uiParameter
			mov         ebx, DelayFunction
			call        ebx
			pop         ebx

            // Get the count after the delay function
			rdtsc
			mov         edx2, edx
			mov         eax2, eax

			mov         edx1, edi
			mov         eax1, esi
		}
    } __except (1) {

        // Something horrible has happened.  Abort
		return 0;
	}
#   undef rdtsc

    // Compute the total cycle difference
	return ((((int64)edx2) << 32) + eax2) - 
           ((((int64)edx1) << 32) + eax1);
}


void Delay (unsigned int uiMS) {
	LARGE_INTEGER Frequency, StartCounter, EndCounter;
	__int64 x;

	// Get the frequency of the high performance counter.
	if (!QueryPerformanceFrequency (&Frequency)) return;
	x = Frequency.QuadPart / 1000 * uiMS;

	// Get the starting position of the counter.
	QueryPerformanceCounter (&StartCounter);

	do {
		// Get the ending position of the counter.	
		QueryPerformanceCounter (&EndCounter);
	} while (EndCounter.QuadPart - StartCounter.QuadPart < x);
}

void DelayOverhead (unsigned int uiMS)
{
	LARGE_INTEGER Frequency, StartCounter, EndCounter;
	__int64 x;

	// Get the frequency of the high performance counter.
	if (!QueryPerformanceFrequency (&Frequency)) return;
	x = Frequency.QuadPart / 1000 * uiMS;

	// Get the starting position of the counter.
	QueryPerformanceCounter (&StartCounter);
	
	do {
		// Get the ending position of the counter.	
		QueryPerformanceCounter (&EndCounter);
	} while (EndCounter.QuadPart - StartCounter.QuadPart == x);
}



int CPU_speed_in_MHz() {
	unsigned int uiRepetitions = 1;
	unsigned int uiMSecPerRepetition = 50;
	__int64	i64Total = 0, i64Overhead = 0;

	for (unsigned int nCounter = 0; nCounter < uiRepetitions; nCounter ++) {
		i64Total += GetCyclesDifference (Delay, uiMSecPerRepetition);
		i64Overhead += GetCyclesDifference (DelayOverhead, uiMSecPerRepetition);
	}

	// Calculate the MHz speed.
	i64Total -= i64Overhead;
	i64Total /= uiRepetitions;
	i64Total /= uiMSecPerRepetition;
	i64Total /= 1000;

	// Save the CPU speed.
	return (int) i64Total;
}
