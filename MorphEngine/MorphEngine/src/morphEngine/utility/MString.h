#pragma once

#include "../core/GlobalDefines.h"
#include "MArray.h"

#include <cctype>

namespace morphEngine
{
	namespace utility
	{
		class MVector2;
		class MVector3;
		class MVector4;
		class MMatrix;

		struct MSID
		{
			friend class MString;
		public:
			const static MSID WRONG_SID;

		protected:
			static const MInt32 _a;
			static const MInt32 _b;
			static const MInt32 _c;
			static const MInt32 _first;

			MInt32 _value;

			void Generate(const MArray<char>& string);
			void Generate(const char* string);

		public:
			inline MSID() : _value(-1) {}
			inline MSID(const MArray<char>& string) : _value(0)
			{
				Generate(string);
			}
			inline MSID(const char* string) : _value(0)
			{
				Generate(string);
			}
			inline MSID(MInt32 value) : _value(value) { }
			inline MSID(const MSID& other) : _value(other._value)
			{}

#pragma region Operators

			inline bool operator==(const MSID& other) const
			{
				return _value == other._value;
			}

			inline bool operator!=(const MSID& other) const
			{
				return _value != other._value;
			}

			inline bool operator>(const MSID& other) const
			{
				return _value > other._value;
			}

			inline bool operator>=(const MSID& other) const
			{
				return _value >= other._value;
			}

			inline bool operator<(const MSID& other) const
			{
				return _value < other._value;
			}

			inline bool operator<=(const MSID& other) const
			{
				return _value <= other._value;
			}

#pragma endregion
		};

		class MString
		{
		protected:
			MSID _sid;
			MArray<char> _text;

		public:
			inline MString()
			{
				_text.Add('\0');
				_sid.Generate(_text);
			}
			inline MString(const char* text)
			{
				while(*text != '\0')
				{
					_text.Add(*text);
					++text;
				}
				_text.Add('\0');
				_sid.Generate(_text);
			}
			inline MString(const MArray<char>& text) : _text(text)
			{
				if((*_text.GetEnd()) != '\0')
				{
					_text.Add('\0');
				}
				_sid.Generate(_text);
			}
			inline MString(const MString& text) : _text(text._text), _sid(text._sid)
			{
			}

			inline MString(const MSID& msid) :
				_sid(msid)
			{
				_text.Add('\0');
			}

			inline ~MString()
			{
				Shutdown();
			}

			/// <summary>
			/// Clears string data to allow static destruction of object.
			/// </summary>
			inline void Shutdown()
			{
				_text.Shutdown();
			}

			inline MSID GetSID() const { return _sid; }

			//Returns substring starting from given index and with given length
			MString Substring(MSize startIndex = 0, MSize length = UINT_MAX) const;

			//Returns last index of given character (-1 if character not found)
			MInt32 FindLast(char character) const;
			//Returns last index of given character (-1 if character not found)
			//Note: This function is not case sensitive
			MInt32 FindLastNCS(char character) const;
			//Returns last index of given character (-1 if character not found)
			MInt32 FindLast(const MString& string) const;
			//Returns last index of given character (-1 if character not found)
			//Note: This function is not case sensitive
			MInt32 FindLastNCS(const MString& string) const;
			//Returns first index of given character (-1 if character not found)
			MInt32 FindFirst(char character) const;
			//Returns first index of given character (-1 if character not found)
			//Note: This function is not case sensitive
			MInt32 FindFirstNCS(char character) const;
			//Returns first index of given character (-1 if character not found)
			MInt32 FindFirst(const MString& string) const;
			//Returns first index of given character (-1 if character not found)
			//Note: This function is not case sensitive
			MInt32 FindFirstNCS(const MString& string) const;
			
			//Replaces every occurance of given character with given character (:V)
			void Replace(char charToReplace, char charToInsert);
			//Replaces every occurance of given character with given character (:V)
			//Note: This function IS NOT case sensitive
			void ReplaceNCS(char charToReplace, char charToInsert);
			//Replaces every occurance of given string with given string (:V)
			void Replace(const MString& stringToReplace, const MString& stringToInsert);
			//Replaces every occurance of given string with given string (:V)
			//Note: This function IS NOT case sensitive
			void ReplaceNCS(const MString& stringToReplace, const MString& stringToInsert);

			//Compares alphabetically string with other string
			//Returns -1 if this string is before the other, 1 if the other is before this and 0 if strings are equal
			MInt8 CompareAlphabetically(const MString& other) const;

			inline const char* Data() const
			{
				return (const char*)_text._data;
			}

			//Returns length (number of characters)
			inline MSize Length() const
			{
				return _text.GetSize() - 1;
			}

