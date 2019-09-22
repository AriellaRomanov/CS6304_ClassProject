#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <pair>

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
	void RemoveEdge(const long row, const long col);
	long GetEdgeCount() const;
	long GetComponentCount() const;
	double GetAveragePowerPercentageSupplied() const;
	void CutEdges(const double percent);

	void Write(const std::string& filename) const;
};