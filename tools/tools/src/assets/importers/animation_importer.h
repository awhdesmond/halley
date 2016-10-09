#pragma once
#include "halley/tools/assets/iasset_importer.h"
#include <gsl/gsl>

namespace Halley
{
	class Animation;

	class AnimationImporter : public IAssetImporter
	{
	public:
		AssetType getType() const override { return AssetType::Animation; }

		std::vector<Path> import(const ImportingAsset& asset, Path dstDir, ProgressReporter reporter, AssetCollector collector) override;

		static void parseAnimation(Animation& animation, gsl::span<const gsl::byte> data);
	};
}
