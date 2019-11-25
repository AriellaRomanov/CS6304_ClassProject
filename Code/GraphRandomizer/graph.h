#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <tuple>

std::vector<std::string> split(std::string value, const std::string& delim, long max_pieces = -1);
void Log(const std::string& message);

class Graph
{
	struct Node
	{
		Node();
		Node(const long _produced, const long _consumed);
		Node(const Node& copy);

		double produced;
		double consumed;
		bool visited;
	};

	std::vector<Node> nodes;
	std::vector<std::vector<bool>> edges;

public:
	Graph();
	Graph(const Graph& copy);
	Graph(Graph&& source);
	Graph(const std::string& filename);
	~Graph();

	void RandomizeEdges(const long num_swaps);
	void RandomizeNodes(const double min_prod, const double max_prod, const double min_cons, const double max_cons);

	bool IsEdge(const long row, const long col) const;
	void SetEdge(const long row, const long col, const bool edge = true);
	long GetEdgeCount() const;
	void CutEdges(const double percent);

	using component_t = std::vector<long>;
	std::vector<component_t> GetComponents_BFS(const long max_components = -1);
	std::vector<component_t> GetComponents_DFS(const long max_components = -1);

	struct GraphAnalytics
	{
		long num_components;
		long num_edges;
		long num_nodes;
		long num_components_powered;
		double avg_power_percentage;
	};
	GraphAnalytics RunAnalytics(const long max_components = -1);

	void Write(const std::string& filename) const;
};