
#pragma once

#include <sc-memory/kpm/sc_agent.hpp>
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "keynodes/keynodes.hpp"
#include "CourseWorkAgent.generated.hpp"

namespace exampleModule
{

class CourseWorkAgent : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::example_action, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // 
