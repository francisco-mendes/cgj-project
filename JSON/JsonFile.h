#pragma once

#include <filesystem>
#include <vector>

#include "../Math/Matrix.h"
#include "../Math/Quaternion.h"
#include "../Math/Vector.h"
#include "../lib/json/json.hpp"
#include "../Engine/Engine.h"

using json = nlohmann::json;

namespace jsonFile
{
	struct JsonFile
	{
		JsonFile();
		json* j;
		std::filesystem::path default_dir_;
		render::Scene scene;
	};
	bool saveFile(config::Settings settings, std::filesystem::path planeMesh, std::filesystem::path pieceMesh);
	bool loadFile();
}
