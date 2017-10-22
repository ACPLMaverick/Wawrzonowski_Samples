#pragma once

#include "utility/MDictionary.h"
#include "utility/MString.h"

namespace morphEngine
{
	namespace core
	{
		class Config
		{
		public:
			typedef utility::MString KeyType;
			typedef utility::MString ValueElement;
			typedef utility::MArray<ValueElement> ValueType;
			typedef utility::MDictionary<KeyType, ValueType> ConfigType;

		protected:
			ConfigType _configs;
			utility::MString _filename;
			ValueType _empty;

		protected:
			void ReadConfig();
			void WriteConfig();

		public:
			Config();
			Config(const Config& other);
			~Config();

			inline void Read(const utility::MString& filename)
			{
				_filename = filename;
				ReadConfig();
			}

			inline void Write()
			{
				WriteConfig();
			}

			inline const ValueType GetValues(const KeyType& key) const
			{
				if(_configs.Contains(key))
				{
					return _configs[key];
				}

				return _empty;
			}

			inline void GetValues(const KeyType& key, ValueType& outValues) const
			{
				_configs.TryGetValue(key, &outValues);
			}

			inline ConfigType& GetAll() { return _configs; }

			inline void Clear() { _configs.Clear(); }

			inline void SetValue(const KeyType& key, const ValueElement& value)
			{
				if(_configs.Contains(key))
				{
					_configs[key].Add(value);
				}
				else
				{
					ValueType values;
					values.Add(value);
					_configs.Add(key, values);
				}
			}
		};
	}
}