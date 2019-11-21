#include <fstream>
#include <map>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <iostream>

#include "graph.h"

using config_t = std::map<std::string, std::string>;
bool ReadConfiguration(const std::string& filename, config_t& config);
bool DoesConfigKeyExist(const std::string& key);
double GetConfigValue(const std::string& key);
std::string GetConfigString(const std::string& key);

void RandomizeGraph();
void DoGraphStressTest();
bool HasRequiredConfiguration(const std::vector<std::string>& keys);

config_t configuration;
int main(int argc, char** argv)
{
	std::cout << "Program Start." << std::endl;
	srand(time(NULL));

	if (argc < 2)
		std::cout << "Missing config file parameter" << std::endl;
	else
	{
		if (ReadConfiguration(argv[1], configuration))
		{
			if (DoesConfigKeyExist("ProgramMethod") && GetConfigString("ProgramMethod") == "Randomize")
				RandomizeGraph();
			else
				DoGraphStressTest();
		}
	}

	std::cout << "Program Done." << std::endl;
	return 0;
}

void RandomizeGraph()
{
	Log("RandomizeGraph Function Called.");

	std::vector<std::string> required_keys{
		"NumberEdgeSwaps",
		"NumberGraphsGenerated",
		"GraphFilename",
		"OutputDirectory",
		"RandomizeNodes",
		"MinNodeProduction",
		"MaxNodeProduction",
		"MinNodeConsumption",
		"MaxNodeConsumption"
	};

	if (HasRequiredConfiguration(required_keys))
	{
		long num_swaps = GetConfigValue("NumberEdgeSwaps");
		long num_generations = GetConfigValue("NumberGraphsGenerated");
		long do_nodes = GetConfigValue("RandomizeNodes");
		long min_prod = GetConfigValue("MinNodeProduction");
		long max_prod = GetConfigValue("MaxNodeProduction");
		long min_cons = GetConfigValue("MinNodeConsumption");
		long max_cons = GetConfigValue("MaxNodeConsumption");
		std::string graph_file = GetConfigString("GraphFilename");
		std::string output_directory = GetConfigString("OutputDirectory");

		Graph main_graph(graph_file);
		for (int i = 0; i < num_generations; i++)
		{
			Graph sub_graph(main_graph);
			sub_graph.RandomizeEdges(num_swaps);
			if (do_nodes)
				sub_graph.RandomizeNodes(min_prod, max_prod, min_cons, max_cons);
			sub_graph.Write(output_directory + "/random" + std::to_string(i) + ".graph");
		}
	}

	Log("RandomizeGraph Function Complete.");
}

void DoGraphStressTest()
{
	Log("DoGraphStressTest Function Called.");

	std::vector<std::string> required_keys{
		"GraphFilename",
		"PowerSuppliedThreshold",
		"PercentageOfEdgesToCut"
	};

	if (HasRequiredConfiguration(required_keys))
	{
		Log("DoGraphStressTest Function: Has required keys.");

		std::string graph_file = GetConfigString("GraphFilename");
		double power_threshold = GetConfigValue("PowerSuppliedThreshold");
		double edge_cut_percent = GetConfigValue("PercentageOfEdgesToCut");
		
		if (power_threshold < 0 || power_threshold > 1)
			Log("Cannot do operation with PowerSuppliedThreshold < 0 or PowerSuppliedThreshold > 1");
		else if (edge_cut_percent <= 0 || edge_cut_percent > 1)
			Log("Cannot do operation with PercentageOfEdgesToCut <= 0 or PercentageOfEdgesToCut > 1");
		else
		{
			Log("DoGraphStressTest Function: Reading graph.");
			Graph main_graph(graph_file);
			Log("DoGraphStressTest Function: Running initial analytics....");
			auto graph_data = main_graph.RunAnalytics();
			Log("DoGraphStressTest Function: Analytics complete.");
			long starting_edges = graph_data.num_edges;
			long starting_components = graph_data.num_components;
			long starting_power_supplied = graph_data.avg_power_percentage;

			bool stop_conditions_met = false;
			double power_supplied = power_threshold;

			long iteration = 0;
			while (graph_data.avg_power_percentage > power_threshold && graph_data.num_edges > 0)
			{
				std::cout << "Iteration: " << ++iteration << std::endl;
				main_graph.CutEdges(edge_cut_percent);
				std::cout << "  CutEdges(" << edge_cut_percent << ")" << std::endl;
				Log("DoGraphStressTest Function: Running initial analytics....");
				graph_data = main_graph.RunAnalytics();
				Log("DoGraphStressTest Function: Analytics complete.");
				std::cout << "  RunAnalytics() :: " << graph_data.avg_power_percentage << " > " << power_threshold << " = " << (graph_data.avg_power_percentage > power_threshold) << std::endl;
				std::cout << "  Component Count: " << graph_data.num_components << std::endl;
			}

			Log("Number of edges cut: " + std::to_string(starting_edges - graph_data.num_edges) + " (of " + std::to_string(starting_edges) + ")");
			Log("Ending component count: " + std::to_string(graph_data.num_components) + " (of " + std::to_string(starting_components) + ")");
			Log("Ending average percentage power supplied: " + std::to_string(power_supplied * 100) + "% (from " + std::to_string(starting_power_supplied * 100) + "%)");
			
			std::string output_graph = graph_file + ".output";
			main_graph.Write(output_graph);
			Log("Ending graph written to: " + output_graph);
		}
	}

	Log("DoGraphStressTest Function Complete.");
}

bool HasRequiredConfiguration(const std::vector<std::string>& keys)
{
	bool has_all_keys = true;
	for (const auto& key : keys)
		has_all_keys = has_all_keys && DoesConfigKeyExist(key);

	if (!has_all_keys)
	{
		Log("Cannot do operation. Missing required configuration. Required:");
		for (const auto& key : keys)
			Log(key);
	}

	return has_all_keys;
}

bool ReadConfiguration(const std::string& filename, config_t& config)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		Log("Unable to read config file: " + filename);
		return false;
	}
	else
	{
		std::string line;
		while (std::getline(file, line))
		{
			auto components = split(line, "=", 2);
			if (components.size() == 2)
				config[components.at(0)] = components.at(1);
		}
		file.close();
	}
	return true;
}

bool DoesConfigKeyExist(const std::string& key)
{
	auto itr = configuration.find(key);
	bool exists = (itr != configuration.end());
	if (!exists)
		Log("Missing " + key + " from configuration.");
	return exists;
}

double GetConfigValue(const std::string& key)
{
	return stod(GetConfigString(key));
}

std::string GetConfigString(const std::string& key)
{
	auto itr = configuration.find(key);
	if (itr != configuration.end())
		return itr->second;
	return "";
}



