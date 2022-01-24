#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

namespace moco
{
	struct LIMITSETTINGS
	{
		LIMITSETTINGS() {}
		/// <summary> BOTH DIRECTIONS : </summary>
		/// <param name="maxSpeed"></param>
		/// <param name="maxAcceleration"></param>
		/// <param name="maxDeceleration"></param>
		/// <param name="brakeDuration"></param>
		LIMITSETTINGS(uint32_t maxSpeed, uint32_t maxAcceleration,
			uint32_t maxDeceleration, uint32_t brakeDuration);
		/// <summary> BY DIRECTION: </summary> <param name="direction"> - TRUE = Forward - FALSE = Reverse</param>
		/// <param name="maxForwardSpeed"></param> <param name="maxForwardAcceleration"></param>
		/// <param name="maxForwardDeceleration"></param><param name="forwardBrakeDuration"></param>
		LIMITSETTINGS(bool direction, uint32_t maxForwardSpeed, 
			uint32_t maxForwardAcceleration, uint32_t maxForwardDeceleration,
			uint32_t forwardBrakeDuration);
		/// <summary>
		/// ALL SETTINGS
		/// </summary>
		/// <param name="maxSpeed"></param>
		/// <param name="maxAcceleration"></param>
		/// <param name="maxDeceleration"></param>
		/// <param name="brakeDuration"></param>
		/// <param name="maxForwardSpeed"></param>
		/// <param name="maxForwardAcceleration"></param>
		/// <param name="maxForwardDeceleration"></param>
		/// <param name="forwardBrakeDuration"></param>
		/// <param name="maxReverseSpeed"></param>
		/// <param name="maxReverseAcceleration"></param>
		/// <param name="maxReverseDeceleration"></param>
		/// <param name="reverseBrakeDuration"></param>
		LIMITSETTINGS(uint32_t maxSpeed, uint32_t maxAcceleration,
			uint32_t maxDeceleration, uint32_t brakeDuration,
			uint32_t maxForwardSpeed, uint32_t maxForwardAcceleration, uint32_t maxForwardDeceleration,
			uint32_t forwardBrakeDuration, uint32_t maxReverseSpeed,
			uint32_t maxReverseAcceleration, uint32_t maxReverseDeceleration,
			uint32_t reverseBrakeDuration);
		/* For Settings method to identify which group */
		uint8_t SettingLevel;
		uint32_t MaxSpeed;
		uint32_t MaxAcceleration;
		uint32_t MaxDeceleration;
		uint32_t BrakeDuration;
		uint32_t MaxForwardSpeed;
		uint32_t MaxForwardAccel;
		uint32_t MaxForwardDecel;
		uint32_t ForwardBrakeDur;
		uint32_t MaxReverseSpeed;
		uint32_t MaxReverseAccel;
		uint32_t MaxReverseDecel;
		uint32_t ReverseBrakeDur;
		void SpeakUp();

	};
}

#define	_SetFailCode_MaxSpeed 0b1000000000000000
#define _SetFailCode_MaxAccel 0b0100000000000000
#define	_SetFailCode_MaxDecel 0b0010000000000000
#define	_SetFailCode_BrakeDuration 0b0001000000000000
#define	_SetFailCode_MaxForwardSpeed 0b0000100000000000
#define	_SetFailCode_MaxForwardAccel 0b0000010000000000
#define	_SetFailCode_MaxForwardDecel 0b0000001000000000
#define	_SetFailCode_ForwardBrakeDur 0b0000000100000000
#define	_SetFailCode_MaxReverseSpeed 0b0000000010000000
#define	_SetFailCode_MaxReverseAccel 0b0000000001000000
#define	_SetFailCode_MaxReverseDecel 0b0000000000100000
#define	_SetFailCode_ReverseBrakeDur 0b0000000000010000
#define	_SetFailCode_ClearTree 0b0000000000000000
