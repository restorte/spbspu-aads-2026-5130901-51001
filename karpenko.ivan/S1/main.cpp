#include "list.hpp"
#include <iostream>
#include <string>
#include <utility>
#include <cstddef>
#include <limits>
#include <stdexcept>

namespace karpenko
{
  bool read_sequence(
    std::istream& in,
    std::string& name,
    List< size_t >& numbers)
  {
    if (!(in >> name))
    {
      return false;
    }
    numbers.clear();
    while (in.peek() != '\n' && in.peek() != EOF)
    {
      size_t value = 0;
      if (in >> value)
      {
        numbers.push_back(value);
      }
      else
      {
        in.clear();
        in.ignore();
        break;
      }
    }
    in.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    return true;
  }
}

int main()
{
  using namespace karpenko;

  List< std::pair< std::string, List< size_t > > > sequences;

  while (true)
  {
    std::string name;
    List< size_t > numbers;
    if (!read_sequence(std::cin, name, numbers))
    {
      break;
    }
    sequences.push_back(std::make_pair(name, std::move(numbers)));
  }

  if (sequences.empty())
  {
    std::cout << "0\n";
    return 0;
  }

  bool first = true;
  for (List< std::pair< std::string, List< size_t > > >::iterator it = sequences.begin();
       it != sequences.end(); ++it)
  {
    if (!first)
    {
      std::cout << ' ';
    }
    std::cout << it->first;
    first = false;
  }
  std::cout << '\n';

  size_t max_len = 0;
  for (List< std::pair< std::string, List< size_t > > >::const_iterator it = sequences.cbegin();
       it != sequences.cend(); ++it)
  {
    size_t len = it->second.size();
    if (len > max_len)
    {
      max_len = len;
    }
  }

  List< List< size_t > > transposed;
  bool has_numbers = false;

  for (size_t pos = 0; pos < max_len; ++pos)
  {
    List< size_t > new_seq;

    for (List< std::pair< std::string, List< size_t > > >::const_iterator seq_it = sequences.cbegin();
         seq_it != sequences.cend(); ++seq_it)
    {
      List< size_t >::const_iterator num_it = seq_it->second.begin();
      size_t curr = 0;

      while (curr < pos && num_it != seq_it->second.end())
      {
        ++curr;
        ++num_it;
      }

      if (num_it != seq_it->second.end())
      {
        new_seq.push_back(*num_it);
        has_numbers = true;
      }
    }

    if (!new_seq.empty())
    {
      transposed.push_back(std::move(new_seq));
    }
  }

  if (!has_numbers)
  {
    std::cout << "0\n";
    return 0;
  }

  for (List< List< size_t > >::iterator tit = transposed.begin();
       tit != transposed.end(); ++tit)
  {
    bool first_in_row = true;
    for (List< size_t >::const_iterator nit = tit->begin();
         nit != tit->end(); ++nit)
    {
      if (!first_in_row)
      {
        std::cout << ' ';
      }
      std::cout << *nit;
      first_in_row = false;
    }
    std::cout << '\n';
  }

  List< size_t > sums;

  for (List< List< size_t > >::const_iterator tit = transposed.cbegin();
       tit != transposed.cend(); ++tit)
  {
    size_t sum = 0;

    try
    {
      for (List< size_t >::const_iterator nit = tit->begin();
           nit != tit->end(); ++nit)
      {
        if (sum > MAX - *nit)
        {
          throw std::overflow_error("Sum overflow");
        }
        sum += *nit;
      }
    }
    catch (const std::overflow_error& e)
    {
      std::cerr << "Formed lists with exit code 1 and error message in standard error because of overflow\n";
      return 1;
    }

    sums.push_back(sum);
  }

  first = true;
  for (List< size_t >::const_iterator sit = sums.cbegin();
       sit != sums.cend(); ++sit)
  {
    if (!first)
    {
      std::cout << ' ';
    }
    std::cout << *sit;
    first = false;
  }
  std::cout << '\n';

  return 0;
}
