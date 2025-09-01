#include "pch.hpp"

#include "DataResourceManager.hpp"

#include "BlockSelector.hpp"

#include "Renderer.hpp"

DataResourceManager::DataResourceManager(Renderer* renderer)
	:renderer(renderer), Interior(this, renderer)
{
	using enum MyDirection::Direction;

	SpecialBlocks.SevengSeg = AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Display:Seven Seg"),
			.Size = {2,3},
			.InputPin = {
				{.Rotation = MyDirection::Down, .Offset = 1,.Multiplicity = 7},
				{.Rotation = MyDirection::Down, .Offset = 2},
			},
		},
		});

	SpecialBlocks.SixteenSeg = AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Display:Sixteen Seg"),
			.Size = {2,3},
			.InputPin = {
				{.Rotation = MyDirection::Down, .Offset = 1,.Multiplicity = 16},
				{.Rotation = MyDirection::Down, .Offset = 2},
			},
		}
		});
	SpecialBlocks.And = AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Logic:&"),
			.Size = {2,2},
			.InputPin = {
				{.Rotation = MyDirection::Down, .Offset = 0},
				{.Rotation = MyDirection::Down, .Offset = 2},
				},
			.OutputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1},
			},
		}
		});
	SpecialBlocks.Or = AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Logic:>=1"),
			.Size = {2,2},
			.InputPin = {
				{.Rotation = MyDirection::Down, .Offset = 0},
				{.Rotation = MyDirection::Down, .Offset = 2},
				},
			.OutputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1},
			},
		}
		});
	SpecialBlocks.XOr = AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Logic:=1"),
			.Size = {2,2},
			.InputPin = {
				{.Rotation = MyDirection::Down, .Offset = 0},
				{.Rotation = MyDirection::Down, .Offset = 2},
				},
			.OutputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1},
			},
		}
		});
	SpecialBlocks.Mux = AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Connections:Mux"),
			.Size = {2,2},
			.InputPin = {
				{.Rotation = MyDirection::Down, .Offset = 0,.Multiplicity = 128},
				{.Rotation = MyDirection::Down, .Offset = 2,.Multiplicity = 128},
				{.Rotation = MyDirection::Right, .Offset = 1},
				},
			.OutputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1,.Multiplicity = 128},
			},
		}
		});

	AddCompressedData(
		CompressedBlockData{
			.blockExteriorData = {
				.ContainedBlocks = {
				{BlockIdentifiyer::ParsePredefined("Testing:Adder"),{
					{{0,20},Up},
					{{7,20},Right},
					{{14,20},Down},
					{{21,20},Left},
			}},
				{BlockIdentifiyer::ParsePredefined("Display:Seven Seg"),{
					{{0,10},Up},
					{{5,10},Right},
					{{10,10},Down},
					{{15,10},Left},
			}},
				{BlockIdentifiyer::ParsePredefined("Display:Sixteen Seg"),{
					{{0,40},Up},
					{{5,40},Right},
					{{10,40},Down},
					{{15,40},Left},

				{{0,-5},Up},{{3,-5},Up},{{6,-5},Up},{{9,-5},Up},{{12,-5},Up},{{15,-5},Up},{{18,-5},Up},{{21,-5},Up},{{24,-5},Up},{{27,-5},Up},{{30,-5},Up},{{33,-5},Up},{{36,-5},Up},{{39,-5},Up},{{42,-5},Up},{{45,-5},Up},{{48,-5},Up},{{51,-5},Up},{{0,-10},Up},{{3,-10},Up},{{6,-10},Up},{{9,-10},Up},{{12,-10},Up},{{15,-10},Up},{{18,-10},Up},{{21,-10},Up},{{24,-10},Up},{{27,-10},Up},{{30,-10},Up},{{33,-10},Up},{{36,-10},Up},{{39,-10},Up},{{42,-10},Up},{{45,-10},Up},{{0,-15},Up},{{3,-15},Up},{{6,-15},Up},{{9,-15},Up},{{12,-15},Up},{{15,-15},Up},{{18,-15},Up},{{21,-15},Up},{{24,-15},Up},{{27,-15},Up},{{30,-15},Up},{{33,-15},Up},{{36,-15},Up},{{39,-15},Up},{{42,-15},Up},{{45,-15},Up},{{48,-15},Up},{{51,-15},Up},{{0,-20},Up},{{3,-20},Up},{{6,-20},Up},{{9,-20},Up},{{12,-20},Up},{{15,-20},Up},{{18,-20},Up},{{21,-20},Up},{{24,-20},Up},{{27,-20},Up},{{30,-20},Up},{{33,-20},Up},{{36,-20},Up},{{39,-20},Up},{{42,-20},Up},{{45,-20},Up},{{48,-20},Up},{{51,-20},Up},{{54,-20},Up},{{57,-20},Up},{{60,-20},Up},{{63,-20},Up},{{0,-25},Up},{{3,-25},Up},{{6,-25},Up},{{9,-25},Up},{{12,-25},Up},{{15,-25},Up},{{18,-25},Up},{{21,-25},Up},{{24,-25},Up},{{27,-25},Up},{{30,-25},Up},{{33,-25},Up},{{36,-25},Up},{{39,-25},Up},{{42,-25},Up},{{45,-25},Up},{{0,-30},Up},{{3,-30},Up},{{6,-30},Up},{{9,-30},Up},{{12,-30},Up},{{15,-30},Up},{{18,-30},Up},{{21,-30},Up},{{24,-30},Up},{{27,-30},Up},{{0,-35},Up},{{3,-35},Up},{{6,-35},Up},{{9,-35},Up},{{12,-35},Up},{{15,-35},Up},{{18,-35},Up},{{21,-35},Up},{{24,-35},Up},{{27,-35},Up},{{30,-35},Up},{{33,-35},Up},{{36,-35},Up},{{39,-35},Up},{{42,-35},Up},{{45,-35},Up},{{48,-35},Up},{{51,-35},Up},{{54,-35},Up},{{57,-35},Up},{{60,-35},Up},{{63,-35},Up},{{66,-35},Up},{{69,-35},Up},{{72,-35},Up},{{75,-35},Up},{{78,-35},Up},{{81,-35},Up},{{84,-35},Up},{{87,-35},Up},{{90,-35},Up},{{93,-35},Up},{{96,-35},Up},{{99,-35},Up},{{102,-35},Up},{{105,-35},Up},{{108,-35},Up},{{111,-35},Up},{{114,-35},Up},{{117,-35},Up},{{120,-35},Up},{{123,-35},Up},{{126,-35},Up},{{0,-40},Up},{{3,-40},Up},{{6,-40},Up},{{9,-40},Up},{{12,-40},Up},{{15,-40},Up},{{18,-40},Up},{{21,-40},Up},{{24,-40},Up},{{27,-40},Up},{{30,-40},Up},{{33,-40},Up},{{36,-40},Up},{{39,-40},Up},{{42,-40},Up},{{45,-40},Up},{{48,-40},Up},{{51,-40},Up},{{54,-40},Up},{{57,-40},Up},{{60,-40},Up},{{63,-40},Up},{{66,-40},Up},{{69,-40},Up},{{72,-40},Up},{{75,-40},Up},{{78,-40},Up},{{81,-40},Up},{{84,-40},Up},{{87,-40},Up},{{90,-40},Up},{{93,-40},Up},{{96,-40},Up},{{99,-40},Up},{{102,-40},Up},{{105,-40},Up},{{108,-40},Up},{{111,-40},Up},{{114,-40},Up},{{117,-40},Up},{{120,-40},Up},{{123,-40},Up},{{126,-40},Up},{{0,-45},Up},{{3,-45},Up},{{6,-45},Up},{{9,-45},Up},{{12,-45},Up},{{15,-45},Up},{{18,-45},Up},{{21,-45},Up},{{24,-45},Up},{{27,-45},Up},{{30,-45},Up},{{33,-45},Up},{{36,-45},Up},{{39,-45},Up},{{42,-45},Up},{{45,-45},Up},{{48,-45},Up},{{51,-45},Up},{{54,-45},Up},{{57,-45},Up},{{60,-45},Up},{{63,-45},Up},{{66,-45},Up},{{69,-45},Up},{{72,-45},Up},{{75,-45},Up},{{78,-45},Up},{{81,-45},Up},{{84,-45},Up},{{87,-45},Up},{{90,-45},Up},{{93,-45},Up},
			}},
				{BlockIdentifiyer::ParsePredefined("Logic:&"),{
					{{ -5,0 },Up},
					{{ -11,0 },Right},
					{{ -17,0 },Down},
					{{ -23,0 },Left},
			}},
				{BlockIdentifiyer::ParsePredefined("Logic:>=1"),{
					{{ -5,-10 },Up},
					{{ -11,-10 },Right},
					{{ -17,-10 },Down},
					{{ -23,-10 },Left},
			}},
				{BlockIdentifiyer::ParsePredefined("Logic:=1"),{
					{{ -5,-20 },Up},
					{{ -11,-20 },Right},
					{{ -17,-20 },Down},
					{{ -23,-20 },Left},
			}},
			{BlockIdentifiyer::ParsePredefined("Connections:Mux"),{
					{{ 0,30 },Up},
					{{ 5,30 },Right},
					{{ 10,30 },Down},
					{{ 15,30 },Left},
			}},
			{BlockIdentifiyer::ParsePredefined("Testing:Block2-2"),{
					{{ -40,0 },Up},
					{{ -50,0 },Right},
					{{ -60,0 },Down},
					{{ -70,0 },Left},
			}},
			{BlockIdentifiyer::ParsePredefined("Testing:Block2-3"),{
					{{ -40,10 },Up},
					{{ -50,10 },Right},
					{{ -60,10 },Down},
					{{ -70,10 },Left},
			}},
				{BlockIdentifiyer::ParsePredefined("Testing:Block3-2"),{
					{{ -40,20 },Up},
					{{ -50,20 },Right},
					{{ -60,20 },Down},
					{{ -70,20 },Left},
			}},
				{BlockIdentifiyer::ParsePredefined("Testing:Block3-3"),{
					{{ -40,30 },Up},
					{{ -50,30 },Right},
					{{ -60,30 },Down},
					{{ -70,30 },Left},
			}},
				{BlockIdentifiyer::ParsePredefined("Testing:Block2-4"),{
					{{ -40,40 },Up},
					{{ -50,40 },Right},
					{{ -60,40 },Down},
					{{ -70,40 },Left},
			}},
				{BlockIdentifiyer::ParsePredefined("Testing:Block4-2"),{
					{{ -40,50 },Up},
					{{ -50,50 },Right},
					{{ -60,50 },Down},
					{{ -70,50 },Left},
			}},
				{BlockIdentifiyer::ParsePredefined("Testing:Block3-4"),{
					{{ -40,60 },Up},
					{{ -50,60 },Right},
					{{ -60,60 },Down},
					{{ -70,60 },Left},
			}},
				{BlockIdentifiyer::ParsePredefined("Testing:Block4-3"),{
					{{ -40,70 },Up},
					{{ -50,70 },Right},
					{{ -60,70 },Down},
					{{ -70,70 },Left},
			}},
				{BlockIdentifiyer::ParsePredefined("Testing:Block4-4"),{
					{{ -40,80 },Up},
					{{ -50,80 },Right},
					{{ -60,80 },Down},
					{{ -70,80 },Left},
			}},
			{BlockIdentifiyer::ParsePredefined("Testing:Block2-5"),{
					{{ -40,90 },Up},
					{{ -50,90 },Right},
					{{ -60,90 },Down},
					{{ -70,90 },Left},
			}},
			{ BlockIdentifiyer::ParsePredefined("Testing:Block5-2"),{
					// {{0,0},Up},
					{{ -40,100 },Up},
					{{ -50,100 },Right},
					{{ -60,100 },Down},
					{{ -70,100 },Left},
			} },
			{ BlockIdentifiyer::ParsePredefined("Testing:Block3-5"),{
					{{ -40,110 },Up},
					{{ -50,110 },Right},
					{{ -60,110 },Down},
					{{ -70,110 },Left},
			} },
			{ BlockIdentifiyer::ParsePredefined("Testing:Block5-3"),{
					{{ -40,120 },Up},
					{{ -50,120 },Right},
					{{ -60,120 },Down},
					{{ -70,120 },Left},
			} },
			{ BlockIdentifiyer::ParsePredefined("Testing:Block4-5"),{
					{{ -40,130 },Up},
					{{ -50,130 },Right},
					{{ -60,130 },Down},
					{{ -70,130 },Left},
			} },
			{ BlockIdentifiyer::ParsePredefined("Testing:Block5-4"),{
					{{ -40,140 },Up},
					{{ -50,140 },Right},
					{{ -60,140 },Down},
					{{ -70,140 },Left},
			} },
			{ BlockIdentifiyer::ParsePredefined("Testing:Block5-5"),{
					{{ -40,150 },Up},
					{{ -50,150 },Right},
					{{ -60,150 },Down},
					{{ -70,150 },Left},
			} },
			},

			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:MainBlock")
			}
		});

	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block2-2"),
			.Size = {2,2},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block3-2"),
			.Size = {3,2},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block2-3"),
			.Size = {2,3},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block3-3"),
			.Size = {3,3},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block4-2"),
			.Size = {4,2},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block2-4"),
			.Size = {2,4},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block3-4"),
			.Size = {3,4},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block4-3"),
			.Size = {4,3},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});

	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block4-4"),
			.Size = {4,4},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block2-5"),
			.Size = {2,5},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block5-2"),
			.Size = {5,2},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block3-5"),
			.Size = {3,5},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block5-3"),
			.Size = {5,3},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block4-5"),
			.Size = {4,5},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block5-4"),
			.Size = {5,4},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});
	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Block5-5"),
			.Size = {5,5},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "i0"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "i3"},
				},
		}
		});

	/*


float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
	vec2 pa = p-a, ba = b-a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	vec2 r = ba*h;
	return abs(pa.x-r.x)+abs(pa.y-r.y);
}

	*/

	/*AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Name = "ContainedBlock",
			.Size = {2,3},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 0},
				{.Rotation = MyDirection::Left, .Offset = 0},
				{.Rotation = MyDirection::Left, .Offset = 2},
				},
			.OutputPin = {
				{.Rotation = MyDirection::Down, .Offset = 0},
				{.Rotation = MyDirection::Right, .Offset = 1},
				{.Rotation = MyDirection::Right, .Offset = 3},
			},
		}
		});*/

	AddCompressedData(
		CompressedBlockData{
		.blockExteriorData = {
			.Identifiyer = BlockIdentifiyer::ParsePredefined("Testing:Adder"),
			.Size = {17,5},
			.InputPin = {
				{.Rotation = MyDirection::Up, .Offset = 1, .Multiplicity = 1,.Name = "C In"},
				{.Rotation = MyDirection::Left, .Offset = 0, .Multiplicity = 1,.Name = "i1"},
				{.Rotation = MyDirection::Left, .Offset = 1, .Multiplicity = 1,.Name = "i2"},
				{.Rotation = MyDirection::Left, .Offset = 2, .Multiplicity = 1,.Name = "i3"},
				{.Rotation = MyDirection::Left, .Offset = 3, .Multiplicity = 1,.Name = "i4"},
				},
			.OutputPin = {
				{.Rotation = MyDirection::Down, .Offset = 1, .Multiplicity = 1,.Name = "C Out"},
				{.Rotation = MyDirection::Right, .Offset = 0, .Multiplicity = 1,.Name = "o1"},
				{.Rotation = MyDirection::Right, .Offset = 1, .Multiplicity = 1,.Name = "o2"},
				{.Rotation = MyDirection::Right, .Offset = 2, .Multiplicity = 1,.Name = "o3"},
				{.Rotation = MyDirection::Right, .Offset = 3, .Multiplicity = 1,.Name = "o4"},
			},
		}
		});

	/*


	PinVerts.emplace_back(-1, 1, 2, 0, 0.5, 1);
	PinVerts.emplace_back(-1, 2, 2, 0, 0.5, 1);
	PinVerts.emplace_back(1, -1, 3, 0, 0.5, 1);
	PinVerts.emplace_back(3, 0, 0, 0, 0.5, 1);
	PinVerts.emplace_back(3, 2, 0, 0, 0.5, 1);
	PinVerts.emplace_back(1, 4, 1, 0, 0.5, 1);

	*/
}

