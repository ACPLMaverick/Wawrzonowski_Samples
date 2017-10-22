#include "MString.h"
#include "utility/MMath.h"
#include "utility/MVector.h"
#include "utility/MMatrix.h"

#include <string>

namespace morphEngine
{
	namespace utility
	{
		const MSID MSID::WRONG_SID = MSID();

		const MInt32 MSID::_a = 54059;
		const MInt32 MSID::_b = 76963;
		const MInt32 MSID::_c = 86969;
		const MInt32 MSID::_first = 37;

		void MSID::Generate(const MArray<char>& string)
		{
			MInt32 hash = _first;
			MSize i = 0;
			MSize size = string.GetSize() - 1;
			while(i < size)
			{
				hash = (hash * _a) ^ (string[i] * _b);
				++i;
			}

			_value = hash % _c;
		}

		void MSID::Generate(const char* string)
		{
			MInt32 hash = _first;
			MUint32 i = 0;
			MUint32 size = (MUint32)std::strlen(string);
			while(i < size)
			{
				hash = (hash * _a) ^ (string[i] * _b);
				++i;
			}

			_value = hash % _c;
		}

		MString MString::Substring(MSize startIndex, MSize length) const
		{
			if(length <= 0)
			{
				return "";
			}

			startIndex = MMath::Max(0, startIndex);
			length = MMath::Min(length, Length() - startIndex);

			MArray<char> toRet;
			MSize endIndex = startIndex + length;
			for(MSize i = startIndex; i < endIndex; ++i)
			{
				toRet.Add(_text[i]);
			}

			return MString(toRet);
		}

		MInt32 MString::FindLast(char character) const
		{
			MSize length = Length();
			for(MInt32 i = (MInt32)length - 1; i >= 0; --i)
			{
				if(_text[i] == character)
				{
					return i;
				}
			}
			return -1;
		}

		MInt32 MString::FindLastNCS(char character) const
		{
			MSize length = Length();
			char chacterLower = std::tolower(character);
			for(MSize i = length - 1; i >= 0; --i)
			{
				if(std::tolower(_text[i]) == chacterLower)
				{
					return (MInt32)i;
				}
			}
			return -1;
		}

		MInt32 MString::FindLast(const MString& string) const
		{
			MSize length = Length();
			MSize stringLength = string.Length();
			if(stringLength > length)
			{
				return -1;
			}

			for(MInt32 i = (MInt32)(length - stringLength); i >= 0; --i)
			{
				if(_text[i] == string[0])
				{
					bool found = true;
					for(MSize j = i + 1; j < (i + stringLength); ++j)
					{
						if(_text[j] != string[j - i])
						{
							found = false;
							break;
						}
					}
					if(found)
					{
						return i;
					}
				}
			}

			return -1;
		}

		MInt32 MString::FindLastNCS(const MString& string) const
		{
			MSize length = Length();
			MSize stringLength = string.Length();
			if(stringLength > length)
			{
				return -1;
			}

			for(MInt32 i = (MInt32)(length - stringLength); i >= 0; --i)
			{
				if(std::tolower(_text[i]) == std::tolower(string[0]))
				{
					bool found = true;
					for(MSize j = i + 1; j < (i + stringLength); ++j)
					{
						if(std::tolower(_text[j]) != std::tolower(string[j - i]))
						{
							found = false;
							break;
						}
					}
					if(found)
					{
						return i;
					}
				}
			}

			return -1;
		}

		MInt32 MString::FindFirst(char character) const
		{
			MSize length = Length();
			for(MSize i = 0; i < length; ++i)
			{
				if(_text[i] == character)
				{
					return (MInt32)i;
				}
			}
			return -1;
		}

		MInt32 MString::FindFirstNCS(char character) const
		{
			MSize length = Length();
			char characterLower = std::tolower(character);
			for(MSize i = 0; i < length; ++i)
			{
				if(std::tolower(_text[i]) == characterLower)
				{
					return (MInt32)i;
				}
			}
			return -1;
		}

