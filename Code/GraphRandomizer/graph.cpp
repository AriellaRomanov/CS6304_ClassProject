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
	std::cout << "Reading graph file" << std::endl;

	std::ifstream file(filename);
	if (!file.is_open())
		Log("Unable to read graph file: " + filename);
	else
	{
		std::vector<std::string> lines;

		std::string line;
		while (std::getline(file, line))
		{
			std::cout << "line: " << line << std::endl;
			lines.push_back(line);
		}
		file.close();

		auto size = static_cast<long>(lines.size());
		std::cout << "size: " << size << std::endl;
		
		nodes.reserve(size);
		for (long i = 0; i < size; i++)
			nodes.emplace_back();

		edges.reserve(size);
		for (long i = 0; i < size; i++)
		{
			edges.emplace_back();
			edges.at(i).reserve(size - i);
			for (long j = 0; j < size - i; j++)
				edges.at(i).emplace_back(false);
		}


		for (int r = 0; r < static_cast<long>(edges.size()); r++)
		{
			std::cout << r << ": ";
			for (int c = 0; c < static_cast<long>(edges.at(r).size()); c++)
				std::cout << edges.at(r).at(c) << " ";
			std::cout << std::endl;
		}


		for (int i = 0; i < size; i++)
		{
			std::cout << "i: " << i << std::endl;
			auto components = split(lines.at(i), ",");
			auto _size = static_cast<long>(components.size());
			std::cout << "_size: " << _size << std::endl;
			
			Node n;
			n.consumed = (_size > 1) ? stod(components.at(0)) : 0;
			n.produced = (_size > 2) ? stod(components.at(1)) : 0;
			nodes.emplace_back(n);

			for (int j = 2; j < _size; j++)
			{
				std::cout << "j: " << j << std::endl;
				auto neighbor = components.at(j);
				auto row = (i > stoi(neighbor)) ? i : stoi(neighbor);
				auto col = (i < stoi(neighbor)) ? i : stoi(neighbor);

				std::cout << "(row, col): (" << row << ", " << col << ")" << std::endl;
				edges.at(row).at(col) = true;
			}
		}
	}

	std::cout << "Done reading graph file" << std::endl;
	Write("");
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
	long edge_count = 0;
	auto size = static_cast<long>(nodes.size());

	for (int row = 0; row < size; row++)
		for (int col = 0; col < row; col++)
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

	long i = 0;
	for (const auto& node : nodes)
		std::cout << "Node" << i++ << " Produces: " << node.produced << " Consumed: " << node.consumed << std::endl;

	long size = static_cast<long>(nodes.size());
	for (long row = 0; row < size; row++)
	{
		for (long col = 0; col < row + 1; col++)
			std::cout << edges.at(row).at(col) << " ";
		std::cout << "\n";
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