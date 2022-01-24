/*
 Name:		MOCO.cpp
 Created:	6/12/2021 2:58:42 PM
 Author:	Robert Hoople
 Editor:	http://www.visualmicro.com
*/

#include <ADM.h>
#include "MOCO.h"

namespace moco
{
	namespace _
	{
		con::CONTROLPANEL* MOCO::mControlPanel = nullptr;
		bool MOCO::mMOCOinit = false;
		uint8_t MOCO::mRegisteredChains = 0;
		
		const size_t mChainRegisterSizeMAX = MoCo_TxRx_REGISTER_SIZE;
		const size_t mDeviceRegisterSizeMAX = MoCo_DevID_REGISTER_SIZE;

		UARTClass* MOCO::mSerialRegister[mChainRegisterSizeMAX] = { nullptr };
		uint32_t MOCO::mChainBaudRate[mChainRegisterSizeMAX] = { 0 };
		uint32_t MOCO::mErrorPinRegister[mChainRegisterSizeMAX] = { 0 };
		uint32_t MOCO::mResetPinRegister[mChainRegisterSizeMAX] = { 0 };
		uint8_t MOCO::mDevicesRegisteredOnChain[mChainRegisterSizeMAX] = { 0 };
		bool MOCO::mChainError[mChainRegisterSizeMAX] = { false };
		
		MOCO* MOCO::mDeviceRegister[mChainRegisterSizeMAX][mDeviceRegisterSizeMAX] = { {0},{0} };
		uint8_t MOCO::mDevIDRegister[mChainRegisterSizeMAX][mDeviceRegisterSizeMAX] = { {0},{0} };

