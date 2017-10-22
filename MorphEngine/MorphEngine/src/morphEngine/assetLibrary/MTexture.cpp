#include "MTexture.h"
#include <IL/il.h>
#include <IL/ilu.h>

#include "core/Engine.h"
#include "resourceManagement\fileSystem\FileSystem.h"

// TGA RELATED

/*
typedef struct
{
byte  identsize;         0 // size of ID field that follows 18 byte header (0 usually)
byte  colourmaptype;     1 // type of colour map 0=none, 1=has palette
byte  imagetype;         2 // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

short colourmapstart;    3 // first colour map entry in palette
short colourmaplength;   5 // number of colours in palette
byte  colourmapbits;     7 // number of bits per palette entry 15,16,24,32

short xstart;            8 // image x origin
short ystart;            10 // image y origin
short width;             12 // image width in pixels
short height;            14 // image height in pixels
byte  bits;              16 // image bits per pixel 8,16,24,32
byte  descriptor;        17 // image descriptor bits (vh flip bits)
18 // image data
// pixel data follows header

} TGA_HEADER
*/

// ////////////////////////////////////////////////////////////

// DDS RELATED

const uint32_t DDS_MAGIC = 0x20534444; // "DDS "

struct DdsPixelformat
{
	MUint32    size;
	MUint32    flags;
	MUint32    fourCC;
	MUint32    RGBBitCount;
	MUint32    RBitMask;
	MUint32    GBitMask;
	MUint32    BBitMask;
	MUint32    ABitMask;
};

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA
#define DDS_BUMPDUDV    0x00080000  // DDPF_BUMPDUDV

#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH

#define DDS_HEIGHT 0x00000002 // DDSD_HEIGHT
#define DDS_WIDTH  0x00000004 // DDSD_WIDTH

#define ISBITMASK( r,g,b,a ) ( ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a )

enum DdsMiscFlags2
{
	DDS_MISC_FLAGS2_ALPHA_MODE_MASK = 0x7L,
};

struct DdsHeader
{
	uint32_t        size;
	uint32_t        flags;
	uint32_t        height;
	uint32_t        width;
	uint32_t        pitchOrLinearSize;
	uint32_t        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
	uint32_t        mipMapCount;
	uint32_t        reserved1[11];
	DdsPixelformat	ddspf;
	uint32_t        caps;
	uint32_t        caps2;
	uint32_t        caps3;
	uint32_t        caps4;
	uint32_t        reserved2;
};

struct DdsHeaderDx10
{
	uint32_t		dxgiFormat;
	uint32_t        resourceDimension;
	uint32_t        miscFlag; // see D3D11_RESOURCE_MISC_FLAG
	uint32_t        arraySize;
	uint32_t        miscFlags2;
};

#define DXGI_FORMAT_DEFINED 1

