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
#include "datatypes.h"
#include "entitydef.h"
#include "property.h"


namespace KBEngine{

uint32 PropertyDescription::propertyDescriptionCount_ = 0;

//-------------------------------------------------------------------------------------
PropertyDescription::PropertyDescription(ENTITY_PROPERTY_UID utype, 
										std::string dataTypeName, 
										std::string name, uint32 flags, 
										bool isPersistent, 
										DataType* dataType, 
										bool isIdentifier, 
										std::string indexType,
										uint32 databaseLength, 
										std::string defaultStr, 
										DETAIL_TYPE detailLevel):
	name_(name),
	dataTypeName_(dataTypeName),
	flags_(flags),
	isPersistent_(isPersistent),
	dataType_(dataType),
	isIdentifier_(isIdentifier),
	databaseLength_(databaseLength),
	utype_(utype),
	defaultValStr_(defaultStr),
	detailLevel_(detailLevel),
	aliasID_(-1),
	indexType_(indexType)
{
	//dataType_->incRef();

	// mailbox ÎÞ·¨±£´æ
	if(isPersistent && strcmp(dataType_->getName(), "MAILBOX") == 0)
	{
		isPersistent_ = false;
	}

	//EntityDef::md5().append((void*)name_.c_str(), (int)name_.size());
	//EntityDef::md5().append((void*)defaultValStr_.c_str(), (int)defaultValStr_.size());
	//EntityDef::md5().append((void*)dataTypeName.c_str(), (int)dataTypeName.size());
	//EntityDef::md5().append((void*)&utype_, sizeof(ENTITY_PROPERTY_UID));
	//EntityDef::md5().append((void*)&flags_, sizeof(uint32));
	//EntityDef::md5().append((void*)&isPersistent_, sizeof(bool));
	//EntityDef::md5().append((void*)&isIdentifier_, sizeof(bool));
	//EntityDef::md5().append((void*)&databaseLength_, sizeof(uint32));
	//EntityDef::md5().append((void*)&detailLevel_, sizeof(int8));

	DATATYPE_UID uid = dataType->id();
	//EntityDef::md5().append((void*)&uid, sizeof(DATATYPE_UID));

	PropertyDescription::propertyDescriptionCount_++;

	if(dataType == NULL)
	{

	}
}

//-------------------------------------------------------------------------------------
PropertyDescription::~PropertyDescription()
{
	//dataType_->decRef();
}

//-------------------------------------------------------------------------------------
PropertyDescription* PropertyDescription::createDescription(ENTITY_PROPERTY_UID utype, 
															std::string& dataTypeName, 
															std::string& name, 
															uint32 flags, 
															bool isPersistent, 
															DataType* dataType, 
															bool isIdentifier, 
															std::string indexType,
															uint32 databaseLength, 
															std::string& defaultStr, 
															DETAIL_TYPE detailLevel)
{
	PropertyDescription* propertyDescription = NULL;
	if(dataTypeName == "FIXED_DICT" || 
		strcmp(dataType->getName(), "FIXED_DICT") == 0)
	{
		propertyDescription = new FixedDictDescription(utype, dataTypeName, name, flags, isPersistent, 
														dataType, isIdentifier, indexType, databaseLength, 
														defaultStr, detailLevel);
	}
	else if(dataTypeName == "ARRAY" ||
		strcmp(dataType->getName(), "ARRAY") == 0)
	{
		propertyDescription = new ArrayDescription(utype, dataTypeName, name, flags, isPersistent, 
														dataType, isIdentifier, indexType, databaseLength, 
														defaultStr, detailLevel);
		
	}
	else if(dataTypeName == "VECTOR2" || 
		strcmp(dataType->getName(), "VECTOR2") == 0)
	{
		propertyDescription = new VectorDescription(utype, dataTypeName, name, flags, isPersistent, 
														dataType, isIdentifier, indexType, databaseLength, 
														defaultStr, detailLevel, 2);
	}
	else if(dataTypeName == "VECTOR3" || 
		strcmp(dataType->getName(), "VECTOR3") == 0)
	{
		propertyDescription = new VectorDescription(utype, dataTypeName, name, flags, isPersistent, 
														dataType, isIdentifier, indexType, databaseLength, 
														defaultStr, detailLevel, 3);
	}
	else if(dataTypeName == "VECTOR4" || 
		strcmp(dataType->getName(), "VECTOR4") == 0)
	{
		propertyDescription = new VectorDescription(utype, dataTypeName, name, flags, isPersistent, 
														dataType, isIdentifier, indexType, databaseLength, 
														defaultStr, detailLevel, 4);
	}
	else
	{
		propertyDescription = new PropertyDescription(utype, dataTypeName, name, flags, isPersistent, 
														dataType, isIdentifier, indexType, databaseLength, 
														defaultStr, detailLevel);
	}

	return propertyDescription;
}

FixedDictDescription::FixedDictDescription(ENTITY_PROPERTY_UID utype, 
											std::string dataTypeName,
											std::string name, 
											uint32 flags, 
											bool isPersistent, 
											DataType* dataType, 
											bool isIdentifier, 
											std::string indexType,
											uint32 databaseLength, 
											std::string defaultStr, 
											DETAIL_TYPE detailLevel):
	PropertyDescription(utype, dataTypeName, name, flags, isPersistent, 
		dataType, isIdentifier, indexType, databaseLength, defaultStr, detailLevel)
{

}

FixedDictDescription::~FixedDictDescription()
{
}

ArrayDescription::ArrayDescription(ENTITY_PROPERTY_UID utype, 
									std::string dataTypeName,
									std::string name, 
									uint32 flags, 
									bool isPersistent, 
									DataType* dataType, 
									bool isIdentifier, 
									std::string indexType,
									uint32 databaseLength, 
									std::string defaultStr, 
									DETAIL_TYPE detailLevel):
	PropertyDescription(utype, dataTypeName, name, flags, isPersistent, 
		dataType, isIdentifier, indexType, databaseLength, defaultStr, detailLevel)
{
}

ArrayDescription::~ArrayDescription()
{
}

VectorDescription::VectorDescription(ENTITY_PROPERTY_UID utype, 
									std::string dataTypeName, 
									std::string name, 
									uint32 flags, 
									bool isPersistent, 
									DataType* dataType, 
									bool isIdentifier, 
									std::string indexType,
									uint32 databaseLength, 
									std::string defaultStr, 
									DETAIL_TYPE detailLevel, 
									uint8 elemCount):
	PropertyDescription(utype, dataTypeName, name, flags, isPersistent, 
		dataType, isIdentifier, indexType, databaseLength, defaultStr, detailLevel),
	elemCount_(elemCount)
{
}

VectorDescription::~VectorDescription()
{
}

}
