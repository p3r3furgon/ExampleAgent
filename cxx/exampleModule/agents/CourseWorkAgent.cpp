#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_link.hpp>
#include <sc-memory/sc_common_templ.hpp>

#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>

#include "CourseWorkAgent.hpp"

using namespace std;
using namespace utils;

namespace exampleModule
{

// ScAddr GetStartNode(ScLog *logger, std::unique_ptr<ScMemoryContext> &ms_context, ScAddr structure){
// ScAddr startNode;
// ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
// while(it3->Next())
// {
//  startNode = it3->Get(2);
//  break;
// }   
// return startNode;
// }

void CreateDistancesLinks(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr& structure){
  ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(it3->Next())
   {
    ScAddr distanceLink = ms_context->CreateLink(ScType::LinkVar);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, it3->Get(2), distanceLink);
   }   
}

ScAddr GetDistanceLink(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr& node)
{
  ScAddr distanceLink;
  ScIterator3Ptr it3 = ms_context->Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  while(it3->Next())
   {
    distanceLink = it3->Get(2);
   }   
   return distanceLink;
}

uint32_t GetLinkInfo(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr node)
{
  string strResult;
  ScIterator3Ptr it3 = ms_context->Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  SC_LOG_ERROR("1 keypoint");
  while(it3->Next())
   {
    SC_LOG_ERROR("2 keypoint");
    strResult = CommonUtils::getLinkContent(ms_context.get(), it3->Get(2));
   }
   SC_LOG_ERROR("3 keypoint");   
   uint32_t result = stoi(strResult);
   return result;
}

void DFS(ScLog *logger, std::unique_ptr<ScMemoryContext> &ms_context, ScAddr node, 
         ScAddr& visited, ScAddr& globalVisited, uint32_t& tmpSize, ScAddr& distances)
{
  ScAddr distanceLink = GetDistanceLink(ms_context, node);
  ms_context->SetLinkContent(distanceLink, to_string(tmpSize));

  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, visited, node);
  tmpSize++;

  ScIterator3Ptr it3 = ms_context->Iterator3(node, ScType::EdgeDCommonConst, ScType::NodeConst);
  while(it3->Next())
  {
    if(ms_context->HelperCheckEdge(visited, it3->Get(2), ScType::EdgeAccessConstPosPerm) && 
      !ms_context->HelperCheckEdge(globalVisited, it3->Get(2), ScType::EdgeAccessConstPosPerm))
    {
      SC_LOG_ERROR("Cycle is found");
      ScAddr cycleSize = ms_context->CreateNode(ScType::LinkVar);
      //uint32_t value = GetLinkInfo(ms_context, node) - GetLinkInfo(ms_context, it3->Get(2))+1;
      // ms_context->SetLinkContent(cycleSize, to_string(value));
      // ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, distances, cycleSize);
    } 
    else if(!ms_context->HelperCheckEdge(visited, it3->Get(2), ScType::EdgeAccessConstPosPerm))
    {
      DFS(logger, ms_context, it3->Get(2), visited, globalVisited, tmpSize, distances);
    }
  }
  ScIterator3Ptr clearVisited = ms_context->Iterator3(visited, ScType::EdgeAccessConstPosPerm, node);
  while(clearVisited->Next())
  {
   ms_context->EraseElement(clearVisited->Get(1)); 
  } 
  if(!ms_context->HelperCheckEdge(globalVisited, node, ScType::EdgeAccessConstPosPerm))
  {
   ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, globalVisited, node);
  }
}

SC_AGENT_IMPLEMENTATION(CourseWorkAgent)
{

  SC_LOG_ERROR("testAgent started");

  ScLog *logger = ScLog::GetInstance();
  ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);
  ScAddr param = IteratorUtils::getAnyFromSet(ms_context.get(), questionNode);
  ScAddr answer = ms_context->CreateNode(ScType::NodeConstStruct);
  ScAddr structure = IteratorUtils::getAnyFromSet(ms_context.get(), param);
  ScAddr visited = ms_context->CreateNode(ScType::NodeConstClass);
  ScAddr globalVisited = ms_context->CreateNode(ScType::NodeConstClass);
  ScAddr distances = ms_context->CreateNode(ScType::NodeConstClass);
  if (!param.IsValid()||!structure.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  uint32_t tmpSize = 0;

  CreateDistancesLinks(ms_context, structure);

  ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(it3->Next())
  {
   if(!ms_context->HelperCheckEdge(globalVisited, it3->Get(2), ScType::EdgeAccessConstPosPerm))
   {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, it3->Get(2));
    DFS(logger, ms_context, it3->Get(2), visited, globalVisited, tmpSize, distances);
   }
  }   

  ScIterator3Ptr i1 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(i1->Next())
  {
   auto ourNode = i1->Get(2);
   ScIterator3Ptr i2 = ms_context->Iterator3(ourNode, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
   while(i2->Next())
   {
    SC_LOG_ERROR("Node has link");
   }   
  }   


  // ScIterator3Ptr iter = ms_context->Iterator3(distances, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  // while(iter->Next())
  // {
  //  auto linka = iter->Get(2);
  //  logger->Message(ScLog::Type::Info, "res "+ CommonUtils::getLinkContent(ms_context.get(), linka));
  // }  

  SC_LOG_ERROR("testAgent finished");
  utils::AgentUtils::finishAgentWork(ms_context.get(), questionNode, answer);
  return SC_RESULT_OK;
}
}

