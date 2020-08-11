// GenActor.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "entitydef/entitydef.h"
#include "entitydef/common.h"
#include <string>
#include <sstream>
#include <fstream>
namespace KBEngine {
	COMPONENT_TYPE g_componentType = CLIENT_TYPE;
}

int main()
{
	KBEngine::UEntityDef *entityDef = new KBEngine::UEntityDef();
	entityDef->SetProjectName("MUEK");
	entityDef->initialize("../../kbengine/assets/scripts/");

	std::stringstream aliasfile;
	aliasfile << entityDef->AliasToUE4Struct().c_str();
	std::ofstream ofs;
	ofs.open("../../Source/MUEK/Alias.h" , std::ofstream::out);
	ofs << aliasfile.str();
	ofs.close();

	std::stringstream entitydefine;
	entitydefine << entityDef->DefineToUE4Object().c_str();
	std::ofstream deofs;
	deofs.open("../../Source/MUEK/DefineEntity.h", std::ofstream::out);
	deofs << entitydefine.str();
	deofs.close();
    return 0;
}

