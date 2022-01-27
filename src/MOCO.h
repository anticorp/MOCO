/*
 Name:		MOCO.h
 Created:	6/12/2021 2:58:42 PM
 Author:	Robert Hoople
 Editor:	http://www.visualmicro.com
*/

	#ifndef _MOCO_h
	#define _MOCO_h

	#if defined(ARDUINO) && ARDUINO >= 100
		#include "arduino.h"
	#else
		#include "WProgram.h"
	#endif

	#define MoCo_TxRx_REGISTER_SIZE 4
	#define MoCo_DevID_REGISTER_SIZE 8

#include "SettingStruct.h"
#include <ControlPanelInterface.h>
#include <ProfileObject.h>
#include <DriveSharedResources.h>
#include "AddressHelper.h"

namespace moco
{
	namespace Pololu
	{
		enum class LIMITid
		{
			MaxSpeed = 0,
			MaxAccel = 1,
			MaxDecel = 2,
			BrakeDuration = 3,
			MaxForwardSpeed = 4,
			MaxForwardAccel = 5,
			MaxForwardDecel = 6,
			ForwardBrakeDur = 7,
			MaxReverseSpeed = 8,
			MaxReverseAccel = 9,
			MaxReverseDecel = 10,
			ReverseBrakeDur = 11
		};
		enum class StatusFlag
		{
			ErrorStatus = 0,
			ErrorsOccured = 1,
			SerialErrorsOccured = 2,
			LimitStatus = 3,
			UnlimitedRC1rawValue = 4,
			RC1rawValue = 5,
			RC1scaledValue = 6,
			unlimitedRC2rawValue = 8,
			RC2rawValue = 9,
			RC2scaledValue = 10,
			UnlimAN1rawValue = 12,
			AN1rawValue = 13,
			AN1scaledValue = 14,
			UnlimAN2rawValue = 16,
			AN2rawValue = 17,
			AN2scaledValue = 18,
			TargetSpeed = 20,
			Speed = 21,
			BrakeAmount = 22,
			InputVoltage = 23,
			TempA = 24,
			TempB = 25,
			RCperiod = 26,
			BaudRateRegister = 27,
			UpTimeLow = 28,
			UpTimeHigh = 29,
			MaxSpeedForward = 30,
			MaxAccelForward = 31,
			MaxDecelForward = 32,
			BrakeDurationForward = 33,
			StartingSpeedForward = 34,
			MaxSpeedReverse = 36,
			MaxAccelReverse = 37,
			MaxDecelReverse = 38,
			BrakeDurationReverse = 39,
			StartingSpeedReverse = 40,
			CurrentLimit = 42,
			RawCurrent = 43,
			Current = 44,
			CurrentLimitingConsecutiveCount = 45,
			CurrentLimitingOccurrenceCount = 46,
			ResetFlags = 127
		};
	}
	namespace _
	{
		class MOCO
		{
		public:
			virtual bool ClearSafeStart() {}
			static bool InitializeAndResetAllMotorControllerHardware();
			static bool ResetAllMoCoChains();
			static bool RefreshChainErrorStates();
			static bool CheckAllMoCoChainsForErrors();
			bool ActiveErrorCheck();
			bool PassiveErrorCheck();
			virtual bool InitializeAndRegisterWithMoCo(con::CONTROLPANEL* controlPanel, int stackIndex, int driveUnitIndex, int impetusIndex, int MoCoIndex) {}
			virtual bool ConfirmStopped() {}
			virtual bool Forward(uint32_t forwardSpeed) {}
			virtual bool Reverse(uint32_t reverseSpeed) {}
			virtual char SetMotorLimits(Pololu::LIMITid limitID, uint32_t setValue) {}
			virtual uint32_t ReadMoCoStatus(Pololu::StatusFlag statusFlagID) {}
			virtual bool SetAMPlimit() {}
			virtual bool ResetChain() {}
			virtual bool ApplyMotorLimits() {}
			virtual const char* HiMyNameIs() {}
			virtual void SendHardwareReport() {}
			virtual bool SendProfileDataCYCLE(String& aString) {}
			virtual bool FullProfileBuildCYCLE(bool& cancel) {}
			virtual bool StartFullDriveProfileBuild(drv::prBuilder::CONFIGITEM* config);
			virtual uint32_t MMaxMotorSpeed() {}
			virtual uint32_t MMinMotorSpeed() {}
			virtual bool RecieveAndProcessProfileData(scomm::DATARECEIVER* data) {}
			virtual bool ProfileSenderLOOP(scomm::DATASENDER& ds) {}
			virtual void ResetProfileSender() {}
			bool CheckProfileValidation();
			bool DevalidateProfile();
			void DisplayFullProfile();
			ADDRESSHELPER Address = ADDRESSHELPER();
		protected:
			static void SendCommandWithAddress(byte command, byte mod, String& address);
			static con::CONTROLPANEL* mControlPanel;			
			drv::profile::MOCO Profile = drv::profile::MOCO();
			bool mConfigured = false;
			bool mInitialized = false;
			static bool mMOCOinit;
			bool mChainedDevice = false;
			bool mFirstChainedDevice = false;
			bool mErrorOnMyChain = false;
			bool mProfileSent = false;
			bool mSafeStartCleared = false;
			LIMITSETTINGS* mLimitSettings;
			LIMITSETTINGS mCurrLimits;