			//Returns true if string is empty (length equals 0)
			inline bool IsEmpty() const
			{
				return Length() == 0;
			}

			//Returns true if string starts with given string
			inline bool StartsWith(const MString& starting) const
			{
				MSize length = Length();
				MSize startingLength = starting.Length();
				if(length < startingLength)
				{
					return false;
				}

				for(MSize i = 0; i < startingLength; ++i)
				{
					if(_text[i] != starting[i])
					{
						return false;
					}
				}

				return true;
			}

			//Returns true if string starts with given string
			//Note: This function IS NOT case sensitive
			inline bool StartsWithNCS(const MString& starting) const
			{
				MSize length = Length();
				MSize startingLength = starting.Length();
				if(length < startingLength)
				{
					return false;
				}

				for(MSize i = 0; i < startingLength; ++i)
				{
					if(std::tolower(_text[i]) != std::tolower(starting[i]))
					{
						return false;
					}
				}

				return true;
			}

			//Returns true if string ends with given string
			inline bool EndsWith(const MString& ending) const
			{
				MSize length = Length();
				MSize endingLength = ending.Length();
				if(length < endingLength)
				{
					return false;
				}

				for(MSize i = length - 1; i >= length - endingLength; --i)
				{
					if(_text[i] != ending[endingLength - (length - i)])
					{
						return false;
					}
				}

				return true;
			}

			//Returns true if string ends with given string
			//Note: This function IS NOT case sensitive
			inline bool EndsWithNCS(const MString& ending) const
			{
				MSize length = Length();
				MSize endingLength = ending.Length();
				if(length < endingLength)
				{
					return false;
				}

				for(MSize i = length - 1; i >= length - endingLength; --i)
				{
					if(std::tolower(_text[i]) != std::tolower(ending[endingLength - (length - i)]))
					{
						return false;
					}
				}

				return true;
			}

			inline void ToLower()
			{
				MSize length = Length();
				for(MSize i = 0; i < length; ++i)
				{
					_text[i] = std::tolower(_text[i]);
				}
				_sid.Generate(_text);
			}

			inline void ToUpper()
			{
				MSize length = Length();
				for(MSize i = 0; i < length; ++i)
				{
					_text[i] = std::toupper(_text[i]);
				}
				_sid.Generate(_text);
			}

			static MString FromBoolean(bool value);
			static MString FromInt(MInt8 value);
			static MString FromInt(MInt16 value);
			static MString FromInt(MInt32 value);
			static MString FromInt(MInt64 value);
			static MString FromInt(MUint8 value);
			static MString FromInt(MUint16 value);
			static MString FromInt(MUint32 value);
			static MString FromInt(MUint64 value);
			static MString FromFloat(MFloat32 value);
			static MString FromVector2(const MVector2& value);
			static MString FromVector3(const MVector3& value);
			static MString FromVector4(const MVector4& value);
			static MString FromMatrix(const MMatrix& value);
			static MString FromDouble(MDouble64 value);
			static MString FromDouble(MLongDouble64 value);
			static bool ToBoolean(const MString& string);
			static MInt8 ToInt8(const MString& string);
			static MInt16 ToInt16(const MString& string);
			static MInt32 ToInt32(const MString& string);
			static MInt64 ToInt64(const MString& string);
			static MUint8 ToUint8(const MString& string);
			static MUint16 ToUint16(const MString& string);
			static MUint32 ToUint32(const MString& string);
			static MUint64 ToUint64(const MString& string);
			static MFloat32 ToFloat32(const MString& string);
			static MDouble64 ToDouble64(const MString& string);
			static MLongDouble64 ToLongDouble64(const MString& string);
			
			//Replaces every occurance of given character in given string with given character (:V)
			static MString Replace(const MString& string, char charToReplace, char charToInsert);
			//Replaces every occurance of given character in given string with given character (:V)
			//Note: This function IS NOT case sensitive
			static MString ReplaceNCS(const MString& string, char charToReplace, char charToInsert);
			//Replaces every occurance of given string in given string with given string (:V)
			static MString Replace(const MString& string, const MString& stringToReplace, const MString& stringToInsert);
			//Replaces every occurance of given string in given string with given string (:V)
			//Note: This function IS NOT case sensitive
			static MString ReplaceNCS(const MString& string, const MString& stringToReplace, const MString& stringToInsert);

			//Splits string by given characters and places splitted strings in given array
			static void Split(const MString& string, const MArray<char>& charactersToSplitBy, MArray<MString>& splittedStrings);
			//Splits string by given characters and places splitted strings in given array
			//Note: This function IS NOT case sensitive
			static void SplitNCS(const MString& string, const MArray<char>& charactersToSplitBy, MArray<MString>& splittedStrings);

