#include <algorithm>
#include <algorithm>
#include <iostream>
#include <tr1/array>
#include <utility>
#include <vector>
#include <queue>
#include <tr1/unordered_map>

////////////////////////////////////////////////////////////////////////////////////
std::string tab(int size);

std::string
tab(int size)
{
  std::string res;
  for( std::size_t i = 0; i < size; ++i)
  {
    res += " ";
  }
  return res;
}

////////////////////////////////////////////////////////////////////////////////////
struct coord_type
{
  std::size_t x;
  std::size_t y;

  coord_type()
  : x(std::numeric_limits<std::size_t>::max())
  , y(std::numeric_limits<std::size_t>::max())
  {
  }
  
  coord_type(std::size_t x, std::size_t y)
  	: x(x)
    , y(y)
  {
  }
  
  bool
  operator==(const coord_type& rhs)
  const
  {
    return this->x == rhs.x and this->y == rhs.y;
  }
  
  bool
  operator!=(const coord_type& rhs)
  const
  {
    return not (*this == rhs);
  }
  
  bool
  operator<(const coord_type& rhs)
  const
  {
    return this->x < rhs.x and this->y < rhs.y;
  }
};

////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const coord_type& c);

std::ostream&
operator<<(std::ostream& os, const coord_type& c)
{
  os << "{";
  if (c.x == std::numeric_limits<std::size_t>::max())
  {
    os << "~";
  }
  else
  {
    os << c.x;
  }
  os << ",";
  if (c.y == std::numeric_limits<std::size_t>::max())
  {
    os << "~";
  }
  else
  {
    os << c.y;
  }
  os << "}";
  return os;
}

////////////////////////////////////////////////////////////////////////////////////
const unsigned int grid_size = 3;
typedef std::tr1::array<unsigned char, grid_size> line_type;
typedef std::tr1::array< line_type, grid_size > grid_type;
typedef std::vector<coord_type> successors_type;
typedef std::vector<coord_type> path_type;
typedef std::vector<path_type> paths_type;

////////////////////////////////////////////////////////////////////////////////////
bool operator<(const path_type& lhs, const path_type& rhs);


bool
operator<(const path_type& lhs, const path_type& rhs)
{
  return lhs.size() < rhs.size();
}

////////////////////////////////////////////////////////////////////////////////////
successors_type successors(const grid_type& grid, const coord_type& c);

successors_type
successors(const grid_type& grid, const coord_type& c)
{
  successors_type res;

  // x - 1, y
  if (c.x > 0 and grid[c.y][c.x - 1])
  {
    res.push_back(coord_type(c.x - 1, c.y));
  }

  // x + 1, y
  if (c.x < grid_size and grid[c.y][c.x + 1])
  {
    res.push_back(coord_type(c.x + 1, c.y));
  }

  // x, y - 1
  if (c.y > 0 and grid[c.y - 1][c.x])
  {
    res.push_back(coord_type(c.x, c.y - 1));
  }

  // x, y + 1
  if (c.y < grid_size and grid[c.y + 1][c.x])
  {
    res.push_back(coord_type(c.x, c.y + 1));
  }
  
  return res;
}

////////////////////////////////////////////////////////////////////////////////////
void
helper( const grid_type& grid, grid_type& on_stack, const coord_type& current, const coord_type& dst
       , paths_type& paths, path_type& path, int depth = 0);

void
helper( const grid_type& grid, grid_type& on_stack, const coord_type& current, const coord_type& dst
      , paths_type& paths, path_type& stack, int depth)
{

//  std::cout << tab(depth) << current << std::endl;
  stack.push_back(current);
  on_stack[current.y][current.x] = true;
  
  if (current == dst)
  {
//    std::cout << "found dst" << std::endl;
    paths.push_back(path_type(stack));
  }
  else
  {
    for (auto c : successors(grid, current))
    {
      if (not on_stack[c.y][c.x])
      {
        helper(grid, on_stack, c, dst, paths, stack, depth + 2);
      }
    }
  }
  
  stack.pop_back();
  on_stack[current.y][current.x] = false;
}

////////////////////////////////////////////////////////////////////////////////////
struct hash_coord
{
  std::size_t
  operator()(const coord_type& c)
  const
  {
    return std::tr1::hash<std::size_t>()(c.x) xor std::tr1::hash<std::size_t>()(c.y);
  };
};

