#pragma once

#include "utility\MDictionary.h"
#include "utility\MString.h"
#include "core\GlobalDefines.h"

#include <string>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#define strtok_r strtok_s
#endif

#define VALID_DUMMY 123456

template <typename T>
struct _eat
{
	T _value;

	template <typename Any>
	_eat& operator =(Any value)
	{
		return *this;
	} 

	explicit _eat(T value) : _value(value)
	{

	} 
	operator T() const
	{
		return _value;
	}
};

struct MEnumInfo
{
public:
	std::vector<std::string> Names;
	std::vector<int> Values;

public:
	inline MEnumInfo() { }
	inline MEnumInfo(const MEnumInfo& other) : Names(other.Names), Values(other.Values) { }
};

void smartEnumParse(MEnumInfo& info, const char* enumValList);

class MEnumBase
{
public:
	const int DUMMY = VALID_DUMMY;
	virtual bool FromString(const char* x) = 0;
	virtual std::string ToString() const = 0;
	virtual const std::vector<std::string>& GetNames() const = 0;
	virtual const std::vector<int>& GetValues() const = 0;
	virtual MSize GetSizeOf() const = 0;
	virtual void* GetValuePtr() const = 0;
};

//Declares better enum. The name of this enum will be name, then will be the base and after base come values (example: DECLARE_MENUM(Weekdays, Monday, Tuesday, Wednesday, ...)
#define DECLARE_MENUM(name, ...) \
class name : public MEnumBase { \
public: \
	enum Value { __VA_ARGS__ } _value; \
	\
	static MEnumInfo _info; \
public: \
	inline name() : _value((Value)0) { } \
	inline name(Value v) : _value(v) { } \
	inline name(int v) : _value((Value)v) { } \
	explicit inline name(const char* x) : _value((Value)0) { FromString(x); } \
	virtual std::string ToString() const \
	{ \
		return std::string(ToString(_value)); \
	} \
	virtual bool FromString(const char* x) \
	{ \
		if(_info.Names.size() == 0) \
		{ \
			smartEnumParse(_info, #__VA_ARGS__); \
		} \
		size_t i = 0; \
		\
		while(i < _info.Names.size() && i < _info.Values.size())\
		{\
			if(std::strcmp(x, _info.Names[i].c_str()) == 0) \
			{ \
				_value = (Value)_info.Values[i]; \
				return true; \
			} \
			++i; \
		} \
		return false; \
	} \
	inline bool HasDefined(name::Value other) const\
	{\
		return (_value & other) != 0; \
	}\
	static const char* ClassName() {\
		return #name; \
	}\
	\
	static const char* ToString(const Value& v) \
	{ \
		if(_info.Names.size() == 0) \
		{ \
			smartEnumParse(_info, #__VA_ARGS__); \
		} \
		for(size_t i = 0; i < _info.Names.size() && i < _info.Values.size(); ++i) \
		{ \
			if(_info.Values[i] == v) \
			{\
				return _info.Names[i].c_str(); \
			}\
		} \
		return NULL; \
	}\
	virtual MSize GetSizeOf() const \
	{\
		return sizeof(Value); \
	}\
	virtual void* GetValuePtr() const \
	{ \
		return (void*)(&_value); \
	} \
	virtual const std::vector<std::string>& GetNames() const \
	{\
		if(_info.Names.size() == 0) \
		{ \
			smartEnumParse(_info, #__VA_ARGS__); \
		} \
		return _info.Names; \
	}\
	virtual const std::vector<int>& GetValues() const \
	{ \
		if(_info.Values.size() == 0) \
		{ \
			smartEnumParse(_info, #__VA_ARGS__); \
		} \
		return _info.Values; \
	} \
	static const std::vector<std::string>& GetNamesStatic() \
	{\
		if(_info.Names.size() == 0) \
		{ \
			smartEnumParse(_info, #__VA_ARGS__); \
		} \
		return _info.Names; \
	}\
	static const std::vector<int>& GetValuesStatic() \
	{\
		if(_info.Values.size() == 0) \
		{ \
			smartEnumParse(_info, #__VA_ARGS__); \
		} \
		return _info.Values; \
	}\
	inline operator int() const \
	{\
		return (int)_value; \
	} \
	inline name& operator=(const name& other) \
	{ \
		_value = other._value; \
		return *this; \
	}\
	inline name& operator=(int other) \
	{ \
		_value = (Value)other; \
		return *this; \
	}\
	inline bool operator==(const name& other) const \
	{\
		return _value == other._value; \
	}\
	inline bool operator==(name::Value other) const \
	{\
		return _value == other; \
	}\
	inline bool operator==(int value) const \
	{\
		return (int)_value == value; \
	}\
	inline bool operator!=(const name& other) const \
	{\
		return _value != other._value; \
	}\
	inline bool operator!=(name::Value other) const \
	{\
		return _value != other; \
	}\
	inline bool operator!=(int value) const \
	{\
		return (int)_value != value; \
	}\
	inline bool operator<(const name& other) const \
	{\
		return _value < other._value; \
	}\
	inline bool operator<(name::Value other) const \
	{\
		return _value < other; \
	}\
	inline bool operator<(int value) const \
	{\
		return (int)_value < value; \
	}\
	inline bool operator<=(const name& other) const \
	{\
		return _value <= other._value; \
	}\
	inline bool operator<=(name::Value other) const \
	{\
		return _value <= other; \
	}\
	inline bool operator<=(int value) const \
	{\
		return (int)_value <= value; \
	}\
	inline bool operator>(const name& other) const \
	{\
		return _value > other._value; \
	}\
	inline bool operator>(name::Value other) const \
	{\
		return _value > other; \
	}\
	inline bool operator>(int value) const \
	{\
		return (int)_value > value; \
	}\
	inline bool operator>=(const name& other) const \
	{\
		return _value >= other._value; \
	}\
	inline bool operator>=(name::Value other) const \
	{\
		return _value >= other; \
	}\
	inline bool operator>=(int value) const \
	{\
		return (int)_value >= value; \
	}\
};

#define DEFINE_MENUM(name) \
MEnumInfo name::_info;