/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2016 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../stdafx.h"
#include "entity_mailbox.h"
#include "scriptdef_module.h"
#include "method.h"
#include "remote_entity_method.h"
#include "entitydef.h"

namespace KBEngine
{

std::vector<EntityMailbox*> EntityMailbox::mailboxs;
//-------------------------------------------------------------------------------------
EntityMailbox::EntityMailbox(ScriptDefModule* pScriptModule, 
							 //const Network::Address* pAddr, 
							 int32 componentID, 
int32 eid, ENTITY_MAILBOX_TYPE type):
atIdx_(MAILBOXS::size_type(-1))
{
	atIdx_ = EntityMailbox::mailboxs.size();
	EntityMailbox::mailboxs.push_back(this);
}

//-------------------------------------------------------------------------------------
EntityMailbox::~EntityMailbox()
{
	// 如果有2个或以上的Mailbox则将最后一个Mailbox移至删除的这个Mailbox所在位置
	EntityMailbox* pBack = EntityMailbox::mailboxs.back();
	pBack->_setATIdx(atIdx_);
	EntityMailbox::mailboxs[atIdx_] = pBack;
	atIdx_ = MAILBOXS::size_type(-1);
	EntityMailbox::mailboxs.pop_back();
}

//-------------------------------------------------------------------------------------
void EntityMailbox::c_str(char* s, size_t size)
{

}

void EntityMailbox::reload()
{
	//pScriptModule_ = findScriptModule(scriptModuleName_.c_str());
}

//-------------------------------------------------------------------------------------
}
