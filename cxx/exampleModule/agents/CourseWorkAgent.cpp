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

// ScAddr GetStartNode(ScLog *logger, std::unique_ptr<ScMemoryContext> &ms_context, ScAddr structure){
//   ScAddr startNode;
//   ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
//   while(it3->Next())
//    {
//     startNode = it3->Get(2);
//     break;
//    }   
//     return startNode;
// }

void DFS(ScLog *logger, std::unique_ptr<ScMemoryContext> &ms_context, ScAddr node, ScAddr& visited, ScAddr& globalVisited)
{
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, visited, node);
  ScIterator3Ptr it3 = ms_context->Iterator3(node, ScType::EdgeDCommonConst, ScType::NodeConst);
  while(it3->Next())
  {
    if(ms_context->HelperCheckEdge(visited, it3->Get(2), ScType::EdgeAccessConstPosPerm) && 
      !ms_context->HelperCheckEdge(globalVisited, it3->Get(2), ScType::EdgeAccessConstPosPerm))
      SC_LOG_ERROR("Cycle is found");
    else if(!ms_context->HelperCheckEdge(visited, it3->Get(2), ScType::EdgeAccessConstPosPerm))
    {
      //SC_LOG_ERROR("Next step");
      DFS(logger, ms_context, it3->Get(2), visited, globalVisited);
    }
  }
  ScIterator3Ptr clearVisited = ms_context->Iterator3(visited, ScType::EdgeAccessConstPosPerm, node);
  while(clearVisited->Next())
  {
   // SC_LOG_ERROR("Deleted visited edge");
    ms_context->EraseElement(clearVisited->Get(1)); 
  } 
  if(!ms_context->HelperCheckEdge(globalVisited, node, ScType::EdgeAccessConstPosPerm))
  {
     //SC_LOG_ERROR("Created edge between glovalVisited and node");
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, globalVisited, node);
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
  //int n = 0;
  ScAddr structure = IteratorUtils::getAnyFromSet(ms_context.get(), param);
  ScAddr visited = ms_context->CreateNode(ScType::NodeConstClass);
  ScAddr globalVisited = ms_context->CreateNode(ScType::NodeConstClass);
  //ScAddr startNode = GetStartNode(logger, ms_context, structure);;

  ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(it3->Next())
  {
   if(!ms_context->HelperCheckEdge(globalVisited, it3->Get(2), ScType::EdgeAccessConstPosPerm))
   {
    //  SC_LOG_ERROR("There is no edge between node and globalVisited");
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, it3->Get(2));
    DFS(logger, ms_context, it3->Get(2), visited, globalVisited);
   }
  }   
  SC_LOG_ERROR("testAgent finished");
  utils::AgentUtils::finishAgentWork(ms_context.get(), questionNode, answer);
  return SC_RESULT_OK;
}
}

