#include <iostream>
#include <boost/variant.hpp>
#include <queue>
#include <stack>
#include <cassert>
#include <stdexcept>
#include <tr1/functional>

////////////////////////////////////////////////////////////////////////////////////////
struct node;
struct leaf;

typedef boost::variant< boost::recursive_wrapper<leaf>
                      , boost::recursive_wrapper<node>
                      >
        tree_type;

struct node
{
  int value;
  tree_type r;
  tree_type l;

  node(int v, const tree_type& l, const tree_type& r)
  	: value(v)
  	, l(l)
  	, r(r)
  {
  }
};

struct leaf
{
  int value;

  leaf(int v)
  	: value(v)
  {
  }
};

////////////////////////////////////////////////////////////////////////////////////////
void
dfs_inorder(const tree_type& t);

void
dfs_inorder(const tree_type& t)
{
  // left, root, right
  struct visitor : boost::static_visitor<void>
  {
    void
    operator()(const node& n)
    const
    {
      boost::apply_visitor(*this, n.l);
      std::cout.width(3);
      std::cout << std::right << n.value;
      boost::apply_visitor(*this, n.r);
    }

    void
    operator()(const leaf& l)
    const
    {
      std::cout.width(3);
      std::cout << std::right << l.value;
    }
  };

  boost::apply_visitor(visitor(), t);
  std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////
struct is_node_visitor
	: public boost::static_visitor<bool>
{
  bool
  operator()(const node&)
  const
  {
    return true;
  }

  bool
  operator()(const leaf&)
  const
  {
    return false;
  }
};

is_node_visitor v1;
auto is_node = boost::apply_visitor(v1);

struct get_left_visitor
	: public boost::static_visitor<const tree_type*>
{
  const tree_type*
  operator()(const node& n)
  const
  {
    return &n.l;
  }

  const tree_type*
  operator()(const leaf&)
  const
  {
    throw std::logic_error("LWTF!");
  }
};

get_left_visitor v2;
auto get_left = boost::apply_visitor(v2);

struct get_right_visitor
	: public boost::static_visitor<const tree_type*>
{
  const tree_type*
  operator()(const node& n)
  const
  {
    return &n.r;
  }

  const tree_type*
  operator()(const leaf&)
  const
  {
    throw std::logic_error("RWTF!");
  }
};

get_right_visitor v3;
auto get_right = boost::apply_visitor(v3);


struct get_value_visitor
	: public boost::static_visitor<int>
{
  template<typename T>
  int
  operator()(const T& x)
  const
  {
    return x.value;
  }
};

get_value_visitor v4;
auto get_value = boost::apply_visitor(v4);


void
iterative_dfs_inorder(const tree_type& t);

void
iterative_dfs_inorder(const tree_type& t)
{
  // left, root, right
  typedef std::stack<const tree_type*> stack_type;
  stack_type stack;

  const tree_type* current = &t;

  while (true)
  {
    if (is_node(*current))
    {
      stack.push(current);
      current = get_left(*current);
      continue;
    }

    if (stack.empty())
    {
      break;
    }
    std::cout.width(3);
    std::cout << std::right << get_value(*current);
    current = stack.top();
    std::cout.width(3);
    std::cout << std::right << get_value(*current);
    stack.pop();
    current = get_right(*current);
  }
  std::cout.width(3);
  std::cout << std::right << get_value(*current);

  std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////
void
dfs_preorder(const tree_type& t);

void
dfs_preorder(const tree_type& t)
{
  // root, left, right
  struct visitor : boost::static_visitor<void>
  {
    void
    operator()(const node& n)
    const
    {
      std::cout.width(3);
      std::cout << std::right << n.value;
      boost::apply_visitor(*this, n.l);
      boost::apply_visitor(*this, n.r);
    }

    void
    operator()(const leaf& l)
    const
    {
      std::cout.width(3);
      std::cout << std::right << l.value;
    }
  };

  boost::apply_visitor(visitor(), t);
  std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////
void
iterative_dfs_preorder(const tree_type& t);

void
iterative_dfs_preorder(const tree_type& t)
{
  // root, left, right
  typedef std::stack<const tree_type*> stack_type;
  stack_type stack;

  stack.push(&t);
  while (not stack.empty())
  {
    std::cout.width(3);
    const tree_type* t = stack.top();
    stack.pop();
    std::cout << get_value(*t);
    if (is_node(*t))
    {
      stack.push(get_right(*t));
      stack.push(get_left(*t));
    }
  }
  std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////
void
dfs_postorder(const tree_type& t);

void
dfs_postorder(const tree_type& t)
{
  // left, right, root
  struct visitor : boost::static_visitor<void>
  {
    void
    operator()(const node& n)
    const
    {
      boost::apply_visitor(*this, n.l);
      boost::apply_visitor(*this, n.r);
      std::cout.width(3);
      std::cout << std::right << n.value;
    }

    void
    operator()(const leaf& l)
    const
    {
      std::cout.width(3);
      std::cout << std::right << l.value;
    }
  };

  boost::apply_visitor(visitor(), t);
  std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////
typedef std::stack<const tree_type*> stack_type;

std::ostream&
operator<<(std::ostream& os, stack_type);

std::ostream&
operator<<(std::ostream& os, stack_type s)
{
  while (not s.empty())
  {
    auto c = s.top();
    s.pop();
    os.width(3);
    os << std::right << get_value(*c);
  }
  return os;
}

void
iterative_dfs_postorder(const tree_type& t);

void
iterative_dfs_postorder(const tree_type& t)
{
  // left, right, root

  stack_type stack;

  const tree_type* current = &t;
  bool should_pop = false;

  while (true)
  {
    if (not is_node(*current))
    {
      std::cout.width(3);
      std::cout << std::right << get_value(*current);
    }

    if (is_node(*current) and not should_pop)
    {
      stack.push(get_right(*current));
      stack.push(current);
      current = get_left(*current);
      continue;
    }

    if (stack.empty())
    {
      break;
    }

    current = stack.top();
    stack.pop();

    if (should_pop)
    {
      should_pop = false;
    }

    if (is_node(*current) and not stack.empty() and get_right(*current) == stack.top())
    {
      stack.pop();
      stack.push(current);
      current = get_right(*current);
    }
    else
    {
      std::cout.width(3);
      std::cout << std::right << get_value(*current);
      should_pop = true;
    }
  }

  std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////
void
bfs(const tree_type& t);

void
bfs(const tree_type& t)
{
//  typedef std::stack<tree_type> queue_type; // dfs
  typedef std::queue<tree_type> queue_type;
  queue_type q;

  struct visitor : boost::static_visitor<void>
  {
    queue_type& q;

    visitor(queue_type& q)
    	: q(q)
    {
    }

    void
    operator()(const node& n)
    const
    {
      std::cout.width(3);
      std::cout << std::right << n.value;
      q.push(n.l);
      q.push(n.r);
    }

    void
    operator()(const leaf& l)
    const
    {
      std::cout.width(3);
      std::cout << std::right << l.value;
    }
  };

  q.push(t);
  visitor v(q);
  while (not q.empty())
  {
//    const tree_type& t = q.top(); // dfs
    const tree_type t = q.front();
    q.pop();
    boost::apply_visitor(v, t);
  }
  std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////
void
bfs2(const tree_type& t);

void
bfs2(const tree_type& t)
{
  typedef std::queue<tree_type> queue_type;
  queue_type q;

  struct visitor : boost::static_visitor<void>
  {
    queue_type& q;
    unsigned int& nodes_at_this_level;
    unsigned int& nodes_at_next_level;

    visitor(queue_type& q, unsigned int& nodes_at_this_level, unsigned int& nodes_at_next_level)
    : q(q)
    , nodes_at_this_level(nodes_at_this_level)
    , nodes_at_next_level(nodes_at_next_level)
    {
    }

    void
    operator()(const node& n)
    const
    {
      std::cout.width(3);
      std::cout << std::right << n.value;
      q.push(n.l);
      q.push(n.r);
      nodes_at_next_level += 2;
    }

    void
    operator()(const leaf& l)
    const
    {
      std::cout.width(3);
      std::cout << std::right << l.value;
    }
  };

  unsigned int nodes_at_this_level = 1;
  unsigned int nodes_at_next_level = 0;
  q.push(t);
  visitor v(q, nodes_at_this_level, nodes_at_next_level);
  while (not q.empty())
  {
    const tree_type& t = q.front();
    --nodes_at_this_level;
    boost::apply_visitor(v, t);
    if (nodes_at_this_level == 0)
    {
      std::cout << std::endl;
      nodes_at_this_level = nodes_at_next_level;
      nodes_at_next_level = 0;
    }
    q.pop();
  }
}

////////////////////////////////////////////////////////////////////////////////////////
void
bfs3(const tree_type& t);

void
bfs3(const tree_type& t)
{
  typedef std::queue<tree_type> queue_type;
  queue_type q_current_level;
  queue_type q_next_level;

  struct visitor : boost::static_visitor<void>
  {
    queue_type& q_current_level;
    queue_type& q_next_level;

    visitor(queue_type& q_curr, queue_type& q_nxt)
    : q_current_level(q_curr)
    , q_next_level(q_nxt)
    {
    }

    void
    operator()(const node& n)
    const
    {
      std::cout.width(3);
      std::cout << std::right << n.value;
      q_next_level.push(n.l);
      q_next_level.push(n.r);
    }

    void
    operator()(const leaf& l)
    const
    {
      std::cout.width(3);
      std::cout << std::right << l.value;
    }
  };

  q_current_level.push(t);
  visitor v(q_current_level, q_next_level);
  while (not q_current_level.empty())
  {
    const tree_type& t = q_current_level.front();
    boost::apply_visitor(v, t);
    q_current_level.pop();
    if (q_current_level.empty())
    {
      std::cout << std::endl;
      swap(q_current_level, q_next_level);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////
int
main ()
{
  tree_type t = node(6, node(2, leaf(1)
                              , node(4, leaf(3)
                                      , leaf(5)
                                     )
                            )
                      , node(8, leaf(7), leaf(9))
                    );

//  std::cout << "sizeof(tree_type) = " << sizeof(tree_type) << std::endl;
//  std::cout << "sizeof(node) = " << sizeof(node) << std::endl;
//  std::cout << "sizeof(leaf) = " << sizeof(leaf) << std::endl;
  //  std::cout << std::endl;
//  dfs_inorder(t);
//  std::cout << std::endl;
//  iterative_dfs_inorder(t);
//  std::cout << std::endl;
//  dfs_preorder(t);
//  std::cout << std::endl;
//  iterative_dfs_preorder(t);
//  std::cout << std::endl;
//  std::cout << std::endl;
  std::cout << "**************" << std::endl;
  dfs_postorder(t);
  std::cout << std::endl;
  std::cout << "**************" << std::endl;
  iterative_dfs_postorder(t);
  std::cout << std::endl;
//  bfs(t);
//  std::cout << std::endl;
//  bfs2(t);
//  std::cout << std::endl;
//  bfs3(t);
//  std::cout << std::endl;
  return 0;
}