typedef enum DXGI_FORMAT
{
	DXGI_FORMAT_UNKNOWN = 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	DXGI_FORMAT_R32G32B32A32_UINT = 3,
	DXGI_FORMAT_R32G32B32A32_SINT = 4,
	DXGI_FORMAT_R32G32B32_TYPELESS = 5,
	DXGI_FORMAT_R32G32B32_FLOAT = 6,
	DXGI_FORMAT_R32G32B32_UINT = 7,
	DXGI_FORMAT_R32G32B32_SINT = 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
	DXGI_FORMAT_R16G16B16A16_UNORM = 11,
	DXGI_FORMAT_R16G16B16A16_UINT = 12,
	DXGI_FORMAT_R16G16B16A16_SNORM = 13,
	DXGI_FORMAT_R16G16B16A16_SINT = 14,
	DXGI_FORMAT_R32G32_TYPELESS = 15,
	DXGI_FORMAT_R32G32_FLOAT = 16,
	DXGI_FORMAT_R32G32_UINT = 17,
	DXGI_FORMAT_R32G32_SINT = 18,
	DXGI_FORMAT_R32G8X24_TYPELESS = 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
	DXGI_FORMAT_R10G10B10A2_UNORM = 24,
	DXGI_FORMAT_R10G10B10A2_UINT = 25,
	DXGI_FORMAT_R11G11B10_FLOAT = 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
	DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	DXGI_FORMAT_R8G8B8A8_UINT = 30,
	DXGI_FORMAT_R8G8B8A8_SNORM = 31,
	DXGI_FORMAT_R8G8B8A8_SINT = 32,
	DXGI_FORMAT_R16G16_TYPELESS = 33,
	DXGI_FORMAT_R16G16_FLOAT = 34,
	DXGI_FORMAT_R16G16_UNORM = 35,
	DXGI_FORMAT_R16G16_UINT = 36,
	DXGI_FORMAT_R16G16_SNORM = 37,
	DXGI_FORMAT_R16G16_SINT = 38,
	DXGI_FORMAT_R32_TYPELESS = 39,
	DXGI_FORMAT_D32_FLOAT = 40,
	DXGI_FORMAT_R32_FLOAT = 41,
	DXGI_FORMAT_R32_UINT = 42,
	DXGI_FORMAT_R32_SINT = 43,
	DXGI_FORMAT_R24G8_TYPELESS = 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
	DXGI_FORMAT_R8G8_TYPELESS = 48,
	DXGI_FORMAT_R8G8_UNORM = 49,
	DXGI_FORMAT_R8G8_UINT = 50,
	DXGI_FORMAT_R8G8_SNORM = 51,
	DXGI_FORMAT_R8G8_SINT = 52,
	DXGI_FORMAT_R16_TYPELESS = 53,
	DXGI_FORMAT_R16_FLOAT = 54,
	DXGI_FORMAT_D16_UNORM = 55,
	DXGI_FORMAT_R16_UNORM = 56,
	DXGI_FORMAT_R16_UINT = 57,
	DXGI_FORMAT_R16_SNORM = 58,
	DXGI_FORMAT_R16_SINT = 59,
	DXGI_FORMAT_R8_TYPELESS = 60,
	DXGI_FORMAT_R8_UNORM = 61,
	DXGI_FORMAT_R8_UINT = 62,
	DXGI_FORMAT_R8_SNORM = 63,
	DXGI_FORMAT_R8_SINT = 64,
	DXGI_FORMAT_A8_UNORM = 65,
	DXGI_FORMAT_R1_UNORM = 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
	DXGI_FORMAT_BC1_TYPELESS = 70,
	DXGI_FORMAT_BC1_UNORM = 71,
	DXGI_FORMAT_BC1_UNORM_SRGB = 72,
	DXGI_FORMAT_BC2_TYPELESS = 73,
	DXGI_FORMAT_BC2_UNORM = 74,
	DXGI_FORMAT_BC2_UNORM_SRGB = 75,
	DXGI_FORMAT_BC3_TYPELESS = 76,
	DXGI_FORMAT_BC3_UNORM = 77,
	DXGI_FORMAT_BC3_UNORM_SRGB = 78,
	DXGI_FORMAT_BC4_TYPELESS = 79,
	DXGI_FORMAT_BC4_UNORM = 80,
	DXGI_FORMAT_BC4_SNORM = 81,
	DXGI_FORMAT_BC5_TYPELESS = 82,
	DXGI_FORMAT_BC5_UNORM = 83,
	DXGI_FORMAT_BC5_SNORM = 84,
	DXGI_FORMAT_B5G6R5_UNORM = 85,
	DXGI_FORMAT_B5G5R5A1_UNORM = 86,
	DXGI_FORMAT_B8G8R8A8_UNORM = 87,
	DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	DXGI_FORMAT_BC6H_TYPELESS = 94,
	DXGI_FORMAT_BC6H_UF16 = 95,
	DXGI_FORMAT_BC6H_SF16 = 96,
	DXGI_FORMAT_BC7_TYPELESS = 97,
	DXGI_FORMAT_BC7_UNORM = 98,
	DXGI_FORMAT_BC7_UNORM_SRGB = 99,
	DXGI_FORMAT_AYUV = 100,
	DXGI_FORMAT_Y410 = 101,
	DXGI_FORMAT_Y416 = 102,
	DXGI_FORMAT_NV12 = 103,
	DXGI_FORMAT_P010 = 104,
	DXGI_FORMAT_P016 = 105,
	DXGI_FORMAT_420_OPAQUE = 106,
	DXGI_FORMAT_YUY2 = 107,
	DXGI_FORMAT_Y210 = 108,
	DXGI_FORMAT_Y216 = 109,
	DXGI_FORMAT_NV11 = 110,
	DXGI_FORMAT_AI44 = 111,
	DXGI_FORMAT_IA44 = 112,
	DXGI_FORMAT_P8 = 113,
	DXGI_FORMAT_A8P8 = 114,
	DXGI_FORMAT_B4G4R4A4_UNORM = 115,
	DXGI_FORMAT_FORCE_UINT = 0xffffffff
} DXGI_FORMAT;

