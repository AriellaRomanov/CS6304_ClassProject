#include "graph.h"
#include <queue>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <cstring>

Graph::Node::Node()
	: produced(0),
	consumed(0)
{

}

Graph::Node::Node(const long _produced, const long _consumed)
	: produced(_produced),
	consumed(_consumed),
	visited(false)
{
	if (consumed <= 0)
		consumed = 0.000001;
}

Graph::Node::Node(const Node& copy)
	: produced(copy.produced),
	consumed(copy.consumed),
	visited(copy.visited)
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
	std::ifstream file(filename);
	if (!file.is_open())
		Log("Unable to read graph file: " + filename + "[" + std::string(std::strerror(errno)) + "]");
	else
	{
		std::vector<std::string> lines;

		std::string line;
		while (std::getline(file, line))
			lines.push_back(line);
		file.close();

		auto size = static_cast<long>(lines.size());

		nodes.reserve(size);

		edges.reserve(size);
		for (long i = 0; i < size; i++)
		{
			edges.emplace_back();
			edges.at(i).reserve(i + 1);
			for (long j = 0; j < i + 1; j++)
				edges.at(i).emplace_back(false);
		}

		for (int i = 0; i < size; i++)
		{
			auto components = split(lines.at(i), ",");
			auto _size = static_cast<long>(components.size());

			double produced = (_size > 1) ? stod(components.at(0)) : 0;
			double consumed = (_size > 2) ? stod(components.at(1)) : 0;
			nodes.emplace_back(produced, consumed);

			for (int j = 2; j < _size; j++)
			{
				if (components.at(j) != "")
					SetEdge(i, stoi(components.at(j)));
			}
		}
	}
}

Graph::~Graph()
{

}

void Graph::RandomizeEdges(const long num_swaps)
{
	std::vector<std::pair<long, long>> _edges;
	for (int row = 0; row < static_cast<long>(edges.size()); row++)
	{
		for (int col = 0; col < static_cast<long>(edges.at(row).size()); col++)
		{
			if (edges.at(row).at(col))
			{
				_edges.emplace_back(row, col);
				edges.at(row).at(col) = false;
			}
		}
	}

	long edge_count = static_cast<long>(_edges.size());
	for (int i = 0; i < num_swaps; i++)
	{
		auto idx1 = rand() % edge_count;
		auto idx2 = rand() % edge_count;

		auto temp = _edges.at(idx1).second;
		_edges.at(idx1).second = _edges.at(idx2).second;
		_edges.at(idx2).second = temp;
	}

	for (const auto& e : _edges)
		SetEdge(e.first, e.second);
}

void Graph::RandomizeNodes(const double min_prod, const double max_prod, const double min_cons, const double max_cons)
{

}

bool Graph::IsEdge(const long row, const long col) const
{
	auto _row = (row > col) ? row : col;
	auto _col = (row < col) ? row : col;
	return edges.at(_row).at(_col);
}

void Graph::SetEdge(const long row, const long col, const bool edge)
{
	auto _row = (row > col) ? row : col;
	auto _col = (row < col) ? row : col;
	edges.at(_row).at(_col) = edge;
}

long Graph::GetEdgeCount() const
{
	long edge_count = 0;

	for (int row = 0; row < static_cast<long>(edges.size()); row++)
		for (int col = 0; col < static_cast<long>(edges.at(row).size()); col++)
			if (edges.at(row).at(col))
				edge_count++;

	return edge_count;
}

void Graph::CutEdges(const double percent)
{
	std::vector<std::pair<long, long>> edge_list;

	for (int row = 0; row < static_cast<long>(edges.size()); row++)
		for (int col = 0; col < static_cast<long>(edges.at(row).size()); col++)
			if (edges.at(row).at(col))
				edge_list.emplace_back(row, col);

	long edge_count = static_cast<long>(edge_list.size());
	long cut_count = edge_count * percent;
	if (cut_count < 1)
		cut_count = 1;

	for (int i = 0; i < cut_count; i++)
	{
		auto idx = rand() % static_cast<long>(edge_list.size());
		auto row = edge_list.at(idx).first;
		auto col = edge_list.at(idx).second;
		edges.at(row).at(col) = false;
		edge_list.erase(edge_list.begin() + idx);
	}
}

