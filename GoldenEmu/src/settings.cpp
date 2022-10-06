#include "settings.h"
#include <fstream>

void SettingsSerializer::Serialize(Settings settings, std::string filepath)
{
	YAML::Node config;

	config["RefreshRateHz"] = settings.RefreshRateHz;
	config["Volume"] = settings.Volume;


	std::ofstream fout(filepath);
	fout << config;
}

void SettingsSerializer::Deserialize(Settings& settings, std::string filepath)
{
	YAML::Node data;
	try
	{
		data = YAML::LoadFile(filepath);
	}
	catch (std::exception e)
	{
		return;
	}

	if(data["RefreshRateHz"])
		settings.RefreshRateHz = data["RefreshRateHz"].as<int>();
	if (data["Volume"])
		settings.Volume = data["Volume"].as<int>();

}