DXGI_FORMAT MakeSRGB(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM_SRGB;

	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM_SRGB;

	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM_SRGB;

	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

	case DXGI_FORMAT_BC7_UNORM:
		return DXGI_FORMAT_BC7_UNORM_SRGB;

	default:
		return format;
	}
}

DXGI_FORMAT GetDXGIFormat(const DdsPixelformat& ddpf)
{
	if (ddpf.flags & DDS_RGB)
	{
		// Note that sRGB formats are written using the "DX10" extended header

		switch (ddpf.RGBBitCount)
		{
		case 32:
			if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
			{
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
			{
				return DXGI_FORMAT_B8G8R8A8_UNORM;
			}

			if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
			{
				return DXGI_FORMAT_B8G8R8X8_UNORM;
			}

			// No DXGI format maps to ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) aka D3DFMT_X8B8G8R8

			// Note that many common DDS reader/writers (including D3DX) swap the
			// the RED/BLUE masks for 10:10:10:2 formats. We assume
			// below that the 'backwards' header mask is being used since it is most
			// likely written by D3DX. The more robust solution is to use the 'DX10'
			// header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

			// For 'correct' writers, this should be 0x000003ff,0x000ffc00,0x3ff00000 for RGB data
			if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
			{
				return DXGI_FORMAT_R10G10B10A2_UNORM;
			}

			// No DXGI format maps to ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) aka D3DFMT_A2R10G10B10

			if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
			{
				return DXGI_FORMAT_R16G16_UNORM;
			}

			if (ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
			{
				// Only 32-bit color channel format in D3D9 was R32F
				return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
			}
			break;

		case 24:
			// No 24bpp DXGI formats aka D3DFMT_R8G8B8
			break;

		case 16:
			if (ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
			{
				return DXGI_FORMAT_B5G5R5A1_UNORM;
			}
			if (ISBITMASK(0xf800, 0x07e0, 0x001f, 0x0000))
			{
				return DXGI_FORMAT_B5G6R5_UNORM;
			}

			// No DXGI format maps to ISBITMASK(0x7c00,0x03e0,0x001f,0x0000) aka D3DFMT_X1R5G5B5

			if (ISBITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
			{
				return DXGI_FORMAT_B4G4R4A4_UNORM;
			}

			// No DXGI format maps to ISBITMASK(0x0f00,0x00f0,0x000f,0x0000) aka D3DFMT_X4R4G4B4

			// No 3:3:2, 3:3:2:8, or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_R3G3B2, D3DFMT_P8, D3DFMT_A8P8, etc.
			break;
		}
	}
	else if (ddpf.flags & DDS_LUMINANCE)
	{
		if (8 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
			{
				return DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension
			}

			// No DXGI format maps to ISBITMASK(0x0f,0x00,0x00,0xf0) aka D3DFMT_A4L4

			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
			{
				return DXGI_FORMAT_R8G8_UNORM; // Some DDS writers assume the bitcount should be 8 instead of 16
			}
		}

		if (16 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
			{
				return DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
			}
			if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
			{
				return DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
			}
		}
	}
	else if (ddpf.flags & DDS_ALPHA)
	{
		if (8 == ddpf.RGBBitCount)
		{
			return DXGI_FORMAT_A8_UNORM;
		}
	}
	else if (ddpf.flags & DDS_BUMPDUDV)
	{
		if (16 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x00ff, 0xff00, 0x0000, 0x0000))
			{
				return DXGI_FORMAT_R8G8_SNORM; // D3DX10/11 writes this out as DX10 extension
			}
		}

		if (32 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
			{
				return DXGI_FORMAT_R8G8B8A8_SNORM; // D3DX10/11 writes this out as DX10 extension
			}
			if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
			{
				return DXGI_FORMAT_R16G16_SNORM; // D3DX10/11 writes this out as DX10 extension
			}

			// No DXGI format maps to ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000) aka D3DFMT_A2W10V10U10
		}
	}
	else if (ddpf.flags & DDS_FOURCC)
	{
		if (MAKEFOURCC('D', 'X', 'T', '1') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC1_UNORM;
		}
		if (MAKEFOURCC('D', 'X', 'T', '3') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC2_UNORM;
		}
		if (MAKEFOURCC('D', 'X', 'T', '5') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC3_UNORM;
		}

		// While pre-multiplied alpha isn't directly supported by the DXGI formats,
		// they are basically the same as these BC formats so they can be mapped
		if (MAKEFOURCC('D', 'X', 'T', '2') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC2_UNORM;
		}
		if (MAKEFOURCC('D', 'X', 'T', '4') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC3_UNORM;
		}

		if (MAKEFOURCC('A', 'T', 'I', '1') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC4_UNORM;
		}
		if (MAKEFOURCC('B', 'C', '4', 'U') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC4_UNORM;
		}
		if (MAKEFOURCC('B', 'C', '4', 'S') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC4_SNORM;
		}

		if (MAKEFOURCC('A', 'T', 'I', '2') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC5_UNORM;
		}
		if (MAKEFOURCC('B', 'C', '5', 'U') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC5_UNORM;
		}
		if (MAKEFOURCC('B', 'C', '5', 'S') == ddpf.fourCC)
		{
			return DXGI_FORMAT_BC5_SNORM;
		}

		// BC6H and BC7 are written using the "DX10" extended header

		if (MAKEFOURCC('R', 'G', 'B', 'G') == ddpf.fourCC)
		{
			return DXGI_FORMAT_R8G8_B8G8_UNORM;
		}
		if (MAKEFOURCC('G', 'R', 'G', 'B') == ddpf.fourCC)
		{
			return DXGI_FORMAT_G8R8_G8B8_UNORM;
		}

		if (MAKEFOURCC('Y', 'U', 'Y', '2') == ddpf.fourCC)
		{
			return DXGI_FORMAT_YUY2;
		}

		// Check for D3DFORMAT enums being set here
		switch (ddpf.fourCC)
		{
		case 36: // D3DFMT_A16B16G16R16
			return DXGI_FORMAT_R16G16B16A16_UNORM;

		case 110: // D3DFMT_Q16W16V16U16
			return DXGI_FORMAT_R16G16B16A16_SNORM;

		case 111: // D3DFMT_R16F
			return DXGI_FORMAT_R16_FLOAT;

		case 112: // D3DFMT_G16R16F
			return DXGI_FORMAT_R16G16_FLOAT;

		case 113: // D3DFMT_A16B16G16R16F
			return DXGI_FORMAT_R16G16B16A16_FLOAT;

		case 114: // D3DFMT_R32F
			return DXGI_FORMAT_R32_FLOAT;

		case 115: // D3DFMT_G32R32F
			return DXGI_FORMAT_R32G32_FLOAT;

		case 116: // D3DFMT_A32B32G32R32F
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
	}

	return DXGI_FORMAT_UNKNOWN;
}

// ////////////////////////////////////////////////////////////

namespace morphEngine
{
	using namespace utility;

	namespace assetLibrary
	{
		bool MTexture::LoadMAsset(resourceManagement::fileSystem::File & file)
		{
			MFixedArray<MUint8> data(file.GetSize());
			file.Read(data.GetDataPointer(), 0, file.GetSize());
			return LoadFromData(data);
		}

		bool MTexture::LoadTGA(resourceManagement::fileSystem::File & file)
		{
			MFixedArray<MUint8> tempData;
			// read dimensions
			file.Read(tempData, 12, 2);
			_width = FixedArrayToUInt16(tempData);
			file.Read(tempData, 14, 2);
			_height = FixedArrayToUInt16(tempData);

			// read compression
			file.Read(tempData, 2, 1);
			bool isRLE = tempData[0] == 10;
			if (isRLE)
			{
				return false;
			}

			// read alpha
			file.Read(tempData, 16, 1);
			bool isAlpha = tempData[0] == 32;
			MUint8 channels = isAlpha ? 4 : 3;

			// load data

			// if no alpha - need to manually add it to each pixel
			MSize dataSize = _width * _height * channels;
			MFixedArray<MUint8> storeData;
			storeData.Allocate(dataSize);
			file.Read(storeData, 18, dataSize);

			_rawData.Allocate(_width * _height * 4);
			for (MSize i = 0, w = 0; i < dataSize; i += channels, w += 4)
			{
				MUint8 r = storeData[i];
				MUint8 g = storeData[i + 1];
				MUint8 b = storeData[i + 2];

				_rawData[w] = b;
				_rawData[w + 1] =  g;
				_rawData[w + 2] = r;
				_rawData[w + 3] = 0xFF;
			}

			_bpp = 32;
			_bCompressed = false;
			_mipmapCount = 0;
			_dataFormat = static_cast<renderer::device::GraphicDataFormat>(renderer::device::GraphicDataFormat::UNORM_R8G8B8A8);

			return true;
		}

		bool MTexture::LoadPNG(resourceManagement::fileSystem::File & file)
		{
			MFixedArray<MUint8> data;
			file.Read(data);

			MUint8* texData = nullptr;

			ILint imgID = ilGenImage();
			ilBindImage(imgID);

			//ilSetInteger(IL_IMAGE_CHANNELS, 4);

			ILboolean result = ilLoadL(IL_PNG, data.GetDataPointer(), static_cast<ILuint>(data.GetSize()));

			texData = ilGetData();
			_width = ilGetInteger(IL_IMAGE_WIDTH);
			_height = ilGetInteger(IL_IMAGE_HEIGHT);
			_bpp = ilGetInteger(IL_IMAGE_CHANNELS) * 8;

			if (result != IL_TRUE || texData == nullptr)
			{
				return false;
			}

			MUint8 channels = _bpp / 8;
			MSize size = _width * _height * channels;
			_rawData.Allocate(_width * _height * 4);

			for (MSize x = 0, y = 0; y < _height;)
			{
				MSize i = (_width * (_height - y - 1) + x) * channels;
				MSize w = (_width * y + x) * 4;
				_rawData[w] = texData[i];
				_rawData[w + 1] = texData[i + 1];
				_rawData[w + 2] = texData[i + 2];
				_rawData[w + 3] = channels == 3 ? 0xFF : texData[i + 3];

				if (x >= _width)
				{
					++y;
					x = 0;
				}
				else
				{
					++x;
				}
			}

			_bpp = 32;
			_bCompressed = false;
			_mipmapCount = 0;

			ilDeleteImage(imgID);

			return true;
		}

		bool MTexture::LoadDDS(resourceManagement::fileSystem::File & file)
		{
			// check magic number
			MUint32 magicNumber;
			file.Read(reinterpret_cast<MUint8*>(&magicNumber), 0, sizeof(MUint32));

			if (magicNumber != DDS_MAGIC)
			{
				return false;
			}

			// read header
			DdsHeader header;
			file.Read(reinterpret_cast<MUint8*>(&header), 4, sizeof(DdsHeader));

			// read data from header
			_width = header.width;
			_height = header.height;
			_bpp = (header.ddspf.flags & DDS_ALPHA) ? 24 : 16;
			_mipmapCount = header.mipMapCount;

			// check for DX10 header
			bool isDx10Header = (header.flags & DDS_FOURCC) != 0;
			char text[5] = { static_cast<char>(header.ddspf.fourCC), 
				static_cast<char>(header.ddspf.fourCC >> 8), 
				static_cast<char>(header.ddspf.fourCC >> 16),
				static_cast<char>(header.ddspf.fourCC >> 24), '\0'};
			isDx10Header &= (text == "DX10");

			if (isDx10Header)
			{
				DdsHeaderDx10 header10;
				file.Read(reinterpret_cast<MUint8*>(&header10), 128, sizeof(DdsHeaderDx10));
				_dataFormat = static_cast<renderer::device::GraphicDataFormat>(header10.dxgiFormat);
			}
			else
			{
				_dataFormat = static_cast<renderer::device::GraphicDataFormat>(GetDXGIFormat(header.ddspf));
			}

			// store compressed data into rawdata
			MSize offset = sizeof(MUint32) + sizeof(DdsHeader);
			if (isDx10Header) offset += sizeof(DdsHeaderDx10);
			file.Read(_rawData, offset);
			_bCompressed = true;

			return true;
		}

		bool MTexture::SaveMAsset(resourceManagement::fileSystem::File & file) const
		{
			if (!_bCompressed)
			{
				MFixedArray<MUint8> compressedData;
				renderer::device::GraphicDataFormat fmt;
				MUint8 chnls;
				MUint8 generatedMips;
				CompressRawDataToDDS(_rawData, compressedData, fmt, chnls, generatedMips);
				return SaveMAssetInternal(file, compressedData, fmt, chnls, generatedMips);
			}
			else
			{
				return SaveMAssetInternal(file, _rawData, _dataFormat, _bpp, _mipmapCount);
			}
		}

		bool MTexture::SaveMAssetInternal(resourceManagement::fileSystem::File & file, const MFixedArray<MUint8>& data,
			renderer::device::GraphicDataFormat savedFormat, MUint8 savedBpp, MUint8 savedMips) const
		{
			file.WriteAppend(reinterpret_cast<const MUint8*>(&_type), sizeof(MAssetType));

			InternalHeader hdr;
			hdr._width = _width;
			hdr._height = _height;
			hdr._bpp = savedBpp;
			hdr._dataFormat = savedFormat;
			hdr._samplerFormat = _samplerFormat;
			hdr._mipmapCount = savedMips;
			hdr._bReadOnly = _bReadOnly;
			hdr._totalDataSize = _rawData.GetSize();
			file.WriteAppend(reinterpret_cast<const MUint8*>(&hdr), sizeof(InternalHeader));

			file.WriteAppend(_rawData.GetDataPointer(), _rawData.GetSize());

			return true;
		}

		void MTexture::Shutdown()
		{
			_rawData.Deallocate();
			_width = 0;
			_height = 0;
			_bpp = 0;
			_mipmapCount = 0;
			_bCompressed = false;
			_bReadOnly = false;
		}

		bool MTexture::LoadFromData(const utility::MFixedArray<MUint8>& data)
		{
			if (!CheckType(data.GetDataPointer()))
				return false;


			MSize offset = sizeof(MAssetType);

			InternalHeader hdr;
			memcpy(&hdr, data.GetDataPointer() + offset, sizeof(InternalHeader));

			_width = hdr._width;
			_height = hdr._height;
			_bpp = hdr._bpp;
			_dataFormat = hdr._dataFormat;
			_samplerFormat = hdr._samplerFormat;
			_mipmapCount = hdr._mipmapCount;
			_bReadOnly = hdr._bReadOnly;

			offset += sizeof(InternalHeader);

			MSize siz = hdr._totalDataSize;
			_rawData.Allocate(siz);
			memcpy(_rawData.GetDataPointer(), data.GetDataPointer() + offset, siz);

			return true;
		}

		bool MTexture::LoadFromData(const utility::MArray<MUint8>& data)
		{
			return LoadFromData(reinterpret_cast<const utility::MFixedArray<MUint8>&>(data));
		}

		inline MUint16 MTexture::FixedArrayToUInt16(const utility::MFixedArray<MUint8>& array)
		{
			MUint16 ret = 0;
			ret = array[1] << 8;
			ret |= array[0];
			return ret;
		}

		inline void MTexture::CompressRawDataToDDS(const utility::MFixedArray<MUint8>& rawData, utility::MFixedArray<MUint8>& outDdsData,
			renderer::device::GraphicDataFormat& outFormat, MUint8& outBpp, MUint8& outMipmapCount) const
		{
			// reverse Y coordinate to properly save texture
			MFixedArray<MUint8> rawReversed;
			rawReversed.Allocate(rawData.GetSize());
			
			MUint8 channels = _bpp / 8;
			for (MSize x = 0, y = 0; y < _height;)
			{
				MSize i = (_width * (_height - y - 1) + x) * channels;
				MSize w = (_width * y + x) * channels;
				rawReversed[w] = rawData[i];
				rawReversed[w + 1] = rawData[i + 1];
				rawReversed[w + 2] = rawData[i + 2];
				rawReversed[w + 3] = rawData[i + 3];

				if (x >= _width)
				{
					++y;
					x = 0;
				}
				else
				{
					++x;
				}
			}

			ILuint id = iluGenImage();
			ilBindImage(id);

			ILboolean b = ilTexImage(_width, _height, 1, channels, IL_RGBA, IL_UNSIGNED_BYTE, rawReversed.GetDataPointer());
			ilEnable(IL_FILE_OVERWRITE);
			
			if (channels == 4)
			{
				ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
			}
			else
			{
				ilSetInteger(IL_DXTC_FORMAT, IL_DXT1);
			}

			if (_mipmapCount < 2)
			{
				b = iluBuildMipmaps();
			}

			MSize offset = sizeof(MUint32) + sizeof(DdsHeader);

			// Doing it like this, because devil screws up data when saving to memory. Need some workaround in the future to avoid using disk.
			ilSaveImage("Resources\\Textures\\TempDDS.dds");

			resourceManagement::fileSystem::File& f = core::Engine::GetInstance()->GetFileSystem().OpenFile("Textures\\TempDDS.dds", resourceManagement::fileSystem::FileAccessMode::READ);
			MSize ddsSize = f.GetSize() - offset;
			outDdsData.Allocate(ddsSize);
			f.Read(outDdsData.GetDataPointer(), offset, ddsSize);
			core::Engine::GetInstance()->GetFileSystem().CloseFile(f);
			core::Engine::GetInstance()->GetFileSystem().RemoveFile("Textures\\TempDDS.dds");

			// getting mipmap count like this because fuck you, ilu has no function that returns number of mipmaps generated.
			outMipmapCount = 1;
			MUint16 mc = _width;
			while (mc >>= 1)
				++outMipmapCount;

			iluDeleteImage(id);

			outFormat = static_cast<renderer::device::GraphicDataFormat>(channels == 4 ? DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM : DXGI_FORMAT::DXGI_FORMAT_BC1_UNORM);
			outBpp = channels == 4 ? 32 : 16;
		}

		//inline void MTexture::LoadDDSToRawData(const MFixedArray<MUint8>& ddsData, MFixedArray<MUint8>& outRawData) const
		//{

		//}

		//void MTexture::ConvertToNormalMap()
		//{
		//}

		//void MTexture::ConvertFromNormalMap()
		//{
		//}
}
}