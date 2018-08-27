template<typename T>
struct node
  : public T
{
  const std::string id;

  template <typename… Args>
  node(const std::string& id, Args…&& args)
  	: T(std::forward<Args...>(args)...), id(id)
  {}
};

struct place_specific
{
  marking_type marking;

  place_specific()
  	: marking(0)
  {}
};
using place = node<place_specific>

struct transition_specific
{
  transition_specific() {}
};
using transition = node<transition_specific>