std::vector<Graph::component_t> Graph::GetComponents_BFS(const long max_components)
{
	for (auto& n : nodes)
		n.visited = false;

	auto get_next_start_node = [this]()->long
	{
		for (int i = 0; i < static_cast<long>(nodes.size()); i++)
			if (!nodes.at(i).visited)
				return i;
		return -1;
	};

	std::vector<component_t> component_list;

	int start_node = get_next_start_node();
	while (start_node != -1)
	{
		component_t component;

		// BFS
		std::queue<long> queue;
		auto visit = [&](const long node_id)
		{
			nodes.at(node_id).visited = true;
			queue.push(node_id);
			component.push_back(node_id);
		};
		visit(start_node);

		while (static_cast<long>(queue.size()) > 0)
		{
			auto current_node = queue.front();
			queue.pop();

			for (int i = 0; i < static_cast<long>(nodes.size()); i++)
			{
				if (i != current_node && !nodes.at(i).visited && IsEdge(current_node, i))
					visit(i);
			}
		}

		if (static_cast<long>(component.size()) > 0)
			component_list.push_back(component);

		start_node = get_next_start_node();
		if (max_components > 0 && static_cast<long>(component_list.size()) > max_components)
			start_node = -1;
	}
	return component_list;
}

std::vector<Graph::component_t> Graph::GetComponents_DFS(const long max_components)
{
	return GetComponents_BFS(max_components);
}

Graph::GraphAnalytics Graph::RunAnalytics(const long max_components)
{
	GraphAnalytics data;

	auto components = GetComponents_BFS(max_components);
	data.num_components = components.size();
	data.num_nodes = nodes.size();
	data.num_edges = GetEdgeCount();

	data.num_components_powered = 0;
	data.avg_power_percentage = 0;
	for (auto& c : components)
	{
		double produced = 0;
		double consumed = 0;
		for (auto& n : c)
		{
			produced += nodes.at(n).produced;
			consumed += nodes.at(n).consumed;
		}
		if (produced >= consumed)
			data.num_components_powered++;
		double power_percentage = (produced / consumed);
		if (power_percentage > 1)
			power_percentage = 1;
		data.avg_power_percentage += power_percentage;
	}
	data.avg_power_percentage /= static_cast<double>(components.size());

	return data;
}

void Graph::Write(const std::string& filename) const
{
	std::cout << "Writing..." << std::endl;

	/*long i = 0;
	for (const auto& node : nodes)
		std::cout << "Node" << i++ << " Produces: " << node.produced << " Consumed: " << node.consumed << std::endl;

	long size = static_cast<long>(edges.size());
	for (long row = 0; row < size; row++)
	{
		for (long col = 0; col < static_cast<long>(edges.at(row).size()); col++)
			std::cout << edges.at(row).at(col) << " ";
		std::cout << "\n";
	}*/

	std::ofstream file(filename.c_str());
	if (!file.is_open())
		Log("Unable to open file to write graph: " + filename);
	else
	{
		long size = static_cast<long>(edges.size());
		for (long n = 0; n < size; n++)
		{
			file << nodes.at(n).produced << "," << nodes.at(n).consumed;
			for (long i = 0; i < size; i++)
				if (IsEdge(n, i))
					file << "," << i;
			if (n < size - 1)
				file << "\n";
		}
	}

	std::cout << "Done writing" << std::endl;
}




std::vector<std::string> split(std::string value, const std::string& delim, long max_pieces)
{
	std::vector<std::string> pieces;
	if (max_pieces == 1)
		pieces.push_back(value);
	else
	{
		auto idx = value.find_first_of(delim);
		if (idx == std::string::npos)
			pieces.push_back(value);
		else
		{
			pieces.push_back(value.substr(0, idx));
			auto _pieces = split(value.substr(idx + delim.length()), delim, max_pieces - 1);
			for (auto& _value : _pieces)
				pieces.push_back(_value);
		}
	}
	return std::move(pieces);
}

void Log(const std::string& message)
{
	std::cout << "[LOG] " << message << std::endl;

	std::ofstream file("runtime.log");
	if (!file.is_open())
		std::cout << "Unable access log file." << std::endl;
	else
	{
		file << message << std::endl;
		file.close();
	}
}