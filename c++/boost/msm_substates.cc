#include <iostream>
#include <memory>

#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;

struct Data
{
  int x = 0;
};

struct Visitor
{
  std::vector<std::string> names;

  template <typename T>
  void
  visit_state(const T&)
  {
    names.emplace_back(T::name());
  }
};

struct VisitableState
{
  using accept_sig = msm::back::args<void, Visitor&>;

  virtual ~VisitableState()
  {}

  void
  accept(Visitor&)
  const
  {}
};

/*----------------------------------------------------------------------------*/

struct SubEvt0{};
struct SubEvt1{};

struct SubState0
  : public msm::front::state<VisitableState>
{
  template <typename Event, typename SM>
  void
  on_entry(const Event&, SM& sm)
  {
    std::cout << "Entering SubState0\n";
    ++sm.data->x;
  }

  template <typename Event, typename SM>
  void
  on_exit(const Event&, const SM&)
  {
    std::cout << "Exiting SubState0\n";
  }

  void
  accept(Visitor& v)
  const
  {
    v.visit_state(*this);
  }

  static
  std::string
  name()
  {
    return "SubState0";
  }
};

struct SubState1
  : public msm::front::state<VisitableState>
{
  void
  accept(Visitor& v)
  const
  {
    v.visit_state(*this);
  }

  static
  std::string
  name()
  {
    return "SubState1";
  }
};

struct SubStateMachine_
  : public msm::front::state_machine_def<SubStateMachine_, VisitableState>
{
  std::shared_ptr<Data> data;

  using initial_state = SubState0;

  struct transition_table
    : boost::mpl::vector<
      Row < SubState0
          , SubEvt0
          , SubState1
          , none
          , none
          >
    , Row < SubState1
          , SubEvt1
          , SubState0
          , none
          , none
          >
      >
  {};

  template <class FSM,class Event>
  void no_transition(Event const& e, FSM&,int state)
  {
    std::cout << "no transition from sub state " << state
              << " on event " << typeid(e).name() << std::endl;
  }

  void
  setData(std::shared_ptr<Data> d)
  {
    data = d;
  }
};
using SubStateMachine = msm::back::state_machine<SubStateMachine_>;

/*----------------------------------------------------------------------------*/

struct Evt0{};
struct Evt1{};
struct EvtError{};
struct EvtRecoverableError{};
struct EvtRecoverError{};
struct EvtInternalError{};

struct S0_
  : msm::front::state_machine_def<S0_, VisitableState>
{
  struct Sub_
    : SubStateMachine
  {};

  using initial_state = Sub_;

  void
  accept(Visitor& v)
  const
  {
    v.visit_state(*this);
  }

  static
  std::string
  name()
  {
    return "S0";
  }

  void
  setData(std::shared_ptr<Data> d)
  {}
};
using S0 = msm::back::state_machine<S0_>;

struct S1
  : public msm::front::state<VisitableState>
{
  using deferred_events = mpl::vector<Evt0>;

  void
  accept(Visitor& v)
  const
  {
    v.visit_state(*this);
  }

  static
  std::string
  name()
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

  static
  std::string
  name()
  {
    return "OK";
  }
};

struct Error
  : public msm::front::terminate_state<VisitableState>
{
  void
  accept(Visitor& v)
  const
  {
    v.visit_state(*this);
  }

  static
  std::string
  name()
  {
    return "Error";
  }
};

struct RecoverableError
  : public msm::front::interrupt_state<EvtRecoverError, VisitableState>
{
  void
  accept(Visitor& v)
  const
  {
    v.visit_state(*this);
  }

  static
  std::string
  name()
  {
    return "RecoverableError";
  }
};

