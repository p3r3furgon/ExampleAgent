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

//Remove links from past executions
void RemoveOldLinks(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr& structure)
{
  //This iterator finds all graph nodes
  ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(it3->Next())
  {
   auto ourNode = it3->Get(2);
   //This iterator finds links that connected to node
   ScIterator3Ptr it3_2 = ms_context->Iterator3(ourNode, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
   while(it3_2->Next())
   {
    //Remove edge between link and node 
    //ms_context->EraseElement(it3_2->Get(1)); //???
    //Remove link
    ms_context->EraseElement(it3_2->Get(2));
   }   
  }  
}

//Create new "clear" link to each graph node
void CreateDistancesLinks(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr& structure){
  ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(it3->Next())
   {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, it3->Get(2), ms_context->CreateLink(ScType::LinkVar));
   }   
}

//Get link that connected to known node
// ScAddr GetDistanceLink(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr& node)
// {
//   ScIterator3Ptr it3 = ms_context->Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
//   while(it3->Next())
//    {
//     return it3->Get(2);
//    }   
//    return {};
// }

//Get link information, ???(cast)
uint32_t GetLinkInfo(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr node)
{
  uint32_t res;
  string strRes;
  ScIterator3Ptr it3 = ms_context->Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  while(it3->Next())
   {
    strRes = CommonUtils::getLinkContent(ms_context.get(), it3->Get(2));
   }  
   res = stoi(strRes);
   return res;
}

//Checking distances node to find link with max value
ScAddr GetMaxCycleSize(std::unique_ptr<ScMemoryContext> &ms_context, ScAddr distances)
{
  ScAddr res = ms_context->CreateNode(ScType::LinkVar);
  uint32_t maxSize = 0;
  uint32_t tmp;
  //This iterator finds all links that connected to distances node
  ScIterator3Ptr it3 = ms_context->Iterator3(distances, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  //Common way to find max value
  while(it3->Next())
  {
   auto strTmp = CommonUtils::getLinkContent(ms_context.get(), it3->Get(2));
   tmp = stoi(strTmp); 
   if (tmp > maxSize)
   {
    ms_context->SetLinkContent(res, to_string(tmp));
    maxSize = tmp;
   }
  } 
  return res; 
}

void GetNodeIdtf(std::unique_ptr<ScMemoryContext> &ms_context, ScLog *logger, ScAddr node)
{
  ScIterator3Ptr it3 = ms_context->Iterator3(node, ScType::EdgeDCommonConst, ScType::LinkConst);
  while(it3->Next())
   {
    logger->Message(ScLog::Type::Info, "Im in:  "+ CommonUtils::getLinkContent(ms_context.get(), it3->Get(2)));
   }  
}

//Graph detour using DFS
void DFS(ScLog *logger, std::unique_ptr<ScMemoryContext> &ms_context, ScAddr node, 
         ScAddr& visited, ScAddr& globalVisited, uint32_t& tmpSize, ScAddr& distances)
{
  //Put tmpSize value to distanceLink
  GetNodeIdtf(ms_context, logger, node);
  ScIterator3Ptr it3_3 = ms_context->Iterator3(node, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  while(it3_3->Next())
  {
    ms_context->SetLinkContent(it3_3->Get(2), to_string(tmpSize));
  }
  //Create edge between current node and visited node
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, visited, node);

  tmpSize++;

  //This iterator finds nodes that are adjacent to starting node
  ScIterator3Ptr it3 = ms_context->Iterator3(node, ScType::EdgeDCommonConst, ScType::NodeConst);
  while(it3->Next())
  {
    //if adjacent node is visited and isn't globalVisited 
    if(ms_context->HelperCheckEdge(visited, it3->Get(2), ScType::EdgeAccessConstPosPerm) && 
      !ms_context->HelperCheckEdge(globalVisited, it3->Get(2), ScType::EdgeAccessConstPosPerm))
    {
      //Put cycle length to link and connect this link to distances node
      ScAddr cycleSize = ms_context->CreateNode(ScType::LinkVar);
      uint32_t value = GetLinkInfo(ms_context, node) - GetLinkInfo(ms_context, it3->Get(2)) + (uint32_t)1;
      ms_context->SetLinkContent(cycleSize, to_string(value));
      ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, distances, cycleSize);
    } 
    //if adjacent node isn't visited
    else if(!ms_context->HelperCheckEdge(visited, it3->Get(2), ScType::EdgeAccessConstPosPerm))
    {
      //Move to the next node 
      DFS(logger, ms_context, it3->Get(2), visited, globalVisited, tmpSize, distances);
      GetNodeIdtf(ms_context, logger, node);
    }
  }
  //Remove edge between visited and current node
  //This iterator finds edge between visited and node 
  ScIterator3Ptr it3_2 = ms_context->Iterator3(visited, ScType::EdgeAccessConstPosPerm, node);
  while(it3_2->Next())
  {
   //Remove edge
   ms_context->EraseElement(it3_2->Get(1)); 
  }
  //Create edge between current node and globalVisited node
  if(!ms_context->HelperCheckEdge(globalVisited, node, ScType::EdgeAccessConstPosPerm))//if this edge isn't exist 
  {
   //Create edge between current node and globalVisited node 
   ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, globalVisited, node);
  }
  tmpSize--;
}

SC_AGENT_IMPLEMENTATION(CourseWorkAgent)
{
  //Main nodes and tmpSize initialization 
  ScLog *logger = ScLog::GetInstance();
  ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);
  ScAddr param = IteratorUtils::getAnyFromSet(ms_context.get(), questionNode);
  ScAddr answer = ms_context->CreateNode(ScType::NodeConstStruct);
  ScAddr structure = IteratorUtils::getAnyFromSet(ms_context.get(), param);
  ScAddr visited = ms_context->CreateNode(ScType::NodeConstClass);
  ScAddr globalVisited = ms_context->CreateNode(ScType::NodeConstClass);
  ScAddr distances = ms_context->CreateNode(ScType::NodeConstClass);
  uint32_t tmpSize = 0;

  if (!param.IsValid()||!structure.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  //This iterator finds all nodes of graph in the structure
  ScIterator3Ptr it3 = ms_context->Iterator3(structure, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while(it3->Next())
  {
   if(!ms_context->HelperCheckEdge(globalVisited, it3->Get(2), ScType::EdgeAccessConstPosPerm)) // if node isn't globalVisited
   {
    RemoveOldLinks(ms_context, structure); //Clear past links
    CreateDistancesLinks(ms_context, structure); //Create new "clear" links
    DFS(logger, ms_context, it3->Get(2), visited, globalVisited, tmpSize, distances); //Call DFS to current node
   }
  }   


  //Check if DFS found cycles
  bool hasCycles = false;
  //This iterator finds links that are connected with distances link
  ScIterator3Ptr it3_2 = ms_context->Iterator3(distances, ScType::EdgeAccessConstPosPerm, ScType::LinkVar);
  while(it3_2->Next())
   {
    hasCycles = true;
    break;
   }  
  //If Cycles exist
  if(hasCycles)
  {
    ScAddr result = GetMaxCycleSize(ms_context, distances); //Create link that contains the greatest cycle size
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer, result); //Connect result with answer 
    logger->Message(ScLog::Type::Info, "Graph circumference:  "+ CommonUtils::getLinkContent(ms_context.get(), result)); //Send log to console
  }
  else
    logger->Message(ScLog::Type::Info, "There are no cycles in the graph");

  utils::AgentUtils::finishAgentWork(ms_context.get(), questionNode, answer);
  return SC_RESULT_OK;
  
}
}

