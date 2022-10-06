#pragma once

#include <string>
#include "yaml-cpp/yaml.h"


struct Settings {
	int RefreshRateHz = 500;
	int Volume = 128;

	// Filepath of settings file (Not serialized)
	std::string Filepath;
};

class SettingsSerializer {
public:
	static void Serialize(Settings settings, std::string filepath);
	static void Deserialize(Settings& settings, std::string filepath);
};