#include "Config.h"

#include "Engine.h"

using namespace morphEngine::utility;
using namespace morphEngine::resourceManagement::fileSystem;
using namespace morphEngine::debugging;

namespace morphEngine
{
	namespace core
	{
		struct ConfigError
		{
			MSize line;
			MString errorMessage;

			inline ConfigError(MSize lineIndex, const MString& errorMsg) : line(lineIndex), errorMessage(errorMsg)	{ }

			inline bool operator==(const ConfigError& other) const
			{
				return line == other.line && errorMessage == other.errorMessage;
			}

			inline bool operator!=(const ConfigError& other) const
			{
				return line != other.line || errorMessage != other.errorMessage;
			}
		};

		void Config::ReadConfig()
		{
			File& f = Engine::GetInstance()->GetFileSystem().OpenFile(_filename, FileAccessMode::READ);
			if(!f.IsOpened())
			{
				return;
			}
			MFixedArray<MUint8> readData;
			f.Read(readData);
			Engine::GetInstance()->GetFileSystem().CloseFile(f);
			MSize index = 0;
			MSize size = readData.GetSize();
			MArray<ConfigError> errors;
			MSize lineIndex = 0;
			while(index < size)
			{
				MString line = "";
				while(readData[index] != '\n' && index < size)
				{
					line += readData[index++];
				}
				++index;
				if(line.Length() > 0)
				{
					line = MString::TrimFromEnd(line, '\r');
					line = MString::TrimFromEnd(line, '\n');
					line = MString::TrimFromEnd(line, '\t');
					line = MString::TrimFromStart(line, '\r');
					line = MString::TrimFromStart(line, '\n');
					line = MString::TrimFromStart(line, '\t');
					if(line[0] == '[' || line.Length() == 0)
					{
						continue;
					}
					++lineIndex;
				}

				MString key;
				MString values;
				MSize j = 0;
				MSize lineSize = line.Length();
				while(j < lineSize && line[j] != '=')
				{
					++j;
				}
				if(j == lineSize && lineSize > 1)
				{
					errors.Add(ConfigError(lineIndex, "There is no \'=\' sign in this line"));
					continue;
				}
				key = line.Substring(0, j);
				key.ReplaceNCS(" ", "");
				++j; //to pass = sign
				while(line[j] == ' ')
				{
					++j;
				}
				if(j == lineSize)
				{
					errors.Add(ConfigError(lineIndex, "There are no values here"));
					continue;
				}
				else
				{
					values = line.Substring(j, lineSize - j);
				}
				
				if(!_configs.Contains(key))
				{
					_configs.Add(key, ValueType());
				}
				_configs[key].Add(values);
			}

			MArray<ConfigError>::MIteratorArray it = errors.GetBegin();
			for(; it.IsValid(); ++it)
			{
				ConfigError& ce = (*it);
				Debug::Log(ELogType::WARNING, MString("Config file error, filename: ") + _filename + MString(", line: ") + MString::FromInt(ce.line) + MString(": ") + ce.errorMessage);
			}
		}

		void Config::WriteConfig()
		{
			File& f = Engine::GetInstance()->GetFileSystem().OpenFile(_filename, FileAccessMode::WRITE);
			if(!f.IsOpened())
			{
				return;
			}
			MString toWrite = "";
			ConfigType::MIteratorDictionary it = _configs.GetBegin();
			for(; it.IsValid(); ++it)
			{
				MString& key = (*it).GetKey();
				ValueType& value = (*it).GetValue();
				ValueType::MIteratorArray valuesIt = value.GetBegin();
				for(; valuesIt.IsValid(); ++valuesIt)
				{
					ValueElement& element = (*valuesIt);
					MString line = key + " = " + element;
					toWrite += line + "\n";
				}
			}
			f.Write(toWrite);
			Engine::GetInstance()->GetFileSystem().CloseFile(f);
		}

		Config::Config()
		{
		}

		Config::Config(const Config& other) : _configs(other._configs), _filename(other._filename)
		{

		}

		Config::~Config()
		{
			_filename.Shutdown();
			_configs.Shutdown();
		}
	}
}