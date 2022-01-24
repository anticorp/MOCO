#include "SettingStruct.h"
#include <ADM.h>

moco::LIMITSETTINGS::LIMITSETTINGS(uint32_t maxSpeed, uint32_t maxAcceleration, uint32_t maxDeceleration, uint32_t brakeDuration)
{
	MaxSpeed = maxSpeed;
	MaxAcceleration = maxAcceleration;
	MaxDeceleration = maxDeceleration;
	BrakeDuration = brakeDuration;
	SettingLevel = 0;
	MaxForwardSpeed = 0;
	MaxForwardAccel = 0;
	MaxForwardDecel = 0;
	ForwardBrakeDur = 0;
	MaxReverseSpeed = 0;
	MaxReverseAccel = 0;
	MaxReverseDecel = 0;
	ReverseBrakeDur = 0;
}

moco::LIMITSETTINGS::LIMITSETTINGS(bool direction, uint32_t maxDirectionSpeed, uint32_t maxDirectionAcceleration, uint32_t maxDirectionDeceleration, uint32_t directionBrakeDuration)
{
	if (direction)
	{
		MaxSpeed = 0;
		MaxAcceleration = 0;
		MaxDeceleration = 0;
		BrakeDuration = 0;
		MaxForwardSpeed = maxDirectionSpeed;
		MaxForwardAccel = maxDirectionAcceleration;
		MaxForwardDecel = maxDirectionDeceleration;
		ForwardBrakeDur = directionBrakeDuration;
		SettingLevel = 1;
		MaxReverseSpeed = 0;
		MaxReverseAccel = 0;
		MaxReverseDecel = 0;
		ReverseBrakeDur = 0;
	}
	else
	{
		MaxSpeed = 0;
		MaxAcceleration = 0;
		MaxDeceleration = 0;
		BrakeDuration = 0;
		MaxForwardSpeed = 0;
		MaxForwardAccel = 0;
		MaxForwardDecel = 0;
		ForwardBrakeDur = 0;
		MaxReverseSpeed = maxDirectionSpeed;
		MaxReverseAccel = maxDirectionAcceleration;
		MaxReverseDecel = maxDirectionDeceleration;
		ReverseBrakeDur = directionBrakeDuration;
		SettingLevel = 2;
	}

}

moco::LIMITSETTINGS::LIMITSETTINGS(uint32_t maxSpeed, uint32_t maxAcceleration, uint32_t maxDeceleration, uint32_t brakeDuration, uint32_t maxForwardSpeed, uint32_t maxForwardAcceleration, uint32_t maxForwardDeceleration, uint32_t forwardBrakeDuration, uint32_t maxReverseSpeed, uint32_t maxReverseAcceleration, uint32_t maxReverseDeceleration, uint32_t reverseBrakeDuration)
	:	MaxSpeed(maxSpeed), MaxAcceleration(maxAcceleration),
		MaxDeceleration(maxDeceleration), BrakeDuration(brakeDuration),
		MaxForwardSpeed(maxForwardSpeed), MaxForwardAccel(maxForwardAcceleration),
		MaxForwardDecel(maxForwardDeceleration), ForwardBrakeDur(forwardBrakeDuration),
		MaxReverseSpeed(maxReverseSpeed), MaxReverseAccel(maxReverseAcceleration),
		MaxReverseDecel(maxReverseDeceleration), ReverseBrakeDur(reverseBrakeDuration),
		SettingLevel(3)
{
}

void moco::LIMITSETTINGS::SpeakUp()
{
	xbugWatch();
	bug << bugVar(SettingLevel);
	bug << bugVar(MaxSpeed) << bugVar(MaxAcceleration) << bugVar(MaxDeceleration) << bugVar(BrakeDuration);
	bug << bugVar(MaxForwardSpeed) << bugVar(MaxForwardAccel) << bugVar(MaxForwardDecel) << bugVar(ForwardBrakeDur);
	bug << bugVar(MaxReverseSpeed) << bugVar(MaxReverseAccel) << bugVar(MaxReverseDecel) << bugVar(ReverseBrakeDur);

}
