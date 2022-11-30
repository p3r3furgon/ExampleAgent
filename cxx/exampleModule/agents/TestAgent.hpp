#pragma once

#include <sc-memory/kpm/sc_agent.hpp>
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "keynodes/keynodes.hpp"
#include "TestAgent.generated.hpp"

namespace exampleModule
{

class TestAgent : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::test_action, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

}