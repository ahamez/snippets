#include <iostream>
#include <stack>

////////////////////////////////////////////////////////////////////////////////////
struct node
{
  int x;
  node* l;
  node* r;
//  node* p;

  node()
  : x()
  , l(NULL)
  , r(NULL)
//  , p(NULL)
  {
  }

  node(int x, node* l, node* r)
    : x(x)
    , l(l)
    , r(r)
//    , p(NULL)
  {
//    l->p = this;
//    r->p = this;
  }

  node(int x)
  : x(x)
  , l(NULL)
  , r(NULL)
//  , p(NULL)
  {
  }

};

////////////////////////////////////////////////////////////////////////////////////
struct inorder_iterator
{

  node* current;
  std::stack<node*> stack;

  inorder_iterator(node* n)
    : current(n)
    , stack()
  {
    while (current->l)
    {
      stack.push(current);
      current = current->l;
    }
  }

  bool
  has_next()
  const
  {
    return current != NULL;
  }

  void
  next()
  {
    if(not current)
    {
      return;
    }

    if (stack.empty() and not current->r and not current->l)
    {
      current = NULL;
      return;
    }

    if (current->r)
    {
      current = current->r;
      while (current->l)
      {
        stack.push(current);
        current = current->l;
      }
      return;
    }

    if (not stack.empty())
    {
      current = stack.top();
      stack.pop();
    }
    else
    {
      current = NULL;
    }
  }

  const node*
  get()
  const
  {
    return current;
  }

};

////////////////////////////////////////////////////////////////////////////////////
struct preorder_iterator
{

  std::stack<node*> stack;

  preorder_iterator(node* n)
    : stack()
  {
    if (n)
    {
      stack.push(n);
    }
  }

  bool
  has_next()
  const
  {
    return not stack.empty();
  }

  void
  next()
  {
    const node* tmp = stack.top();
    stack.pop();
    if (tmp->r)
    {
      stack.push(tmp->r);
    }
    if (tmp->l)
    {
      stack.push(tmp->l);
    }
  }

  const node*
  get()
  const
  {
    return stack.top();
  }

};

////////////////////////////////////////////////////////////////////////////////////
struct postorder_iterator
{

  std::stack<node*> stack;
  node* current;
  node* next_;
  bool left_loop;
  bool end;

  postorder_iterator(node* n)
    : stack()
    , current(NULL)
    , next_(n)
    , left_loop(true)
    , end(false)
  {
    next();
  }

  bool
  has_next()
  const
  {
    return not end;
  }

  void
  next()
  {
    while (true)
    {
      if (left_loop)
      {
        current = next_;
        while (current != NULL)
        {
          stack.push(current);
          current = current->l;
        }
      }

      if (not stack.empty() and current == stack.top()->r)
      {
        current = stack.top();
        stack.pop();
        left_loop = false;
        return;
      }

      left_loop = true;

      if (stack.empty())
      {
        end = true;
        return;
      }

      next_ = stack.top()->r;
    }
  }

  const node*
  get()
  const
  {
    return current;
  }

};

////////////////////////////////////////////////////////////////////////////////////
void iterative_dfs_postorder(const node*);

void
iterative_dfs_postorder(const node* t)
{
  // left, right, root

  std::stack<const node*>  stack;

  const node* current = t;

  while (true)
  {
    while (current)
    {
      stack.push(current);
      current = current->l;
    }

    while (not stack.empty() and current == stack.top()->r)
    {
      current = stack.top();
      stack.pop();
      std::cout.width(3);
      std::cout << std::left << current->x;
    }

    if (stack.empty())
    {
      break;
    }

    current = stack.top()->r;
  }

  std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////
void iterative_dfs_preorder(const node*);

void
iterative_dfs_preorder(const node* t)
{
  // root, left, right

  std::stack<const node*>  stack;

  const node* current;
  stack.push(t);

  while (not stack.empty())
  {
    current = stack.top();
    stack.pop();
    const node* r = current->r;
    if (r)
    {
      stack.push(r);
    }
    const node* l = current->l;
    if (l)
    {
      stack.push(l);
    }

    std::cout.width(3);
    std::cout << std::left << current->x;
  }

  std::cout << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////
void iterative_dfs_inorder(const node*);

void
iterative_dfs_inorder(const node* t)
{
  // root, left, right

  std::stack<const node*>  stack;

  const node* current = t;

  while (true)
  {
    if (current)
    {
      stack.push(current);
      current = current->l;
      continue;
    }

    if (stack.empty())
    {
      return;
    }

    current = stack.top();
    stack.pop();
    std::cout.width(3);
    std::cout << std::left << current->x;
    current = current->r;
  }

  std::cout << std::endl;
}


////////////////////////////////////////////////////////////////////////////////////
int main (int argc, const char * argv[])
{
  node* t = new node(6, new node(2, new node(1)
                                  , new node(4, new node(3)
                                              , new node(5)
//                                              , NULL
                                            )
                                )
                      , new node(8, new node(7)
                                  , new node(9)
//                                  , NULL
                                )
                    );


//  std::cout << "sizeof(node) = " << sizeof(node) << std::endl;
  postorder_iterator it(t);

  while (it.has_next())
  {
    std::cout.width(3);
    std::cout << std::left << it.get()->x;
    it.next();
  }
  std::cout << std::endl;
  std::cout << std::endl;
  iterative_dfs_preorder(t);

  return 0;
}
