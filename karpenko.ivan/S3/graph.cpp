#include "graph.hpp"
#include "list.hpp"

namespace karpenko
{

  void addEdgeToMap(OuterEdgeMap& edges, const Vertex& from, const Vertex& to, Weight w)
  {
    auto it = edges.find(from);
    if (it == edges.end())
    {
      InnerEdgeMap inner;
      edge_weights_t ew;
      ew.weights.push_back(w);
      inner.add(to, ew);
      edges.add(from, inner);
    }
    else
    {
      InnerEdgeMap& inner = it->second;
      auto to_it = inner.find(to);
      if (to_it == inner.end())
      {
        edge_weights_t ew;
        ew.weights.push_back(w);
        inner.add(to, ew);
      }
      else
      {
        to_it->second.weights.push_back(w);
      }
    }
  }

  bool removeWeightFromMap(OuterEdgeMap& edges, const Vertex& from, const Vertex& to, Weight w)
  {
    auto it = edges.find(from);
    if (it == edges.end())
    {
      return false;
    }
    InnerEdgeMap& inner = it->second;
    auto to_it = inner.find(to);
    if (to_it == inner.end())
    {
      return false;
    }
    List < Weight >& weights = to_it->second.weights;

    for (auto cur = weights.begin(); cur != weights.end(); ++cur)
    {
      if (*cur == w)
      {
        detail::NodeBase* target = cur.get_ptr();

        if (cur == weights.begin())
        {
          weights.pop_front();
        }
        else if (target->next_ == weights.end().get_ptr())
        {
          weights.pop_back();
        }
        else
        {
          detail::NodeBase* prev = weights.end().get_ptr();
          auto scan = weights.begin();
          while (scan != weights.end() && scan.get_ptr() != target)
          {
            prev = scan.get_ptr();
            ++scan;
          }
          weights.erase_after(LIter < Weight >(prev));
        }

        if (weights.empty())
        {
          inner.remove(to);
        }
        return true;
      }
    }
    return false;
  }

  namespace detail
  {

    template < typename T >
    T* listToSortedArray(const List < T >& lst, std::size_t& outSize)
    {
      outSize = lst.size();
      if (outSize == 0)
      {
        return nullptr;
      }
      T* arr = static_cast < T* >(::operator new(sizeof(T) * outSize));
      std::size_t i = 0;
      for (auto it = lst.begin(); it != lst.end(); ++it, ++i)
      {
        new (arr + i) T(*it);
      }
      std::sort(arr, arr + outSize);
      return arr;
    }

  }

  void printEdges(std::ostream& out, const std::string& vertex, const OuterEdgeMap& edges)
  {
    auto it = edges.find(vertex);
    if (it == edges.end())
    {
      out << '\n';
      return;
    }
    const InnerEdgeMap& inner = it->second;
    List < std::string > targetList;
    for (auto ti = inner.begin(); ti != inner.end(); ++ti)
    {
      targetList.push_back(ti->first);
    }
    std::size_t n = 0;
    std::string* targets = detail::listToSortedArray(targetList, n);
    for (std::size_t t = 0; t < n; ++t)
    {
      const std::string& target = targets[t];
      auto wi = inner.find(target);
      const List < Weight >& wlist = wi->second.weights;
      std::size_t wc = 0;
      Weight* sorted_weights = detail::listToSortedArray(wlist, wc);
      out << target;
      for (std::size_t w = 0; w < wc; ++w)
      {
        out << ' ' << sorted_weights[w];
      }
      out << '\n';
      for (std::size_t w = 0; w < wc; ++w)
      {
        sorted_weights[w].~Weight();
      }
      ::operator delete(sorted_weights);
    }
    for (std::size_t t = 0; t < n; ++t)
    {
      targets[t].~basic_string();
    }
    ::operator delete(targets);
    if (n == 0)
    {
      out << '\n';
    }
  }

  void printSortedVertices(std::ostream& out, const Graph& g)
  {
    List < std::string > vertList;
    for (auto it = g.vertices_.begin(); it != g.vertices_.end(); ++it)
    {
      vertList.push_back(it->first);
    }
    std::size_t n = 0;
    std::string* verts = detail::listToSortedArray(vertList, n);
    if (n == 0)
    {
      out << '\n';
    }
    else
    {
      for (std::size_t i = 0; i < n; ++i)
      {
        out << verts[i] << '\n';
      }
    }
    for (std::size_t i = 0; i < n; ++i)
    {
      verts[i].~basic_string();
    }
    ::operator delete(verts);
  }

  bool hasWeightInMap(const OuterEdgeMap& edges, const Vertex& from, const Vertex& to, Weight w)
  {
    auto it = edges.find(from);
    if (it == edges.end())
    {
      return false;
    }
    const InnerEdgeMap& inner = it->second;
    auto to_it = inner.find(to);
    if (to_it == inner.end())
    {
      return false;
    }
    const List < Weight >& weights = to_it->second.weights;
    for (auto wi = weights.cbegin(); wi != weights.cend(); ++wi)
    {
      if (*wi == w)
      {
        return true;
      }
    }
    return false;
  }

}
