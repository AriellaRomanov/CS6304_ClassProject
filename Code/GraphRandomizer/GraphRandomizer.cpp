#include <fstream>
#include <map>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <iostream>
#include <string>
#include <experimental/filesystem>

#include "graph.h"

using config_t = std::map<std::string, std::string>;
bool ReadConfiguration(const std::string& filename, config_t& config);
bool DoesConfigKeyExist(const std::string& key);
double GetConfigValue(const std::string& key);
std::string GetConfigString(const std::string& key);

void RandomizeGraph();
void TestGraph();
void GraphStressTest(const std::string& graph_file, const double power_threshold, const double edge_percentage);
void BatchStressTest();
void DoGraphStressTest();
void PrintEdgeList();
bool HasRequiredConfiguration(const std::vector<std::string>& keys);

config_t configuration;
int main(int argc, char** argv)
{
	srand(time(NULL));

	if (argc < 2)
		std::cout << "Missing config file parameter" << std::endl;
	else
	{
		if (ReadConfiguration(argv[1], configuration))
		{
			if (DoesConfigKeyExist("ProgramMethod") && GetConfigString("ProgramMethod") == "Randomize")
				RandomizeGraph();
			else if (DoesConfigKeyExist("ProgramMethod") && GetConfigString("ProgramMethod") == "Test")
				TestGraph();
			else if (DoesConfigKeyExist("ProgramMethod") && GetConfigString("ProgramMethod") == "BatchStress")
				BatchStressTest();
			else if (DoesConfigKeyExist("ProgramMethod") && GetConfigString("ProgramMethod") == "PrintEdgeList")
				PrintEdgeList();
			else
				DoGraphStressTest();
		}
	}
	return 0;
}

void RandomizeGraph()
{
	Log("RandomizeGraph Function Called.");

	std::vector<std::string> required_keys{
		"NumberGraphsGenerated",
		"GraphFilename",
		"OutputDirectory"
	};

	if (HasRequiredConfiguration(required_keys))
	{
		long num_generations = GetConfigValue("NumberGraphsGenerated");
		std::string graph_file = GetConfigString("GraphFilename");
		std::string output_directory = GetConfigString("OutputDirectory");

		Graph main_graph(graph_file);
		auto graph_data = main_graph.RunAnalytics(1);
		if (graph_data.num_components != 1)
		{
			Log("Must start from a single component graph!");
			return;
		}
		auto num_swaps = 50 * graph_data.num_edges;

		for (int i = 0; i < num_generations; i++)
		{
			Log("Creating graph #" + std::to_string(i));
			Graph sub_graph(main_graph);
			sub_graph.RandomizeEdges(num_swaps);
			
			graph_data = sub_graph.RunAnalytics(1);
			if (graph_data.num_components == 1)
				sub_graph.Write(output_directory + "/random" + std::to_string(i) + ".graph");
			else
			{
				Log("Graph discarded: multiple components");
				i--;
			}
		}
	}

	Log("RandomizeGraph Function Complete.");
}

void TestGraph()
{
	std::vector<std::string> required_keys{
		"GraphFilename"
	};

	if (HasRequiredConfiguration(required_keys))
	{
		std::string graph_file = GetConfigString("GraphFilename");
		Graph main_graph(graph_file);
		auto graph_data = main_graph.RunAnalytics();
		Log("TestGraph Function: Analytics complete.");
		Log("Average Power Percentage: " + std::to_string(graph_data.avg_power_percentage));
		Log("Component Count: " + std::to_string(graph_data.num_components));
		Log("Powered Components: " + std::to_string(graph_data.num_components_powered));
		Log("Edge Count: " + std::to_string(graph_data.num_edges));
	}

	return;
}

void GraphStressTest(const std::string& graph_file, const double power_threshold, const double edge_percentage)
{
	Log("GraphStressTest Function: Reading graph: " + graph_file);
	Graph main_graph(graph_file);
	Log("    GraphStressTest Function: Running initial analytics....");
	auto graph_data = main_graph.RunAnalytics();
	Log("    GraphStressTest Function: Analytics complete.");
	long starting_edges = graph_data.num_edges;
	long starting_components = graph_data.num_components;
	long starting_power_supplied = graph_data.avg_power_percentage;

	bool stop_conditions_met = false;
	double power_supplied = power_threshold;

	long iteration = 0;
	while (graph_data.avg_power_percentage > power_threshold && graph_data.num_edges > 0)
	{
		main_graph.CutEdges(edge_percentage);
		graph_data = main_graph.RunAnalytics();
	}

	std::cout << graph_file << ',' << graph_data.num_nodes << ',' << starting_edges << ',' << graph_data.num_edges << ',' << graph_data.num_components
		<< ',' << graph_data.num_components_powered << ',' << graph_data.avg_power_percentage << std::endl;

	Log("    Number of edges cut: " + std::to_string(starting_edges - graph_data.num_edges) + " (of " + std::to_string(starting_edges) + ")");
	Log("    Ending component count: " + std::to_string(graph_data.num_components) + " (of " + std::to_string(starting_components) + ")");
	Log("    Ending average percentage power supplied: " + std::to_string(power_supplied * 100) + "% (from " + std::to_string(starting_power_supplied * 100) + "%)");
}

void BatchStressTest()
{
	std::vector<std::string> required_keys{
		"Directory",
		"PowerSuppliedThreshold",
		"PercentageOfEdgesToCut"
	};

	if (HasRequiredConfiguration(required_keys))
	{
		std::string directory = GetConfigString("Directory");
		double power_threshold = GetConfigValue("PowerSuppliedThreshold");
		double edge_cut_percent = GetConfigValue("PercentageOfEdgesToCut");

		if (power_threshold < 0 || power_threshold > 1)
			Log("Cannot do operation with PowerSuppliedThreshold < 0 or PowerSuppliedThreshold > 1");
		else if (edge_cut_percent <= 0 || edge_cut_percent > 1)
			Log("Cannot do operation with PercentageOfEdgesToCut <= 0 or PercentageOfEdgesToCut > 1");
		else
		{
			for (const auto & entry : std::experimental::filesystem::directory_iterator(directory))
				GraphStressTest(entry.path(), power_threshold, edge_cut_percent);
		}
	}
}

void DoGraphStressTest()
{
	std::vector<std::string> required_keys{
		"GraphFilename",
		"PowerSuppliedThreshold",
		"PercentageOfEdgesToCut"
	};

	if (HasRequiredConfiguration(required_keys))
	{
		std::string graph_file = GetConfigString("GraphFilename");
		double power_threshold = GetConfigValue("PowerSuppliedThreshold");
		double edge_cut_percent = GetConfigValue("PercentageOfEdgesToCut");
		
		if (power_threshold < 0 || power_threshold > 1)
			Log("Cannot do operation with PowerSuppliedThreshold < 0 or PowerSuppliedThreshold > 1");
		else if (edge_cut_percent <= 0 || edge_cut_percent > 1)
			Log("Cannot do operation with PercentageOfEdgesToCut <= 0 or PercentageOfEdgesToCut > 1");
		else
		{
			GraphStressTest(graph_file, power_threshold, edge_cut_percent);
		}
	}
}

void PrintEdgeList()
{
	std::vector<std::string> required_keys{
		"GraphFilename",
		"OutputFilename"
	};

	if (HasRequiredConfiguration(required_keys))
	{
		std::string graph_file = GetConfigString("GraphFilename");
		std::string output_file = GetConfigString("OutputFilename");

		Graph main_graph(graph_file);
		main_graph.PrintEdges(output_file);
	}
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