CompressedBlockDataIndex  DataResourceManager::AddCompressedData(CompressedBlockData&& CBD) {
	CompressedBlockDataIndex NewIndex;
	if (FreeBlockIndecies.empty()) {
		NewIndex = NextNewBlockIndex++;
	}
	else {
		NewIndex = FreeBlockIndecies.front();
		FreeBlockIndecies.pop();
	}
	IdentifyerToBlockIndex.emplace(CBD.blockExteriorData.Identifiyer, NewIndex);
	BlockIndexToIdentifiyer.emplace(NewIndex,CBD.blockExteriorData.Identifiyer);
	Blocks.emplace(NewIndex, std::move(CBD));
	return NewIndex;
}

void DataResourceManager::RemoveCompressedData(const CompressedBlockDataIndex& Index) {
	if (auto it = Blocks.find(Index); it != Blocks.end()) {
		Blocks.erase(it);
		auto it2 = std::find_if(IdentifyerToBlockIndex.begin(), IdentifyerToBlockIndex.end(),
			[Index](const auto& pair) {
				return pair.second == Index;
			});
		if (it2 != IdentifyerToBlockIndex.end()) IdentifyerToBlockIndex.erase(it2);
		BlockIndexToIdentifiyer.erase(Index);
		FreeBlockIndecies.push(Index);
	}
}