		MInt32 MString::FindFirst(const MString& string) const
		{
			MSize length = Length();
			MSize stringLength = string.Length();
			if(stringLength > length)
			{
				return -1;
			}
			else if(stringLength == length)
			{
				for(MSize i = 0; i < stringLength; ++i)
				{
					if(_text[i] != string[i])
					{
						return -1;
					}
				}

				return 0;
			}

			MSize endIndex = length - stringLength + 1;
			for(MSize i = 0; i < endIndex; ++i)
			{
				if(_text[i] == string[0])
				{
					bool found = true;
					for(MSize j = i + 1; j < i + stringLength; ++j)
					{
						if(_text[j] != string[j - i])
						{
							found = false;
							break;
						}
					}
					if(found)
					{
						return (MInt32)i;
					}
				}
			}

			return -1;
		}

		MInt32 MString::FindFirstNCS(const MString& string) const
		{
			MSize length = Length();
			MSize stringLength = string.Length();
			const char* t = *(*this);
			if(stringLength > length)
			{
				return -1;
			}
			else if(stringLength == length)
			{
				for(MSize i = 0; i < stringLength; ++i)
				{
					if(std::tolower(_text[i]) != std::tolower(string[i]))
					{
						return -1;
					}
				}

				return 0;
			}

			MSize endIndex = length - stringLength + 1;
			for(MSize i = 0; i < endIndex; ++i)
			{
				if(std::tolower(_text[i]) == std::tolower(string[0]))
				{
					bool found = true;
					for(MSize j = i + 1; j < i + stringLength; ++j)
					{
						if(std::tolower(_text[j]) != std::tolower(string[j - i]))
						{
							found = false;
							break;
						}
					}
					if(found)
					{
						return (MInt32)i;
					}
				}
			}

			return -1;
		}

		void MString::Replace(char charToReplace, char charToInsert)
		{
			MSize length = _text.GetSize();
			for(MSize i = 0; i < length; ++i)
			{
				if(_text[i] == charToReplace)
				{
					_text[i] = charToInsert;
				}
			}
			_sid.Generate(_text);
		}

		void MString::ReplaceNCS(char charToReplace, char charToInsert)
		{
			MSize length = _text.GetSize();
			char charToReplaceLower = std::tolower(charToReplace);
			for(MSize i = 0; i < length; ++i)
			{
				if(std::tolower(_text[i]) == charToReplaceLower)
				{
					_text[i] = charToInsert;
				}
			}
			_sid.Generate(_text);
		}

		void MString::Replace(const MString& stringToReplace, const MString& stringToInsert)
		{
			MSize stringToReplaceLength = stringToReplace.Length();
			MSize stringToInsertLength = stringToInsert.Length();
			if(stringToReplaceLength == 0)
			{
				return;
			}

			if(stringToReplaceLength == stringToInsertLength)
			{
				MSize endIndex = MMath::Max((MSize)0, Length() - stringToReplaceLength + 1);
				char firstChar = stringToReplace[0];
				for(MSize i = 0; i < endIndex; ++i)
				{
					if(_text[i] == firstChar)
					{
						bool found = true;
						for(MSize j = i + 1; j < (i + stringToReplaceLength); ++j)
						{
							if(_text[j] != stringToReplace[j - i])
							{
								found = false;
								break;
							}
						}
						if(found)
						{
							for(MSize j = i + 1; j < (i + stringToReplaceLength); ++j)
							{
								_text[j] = stringToInsert[j - i];
							}
							_text[i] = stringToInsert[0];
							i += stringToReplaceLength - 1;
						}
					}
				}
			}
			else
			{
				ReplaceReallocate(stringToReplace, stringToInsert);
			}
			_sid.Generate(_text);
		}

