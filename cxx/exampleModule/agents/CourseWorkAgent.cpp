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

void RemoveOldLinks(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr& structure)
{
  ScIterator3Ptr i1 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(i1->Next())
  {
   auto ourNode = i1->Get(2);
   ScIterator3Ptr i2 = ms_context->Iterator3(ourNode, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
   while(i2->Next())
   {
    ms_context->EraseElement(i2->Get(1));
    ms_context->EraseElement(i2->Get(2));
   }   
  }  
}

void CreateDistancesLinks(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr& structure){
  ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(it3->Next())
   {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, it3->Get(2), ms_context->CreateLink(ScType::LinkVar));
   }   
}

ScAddr GetDistanceLink(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr& node)
{
  ScIterator3Ptr it3 = ms_context->Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  while(it3->Next())
   {
    return it3->Get(2);
   }   
   return {};
}

uint32_t GetLinkInfo(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr node)
{
  uint32_t res;
  string strRes;
   //SC_LOG_ERROR("1 keypoint");
  ScIterator3Ptr it3 = ms_context->Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  while(it3->Next())
   {
    //SC_LOG_ERROR("2 keypoint");
    strRes = CommonUtils::getLinkContent(ms_context.get(), it3->Get(2));
   }
  //SC_LOG_ERROR("3 keypoint");   
   res = stoi(strRes);
   return res;
}

ScAddr GetMaxCycleSize(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr distances)
{
  ScAddr res = ms_context->CreateNode(ScType::LinkVar);
  uint32_t maxSize = 0;
  uint32_t tmp;
  ScIterator3Ptr iter = ms_context->Iterator3(distances, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  while(iter->Next())
  {
   auto strTmp = CommonUtils::getLinkContent(ms_context.get(), iter->Get(2));
   tmp = stoi(strTmp); 
   if (tmp > maxSize)
   {
    ms_context->SetLinkContent(res, to_string(tmp));
    maxSize = tmp;
   }
  } 
  return res; 
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
      //SC_LOG_ERROR("Cycle is found");
      ScAddr cycleSize = ms_context->CreateNode(ScType::LinkVar);
      uint32_t value = GetLinkInfo(ms_context, node) - GetLinkInfo(ms_context, it3->Get(2)) + (uint32_t)1;
      ms_context->SetLinkContent(cycleSize, to_string(value));
      ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, distances, cycleSize);
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
  if (!param.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  uint32_t tmpSize = 0;

  ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(it3->Next())
  {
   if(!ms_context->HelperCheckEdge(globalVisited, it3->Get(2), ScType::EdgeAccessConstPosPerm))
   {
    RemoveOldLinks(ms_context, structure);
    CreateDistancesLinks(ms_context, structure);
    DFS(logger, ms_context, it3->Get(2), visited, globalVisited, tmpSize, distances);
   }
  }   

  // ScAddr link = ms_context->CreateNode(ScType::LinkVar);
  // uint32_t res;

  // auto sp = CommonUtils::getLinkContent(ms_context.get(), link);
  // uint32_t ip = stoi(sp);
  // ip*=ip;
  // ms_context->SetLinkContent(link, to_string(ip));
  ScIterator3Ptr iter = ms_context->Iterator3(distances, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  while(iter->Next())
  {
   auto linka = iter->Get(2);
   logger->Message(ScLog::Type::Info, "cycle size "+ CommonUtils::getLinkContent(ms_context.get(), linka));
  }  
  ScAddr result = GetMaxCycleSize(ms_context, distances);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, result);
  logger->Message(ScLog::Type::Info, "Graph circumference:  "+ CommonUtils::getLinkContent(ms_context.get(), result));
  SC_LOG_ERROR("testAgent finished");
  utils::AgentUtils::finishAgentWork(ms_context.get(), questionNode, answer);
  return SC_RESULT_OK;
}
}

