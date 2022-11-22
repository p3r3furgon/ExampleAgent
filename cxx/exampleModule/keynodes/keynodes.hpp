/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include <sc-memory/sc_object.hpp>
#include <sc-memory/sc_addr.hpp>

#include "keynodes.generated.hpp"

namespace exampleModule
{

class Keynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:

  SC_PROPERTY(Keynode("question_find_subdividing"), ForceCreate)
  static ScAddr question_find_subdividing;

  SC_PROPERTY(Keynode("nrel_subdividing"), ForceCreate)
  static ScAddr nrel_subdividing;

  SC_PROPERTY(Keynode("question_find_isomorphic_structures"), ForceCreate)
  static ScAddr question_find_isomorphic_structures;

  SC_PROPERTY(Keynode("example_action"), ForceCreate)
  static ScAddr example_action;

  SC_PROPERTY(Keynode("nrel_developer"), ForceCreate)
  static ScAddr nrel_developer;

  SC_PROPERTY(Keynode("visited"), ForceCreate)
  static ScAddr visited;  

  SC_PROPERTY(Keynode("globalVisited"), ForceCreate)
  static ScAddr globalVisited;  
};
};

// namespace exampleModule
