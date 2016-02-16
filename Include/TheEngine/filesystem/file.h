#ifndef _THE_FILE_
#define _THE_FILE_

#include "TheEngine/common.h"


enum ChunkType
{
	SURFACE = 0,
	VERTEX,
	BONES,
	BONE_ANIMATION_HEADER,
	BONE_ANIMATION_CURVE
};

struct ChunkHeader
{
	ChunkType type;
	int32_t offset;
	int32_t size;
	int32_t dummyData[4];
};

class TheFile
{
	std::vector<ChunkHeader> headers;
public:
	TheFile(the::filesystem::stream &stream);
	~TheFile(){}

	std::vector<ChunkHeader> getHeaders(ChunkType type);
	ChunkHeader getHeader(ChunkType type);
};

#endif