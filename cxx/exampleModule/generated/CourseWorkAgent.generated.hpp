#include <memory>

#include "sc-memory/sc_memory.hpp"


#include "sc-memory/sc_event.hpp"




#define CourseWorkAgent_hpp_16_init  bool _InitInternal() override \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "CourseWorkAgent::_InitInternal"); \
    bool result = true; \
    return result; \
}


#define CourseWorkAgent_hpp_16_initStatic static bool _InitStaticInternal()  \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "CourseWorkAgent::_InitStaticInternal"); \
    bool result = true; \
    return result; \
}


#define CourseWorkAgent_hpp_16_decl \
private:\
	typedef ScAgent Super;\
protected: \
	CourseWorkAgent(char const * name, sc_uint8 accessLvl) : Super(name, accessLvl) {}\
	virtual sc_result Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override; \
private:\
	static std::unique_ptr<ScEvent> ms_event;\
    static std::unique_ptr<ScMemoryContext> ms_context;\
public: \
	static bool handler_emit(ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & otherAddr)\
	{\
		CourseWorkAgent Instance("CourseWorkAgent", sc_access_lvl_make_min);\
		return Instance.Run(addr, edgeAddr, otherAddr) == SC_RESULT_OK;\
	}\
	static void RegisterHandler()\
	{\
		SC_ASSERT(!ms_event.get(), ());\
		SC_ASSERT(!ms_context.get(), ());\
		ms_context.reset(new ScMemoryContext(sc_access_lvl_make_min, "handler_CourseWorkAgent"));\
		ms_event.reset(new ScEvent(*ms_context, Keynodes::question_find_graph_circumference, ScEvent::Type::AddOutputEdge, &CourseWorkAgent::handler_emit));\
        if (ms_event.get())\
        {\
            SC_LOG_INFO("Register agent CourseWorkAgent");\
        }\
        else\
        {\
            SC_LOG_ERROR("Can't register agent CourseWorkAgent");\
        }\
	}\
	static void UnregisterHandler()\
	{\
		ms_event.reset();\
		ms_context.reset();\
	}

#define CourseWorkAgent_hpp_CourseWorkAgent_impl \
std::unique_ptr<ScEvent> CourseWorkAgent::ms_event;\
std::unique_ptr<ScMemoryContext> CourseWorkAgent::ms_context;

#undef ScFileID
#define ScFileID CourseWorkAgent_hpp

