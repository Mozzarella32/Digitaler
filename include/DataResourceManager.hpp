#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "Block.hpp"

class Renderer;

//
class BlockFileAddress {
	std::string Package;
	std::vector<std::string> Dirs;
	std::string Name;
};

//class BlockPackage {
//	
//};
//
////Index Into 
//struct BlockDataId {
//	using BlockInPackageIndex = unsigned int;
//	using PackageIndex = unsigned int;
//};
//
//class VisualBlockInterior;
//class VisualBlockExterior;
//
//class VisualPath;
//
//class VisualPathData;
//

using VisualPathDataIndex = unsigned int;

class DataResourceManager {
	/*std::unordered_map<BlockDataID, BlockFileAddress> IdToAddress;
	std::unordered_map<BlockFileAddress, BlockDataID> AddressToId;*/

	Renderer* renderer;

	std::queue<CompressedBlockDataIndex> FreeBlockIndecies;
	CompressedBlockDataIndex NextNewBlockIndex = 0;

	std::unordered_map<CompressedBlockDataIndex, CompressedBlockData> Blocks;

	std::map<std::string, CompressedBlockDataIndex> NameToBlockIndex;

	struct SpecialBlockIndex {
		CompressedBlockDataIndex SevengSeg;
		CompressedBlockDataIndex SixteenSeg;
		CompressedBlockDataIndex And;
		CompressedBlockDataIndex Or;
		CompressedBlockDataIndex XOr;
		CompressedBlockDataIndex Mux;
	};

	SpecialBlockIndex SpecialBlocks;

	//Uncompressed:
	//Index -> UseCount, Data
	//std::unordered_map<BlockExteriorDataIndex, BlockExteriorData> blockExteriorData;


	CompressedBlockDataIndex AddCompressedData(CompressedBlockData&& CBD);

	void RemoveCompressedData(const CompressedBlockDataIndex& Index);

public:
	std::unique_ptr<VisualBlockInterior> CurrentInterior;

	DataResourceManager(Renderer* renderer);

	std::optional<CompressedBlockData> GetCompressedBlockData(const CompressedBlockDataIndex& index) const;

	std::optional<CompressedBlockData> GetCompressedBlockData(const std::string& Name) const;

	CompressedBlockDataIndex GetBlockIndex(const std::string& Name) const;

	void SetCurrent(const CompressedBlockData& cbd);

	const SpecialBlockIndex& GetSpecialBlockIndex() const;
};