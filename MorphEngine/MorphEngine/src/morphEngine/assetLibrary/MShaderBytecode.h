#pragma once

#include "MAsset.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MShaderBytecode : public MAsset
		{
			friend class resourceManagement::ResourceManager;
		protected:

			utility::MFixedArray<MUint8> _shaderBytecode;

			virtual void Shutdown() override;

		public:

			//Default constructor
			inline MShaderBytecode() : MAsset() { _type = MAssetType::SHADER; }
			//Constructs MAsset with given type and loads data from given bytes array
			inline MShaderBytecode(MAssetType type, const utility::MFixedArray<MUint8>& data) : MAsset(type, data)
			{
				LoadFromData(data);
			}
			inline MShaderBytecode(const MShaderBytecode& other) : MAsset(other), _shaderBytecode(other._shaderBytecode)
			{

			}

			inline virtual ~MShaderBytecode()
			{
				Shutdown();
			}

			inline const utility::MFixedArray<MUint8>& GetBytecode() const { return _shaderBytecode; }

			// Inherited via MAsset

			// This function only copies bytecode to the MShaderBytecode's array.
			virtual bool LoadFromData(const utility::MFixedArray<MUint8>& data) override;

			// This function only copies bytecode to the MShaderBytecode's array.
			virtual bool LoadFromData(const utility::MArray<MUint8>& data) override;

			// Loads bytecode from file and store it in data array, nothing else.
			virtual bool LoadFromFile(resourceManagement::fileSystem::File & file) override;

			// You generally don't save shader bytecode, so this function is unimplemented and will always return false.
			virtual bool SaveToFile(resourceManagement::fileSystem::File & file) const override;

			inline virtual void Initialize()
			{

			}

		};
	}
}