#pragma once

#include "core/GlobalDefines.h"
#include "utility/Collections.h"

using namespace morphEngine::utility;

#include <fbxsdk.h>

#include <vector>
#include <string>
#include <streambuf>
#include <istream>

class FbxMemoryStream : public FbxStream
{
private:
	MFixedArray<MUint8>& _data;
	long _position;
	EState _state;
	int _errorCode;
	int _readerId;

public:
	inline FbxMemoryStream(MFixedArray<MUint8>& data, int readerId)
		: _data(data), _readerId(readerId),
		_position(0), _state(EState::eClosed)
	{

	}

	// Inherited via FbxStream
	virtual inline EState GetState() override
	{
		return _state;
	}

	virtual inline bool Open(void* pStreamData) override
	{
		_state = EState::eOpen;
		_position = 0;
		return true;
	}

	virtual inline bool Close() override
	{
		_state = EState::eClosed;
		_position = 0;
		return true;
	}

	virtual inline bool Flush() override
	{
		_position = 0;
		return true;
	}

	virtual inline int Write(const void *, int) override
	{
		_errorCode = 1;
		return 1;
	}

	virtual inline int Read(void * buffer, int count) const override
	{
		long remain = static_cast<long>(_data.GetSize()) - _position;
		long finalCount = MMath::Min(count, remain);
		if (finalCount > 0)
		{
			memcpy(buffer, _data.GetDataPointer() + _position, finalCount);
		}
		else
		{
			return 1;
		}
		const_cast<FbxMemoryStream*>(this)->_position += finalCount;
		return finalCount;
	}

	virtual inline int GetReaderID() const override
	{
		return _readerId;
	}

	virtual inline int GetWriterID() const override
	{
		return 1;
	}

	virtual inline void Seek(const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos) override
	{
		long offset = static_cast<long>(pOffset);
		switch (pSeekPos)
		{
		case FbxFile::ESeekPos::eCurrent:
			_position += offset;
			break;
		case FbxFile::ESeekPos::eEnd:
			_position = static_cast<long>(_data.GetSize()) - offset;
			break;
		default:
			_position = offset;
			break;
		}
	}

	virtual inline long GetPosition() const override
	{
		return _position;
	}

	virtual inline void SetPosition(long pPosition) override
	{
		_position = pPosition;
	}

	virtual inline int GetError() const override
	{
		return _errorCode;
	}

	virtual inline void ClearError() override
	{
		_errorCode = 0;
	}
};

inline FbxScene* LoadSceneFromBuffer(FbxManager* manager, MFixedArray<MUint8>& bytes)
{
	FbxMemoryStream* stream = new FbxMemoryStream(bytes, 1);
	FbxImporter* importer = FbxImporter::Create(manager, "");
	bool success = importer->Initialize(stream, nullptr, 0, manager->GetIOSettings());
	if (!success) { return false; }
	FbxScene* scene = FbxScene::Create(manager, "A");
	success = importer->Import(scene);
	if (!success) { return false; }
	importer->Destroy(true);
	stream->Close();
	delete stream;

	FbxSystemUnit::m.ConvertScene(scene);
	FbxAxisSystem::OpenGL.ConvertScene(scene);

	return scene;
}

inline MVector3 FbxVec4ToVector3(const FbxVector4& db)
{
	return MVector3(static_cast<MFloat32>(db[0]), static_cast<MFloat32>(db[1]), static_cast<MFloat32>(db[2]));
}

inline MVector2 FbxVec2ToVector2(const FbxVector2& db)
{
	return MVector2(static_cast<MFloat32>(db[0]), static_cast<MFloat32>(db[1]));
}

inline MMatrix FbxMatToMatrix(const FbxAMatrix& inMat)
{
	MMatrix ret;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ret[i][j] = static_cast<MFloat32>(inMat[j][i]);
		}
	}
	return ret;
}

inline void PrintFbxMatrix(const morphEngine::utility::MString& name, const FbxAMatrix& inMat)
{
	MString overall = "===================\n" + name + "\n----------------\n";
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			overall += morphEngine::utility::MString::FromFloat(static_cast<MFloat32>(inMat[j][i])) + " ";
		}
		overall += "\n";
	}
	overall += "======================\n";
	morphEngine::debugging::Debug::Log(morphEngine::debugging::ELogType::LOG, overall);
}