			uint8_t mChainIndex = 255;
			uint8_t mDevIndex = 255;

			static uint8_t mRegisteredChains;
	
			UARTClass* mSerial;
			uint32_t mBaudRate;
			uint32_t mErrorPin;
			uint32_t mResetPin;
			uint8_t mDevID;
			uint16_t mLimitSetFailTree;
			static const size_t mChainRegisterSizeMAX = MoCo_TxRx_REGISTER_SIZE;
			static const size_t mDeviceRegisterSizeMAX = MoCo_DevID_REGISTER_SIZE;
			
			static UARTClass* mSerialRegister[mChainRegisterSizeMAX];
			static uint32_t mChainBaudRate[mChainRegisterSizeMAX];
			static uint32_t mErrorPinRegister[mChainRegisterSizeMAX];
			static uint32_t mResetPinRegister[mChainRegisterSizeMAX];
			static uint8_t mDevicesRegisteredOnChain[mChainRegisterSizeMAX];
			static bool mChainError[mChainRegisterSizeMAX];
			
			static MOCO* mDeviceRegister[mChainRegisterSizeMAX][mDeviceRegisterSizeMAX];
			static uint8_t mDevIDRegister[mChainRegisterSizeMAX][mDeviceRegisterSizeMAX];

		protected:
			bool mSetAndConfirmRegisters();
			char ReadSerialByte();

		};

	}
	namespace Pololu
	{
		namespace __
		{
			class SMC_G2_18v5__ : public _::MOCO
			{
			public:
				SMC_G2_18v5__() {}
				/*		This Constructor is to simplify setup of chained Devices *AFTER* First is set up.
					If Constructed out of order, Initialization will fail. */
				SMC_G2_18v5__(UARTClass& serialChannel, uint8_t devID, LIMITSETTINGS& limitSettings, const char* name);
				SMC_G2_18v5__(bool chainedDevice, UARTClass& serialChannel, uint32_t baudRate, uint32_t errorPin, uint32_t resetPin, uint8_t devID, LIMITSETTINGS& limitSettings, const char* name);
				bool Forward(uint32_t forwardSpeed) override;
				bool Reverse(uint32_t reverseSpeed) override;					
				bool ClearSafeStart() override;
				//bool CheckError() override;
				bool InitializeAndRegisterWithMoCo(con::CONTROLPANEL* controlPanel, int stackIndex, int driveUnitIndex, int impetusIndex, int MoCoIndex) override;
				bool ConfirmStopped() override;
				char SetMotorLimits(LIMITid limitID, uint32_t setValue) override;
				bool ApplyMotorLimits() override;				
				static const char* InterpretSetLimitResponse(int response);
				bool RecieveAndProcessProfileData(scomm::DATARECEIVER* data) override;
				uint32_t ReadMoCoStatus(StatusFlag statusFlagID) override;
				/*	WARNING: Sends reset to all connected MoCos in the
					chain. Selective reset is not possible, without
					extensive hardware modifications. */
				bool ResetChain() override;
				const char* HiMyNameIs();
				void SendHardwareReport() override;
				bool ProfileSenderLOOP(scomm::DATASENDER& ds) override;
				void ResetProfileSender() override;
				bool SendProfileDataCYCLE(String& aString) override;
				bool FullProfileBuildCYCLE(bool& cancel) override;
				uint32_t MMaxMotorSpeed() { return 3200; }
				uint32_t MMinMotorSpeed() { return -3200; }
				//const char* Address() { return mAddress; }
				const int MaxMotorSpeed_FWD = 3200;
				const int MaxMotorSpeed_REV = -3200;
			private:
				
				const char* mName;
				LIMITSETTINGS mSetToSettings;
				char mAddress[8] = { '\0' };
				int mSI = -1;
				int mDI = -1;
				int mII = -1;
				int mMI = -1;
				bool mProfileValidated = false;
			};
		}
	}

}

#endif