		void MString::ReplaceNCS(const MString& stringToReplace, const MString& stringToInsert)
		{
			MSize stringToReplaceLength = stringToReplace.Length();
			MSize stringToInsertLength = stringToInsert.Length();
			if(stringToReplaceLength == 0)
			{
				return;
			}

			if(stringToReplaceLength == stringToInsertLength)
			{
				MSize endIndex = MMath::Max((MSize)0, Length() - stringToReplaceLength + 1);
				char firstChar = std::tolower(stringToReplace[0]);
				for(MSize i = 0; i < endIndex; ++i)
				{
					if(std::tolower(_text[i]) == firstChar)
					{
						bool found = true;
						for(MSize j = i + 1; j < (i + stringToReplaceLength); ++j)
						{
							if(std::tolower(_text[j]) != std::tolower(stringToReplace[j - i]))
							{
								found = false;
								break;
							}
						}
						if(found)
						{
							for(MSize j = i + 1; j < (i + stringToReplaceLength); ++j)
							{
								_text[j] = stringToInsert[j - i];
							}
							_text[i] = stringToInsert[0];
							i += stringToReplaceLength - 1;
						}
					}
				}
			}
			else
			{
				ReplaceReallocateNCS(stringToReplace, stringToInsert);
			}
			_sid.Generate(_text);
		}

		MInt8 MString::CompareAlphabetically(const MString& other) const
		{
			MSize lowerLength = Length() < other.Length() ? Length() : other.Length();
			for(MSize i = 0; i < lowerLength; ++i)
			{
				MInt32 thisChar = std::tolower(_text[i]);
				MInt32 otherChar = std::tolower(other[i]);
				if(thisChar != otherChar)
				{
					return thisChar < otherChar ? -1 : 1;
				}
			}

			return Length() < other.Length() ? -1 : (Length() == other.Length() ? 0 : 1);
		}

		MString MString::FromBoolean(bool value)
		{
			return value ? "True" : "False";
		}

		MString MString::FromInt(MInt8 value)
		{
			char* buffer = new char[30];
			sprintf_s(buffer, 30, "%i", value);
			MString toRet(buffer);
			delete[] buffer;
			return toRet;
		}

		MString MString::FromInt(MInt16 value)
		{
			char* buffer = new char[30];
			sprintf_s(buffer, 30, "%i", value);
			MString toRet(buffer);
			delete[] buffer;
			return toRet;
		}

		MString MString::FromInt(MInt32 value)
		{
			char* buffer = new char[30];
			sprintf_s(buffer, 30, "%i", value);
			MString toRet(buffer);
			delete[] buffer;
			return toRet;
		}

		MString MString::FromInt(MInt64 value)
		{
			char* buffer = new char[30];
			sprintf_s(buffer, 30, "%lli", value);
			MString toRet(buffer);
			delete[] buffer;
			return toRet;
		}

		MString MString::FromInt(MUint8 value)
		{
			char* buffer = new char[30];
			sprintf_s(buffer, 30, "%i", value);
			MString toRet(buffer);
			delete[] buffer;
			return toRet;
		}

		MString MString::FromInt(MUint16 value)
		{
			char* buffer = new char[30];
			sprintf_s(buffer, 30, "%i", value);
			MString toRet(buffer);
			delete[] buffer;
			return toRet;
		}

		MString MString::FromInt(MUint32 value)
		{
			char* buffer = new char[30];
			sprintf_s(buffer, 30, "%i", value);
			MString toRet(buffer);
			delete[] buffer;
			return toRet;
		}

		MString MString::FromInt(MUint64 value)
		{
			char* buffer = new char[30];
			sprintf_s(buffer, 30, "%lli", value);
			MString toRet(buffer);
			delete[] buffer;
			return toRet;
		}

		MString MString::FromFloat(MFloat32 value)
		{
			char* buffer = new char[30];
			sprintf_s(buffer, 30, "%f", value);
			MString toRet(buffer);
			delete[] buffer;
			return TrimFromEnd(MString(toRet), '0');
		}

