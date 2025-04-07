#pragma once

#include "natives_decl.hpp"

#include "Label.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class AbstractModel
	{
	private:
		Hash model;

	public:
		constexpr AbstractModel(Hash model) noexcept
			: model(model)
		{
		}

		void operator =(Hash model) noexcept
		{
			this->model = model;
		}

		operator Hash() const noexcept
		{
			return model;
		}

		[[nodiscard]] bool isValidModel() const; // because of the get_model_info hook, this will return false for null models
		[[nodiscard]] bool isNull() const noexcept;
		[[nodiscard]] bool isNullVehicle() const noexcept;

		[[nodiscard]] CBaseModelInfo* getModelInfo() const noexcept;
		[[nodiscard]] CVehicleModelInfo* getVehicleModelInfo() const noexcept;

		[[nodiscard]] bool isPed() const noexcept;
		[[nodiscard]] bool isObject() const noexcept;
		[[nodiscard]] bool isObjectStrict() const noexcept;
		[[nodiscard]] bool isVehicle() const noexcept;

		[[nodiscard]] Vector3 getDimensions() const;

		[[nodiscard]] Label getLabel() const;
		[[nodiscard]] Label getLabelPed() const;
		
		// Vehicle Specific

		[[nodiscard]] VehicleType getVehicleType() const noexcept;
		[[nodiscard]] bool isSubmarine() const noexcept;
		[[nodiscard]] bool isTrailer() const noexcept;
		[[nodiscard]] bool isPlane() const noexcept;
		[[nodiscard]] bool isHeli() const noexcept;
		[[nodiscard]] bool isBike() const noexcept;
		[[nodiscard]] bool isBoat() const noexcept;
		[[nodiscard]] bool isBicycle() const noexcept;
		[[nodiscard]] bool isSubmarineCar() const noexcept;
		[[nodiscard]] bool isTrain() const noexcept;

		[[nodiscard]] bool isAirborne() const noexcept;
		[[nodiscard]] bool isAquatic() const noexcept;

		[[nodiscard]] int getNumSeats() const;

		[[nodiscard]] bool isTank() const noexcept;
		[[nodiscard]] bool isTraffic() const noexcept;
		[[nodiscard]] bool isTowTruck() const noexcept;
		[[nodiscard]] bool isCargobob() const noexcept;
		[[nodiscard]] bool isBigVehicle() const noexcept;

		[[nodiscard]] bool hasSpecialFlight() const noexcept;
		[[nodiscard]] bool hasVtol() const noexcept;
		[[nodiscard]] bool hasDriftTune() const noexcept;

		[[nodiscard]] bool hasWindowTintMinusOne() const noexcept;
		[[nodiscard]] bool isStealthVehicle() const noexcept;
		[[nodiscard]] bool isRemoteControllable() const noexcept;
		[[nodiscard]] bool isRemoteControllablePv() const noexcept;
		[[nodiscard]] bool usesAttachmentCameras() const noexcept;
		[[nodiscard]] bool canBeStoodUpon() const noexcept;

		// Object Specific

		[[nodiscard]] bool isCageObject() const noexcept;
		[[nodiscard]] bool isScubaObject() const noexcept;

		// Ped Specific

		[[nodiscard]] bool isStoryCharacter() const noexcept;
		[[nodiscard]] bool isOnlineCharacter() const noexcept;
		[[nodiscard]] bool isPlayableOnlineCharacter() const noexcept;

		[[nodiscard]] bool isAnimal() const noexcept;
		[[nodiscard]] bool isWaterAnimal() const noexcept;
		[[nodiscard]] bool isObtainableFromPeyote() const noexcept;

		[[nodiscard]] bool canBecomeHostile() const noexcept;
		[[nodiscard]] bool canUseProjectiles() const noexcept;
		[[nodiscard]] bool canUseWeapons() const noexcept;

		[[nodiscard]] bool isCsPed() const noexcept;
		[[nodiscard]] bool isHiResPed() const noexcept;

		// Misc

		[[nodiscard]] bool isSusAttachee() const noexcept;
		[[nodiscard]] bool isSusAttachment() const noexcept;
		[[nodiscard]] bool isValidParachute() const noexcept;

		[[nodiscard]] bool supportsInteriorColours() const noexcept;
	};
#pragma pack(pop)
	static_assert(sizeof(AbstractModel) == sizeof(int));
}