		bool MOCO::InitializeAndResetAllMotorControllerHardware()
		{
			if (mMOCOinit) return false;
			if (mRegisteredChains > 0)
			{
				bool Limits = false;
				bool SerialReady = false;
				int mDI = 0;
				while (true)
				{
					int i = (mDI + 1) % (mRegisteredChains + 1);
					if (!i) break;
					mSerialRegister[mDI]->begin(mChainBaudRate[mDI]);
					SerialReady = true;
					mControlPanel->SendMessage("\nMoCo : Chain Serial Started. Delaying (10)...\n");
					delay(1);
					pinMode(mErrorPinRegister[mDI], INPUT);
					mControlPanel->SendMessage("\nMoCo : Chain mError Pin Started. Delaying (10)...\n");
					delay(1);
					mDI = i;					
				}
				bool reset = ResetAllMoCoChains();								
				mMOCOinit = (SerialReady && reset);
			}
			return mMOCOinit;
		}
		bool MOCO::ResetAllMoCoChains()
		{
			int mDI = 0;
			while (true)
			{
				int i = (mDI + 1) % (mRegisteredChains + 1);
				if (!i) break;
				bug << xpara << F("MoCo : Attempting Chain Restart...");
				pinMode(mResetPinRegister[mDI], OUTPUT);
				digitalWrite(mResetPinRegister[mDI], LOW);
				delay(1);
				pinMode(mResetPinRegister[mDI], INPUT);
				bug << xpara << F("MoCo : Chain Restarted!");
				mDI = i;
			}
			return true;
		}
		bool MOCO::RefreshChainErrorStates()
		{
			/* ACTS ON ALL REGISTERED MoCo chains and updates all MoCo devices */
			int mDI = 0;
			bool mError = true;
			while (true)
			{
				int i = (mDI + 1) % (mRegisteredChains + 1);
				if (!i) break;
				mError = mError && (bool)digitalRead(mErrorPinRegister[mDI]);
				mChainError[mDI] = mError;
				mDI = i;
			}
			return mError;
		}
		bool MOCO::CheckAllMoCoChainsForErrors()
		{
			return false;
		}
		bool MOCO::ActiveErrorCheck()
		{
			/* Returns Individual state, but refreshes all chains. Every other Moco can
				be checked for errors confidently with a passive check immediately after
				the first MoCo calls this. */
			RefreshChainErrorStates();
			bool mError = mChainError[mChainIndex];
			return mError;
		}
		bool MOCO::PassiveErrorCheck()
		{
			/* Acts on Individual MoCo using cached error state */
			bool mError = mChainError[mChainIndex];
			return mError;
		}
		bool MOCO::StartFullDriveProfileBuild(drv::prBuilder::CONFIGITEM* config)
		{
			return true;
		}
		bool MOCO::CheckProfileValidation()
		{
			bool val = Profile.Validated();
			bug << xntttt << "MOCO, VALIDATED = " << xbool << val;
			return val;
		}
		bool MOCO::DevalidateProfile()
		{
			return Profile.DeValidateAndClear();
		}
		void MOCO::DisplayFullProfile()
		{
			Profile.DisplayFullProfile();
		}
		bool MOCO::mSetAndConfirmRegisters()
		{
			bool TxRxConfirm = false;
			bool DevIDconfirm = false;
			int I4 = 0;
			while (true)
			{
				int i4 = (I4 + 1) % (mChainRegisterSizeMAX + 1);
				if (!i4) break;
				if (mSerialRegister[I4] == mSerial)
				{
					mChainIndex = I4;					
					mSerial = mSerialRegister[mChainIndex];
					mErrorPin = mErrorPinRegister[mChainIndex];
					mResetPin = mResetPinRegister[mChainIndex];
					mBaudRate = mChainBaudRate[mChainIndex];
					mFirstChainedDevice = false;
					TxRxConfirm = true;
					break;
				}
				I4 = i4;
			}
			if (!TxRxConfirm)
			{
				int i = (mRegisteredChains + 1) % (mChainRegisterSizeMAX + 1);
				if (!i) return false;
				if (mSerialRegister[mRegisteredChains] == nullptr)
				{
					mChainIndex = mRegisteredChains;
					mRegisteredChains = i;
					mSerialRegister[mChainIndex] = mSerial;
					mErrorPinRegister[mChainIndex] = mErrorPin;
					mResetPinRegister[mChainIndex] = mResetPin;
					mChainBaudRate[mChainIndex] = mBaudRate;
					mFirstChainedDevice = true;
					TxRxConfirm = true;
				}
				else return false; /* That spot in the array should be empty. If it's not, something has gone wrong.*/
			}
				
			if (TxRxConfirm)
			{
				/* FIRST CHECK FOR CONFLICTING ID IN THE CHAIN	*/
				int I3 = 0;
				while (true)
				{
					int i3 = (I3 + 1) % (mDeviceRegisterSizeMAX + 1);
					if (!i3) break;
					if (mDevIDRegister[mChainIndex][I3] == mDevID)
					{	
						/*		An ID conflict cannot exist in the same serial chain, so we're
							just going to fail this and do nothing else. User code will need
							to be fixed, or motor controllers will need to be re-addressed. */
						bug << xpara << F("!!! ERROR !!!\n\tMOCO!");
						return false;
					}
					I3 = i3;
				}
				/* Now check register for free spot and increment registration count */
				int i2 = (mDevicesRegisteredOnChain[mChainIndex] + 1) % (mDeviceRegisterSizeMAX + 1);
				if (!i2) return false;
				/*	Still room left to register. Check if spot is blank.*/
				if (mDevIDRegister[mChainIndex][mDevicesRegisteredOnChain[mChainIndex]] == 0)
				{
					mDevIndex = mDevicesRegisteredOnChain[mChainIndex];
					mDevicesRegisteredOnChain[mChainIndex] = i2;
					mDevIDRegister[mChainIndex][mDevIndex] = mDevID;
					DevIDconfirm = true;
				}						
				if (DevIDconfirm)
				{
					mDeviceRegister[mChainIndex][mDevIndex] = this;					
					return true;
				}
				return false;
			}

		}
		char MOCO::ReadSerialByte()
		{
			char c;
			if (mSerial->readBytes(&c, 1) == 0) { return -1; }
			return (byte)c;
		}
	}
	namespace Pololu
	{
		namespace __
		{
			SMC_G2_18v5__::SMC_G2_18v5__(UARTClass& serialChannel, uint8_t devID, LIMITSETTINGS& limitSettings, const char* name)
				:	SMC_G2_18v5__(false, serialChannel, 0, 0, 0, devID, limitSettings, name)
			{
				
			}
			SMC_G2_18v5__::SMC_G2_18v5__(bool firstChainedDevice, UARTClass& serialChannel, uint32_t baudRate, uint32_t errorPin, uint32_t resetPin, uint8_t devID, LIMITSETTINGS& limitSettings, const char* name)
				: mName(name)
			{
				if (!mConfigured)
				{
						//mFirstChainedDevice = firstChainedDevice;						
						mBaudRate = baudRate;
						mResetPin = resetPin;
						mErrorPin = errorPin;
						mSerial = &serialChannel;
						mDevID = devID;
						mLimitSettings = &limitSettings;
						mLimitSetFailTree = 0;
						
						/* This exists only to help safety multiple devices against
							conflicting Serial assignments and to */
						mConfigured = true;
				}
			}
			bool SMC_G2_18v5__::InitializeAndRegisterWithMoCo(con::CONTROLPANEL* controlPanel, int stackIndex, int driveUnitIndex, int impetusIndex, int MoCoIndex)
			{
				bug << xpara << "INITIALIZING MOCO";
				mControlPanel = controlPanel;
				if(mConfigured && !mInitialized) /* */
				{
					mSI = stackIndex;
					mDI = driveUnitIndex;
					mII = impetusIndex;
					mMI = MoCoIndex;
					Address.Set(mSI, mDI, mII, mMI);
					bug << xnt << F("Name = ") << mName << xnt << F("Address = ") << Address;
					Profile.Initialize(MaxMotorSpeed_FWD, MaxMotorSpeed_REV, mName, mSI, mDI, mII, mMI);
					/*	Register this. Checks for Conflicts in Configuration and returns
						FALSE if there are conflicts or registers are full. Register size
						can be changed in the MOCO.h header, if adding more units to a build.*/
					if(!mSetAndConfirmRegisters())
					{
						bug << F("\n\nMOCO REGISTERED! ");
						return false;
					}
					mInitialized = true;
					if (mInitialized) bug << F("\n\nMOCO ") << mName << F(" SUCCESSFUL INITIALIZATION!");
					return true;
				}
				bug << F("\n\n!!! ERROR !!!\n\tMOCO ") << mName << F(" FAILED INITIALIZATION!!");
				return false;
			}
			bool SMC_G2_18v5__::ConfirmStopped()
			{
				xbugWatch(__FUNCTION__);
				xbugif(!ReadMoCoStatus(StatusFlag::Speed))
				{
					xbugTrue;
					return true;
				} xbugFalse;
				return false;
			}
			bool SMC_G2_18v5__::Forward(uint32_t forwardSpeed)
			{
				mSerial->write(0xAA);
				mSerial->write(mDevID);
				mSerial->write(0x05);
				mSerial->write(forwardSpeed & 0x1F);
				mSerial->write(forwardSpeed >> 5 & 0x7F);
				return true;
			}
			bool SMC_G2_18v5__::Reverse(uint32_t reverseSpeed)
			{
				mSerial->write(0xAA);
				mSerial->write(mDevID);
				mSerial->write(0x06);
				mSerial->write(reverseSpeed & 0x1F);
				mSerial->write(reverseSpeed >> 5 & 0x7F);
				return true;
			}
			bool SMC_G2_18v5__::ClearSafeStart()
			{				
				mSerial->write(0xAA);
				mSerial->write(mDevID);
				mSerial->write(0x83);
				delay(10);
				bug << xpara << F("MOCO : SAFE START CLEARED!");				
				return true;
			}

