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
#include "method.h"
#include "entitydef.h"


namespace KBEngine{

uint32	MethodDescription::methodDescriptionCount_ = 0;
	

MethodDescription::MethodDescription(ENTITY_METHOD_UID utype, int32 domain,
									 std::string name, 
									 bool isExposed):
methodDomain_(domain),
name_(name),
utype_(utype),
argTypes_(),
isExposed_(isExposed),
currCallerID_(0),
aliasID_(-1),
Authority_(0)
{
	MethodDescription::methodDescriptionCount_++;

	//EntityDef::md5().append((void*)name_.c_str(), (int)name_.size());
	//EntityDef::md5().append((void*)&utype_, sizeof(ENTITY_METHOD_UID));
	//EntityDef::md5().append((void*)&isExposed_, sizeof(bool));
}

MethodDescription::~MethodDescription()
{
	std::vector<DataType*>::iterator iter = argTypes_.begin();
	//for(; iter != argTypes_.end(); ++iter)
	//	(*iter)->decRef();

	argTypes_.clear();
}

void MethodDescription::setExposed(void)
{ 
	isExposed_ = true; 
	//EntityDef::md5().append((void*)&isExposed_, sizeof(bool));
}


bool MethodDescription::pushArgType(DataType* dataType, const char *argName)
{
	if(dataType == NULL)
	{
		return false;
	}

	//dataType->incRef();
	argTypes_.push_back(dataType);
	if (argName != NULL)
	{
		argName_.push_back(argName);
	}

	DATATYPE_UID uid = dataType->id();
	//EntityDef::md5().append((void*)&uid, sizeof(DATATYPE_UID));
	//EntityDef::md5().append((void*)&isExposed_, sizeof(bool));
	return true;
}

size_t MethodDescription::getArgSize(void)
{
	return argTypes_.size();
}

}
