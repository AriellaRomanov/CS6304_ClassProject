#include "graph.h"

Graph::Node::Node()
	: produced(0),
	consumed(0)
{

}

Graph::Node::Node(const Node& copy)
	: produced(copy.produced),
	consumed(copy.consumed)
{

}

Graph::Graph()
{

}

Graph::Graph(const Graph& copy)
{
	for (auto& n : copy.nodes)
		nodes.push_back(n);

	for (const auto& vect : copy.edges)
	{
		edges.emplace_back();
		for (const auto& e : vect)
			edges.back().push_back(e);
	}
}

Graph::Graph(Graph&& source)
	: nodes(std::move(source.nodes)),
	edges(std::move(source.edges))
{

}

Graph::Graph(const std::string& filename)
{

}

Graph::~Graph()
{

}

void Graph::RandomizeEdges(const long num_swaps)
{

}

void Graph::RandomizeNodes(const double min_prod, const double max_prod, const double min_cons, const double max_cons)
{

}

long Graph::GetEdgeCount() const
{

}

long Graph::GetComponentCount() const
{

}

double Graph::GetAveragePowerPercentageSupplied() const
{

}

void Graph::CutEdges(const double percent)
{

}

void Graph::Write(const std::string& filename) const
{

}
