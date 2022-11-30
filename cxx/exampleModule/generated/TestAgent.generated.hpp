#include <memory>

#include "sc-memory/sc_memory.hpp"


#include "sc-memory/sc_event.hpp"




#define TestAgent_hpp_15_init  bool _InitInternal() override \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "TestAgent::_InitInternal"); \
    bool result = true; \
    return result; \
}


#define TestAgent_hpp_15_initStatic static bool _InitStaticInternal()  \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "TestAgent::_InitStaticInternal"); \
    bool result = true; \
    return result; \
}


#define TestAgent_hpp_15_decl \
private:\
	typedef ScAgent Super;\
protected: \
	TestAgent(char const * name, sc_uint8 accessLvl) : Super(name, accessLvl) {}\
	virtual sc_result Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override; \
private:\
	static std::unique_ptr<ScEvent> ms_event;\
    static std::unique_ptr<ScMemoryContext> ms_context;\
public: \
	static bool handler_emit(ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr)\
	{\
		TestAgent Instance("TestAgent", sc_access_lvl_make_min);\
		return Instance.Run(addr, edgeAddr, otherAddr) == SC_RESULT_OK;\
	}\
	static void RegisterHandler()\
	{\
		SC_ASSERT(!ms_event.get(), ());\
		SC_ASSERT(!ms_context.get(), ());\
		ms_context.reset(new ScMemoryContext(sc_access_lvl_make_min, "handler_TestAgent"));\
		ms_event.reset(new ScEvent(*ms_context, Keynodes::test_action, ScEvent::Type::AddOutputEdge, &TestAgent::handler_emit));\
        if (ms_event.get())\
        {\
            SC_LOG_INFO("Register agent TestAgent");\
        }\
        else\
        {\
            SC_LOG_ERROR("Can't register agent TestAgent");\
        }\
	}\
	static void UnregisterHandler()\
	{\
		ms_event.reset();\
		ms_context.reset();\
	}

#define TestAgent_hpp_TestAgent_impl \
std::unique_ptr<ScEvent> TestAgent::ms_event;\
std::unique_ptr<ScMemoryContext> TestAgent::ms_context;

#undef ScFileID
#define ScFileID TestAgent_hpp