struct StateMachine_
  : public msm::front::state_machine_def<StateMachine_, VisitableState>
{
  using initial_state = mpl::vector<S0, OK>;

  std::shared_ptr<Data> data;

  struct transition_table
    : boost::mpl::vector<
      Row < S0
          , Evt0
          , S1
          , none
          , none
          >
    , Row < S1
          , Evt1
          , S0
          , none
          , none
          >
    , Row < OK
          , EvtError
          , Error
          , none
          , none
          >
    , Row < OK
          , EvtRecoverableError
          , RecoverableError
          , none
          , none
          >
    , Row < RecoverableError
          , EvtRecoverError
          , OK
          , none
          , none
          >
      >
  {};

  void
  setData(std::shared_ptr<Data> d)
  {
    data = d;
  }

  template <class FSM,class Event>
  void no_transition(Event const& e, FSM&,int state)
  {
    std::cout << "no transition from state " << state
              << " on event " << typeid(e).name() << std::endl;
  }
};
using StateMachine = msm::back::state_machine<StateMachine_>;

/*----------------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------------*/

template <bool Cond, typename Ret>
using enable_if = typename std::enable_if<Cond, Ret>::type;

template <typename SM, typename STT>
class Visit
{
public:

  Visit(SM& sm, std::shared_ptr<Data> data)
    : m_sm{sm}
    , m_data{std::move(data)}
  {}

  template <typename StateType>
  enable_if<boost::msm::back::is_composite_state<StateType>::value, void>
  operator()(const boost::msm::wrap<StateType>&)
  {
    StateType& s = m_sm.template get_state<StateType&>();
    s.setData(m_data);
    std::cout << "Composite state " << typeid(StateType).name() << '\n';
    init(s, m_data);
  }

  template <typename StateType>
  enable_if<not boost::msm::back::is_composite_state<StateType>::value, void>
  operator()(const boost::msm::wrap<StateType>&)
  {}

private:

  SM& m_sm;
  std::shared_ptr<Data> m_data;
};

template <typename SM>
void
init(SM& sm, std::shared_ptr<Data> data)
{
  using STT = typename SM::stt;
  using all_states = typename boost::msm::back::generate_state_set<STT>::type;
  boost::mpl::for_each<all_states, boost::msm::wrap<boost::mpl::placeholders::_1>> (Visit<SM, STT>{sm, std::move(data)});
}

/*----------------------------------------------------------------------------*/

int
main()
{
  std::cout << "A\n";
  auto data = std::make_shared<Data>();
  auto sm = StateMachine{};

  init(sm, data);

  // auto sm = StateMachine{msm::back::states_ << S0{msm::back::states_  << S0::Sub_{msm::back::states_ << SubStateMachine_{data}}}, data};
  // auto& s0 = sm.get_state<S0&>().get_state<S0::Sub_&>();
  // auto& s0 = sm.get_state<S0&>().get_state<S0::Sub_&>();
  // s0.data = data;
  sm.start();
  pstate(sm);

  // std::cout << data->x << '\n';

  std::cout << "A.1\n";
  sm.process_event(SubEvt0{});
  pstate(sm);

  // std::cout << data->x << '\n';

  std::cout << "A.2\n";
  sm.process_event(SubEvt1{});
  pstate(sm);

  // std::cout << data->x << '\n';

  std::cout << "B\n";
  sm.process_event(Evt0{});
  pstate(sm);

  std::cout << "C\n";
  sm.process_event(Evt1{});
  pstate(sm);

  std::cout << "D\n";
  sm.process_event(EvtRecoverableError{});
  pstate(sm);

  std::cout << "E\n";
  sm.process_event(Evt0{});
  pstate(sm);

  std::cout << "F\n";
  sm.process_event(EvtRecoverError{});
  pstate(sm);

  std::cout << "G\n";
  sm.process_event(EvtError{});
  pstate(sm);

  std::cout << "H\n";
  sm.process_event(Evt0{});
  pstate(sm);

  std::cout << "I\n";
  sm.process_event(EvtRecoverError{});
  pstate(sm);

  std::cout << "J\n";
  sm.start();
  sm.process_event(Evt0{});
  pstate(sm);

  std::cout << "K\n";
  sm.process_event(Evt0{});
  sm.process_event(Evt0{});
  pstate(sm);

  std::cout << "L\n";
  sm.process_event(Evt1{});
  pstate(sm);

  std::cout << "M\n";
  sm.process_event(Evt1{});
  pstate(sm);

  return 0;
}
