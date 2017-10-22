#include "MEnum.h"

#include "MString.h"

using namespace morphEngine::utility;

#define IS_NUMBER(character) (character >= '0' && character <= '9')
#define IS_LETTER(character) ((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z'))

void trySplitString(const MString& string, char splitChar, MArray<MString>& splitted)
{
	MArray<char> charSplit;
	charSplit.Add('|');
	MString::Split(string, charSplit, splitted);
	MSize size = splitted.GetSize();
	for(MSize i = 0; i < size; ++i)
	{
		splitted[i] = MString::TrimFromEnd(splitted[i], ' ');
		splitted[i] = MString::TrimFromStart(splitted[i], ' ');
	}
}

int tryGetValue(const std::vector<std::string>& names, const std::vector<int>& values, const MString& name)
{
	size_t namesSize = names.size();
	size_t valuesSize = values.size();
	if(namesSize != valuesSize)
	{
		return -1;
	}

	for(size_t i = 0; i < namesSize; ++i)
	{
		const char* t = names[i].c_str();
		const char* x = *name;
		if(MString(names[i].c_str()) == name)
		{
			return values[i];
		}
	}

	return -1;
}

typedef int(*OperatorFunctionPtr)(int, int);

int calcValue(const MArray<MString>& knownNames, const std::vector<std::string>& names, const std::vector<int>& values, OperatorFunctionPtr function)
{
	if(function == nullptr)
	{
		return -1;
	}

	int value = -1;
	MSize knownNamesSize = knownNames.GetSize();
	
	for(MSize i = 0; i < knownNamesSize; ++i)
	{
		int val = tryGetValue(names, values, knownNames[i]);
		if(value < 0)
		{
			value = val;
		}
		else if(val >= 0)
		{
			value = function(value, val);
		}
	}

	return value;
}

int operatorBitOr(int a, int b)
{
	return a | b;
}

int operatorBitAnd(int a, int b)
{
	return a & b;
}

void smartEnumParse(MEnumInfo& info, const char* enumValList)
{
	MString string(enumValList);
	MArray<MString> splitted;
	MArray<char> characters;
	characters.Add(',');
	MString::SplitNCS(string, characters, splitted);
	MSize size = splitted.GetSize();
	MArray<char> charactersInSplitted;
	charactersInSplitted.Add('=');
	for(MSize i = 0; i < size; ++i)
	{
		bool valueFound = false;
		MArray<MString> nameValue;
		MString::Split(splitted[i], charactersInSplitted, nameValue);
		MSize nameValueSize = nameValue.GetSize();
		MString name = MString::TrimFromStart(nameValue[0], ' ');
		name = MString::TrimFromEnd(name, ' ');
		MInt32 value;
		if(nameValueSize >= 2)
		{
			MString valueString = MString::TrimFromStart(nameValue[1], ' ');
			valueString = MString::TrimFromEnd(valueString, ' ');
			if(valueString.IsEmpty())
			{
				continue;
			}

			if(IS_NUMBER(valueString[0]))
			{
				try
				{
					value = (MInt32)std::strtol(*valueString, NULL, 0);
					valueFound = true;
				}
				catch(std::exception e)
				{
					valueFound = false;
				}
			}
			else
			{
				MArray<MString> operatorSplitted;
				trySplitString(valueString, '|', operatorSplitted);
				if(operatorSplitted.GetSize() > 1)
				{
					value = calcValue(operatorSplitted, info.Names, info.Values, operatorBitOr);
				}
				else
				{
					operatorSplitted.Clear();
					trySplitString(valueString, '&', operatorSplitted);
					if(operatorSplitted.GetSize() > 1)
					{
						value = calcValue(operatorSplitted, info.Names, info.Values, operatorBitAnd);
					}
					else
					{
						value = tryGetValue(info.Names, info.Values, valueString);
					}
				}
				operatorSplitted.Clear();

				if(value >= 0)
				{
					valueFound = true;
				}
			}
		}
		else if(nameValueSize > 0)
		{
			size_t size = info.Values.size();
			if(size == 0)
			{
				value = 0;
			}
			else
			{
				value = info.Values[size - 1] + 1;
			}
			valueFound = true;
		}

		if(valueFound)
		{
			info.Names.push_back(*name);
			info.Values.push_back(value);
		}
	}
}
