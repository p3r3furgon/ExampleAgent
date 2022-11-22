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


ScAddr GetStartNode(ScLog *logger, std::unique_ptr<ScMemoryContext> &ms_context, ScAddr structure){
  ScAddr startNode;
  ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(it3->Next())
   {
    startNode = it3->Get(2);
    break;
   }
    logger->Message(ScLog::Type::Info, "function found start node");     
    return startNode;
}

void DFS(ScLog *logger, std::unique_ptr<ScMemoryContext> &ms_context, ScAddr node, int& tmpSize)
{
  tmpSize++;
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::visited, node);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::globalVisited, node);
  ScIterator3Ptr it3 = ms_context->Iterator3(node, ScType::EdgeDCommonConst, ScType::NodeConst);
  while(it3->Next())
   {
    if(ms_context->HelperCheckEdge(it3->Get(2), Keynodes::visited, ScType::EdgeAccessConstPosPerm)) 
    {
      SC_LOG_ERROR("Cycle is found");
    }
    else
      DFS(logger, ms_context, it3->Get(2), tmpSize);
   }
}

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
  ScAddr startNode;

  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, structure);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, Keynodes::visited);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, param);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, Keynodes::globalVisited);

  startNode = GetStartNode(logger, ms_context, structure);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, startNode);
  ScAddr edge = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::visited, startNode);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, edge);
  DFS(logger, ms_context, startNode, tmpSize);

  SC_LOG_ERROR("testAgent finished");
  utils::AgentUtils::finishAgentWork(ms_context.get(), questionNode, answer);
  return SC_RESULT_OK;
}
}