//typedef std::vector<std::pair<path_type::const_iterator, path_type::const_iterator>> tails_type;
typedef std::vector<path_type> tails_type;
typedef std::tr1::unordered_map< coord_type
                               , tails_type
                               , hash_coord
                               >
        set_type;

void
helper2( const grid_type& grid, grid_type& on_stack, const coord_type& current, const coord_type& dst
       , paths_type& paths, path_type& path, set_type&, int depth = 0);

void
helper2( const grid_type& grid, grid_type& on_stack, const coord_type& current, const coord_type& dst
       , paths_type& paths, path_type& stack, set_type& dyn, int depth)
{
//  std::cout << tab(depth) << current << std::endl;
  stack.push_back(current);
  if (current == dst)
  {
    paths.push_back(path_type(stack));
    const auto& tail = paths.back();    
    for (auto tail_cit = tail.begin(); tail_cit != tail.end(); ++tail_cit)
    {
      auto& tails = dyn.insert(make_pair(*tail_cit, tails_type())).first->second;
      tails.push_back(path_type(tail_cit, tail.end()));
    }
  }
  else
  {  
    const auto search = dyn.find(current);
    std::vector<coord_type> already_visited;
    if (search != dyn.end())
    {
      for (const auto& tail : search->second)
      {
        paths.push_back(path_type(stack));
        std::copy(tail.begin(), tail.end(), std::inserter(paths.back(), paths.back().end()));
        already_visited.push_back(tail.front());
      }    
    }

    on_stack[current.y][current.x] = true;
    for (const auto c : successors(grid, current))
    {
      if (std::find(already_visited.begin(), already_visited.end(), c) == already_visited.end())
      {
        if (not on_stack[c.y][c.x])
        {
          helper2(grid, on_stack, c, dst, paths, stack, dyn, depth + 2);
        }        
      }
    }

    on_stack[current.y][current.x] = false;
  }
  stack.pop_back();
}


////////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const path_type& path);

std::ostream&
operator<<(std::ostream& os, const path_type& path)
{
  grid_type grid;
//  const line_type line = {0,0,0,0,0,0,0,0,0,0};
  const line_type line = {0,0,0};
  grid.assign(line);

  auto cit = path.begin();
  grid[cit->y][cit->y] = 'o';
  for (++cit; cit != path.end(); ++cit)
  {
//    grid[coord.y][coord.x] = true;
    const auto next = cit + 1;
    if (next != path.end())
    {
      if (next->x != cit->x and next->y != cit->y)
      {
        grid[cit->y][cit->x] = '+';
      }
      else if(next->x != cit->x and next->y == cit->y)
      {
        grid[cit->y][cit->x] = '-';
      }
      else if(next->x == cit->x and next->y != cit->y)
      {
        grid[cit->y][cit->x] = '|';
      }
    }
    else
    {
      grid[cit->y][cit->x] = 'x';
    }
  }
  
  for (auto x : grid)
  {
    for (auto y : x)
    {
      os << static_cast<unsigned char>(y ? y : ' ') << " ";
    }
    os << std::endl;
  }
  
  return os;
}

////////////////////////////////////////////////////////////////////////////////////
enum method {brute,dyn};

path_type
find_shortest_path(const grid_type& grid, const coord_type& src, const coord_type& dst, method m = brute);

path_type
find_shortest_path(const grid_type& grid, const coord_type& src, const coord_type& dst, method m)
{
  if (not grid[src.y][src.x] or not grid[dst.y][dst.x])
  {
    return path_type();
  }
  
  paths_type paths;
  path_type path;
  grid_type on_stack;
  set_type dyn;
//  const line_type line = {0,0,0,0,0,0,0,0,0,0};
  const line_type line = {0,0,0};
  on_stack.assign(line);

  if (m == method::dyn)
  {
    helper2(grid, on_stack, src, dst, paths, path,dyn);
  }
  else
  {
    helper(grid, on_stack, src, dst, paths, path);
  }

  std::cout << std::endl << paths.size() << " path(s)" << std::endl;
  for (auto p : paths)
  {
    std::cout << p << std::endl << std::endl;
  }
  
  return *std::min_element(paths.begin(), paths.end());
}

////////////////////////////////////////////////////////////////////////////////////
path_type
dijkstra(const grid_type& grid, const coord_type& src, const coord_type& dst);

typedef std::tr1::array<unsigned int, 10> distance_line_type;
typedef std::tr1::array<distance_line_type, 10 > distance_grid_type;

