#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_link.hpp>
#include <sc-memory/sc_common_templ.hpp>

#include <sc-agents-common/utils/GenerationUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>
#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/CommonUtils.hpp>

#include "TestAgent.hpp"

using namespace std;
using namespace utils;

namespace exampleModule
{
    SC_AGENT_IMPLEMENTATION(TestAgent)
{
  //Main nodes and tmpSize initialization 
  ScLog *logger = ScLog::GetInstance();
  ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);
  ScAddr answer = ms_context->CreateNode(ScType::NodeConstStruct);
  SC_LOG_ERROR("Variables are initializated");
  
  int a = 5;
  cout << a << endl;
  ScAddr link = ms_context->CreateLink();

  ms_context->SetLinkContent(link, a);
  logger->Message(ScLog::Type::Info, "res:  "+ CommonUtils::getLinkContent(ms_context.get(), link));
  int b;
  utils::AgentUtils::finishAgentWork(ms_context.get(), questionNode, answer);
  return SC_RESULT_OK;
  
}
}