		MString MString::FromVector2(const MVector2 & value)
		{
			return "[" + FromFloat(value.X) + ", " + FromFloat(value.Y) + "]";
		}

		MString MString::FromVector3(const MVector3 & value)
		{
			return "[" + FromFloat(value.X) + ", " + FromFloat(value.Y) + ", " + FromFloat(value.Z) + "]";
		}

		MString MString::FromVector4(const MVector4 & value)
		{
			return "[" + FromFloat(value.X) + ", " + FromFloat(value.Y) + ", " + FromFloat(value.Z) + ", " + FromFloat(value.W) + "]";
		}

		MString MString::FromMatrix(const MMatrix & value)
		{
			return FromVector4(value[0]) + "\n" + FromVector4(value[1]) + "\n" + FromVector4(value[2]) + "\n" + FromVector4(value[3]) + "\n";
		}

		MString MString::FromDouble(MDouble64 value)
		{
			char* buffer = new char[30];
			sprintf_s(buffer, 30, "%lf", value);
			MString toRet(buffer);
			delete[] buffer;
			return TrimFromEnd(MString(toRet), '0');
		}

		MString MString::FromDouble(MLongDouble64 value)
		{
			char* buffer = new char[30];
			sprintf_s(buffer, 30, "%lf", value);
			MString toRet(buffer);
			delete[] buffer;
			return TrimFromEnd(MString(toRet), '0');
		}

		bool MString::ToBoolean(const MString& string)
		{
			return string == "True";
		}

		MInt8 MString::ToInt8(const MString& string)
		{
			return (MInt8)std::stoi(*string);
		}

		MInt16 MString::ToInt16(const MString& string)
		{
			return (MInt16)std::stoi(*string);
		}

		MInt32 MString::ToInt32(const MString& string)
		{
			return (MInt32)std::stoi(*string);
		}

		MInt64 MString::ToInt64(const MString& string)
		{
			return (MInt64)std::stoi(*string);
		}

		MUint8 MString::ToUint8(const MString& string)
		{
			return (MUint8)std::stoul(*string);
		}

		MUint16 MString::ToUint16(const MString& string)
		{
			return (MUint16)std::stoul(*string);
		}

		MUint32 MString::ToUint32(const MString& string)
		{
			return (MUint32)std::stoul(*string);
		}

		MUint64 MString::ToUint64(const MString& string)
		{
			return (MUint64)std::stoul(*string);
		}

		MFloat32 MString::ToFloat32(const MString& string)
		{
			return (MFloat32)std::stof(*string);
		}

		MDouble64 MString::ToDouble64(const MString& string)
		{
			return (MDouble64)std::stod(*string);
		}

		MLongDouble64 MString::ToLongDouble64(const MString& string)
		{
			return (MLongDouble64)std::stold(*string);
		}

		MString MString::Replace(const MString& string, char charToReplace, char charToInsert)
		{
			MString copy(string);
			copy.Replace(charToReplace, charToInsert);
			return copy;
		}

		MString MString::ReplaceNCS(const MString& string, char charToReplace, char charToInsert)
		{
			MString copy(string);
			copy.ReplaceNCS(charToReplace, charToInsert);
			return copy;
		}

		MString MString::Replace(const MString& string, const MString& stringToReplace, const MString& stringToInsert)
		{
			MString copy(string);
			copy.Replace(stringToReplace, stringToInsert);
			return copy;
		}

		MString MString::ReplaceNCS(const MString& string, const MString& stringToReplace, const MString& stringToInsert)
		{
			MString copy(string);
			copy.ReplaceNCS(stringToReplace, stringToInsert);
			return copy;
		}

