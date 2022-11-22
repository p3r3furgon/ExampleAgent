#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_link.hpp>
#include <sc-memory/sc_common_templ.hpp>

#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "CourseWorkAgent.hpp"

#include <vector>

using namespace std;
using namespace utils;

namespace exampleModule
{

SC_AGENT_IMPLEMENTATION(CourseWorkAgent)
{

  SC_LOG_ERROR("testAgent started");

  ScLog *logger = ScLog::GetInstance();

  ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);
  ScAddr param = IteratorUtils::getAnyFromSet(ms_context.get(), questionNode);
  if (!param.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  ScAddr answer = ms_context->CreateNode(ScType::NodeConstStruct);
  //ScAddr visited = ms_context->CreateNode(ScType::NodeConstClass);
  //ScAddr globalVisited = ms_context->CreateNode(ScType::NodeConstClass);
  int tmpSize = 0;
  std::vector<int> distanceVector;
  ScAddr structure = IteratorUtils::getAnyFromSet(ms_context.get(), param);
  ScAddr firstNode;

  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, structure);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, Keynodes::visited);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, param);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, Keynodes::globalVisited);

  ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(it3->Next())
  {
    firstNode = it3->Get(2);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, firstNode);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::visited, firstNode);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::globalVisited, firstNode);
    break;
  }



  //const std::string name = "ExampleName";
 
  //ScAddr linkAddr;
  //ScLink link((ScMemoryContext&) ms_context, &linkAddr);
  //link.Set<std::string>("dsafa");

  
  //ScAddr const linkAddr2 = sc::SetRelationValue((ScMemoryContext&)ms_context, exampleparam, Keynodes::nrel_developer, name);
  //auto ex = ScType::NodeConst >> "hello";

  //ScAddr kekw = ms_context->CreateNode(ScType::NodeConstStruct);
 
  //ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, exampleparam);

  logger->Message(ScLog::Type::Info, "Just a test agent, nothing interesting.");
  //logger->Message(ScLog::Type::Info, "answ " + link.Get<std::string>());

  SC_LOG_ERROR("testAgent finished");
  utils::AgentUtils::finishAgentWork(ms_context.get(), questionNode, answer);
  return SC_RESULT_OK;
}
}

