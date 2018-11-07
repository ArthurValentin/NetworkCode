#include "network.h"
#include "random.h"
#include <algorithm>
#include <stdexcept>

struct greater
{
    template<class T>
    bool operator()(T const &a, T const &b) const 
    {
		return a > b;
	}
};

void Network::resize(const size_t& size)
{
	values.resize(size);
	RandomNumbers rn;
	rn.normal(values);
}

bool Network::add_link(const size_t& a, const size_t& b)
{
	if (a < values.size() and b < values.size() and a != b and !is_pair_in_links(std::pair<size_t, size_t>(a,b)))
	{
		links.emplace(a,b);
		links.emplace(b,a);
		return true;
	}
	return false;
}

size_t Network::random_connect(const double& mean_deg)
{
	links.clear();
	
	size_t size = values.size();
	std::vector<size_t> deg_n_values(size, 0);			// stores the deg(n) values for each node
	std::vector<size_t> actual_connections_nb(size, 0);	// stores the nb of connections that each node has yet 
	RandomNumbers rn;
	rn.poisson(deg_n_values, mean_deg);
	
	for (size_t i(0); i < size; ++i)
	{
		if (deg_n_values[i] >= size-1)
		{
			deg_n_values[i] = size-1;
		}
		while (actual_connections_nb[i] < deg_n_values[i])
		{
			int index(rn.uniform_int(i, size-1));
			if (actual_connections_nb[index] < deg_n_values[index] and add_link(i, size_t(index)))
			{
				actual_connections_nb[i] += 1;
				actual_connections_nb[index] += 1;
			}
			else
			{
				size_t j(i+1);
				while ((actual_connections_nb[j] >= deg_n_values[j] or !add_link(i, j)) and j < size)
				{
					++j;
				}
				if (j == size)
				{
					deg_n_values[i] = actual_connections_nb[i];
				}
				else
				{
					actual_connections_nb[i] += 1;
					actual_connections_nb[j] += 1;
				}
			}
		}
	}
	
	return links.size();
}

size_t Network::set_values(const std::vector<double>& new_vect)
{
	size_t n(std::min(values.size(), new_vect.size()));
	for (size_t i(0); i < n; ++i)
	{
		values[i] = new_vect[i];
	}
	return n;
}

size_t Network::size() const
{
	return values.size();
}

size_t Network::degree(const size_t& n) const
{
	return links.count(n);
}

double Network::value(const size_t & n) const
{
	if (n >= values.size())
	{
		throw std::out_of_range("n greater than values.size()");
	}
	return values[n];
}

std::vector<double> Network::sorted_values() const
{
	std::vector<double> sorted_values(values);
	std::sort(sorted_values.begin(), sorted_values.end(), greater());
	return sorted_values;
}

std::vector<size_t> Network::neighbors(const size_t& n) const
{
	std::vector<size_t> neighbors;
	for (auto I : links)
	{
		if (I.first == n)
		{
			neighbors.push_back(I.second);
		}
	}
	return neighbors;
}

bool Network::is_pair_in_links(const std::pair<size_t, size_t>& my_pair)
{
	for (const auto& I : links)
	{
		if (I.first == my_pair.first and I.second == my_pair.second)
		{
			return true;
		}
	}
	return false;
}
