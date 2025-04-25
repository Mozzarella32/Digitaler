#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "Block.hpp"

class Renderer;
class BlockSelector;
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

class DataResourceManager {
	/*std::unordered_map<BlockDataID, BlockFileAddress> IdToAddress;
	std::unordered_map<BlockFileAddress, BlockDataID> AddressToId;*/
public:
	Renderer* renderer;
	BlockSelector* Blockselector;
private:

	std::queue<CompressedBlockDataIndex> FreeBlockIndecies;
	CompressedBlockDataIndex NextNewBlockIndex = 0;

	std::unordered_map<CompressedBlockDataIndex, CompressedBlockData> Blocks;

	std::unordered_map<BlockIdentifiyer, CompressedBlockDataIndex> IdentifyerToBlockIndex;
	std::unordered_map<CompressedBlockDataIndex, BlockIdentifiyer> BlockIndexToIdentifiyer;

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

	VisualBlockInterior Interior;

	std::optional<CompressedBlockDataIndex> InteriorData;

	DataResourceManager(Renderer* renderer);

	std::optional<CompressedBlockData> GetCompressedBlockData(const CompressedBlockDataIndex& index) const;

	std::optional<CompressedBlockData> GetCompressedBlockData(const BlockIdentifiyer& Ident) const;

	std::optional<BlockIdentifiyer> GetIdentifyer(const CompressedBlockDataIndex& Index) const;

	CompressedBlockDataIndex GetBlockIndex(const BlockIdentifiyer& Ident) const;

	void SetCurrent(const CompressedBlockDataIndex& cbdi, double Zoom, const Eigen::Vector2f& Offset, bool Push);

	const SpecialBlockIndex& GetSpecialBlockIndex() const;
};