		void MString::Split(const MString& string, const MArray<char>& charactersToSplitBy, MArray<MString>& splittedStrings)
		{
			MSize length = string.Length();
			MSize charsLength = charactersToSplitBy.GetSize();
			MSize lastSplitIndex = 0;

			for(MSize i = 0; i < length; ++i)
			{
				bool found = false;
				for(MSize j = 0; j < charsLength; ++j)
				{
					if(string[i] == charactersToSplitBy[j])
					{
						found = true;
						break;
					}
				}
				if(found)
				{
					splittedStrings.Add(string.Substring(lastSplitIndex, (i - lastSplitIndex)));
					for(MSize i = 0; i < splittedStrings.GetSize(); ++i)
					{
						const char* tmp = splittedStrings[i];
						int j = 0;
						j = 1;
					}
					lastSplitIndex = i + 1;
				}
			}

			if(lastSplitIndex < length)
			{
				splittedStrings.Add(string.Substring(lastSplitIndex, length - lastSplitIndex));
			}
		}

		void MString::SplitNCS(const MString& string, const MArray<char>& charactersToSplitBy, MArray<MString>& splittedStrings)
		{
			MSize length = string.Length();
			MSize charsLength = charactersToSplitBy.GetSize();
			MSize lastSplitIndex = 0;

			for(MSize i = 0; i < length; ++i)
			{
				bool found = false;
				for(MSize j = 0; j < charsLength; ++j)
				{
					if(std::tolower(string[i]) == std::tolower(charactersToSplitBy[j]))
					{
						found = true;
						break;
					}
				}
				if(found)
				{
					splittedStrings.Add(string.Substring(lastSplitIndex, (i - lastSplitIndex)));
					lastSplitIndex = i + 1;
				}
			}

			if(lastSplitIndex < length)
			{
				splittedStrings.Add(string.Substring(lastSplitIndex, length - lastSplitIndex));
			}
		}

		void MString::ReplaceReallocate(const MString& stringToReplace, const MString& stringToInsert)
		{
			MSize length = Length();
			MSize stringToReplaceLength = stringToReplace.Length();
			if(length < stringToReplaceLength)
			{
				return;
			}
			MSize stringToInsertLength = stringToInsert.Length();

			MArray<char> newString;
			for(MSize i = 0; i < length; ++i)
			{
				if(_text[i] == stringToReplace[0])
				{
					bool found = true;
					for(MSize j = i + 1; j < (i + stringToReplaceLength); ++j)
					{
						if(_text[j] != stringToReplace[j - i])
						{
							found = false;
							break;
						}
					}

					if(found)
					{
						for(MSize k = 0; k < stringToInsertLength; ++k)
						{
							newString.Add(stringToInsert[k]);
						}
						i += stringToReplaceLength - 1;
					}
					else
					{
						newString.Add(_text[i]);
					}
				}
				else
				{
					newString.Add(_text[i]);
				}
			}
			newString.Add('\0');

			_text = newString;
		}

		void MString::ReplaceReallocateNCS(const MString& stringToReplace, const MString& stringToInsert)
		{
			MSize length = Length();
			MSize stringToReplaceLength = stringToReplace.Length();
			if(length < stringToReplaceLength)
			{
				return;
			}
			MSize stringToInsertLength = stringToInsert.Length();

			MArray<char> newString;
			for(MSize i = 0; i < length; ++i)
			{
				if(std::tolower(_text[i]) == std::tolower(stringToReplace[0]))
				{
					bool found = true;
					for(MSize j = i + 1; j < (i + stringToReplaceLength); ++j)
					{
						if(std::tolower(_text[j]) != std::tolower(stringToReplace[j - i]))
						{
							found = false;
							break;
						}
					}

					if(found)
					{
						for(MSize k = 0; k < stringToInsertLength; ++k)
						{
							newString.Add(stringToInsert[k]);
						}
						i += stringToReplaceLength - 1;
					}
					else
					{
						newString.Add(_text[i]);
					}
				}
				else
				{
					newString.Add(_text[i]);
				}
			}
			newString.Add('\0');

			_text = newString;
		}
	}
}