void DataResourceManager::SetCurrent(const CompressedBlockDataIndex& cbdi, double Zoom, const Eigen::Vector2f& Offset, bool Push) {
	auto identopt = GetIdentifyer(cbdi);
	assert(identopt);
	if (InteriorData) {
		Interior.WriteBack(Blocks.at(InteriorData.value()));
	}

	InteriorData = cbdi;
	if (Push) {
		Blockselector->PushInfo({ .Ident = identopt.value(),.Zoom = Zoom,.Offset = Offset });
		renderer->Zoom = 0.01;
		renderer->Offset = { 0,0 };
		renderer->UpdateViewProjectionMatrix();
	}
	else {
		renderer->Zoom = Zoom;
		renderer->Offset = Offset;
		renderer->UpdateViewProjectionMatrix();
	}
	Interior.UpdateCurrentBlock();
}

std::optional<CompressedBlockData> DataResourceManager::GetCompressedBlockData(const CompressedBlockDataIndex& index) const {
	auto it = Blocks.find(index);
	if (it == Blocks.end()) return std::nullopt;
	return it->second;
}

std::optional<CompressedBlockData> DataResourceManager::GetCompressedBlockData(const BlockIdentifiyer& Ident) const {
	auto it = IdentifyerToBlockIndex.find(Ident);
	if (it == IdentifyerToBlockIndex.end()) return std::nullopt;
	return GetCompressedBlockData(it->second);
}

std::optional<BlockIdentifiyer> DataResourceManager::GetIdentifyer(const CompressedBlockDataIndex& Index) const {
	auto it = BlockIndexToIdentifiyer.find(Index);
	if (it == BlockIndexToIdentifiyer.end()) return std::nullopt;
	return it->second;
}

CompressedBlockDataIndex DataResourceManager::GetBlockIndex(const BlockIdentifiyer& Ident) const {
	auto it = IdentifyerToBlockIndex.find(Ident);
	//This is the point at wich i should be looking for not loaded packages
	if (it == IdentifyerToBlockIndex.end()) return InvalidCompressedBlockDataIndex;
	return it->second;
}

const DataResourceManager::SpecialBlockIndex& DataResourceManager::GetSpecialBlockIndex() const {
	return SpecialBlocks;
}
