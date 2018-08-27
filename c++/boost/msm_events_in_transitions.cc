
#include <iostream>
#include <vector>
#include <string>

#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;

struct Visitor
{
  std::vector<std::string> names;

  template <typename T>
  void
  visit_state(const T& x)
  {
    names.emplace_back(x.name());
  }
};

struct VisitableState
{
  using accept_sig = msm::back::args<void, Visitor&>;

  virtual ~VisitableState()
  {}

  void
  accept(Visitor& v)
  const
  {
    v.visit_state(*this);
  }

  virtual
  std::string
  name()
  const
  {
    return "@";
  }
};

struct Evt0{};
struct Evt1{};
struct Evt2{};

struct Action0
{
  template <typename SM, typename Evt, typename Src, typename Target>
  void
  operator()(const Evt&, SM& sm, const Src&, const Target&)
  {
    sm.process_event(Evt1{});
  }
};

struct S0
  : public msm::front::state<VisitableState>
{
  std::string
  name()
  const override
  {
    return "S0";
  }
};

struct S1
  : public msm::front::state<VisitableState>
{
  std::string
  name()
  const override
  {
    return "S1";
  }
};

struct OK
  : public msm::front::state<VisitableState>
{
  void
  accept(Visitor& v)
  const
  {
    v.visit_state(*this);
  }

  std::string
  name()
  const override
  {
    return "OK";
  }
};

struct KO
  : public msm::front::terminate_state<VisitableState>
{
  std::string
  name()
  const override
  {
    return "KO";
  }
};


struct StateMachine_
  : public msm::front::state_machine_def<StateMachine_, VisitableState>
{
  using initial_state = mpl::vector<S0>;

  struct transition_table
    : boost::mpl::vector<
      Row < S0
          , Evt0
          , S1
          , Action0
          , none
          >
    , Row < S1
          , Evt1
          , OK
          , none
          , none
          >
    , Row < S1
          , Evt2
          , KO
          , none
          , none
          >
      >
  {};
};
using StateMachine = msm::back::state_machine<StateMachine_>;

void
pstate(StateMachine& sm)
{
  std::cout << "Active states:\n";
  auto v = Visitor{};
  sm.visit_current_states(boost::ref(v));
  for (const auto& name : v.names)
  {
    std::cout << " - " << name << '\n';
  }
}

int
main()
{
  std::cout << "A\n";
  auto sm = StateMachine{};
  sm.start();
  pstate(sm);

  std::cout << "B\n";
  sm.process_event(Evt0{});
  pstate(sm);

  return 0;
}
