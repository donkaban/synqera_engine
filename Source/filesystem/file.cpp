#include "TheEngine/filesystem/file.h"

TheFile::TheFile(the::filesystem::stream &stream)
{
	int chunkLen = 0;
	int magic[3];
	stream.seek(0);
	stream.read(reinterpret_cast<char *>(&magic), sizeof(magic));
	if (magic[0]!=4 || magic[1]!=15 || magic[2]!=32)
		FATAL_ERROR("[File] stream is not TheFile");
	stream.read(reinterpret_cast<char *>(&chunkLen), sizeof(int));
	headers.resize(chunkLen);
	stream.read(reinterpret_cast<char *>(&headers[0]), sizeof(ChunkHeader)*chunkLen);
}

std::vector<ChunkHeader> TheFile::getHeaders(ChunkType type)
{
	std::vector<ChunkHeader> result;
	std::copy_if(headers.begin(), headers.end(), std::back_inserter(result), [type](ChunkHeader h){return h.type==type;});

	return result;
}

ChunkHeader TheFile::getHeader(ChunkType type)
{
	return *std::find_if(headers.begin(), headers.end(), [type](ChunkHeader h){return h.type==type;});
}