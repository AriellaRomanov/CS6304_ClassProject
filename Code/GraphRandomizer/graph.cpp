#include "graph.h"

Graph::Node::Node()
	: produced(0),
	consumed(0)
{

}

Graph::Node::Node(const long _produced, const long _consumed)
	: produced(_produced),
	consumed(_consumed)
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
	std::ifstream file(filename);
	if (!file.is_open())
		Log("Unable to read graph file: " + filename);
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
				SetEdge(i, stoi(components.at(j)));
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

void Graph::RemoveEdge(const long row, const long col)
{
	SetEdge(row, col, false);
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

long Graph::GetComponentCount() const
{
	return 1;
}

double Graph::GetAveragePowerPercentageSupplied() const
{
	return 1;
}

void Graph::CutEdges(const double percent)
{

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
	std::cout << message << std::endl;

	std::ofstream file("runtime.log");
	if (!file.is_open())
		std::cout << "Unable access log file." << std::endl;
	else
	{
		file << message << std::endl;
		file.close();
	}
}