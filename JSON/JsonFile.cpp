#include "JsonFile.h"

#include <filesystem>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

#include "../Math/Matrix.h"
#include "../Math/Quaternion.h"
#include "../Math/Vector.h"
#include "../lib/json/json.hpp"
#include "../Engine/Engine.h"
#include "../Render/Scene.h"

using json = nlohmann::json;

namespace jsonFile
{

	json* j;
	std::filesystem::path default_dir_ = "JSON/";
	//render::Scene& saveScene;
	std::string input, saveInput;
	bool saving = false, loading = false;

	bool saveFile(config::Settings settings, std::filesystem::path planeMesh, std::filesystem::path pieceMesh) {

		json FileToSave;
		FileToSave["Settings"]["Version"]["Major"] = settings.version.major;
		FileToSave["Settings"]["Version"]["Minor"] = settings.version.minor;
		FileToSave["Settings"]["Window"]["Title"] = settings.window.title;
		FileToSave["Settings"]["Window"]["Size"]["Width"] = settings.window.size.width;
		FileToSave["Settings"]["Window"]["Size"]["Height"] = settings.window.size.height;
		FileToSave["Settings"]["Window"]["Mode"] = settings.window.mode;
		FileToSave["Settings"]["Window"]["VSync"] = settings.window.vsync;
		std::string planeMeshDir{ planeMesh.u8string() };
		FileToSave["Meshes"]["Plane"] = planeMeshDir;
		std::string pieceMeshDir{ pieceMesh.u8string() };
		FileToSave["Meshes"]["Piece"] = pieceMeshDir;

		time_t rawtime = time(NULL);
		tm timeinfo = {};
		char buffer[80];

		localtime_s(&timeinfo, &rawtime);
		//asctime_s(buffer, 80, &timeinfo);
		strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", &timeinfo);
		std::string timestamp(buffer);
		std::string filePath = default_dir_.u8string() + timestamp + ".json";
		std::ofstream file(filePath);
		file << FileToSave;
		std::cout << "File has been saved to " << filePath << std::endl;
		return true;

		// An attempt to implement filename input.
		/*saving = true;

		while (saving) {
			std::cout << "Type the name of the file to save (enter c to cancel): ";
			std::cin >> saveInput;
			
			if (saveInput == "c") {
				saving = false;
				return false;
			}

			if (input.substr(input.size() - 5) != ".json") {
				saveInput += ".json";
			}

			std::ifstream checkForFile(default_dir_ / saveInput);
			std::string overCheck;
			if (checkForFile.good()) {
				bool overwrite = true;
				while (overwrite) {
					std::cout << "This file already exists, do you wish to overwrite it? (y/n) ";
					std::cin >> overCheck;
					if (overCheck != "y" || overCheck != "n")
						std::cout << "Your input is invalid, please try again.";
					else if (overCheck == "n")
						overwrite = false;
					else if (overCheck == "y") {
						std::ofstream file(saveInput, std::ofstream::trunc);
						file << FileToSave;
						overwrite = false;
						saving = false;
						return true;
					}
				}
			}
			else {
				std::ofstream file(saveInput);
				file << FileToSave;
				saving = false;
				return true;
			}
		}*/
		//return false;
	}
	bool loadFile() {

		std::ifstream file(default_dir_ / "2021-01-19-21-24-18.json");

		json jf = json::parse(file);
		config::Settings settings;
		settings.version.major = jf["Settings"]["Version"]["Major"];
		settings.version.minor = jf["Settings"]["Version"]["Minor"];
		std::string title = jf["Settings"]["Window"]["Title"];
		const char* ctitle = title.c_str();
		settings.window.title = ctitle;
		settings.window.size.width = jf["Settings"]["Window"]["Size"]["Width"];
		settings.window.size.height = jf["Settings"]["Window"]["Size"]["Height"];
		int mode = jf["Settings"]["Window"]["Mode"];
		settings.window.mode = static_cast<config::Mode>(mode);
		int vsync = jf["Settings"]["Window"]["VSync"];
		settings.window.vsync = static_cast<config::VSync>(vsync);
		//planeMesh /= jf["Meshes"]["Plane"];
		//pieceMesh /= jf["Meshes"]["Piece"];
		loading = false;

		std::cout << "Version: " <<settings.version.major << "." << settings.version.minor << std::endl;
		std::cout << "Title: " << settings.window.title << std::endl;
		std::cout << "Size: " << settings.window.size.width << ", " << settings.window.size.height << std::endl;
		std::cout << "Mode: " << mode << std::endl;
		std::cout << "VSync: " << vsync << std::endl;

		return true;
		
		// An attempt to implement filename input.
		/*while (loading) {
			std::cout << "Type the name of the file to load (make sure it's in the JSON folder): ";
			std::cin >> input;
			
			if (input.size() >= 6 && input.substr(input.size() - 5) == ".json") {

				std::ifstream file(default_dir_ / input);
				std::ifstream file(default_dir_ / input);

				json jf = json::parse(file);
				config::Settings settings;
				settings.version.major = jf["Settings"]["Version"]["Major"];
				settings.version.minor = jf["Settings"]["Version"]["Minor"];
				std::string title = jf["Settings"]["Window"]["Title"];
				char* ctitle = new char[title.length() + 1];
				settings.window.title = ctitle;
				settings.window.size.width = jf["Settings"]["Window"]["Size"]["Width"];
				settings.window.size.height = jf["Settings"]["Window"]["Size"]["Height"];
				int mode = jf["Settings"]["Window"]["Mode"];
				settings.window.mode = static_cast<config::Mode>(mode);
				int vsync = jf["Settings"]["Window"]["VSync"];
				settings.window.vsync = static_cast<config::VSync>(vsync);
				//planeMesh /= jf["Meshes"]["Plane"];
				//pieceMesh /= jf["Meshes"]["Piece"];
				loading = false;
				return true;
			}
			else
				std::cout << "The file name you entered is invalid.";
		}*/
		return false;
	}
}