			//Trims given character from the end of given string (until the last character is string is equal to given character)
			static inline MString TrimFromEnd(const MString& string, char character)
			{
				MSize length = string.Length();
				bool done = false;
				while(!done && length > 0)
				{
					if(string[length - 1] == character)
					{
						length -= 1;
					}
					else
					{
						done = true;
					}
				}
				return string.Substring(0, length);
			}

			//Trims given character from the beginning of given string (until the first character is string is equal to given character)
			static inline MString TrimFromStart(const MString& string, char character)
			{
				MSize length = string.Length();
				MSize start = 0;
				bool done = false;
				while(!done && start < length)
				{
					if(string[start] == character)
					{
						start += 1;
					}
					else
					{
						done = true;
					}
				}
				return string.Substring(start, length - start);
			}

#pragma region Operators
			
			inline MString& operator= (const MString& other)
			{
				_text = other._text;
				_sid = other._sid;
				return *this;
			}

			inline bool operator==(const char* otherString) const
			{
				return _sid == MSID(otherString);
			}

			inline bool operator==(const MString& otherString) const
			{
				return _sid == otherString._sid;
			}

			inline bool operator!=(const char* otherString) const
			{
				return _sid != MSID(otherString);
			}

			inline bool operator!=(const MString& otherString) const
			{
				return _sid != otherString._sid;
			}

			inline bool operator>(const MString& otherString) const
			{
				return _sid > otherString._sid;
			}

			inline bool operator>=(const MString& otherString) const
			{
				return _sid >= otherString._sid;
			}

			inline bool operator<(const MString& otherString) const
			{
				return _sid < otherString._sid;
			}

			inline bool operator<=(const MString& otherString) const
			{
				return _sid <= otherString._sid;
			}

			inline void operator+=(const char* text)
			{
				_text.RemoveAt(_text.GetSize() - 1);
				MSize length = (MSize)std::strlen(text);
				for(MSize i = 0; i < length; ++i)
				{
					_text.Add(text[i]);
				}
				_text.Add('\0');
				_sid.Generate(_text);
			}

			inline void operator+=(const MString& other)
			{
				_text.RemoveAt(_text.GetSize() - 1);
				MSize length = (MSize)other.Length();
				for(MSize i = 0; i < length; ++i)
				{
					_text.Add(other[i]);
				}
				_text.Add('\0');
				_sid.Generate(_text);
			}

			inline void operator+=(const char character)
			{
				_text.RemoveAt(_text.GetSize() - 1);
				_text.Add(character);
				_text.Add('\0');
				_sid.Generate(_text);
			}

			inline MString operator+(const char* text) const
			{
				MArray<char> charArray(_text);
				charArray.RemoveAt(charArray.GetSize() - 1);
				MSize length = (MSize)std::strlen(text);
				for(MSize i = 0; i < length; ++i)
				{
					charArray.Add(text[i]);
				}
				MString toRet(charArray);
				return toRet;
			}

			inline MString operator+(const MString& other) const
			{
				MArray<char> charArray(_text);
				charArray.RemoveAt(charArray.GetSize() - 1);
				MSize length = other.Length();
				for(MSize i = 0; i < length; ++i)
				{
					charArray.Add(other[i]);
				}
				MString toRet(charArray);
				return toRet;
			}

			inline MString operator+(const char character) const
			{
				MArray<char> charArray(_text);
				charArray.RemoveAt(charArray.GetSize() - 1);
				charArray.Add(character);
				MString toRet(charArray);
				return toRet;
			}

			inline char operator[](const MSize index) const
			{
				return _text[index];
			}

			inline char& operator[](const MSize index)
			{
				return _text[index];
			}

			inline const char* operator*() const
			{
				return (const char*)_text._data;
			}

			inline operator const char*() const
			{
				return (const char*)_text._data;
			}

			inline friend MString operator+(const char* text, const MString& string);

#pragma endregion

		protected:	
			//Replaces given string in this string with given string (:V) and reallocates memory
			void ReplaceReallocate(const MString& stringToReplace, const MString& stringToInsert);
			//Replaces given string in this string with given string (:V) and reallocates memory
			//Note: This function IS NOT case sensitive
			void ReplaceReallocateNCS(const MString& stringToReplace, const MString& stringToInsert);
		};

		inline MString operator+(const char* text, const MString& string)
		{
			MArray<char> charArray;
			MSize length = (MSize)std::strlen(text);
			for(MSize i = 0; i < length; ++i)
			{
				char tmp = text[i];
				charArray.Add(text[i]);
			}
			length = string.Length();
			for(MSize i = 0; i < length; ++i)
			{
				char tmp = string[i];
				charArray.Add(string[i]);
			}
			MString toRet(charArray);
			return toRet;
		}
	}
}