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
#include "fixeddict.h"
#include "datatypes.h"
//#include "pyscript/py_gc.h"

namespace KBEngine{ 


	
//-------------------------------------------------------------------------------------
FixedDict::FixedDict(DataType* dataType, std::string& strDictInitData)
{
	_dataType = static_cast<FixedDictType*>(dataType);
	//_dataType->incRef();
	initialize(strDictInitData);

}
/*
FixedDict::FixedDict(DataType* dataType, MemoryStream* streamInitData, bool isPersistentsStream)
{
	_dataType = static_cast<FixedDictType*>(dataType);
	_dataType->incRef();
	initialize(streamInitData, isPersistentsStream);

}
*/
FixedDict::FixedDict(DataType* dataType)
{
	_dataType = static_cast<FixedDictType*>(dataType);
	//_dataType->incRef();
	initialize("");

}

FixedDict::~FixedDict()
{
	//_dataType->decRef();

}

void FixedDict::initialize(std::string strDictInitData)
{

}
/*
void FixedDict::initialize(MemoryStream* streamInitData, bool isPersistentsStream)
{
	FixedDictType::FIXEDDICT_KEYTYPE_MAP& keyTypes = _dataType->getKeyTypes();
	FixedDictType::FIXEDDICT_KEYTYPE_MAP::const_iterator iter = keyTypes.begin();

}
*/
}