			char SMC_G2_18v5__::SetMotorLimits(LIMITid limitID, uint32_t setValue)
			{
				mSerial->write(0xAA);
				mSerial->write(mDevID);
				mSerial->write(0x22);	/* Set Limit Byte */
				mSerial->write(uint8_t(limitID));
				mSerial->write(setValue & 0x7F);
				mSerial->write(setValue >> 7);
				char success = ReadSerialByte();
				return success;
			}
			bool SMC_G2_18v5__::ApplyMotorLimits()
			{
				bool omni = mLimitSettings->SettingLevel == 0 ? true : false;
				bool forward = mLimitSettings->SettingLevel == 1 ? true : false;
				bool reverse = mLimitSettings->SettingLevel == 2 ? true : false;
				char response = -1;
				int failTest = 0;
				mControlPanel->SendMessage("\n");
				mControlPanel->SendMessage(mName);
				mControlPanel->SendMessage(" : Apply Settings\n");
				if(forward)
				{
					response = SetMotorLimits(LIMITid::MaxForwardSpeed, mLimitSettings->MaxForwardSpeed);
					failTest += response;
					mControlPanel->SendMessage(InterpretSetLimitResponse(response));
					response = SetMotorLimits(LIMITid::MaxForwardAccel, mLimitSettings->MaxForwardAccel);
					failTest += response;
					mControlPanel->SendMessage(InterpretSetLimitResponse(response));
					response = SetMotorLimits(LIMITid::MaxForwardDecel, mLimitSettings->MaxForwardDecel);
					failTest += response;
					mControlPanel->SendMessage(InterpretSetLimitResponse(response));
					response = SetMotorLimits(LIMITid::ForwardBrakeDur, mLimitSettings->ForwardBrakeDur);
					failTest += response;
					mControlPanel->SendMessage(InterpretSetLimitResponse(response));
					if(failTest)
					{
						return false;
					}
				}
				if(reverse)
				{
					response = SetMotorLimits(LIMITid::MaxReverseSpeed, mLimitSettings->MaxReverseSpeed);
					failTest += response;
					mControlPanel->SendMessage(InterpretSetLimitResponse(response));
					response = SetMotorLimits(LIMITid::MaxReverseAccel, mLimitSettings->MaxReverseAccel);
					failTest += response;
					mControlPanel->SendMessage(InterpretSetLimitResponse(response));
					response = SetMotorLimits(LIMITid::MaxReverseDecel, mLimitSettings->MaxReverseDecel);
					failTest += response;
					mControlPanel->SendMessage(InterpretSetLimitResponse(response));
					response = SetMotorLimits(LIMITid::ReverseBrakeDur, mLimitSettings->ReverseBrakeDur);
					failTest += response;
					mControlPanel->SendMessage(InterpretSetLimitResponse(response));
				}

				if(omni)
				{
					//xbugTrue;
					omni = omni ? (omni && SetMotorLimits(LIMITid::MaxSpeed, mLimitSettings->MaxSpeed)) : false;
					omni ? true : mLimitSetFailTree = mLimitSetFailTree | _SetFailCode_MaxSpeed;
					omni = omni ? (omni && SetMotorLimits(LIMITid::MaxAccel, mLimitSettings->MaxAcceleration)) : false;
					omni ? true : mLimitSetFailTree = mLimitSetFailTree | _SetFailCode_MaxAccel;
					omni = omni ? (omni && SetMotorLimits(LIMITid::MaxDecel, mLimitSettings->MaxDeceleration)) : false;
					omni ? true : mLimitSetFailTree = mLimitSetFailTree | _SetFailCode_MaxDecel;
					omni = omni ? (omni && SetMotorLimits(LIMITid::BrakeDuration, mLimitSettings->BrakeDuration)) : false;
					omni ? true : mLimitSetFailTree = mLimitSetFailTree | _SetFailCode_BrakeDuration;
					forward = true;
					reverse = true;

				}

				if(omni)
				{
					//xbugTrue;
					mCurrLimits.MaxSpeed = mLimitSettings->MaxSpeed;
					mCurrLimits.MaxAcceleration = mLimitSettings->MaxAcceleration;
					mCurrLimits.MaxDeceleration = mLimitSettings->MaxDeceleration;
					mCurrLimits.BrakeDuration = mLimitSettings->BrakeDuration;
				}

				if(forward)
				{
					//xbugTrue;
					mCurrLimits.MaxForwardSpeed = mLimitSettings->MaxSpeed;
					mCurrLimits.MaxForwardAccel = mLimitSettings->MaxAcceleration;
					mCurrLimits.MaxForwardDecel = mLimitSettings->MaxDeceleration;
					mCurrLimits.ForwardBrakeDur = mLimitSettings->BrakeDuration;
				}
				if(reverse)
				{
					//xbugTrue;
					mCurrLimits.MaxReverseSpeed = mLimitSettings->MaxSpeed;
					mCurrLimits.MaxReverseAccel = mLimitSettings->MaxAcceleration;
					mCurrLimits.MaxReverseDecel = mLimitSettings->MaxDeceleration;
					mCurrLimits.ReverseBrakeDur = mLimitSettings->BrakeDuration;
				}
				if (omni || forward || reverse)
				{
					bug << xpara << F("MOCO : LIMITS SET!");
				}
				else
				{
					bug << xpara << F("!!! ERROR !!!\n\tMOCO : UNABLE TO SET LIMITS!");
				}
			}
			uint32_t SMC_G2_18v5__::ReadMoCoStatus(Pololu::StatusFlag statusFlagID)
			{
				mSerial->write(0xAA);
				mSerial->write(mDevID);
				mSerial->write(0x21);
				mSerial->write(uint8_t(statusFlagID));
				return ReadSerialByte() + 256 * ReadSerialByte();
			}
			const char* SMC_G2_18v5__::InterpretSetLimitResponse(int response)
			{
				if (response == -1)
				{
					return "\n[ Limit Set mError Returned from Device! ] ";
				}
				if (response == 0)
				{
					return "[ No problems setting the limit. ] ";
				}
				if (response == 2)
				{
					return "[ Unable to set forward limit to the specified value because of hard motor limit settings.] ";
				}
				if (response == 3)
				{
					return "[ 	Unable to set forward and reverse limits to the specified value because of hard motor limit settings. ] ";
				}
				return "[ Response Code Not Recognized ]";
			}
			bool SMC_G2_18v5__::RecieveAndProcessProfileData(scomm::DATARECEIVER* data)
			{
				return Profile.RecieveAndProcessProfileDataCYCLE(data);
			}
			bool SMC_G2_18v5__::ResetChain()
			{
				/* There needs to be a Time-Critical version of this that
					doesn't use delays or tie-up the loop. Although
					During Initalization it's best to delay here and 
					keep anything else from happening until this is
					complete, so two seperate functions	are likely the 
					cleanest and best option. */
				if (mFirstChainedDevice)
				{
					pinMode(mResetPin, OUTPUT);
					digitalWrite(mResetPin, LOW);
					delay(5);
					pinMode(mResetPin, INPUT);
				}
				return true;
			}
			const char* SMC_G2_18v5__::HiMyNameIs()
			{
				return mName;
			}
			void SMC_G2_18v5__::SendHardwareReport()
			{
				mControlPanel->SendMoCoConfig(mName, mSI, mDI, mII, mMI, mDevID);
			}
			bool SMC_G2_18v5__::ProfileSenderLOOP(scomm::DATASENDER& ds)
			{
				if (!mProfileSent)
				{
					ds.AssembleAsCommandString(20, 5, Address);
					Profile.LoadDataSender(ds);
					ds.Seal();
					mProfileSent = true;
					return false;
				}
				return false;
			}
			void SMC_G2_18v5__::ResetProfileSender()
			{
				mProfileSent = false;
			}
			bool SMC_G2_18v5__::SendProfileDataCYCLE(String& aString)
			{
				return false;
			}
			bool SMC_G2_18v5__::FullProfileBuildCYCLE(bool& cancel)
			{
				mProfileValidated = true;
				return true;
			}
		}
	}
}