std::ostream&
operator<<(std::ostream& os, const distance_grid_type& d);

std::ostream&
operator<<(std::ostream& os, const distance_grid_type& d)
{
  for (auto x : d)
  {
    for (auto y : x)
    {
      if (y == std::numeric_limits<unsigned int>::max())
      {
        os.width(2); os << std::right << "~~";
      }
      else
      {
        os.width(2); os << std::right << y;
      }
      os << " ";
    }
    os << std::endl;
  }
  
  return os;
}


typedef std::tr1::array<coord_type, 10> predecessor_line_type;
typedef std::tr1::array<predecessor_line_type, 10 > predecessor_grid_type;

std::ostream&
operator<<(std::ostream& os, const predecessor_grid_type& p);

std::ostream&
operator<<(std::ostream& os, const predecessor_grid_type& p)
{
  for (auto line : p)
  {
    for (auto y : line)
    {
      os << y << " ";
    }
    os << std::endl;
  }
  return os;
}


struct compare_coord_distance
{
  const distance_grid_type& d;
  
  compare_coord_distance(const distance_grid_type& d)
  	: d(d)
  {
  }
  
  bool
  operator()(const coord_type& lhs, const coord_type& rhs)
  const
  {
    // reverse comparison to make the priority_queue behave like a min-heap
    return d[lhs.y][lhs.x] >= d[rhs.y][rhs.x];
  }
};

typedef std::priority_queue<coord_type, std::vector<coord_type>, compare_coord_distance > queue_type;

unsigned int
weight(const grid_type&, const coord_type&, const coord_type&);

unsigned int
weight(const grid_type&, const coord_type&, const coord_type&)
{
  return 1;
}

path_type
dijkstra(const grid_type& grid, const coord_type& src, const coord_type& dst)
{
  distance_line_type distance_line;
  distance_line.assign(std::numeric_limits<unsigned int>::max());
  distance_grid_type d;
  d.assign(distance_line);
  d[src.y][src.x] = 0;
  
  predecessor_line_type predecessor_line;
  predecessor_line.assign(coord_type());
  predecessor_grid_type p;
  p.assign(predecessor_line);

  queue_type f((compare_coord_distance(d)));
  f.push(src);
  
  while (not f.empty())
  {
    const coord_type u = f.top();
    f.pop();

    if (u == dst)
    {
      break;
    }

    for (auto v: successors(grid, u))
    {
      const unsigned int alt = d[u.y][u.x] + weight(grid, u, v);
      if (d[v.y][v.x] > alt)
      {
        d[v.y][v.x] = alt;
        p[v.y][v.x] = u;
        f.push(v);
      }
    }
  }
  
  std::cout << d << std::endl;
  std::cout << std::endl;
  std::cout << p << std::endl;
  
  path_type res;
  coord_type n = dst;
  while (n != src)
  {
    res.push_back(n);
    n = p[n.y][n.x];
  }
  res.push_back(src);
  
  return res;
}

////////////////////////////////////////////////////////////////////////////////////
int
main ()
{
//  const grid_type grid =
//  {{
//     {0,1,1,1,1,1,1,1,1,1}
//    ,{0,1,0,1,1,0,1,1,0,1}
//    ,{0,1,1,0,0,1,0,1,0,1}
//    ,{0,0,1,0,0,1,0,1,0,1}
//    ,{0,0,1,1,0,1,0,1,0,1}
//    ,{0,0,0,1,1,1,0,1,0,1}
//    ,{0,0,0,0,1,1,1,1,1,1}
//    ,{0,0,0,0,0,1,0,0,0,0}
//    ,{0,0,0,0,0,1,0,0,0,0}
//    ,{0,0,0,0,0,1,0,0,0,0}
//  }};

  grid_type grid;
  const line_type line = {1,1,1};
  grid.assign(line);

//  std::cout << find_shortest_path(grid, coord_type(0,0), coord_type(2,2)) << std::endl;
  std::cout << find_shortest_path(grid, coord_type(0,0), coord_type(2,2), method::dyn) << std::endl;
  //  std::cout << dijkstra(grid, coord_type(1,0), coord_type(9,6)) << std::endl;
  //  std::cout << find_shortest_path(grid, coord_type(9,6), coord_type(5,9)) << std::endl;
  std::cout << "OK" << std::endl;
  return 0;
}