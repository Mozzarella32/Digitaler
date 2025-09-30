#pragma once

#include "pch.hpp"

#include "Include.hpp"

#include "Block.hpp"

class Renderer;
class BlockSelector;
class BlockFileAddress {
	std::string Package;
	std::vector<std::string> Dirs;
	std::string Name;
};

namespace PredefinedNames {
	using namespace std::string_view_literals;
	static constexpr auto And =          "Logic:&"sv;
	static constexpr auto Or =           "Locig:>=1"sv;
	static constexpr auto XOr =          "Locig:=1"sv;
	static constexpr auto Mux =          "Connections:Mux"sv;
	static constexpr auto Driver =       "Connections:Driver"sv;
	static constexpr auto SixteenSeg =   "Output:Sixteen Segment"sv;
	static constexpr auto SevenSeg =     "Output:Seven Segment"sv;
	static constexpr auto ToggleButton = "Input:Toggle Button"sv;
	static constexpr auto Button =       "Input:Button"sv;
	static constexpr auto Constant =     "Input:Constant"sv;
	static constexpr auto Probe =        "Output:Probe"sv;
}

class DataResourceManager {
public:
	Renderer* renderer;
	BlockSelector* Blockselector;
private:

	std::queue<CompressedBlockDataIndex> FreeBlockIndecies;
	CompressedBlockDataIndex NextNewBlockIndex = 0;

	std::unordered_map<CompressedBlockDataIndex, CompressedBlockData> Blocks;

	std::unordered_map<BlockIdentifiyer, CompressedBlockDataIndex> IdentifyerToBlockIndex;
	std::unordered_map<CompressedBlockDataIndex, BlockIdentifiyer> BlockIndexToIdentifiyer;

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
};
