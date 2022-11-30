#include <memory>

#include "sc-memory/sc_memory.hpp"


#include "sc-memory/sc_event.hpp"




#define keynodes_hpp_20_init  bool _InitInternal() override \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "Keynodes::_InitInternal"); \
    bool result = true; \
    return result; \
}


#define keynodes_hpp_20_initStatic static bool _InitStaticInternal()  \
{ \
    ScMemoryContext ctx(sc_access_lvl_make_min, "Keynodes::_InitStaticInternal"); \
    bool result = true; \
	question_find_subdividing = ctx.HelperResolveSystemIdtf("question_find_subdividing", ScType::Node); result = result && question_find_subdividing.IsValid(); \
	nrel_subdividing = ctx.HelperResolveSystemIdtf("nrel_subdividing", ScType::Node); result = result && nrel_subdividing.IsValid(); \
	question_find_isomorphic_structures = ctx.HelperResolveSystemIdtf("question_find_isomorphic_structures", ScType::Node); result = result && question_find_isomorphic_structures.IsValid(); \
	question_find_graph_circumference = ctx.HelperResolveSystemIdtf("question_find_graph_circumference", ScType::Node); result = result && question_find_graph_circumference.IsValid(); \
	test_action = ctx.HelperResolveSystemIdtf("test_action", ScType::Node); result = result && test_action.IsValid(); \
    return result; \
}


#define keynodes_hpp_20_decl 

#define keynodes_hpp_Keynodes_impl 

#undef ScFileID
#define ScFileID keynodes_hpp

