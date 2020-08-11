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
#include "datatype.h"
#include "datatypes.h"
#include "entitydef.h"
#include "fixeddict.h"
#include "fixedarray.h"
#include "entity_mailbox.h"
#include "scriptdef_module.h"


namespace KBEngine{
static DATATYPE_UID _g_dataTypeUID = 1;

//-------------------------------------------------------------------------------------
DataType::DataType(DATATYPE_UID did):
id_(did),
aliasName_()
{
	if(id_ == 0)
		id_ = _g_dataTypeUID++;

	//addDataType(id_, this);

	//EntityDef::md5().append((void*)this->aliasName(), (int)strlen(this->aliasName()));
	//EntityDef::md5().append((void*)&id_, sizeof(DATATYPE_UID));
}

//-------------------------------------------------------------------------------------
DataType::~DataType()
{
	finalise();
}

//-------------------------------------------------------------------------------------
bool DataType::finalise()
{
	_g_dataTypeUID = 1;
	return true;
}

//-------------------------------------------------------------------------------------
bool DataType::initialize(XML* xml, TiXmlNode* node , ADataTypes *dataTypes)
{
	return true;
}

//-------------------------------------------------------------------------------------
UInt64Type::UInt64Type(DATATYPE_UID did):
DataType(did)
{
}

//-------------------------------------------------------------------------------------
UInt64Type::~UInt64Type()
{
}


//-------------------------------------------------------------------------------------
UInt32Type::UInt32Type(DATATYPE_UID did):
DataType(did)
{
}

//-------------------------------------------------------------------------------------
UInt32Type::~UInt32Type()
{
}

//-------------------------------------------------------------------------------------
Int64Type::Int64Type(DATATYPE_UID did):
DataType(did)
{
}

//-------------------------------------------------------------------------------------
Int64Type::~Int64Type()
{
}

//-------------------------------------------------------------------------------------
FloatType::FloatType(DATATYPE_UID did):
DataType(did)
{
}

//-------------------------------------------------------------------------------------
FloatType::~FloatType()
{
}

//-------------------------------------------------------------------------------------
DoubleType::DoubleType(DATATYPE_UID did):
DataType(did)
{
}

//-------------------------------------------------------------------------------------
DoubleType::~DoubleType()
{
}

//-------------------------------------------------------------------------------------
VectorType::VectorType(uint32 elemCount, DATATYPE_UID did):
DataType(did),
name_(),
elemCount_(elemCount)
{
	char tbuf[MAX_BUF];
	_snprintf_s(tbuf, MAX_BUF, "VECTOR%u", elemCount);
	name_ += tbuf;
}

//-------------------------------------------------------------------------------------
VectorType::~VectorType()
{
}

//-------------------------------------------------------------------------------------
StringType::StringType(DATATYPE_UID did):
DataType(did)
{
}

//-------------------------------------------------------------------------------------
StringType::~StringType()
{
}

//-------------------------------------------------------------------------------------
UnicodeType::UnicodeType(DATATYPE_UID did):
DataType(did)
{
}

//-------------------------------------------------------------------------------------
UnicodeType::~UnicodeType()
{
}

//-------------------------------------------------------------------------------------
BlobType::BlobType(DATATYPE_UID did):
DataType(did)
{
}

//-------------------------------------------------------------------------------------
BlobType::~BlobType()
{
}

//-------------------------------------------------------------------------------------
MailboxType::MailboxType(DATATYPE_UID did):
DataType(did)
{
}

//-------------------------------------------------------------------------------------
MailboxType::~MailboxType()
{
}

//-------------------------------------------------------------------------------------
FixedArrayType::FixedArrayType(DATATYPE_UID did):
DataType(did)
{
}

//-------------------------------------------------------------------------------------
FixedArrayType::~FixedArrayType()
{

}

//-------------------------------------------------------------------------------------
bool FixedArrayType::initialize(XML* xml, TiXmlNode* node, ADataTypes *dataTypes)
{
	dataType_ = NULL;
	TiXmlNode* arrayNode = xml->enterNode(node, "of");
	if(arrayNode == NULL)
	{
		//ERROR_MSG("FixedArrayType::initialize: not found \"of\".\n");
		return false;
	}

	std::string strType = xml->getValStr(arrayNode);

	if(strType == "ARRAY")
	{
		FixedArrayType* dataType = new FixedArrayType();
		if(dataType->initialize(xml, arrayNode, dataTypes)){
			dataType_ = dataType;

			dataTypes->addDataType(std::string("_") + genUUID64() +
				dataType->aliasName(), dataType);
		}
		else
		{
			//ERROR_MSG("FixedArrayType::initialize: Array is wrong.\n");
			return false;
		}
	}
	else
	{
		DataType* dataType = dataTypes->getDataType(strType);
		if(dataType != NULL)
		{
			dataType_ = dataType;
		}
		else
		{
			//ERROR_MSG(fmt::format("FixedArrayType::initialize: can't found type[{}] by key[{}].\n", 
			//	strType.c_str(), "ARRAY"));
			
			return false;
		}			
	}

	if(dataType_ == NULL)
	{
		//ERROR_MSG("FixedArrayType::initialize: dataType is NULL.\n");
		return false;
	}

	DATATYPE_UID uid = dataType_->id();
	//EntityDef::md5().append((void*)&uid, sizeof(DATATYPE_UID));
	//EntityDef::md5().append((void*)strType.c_str(), (int)strType.size());
	return true;
}

//-------------------------------------------------------------------------------------
FixedDictType::FixedDictType(DATATYPE_UID did):
DataType(did),
keyTypes_(),
moduleName_()
{
}

//-------------------------------------------------------------------------------------
FixedDictType::~FixedDictType()
{
	FIXEDDICT_KEYTYPE_MAP::iterator iter = keyTypes_.begin();
	for(; iter != keyTypes_.end(); ++iter)
	{
		//iter->second->dataType->decRef();
	}

	keyTypes_.clear();
}

//-------------------------------------------------------------------------------------
std::string FixedDictType::getKeyNames(void)
{
	std::string keyNames = "";

	FIXEDDICT_KEYTYPE_MAP::iterator iter = keyTypes_.begin();
	for(; iter != keyTypes_.end(); ++iter)
	{
		keyNames += iter->first + ",";
	}

	return keyNames;
}

//-------------------------------------------------------------------------------------
std::string FixedDictType::debugInfos(void)
{
	std::string retstr = "";

	FIXEDDICT_KEYTYPE_MAP::iterator iter = keyTypes_.begin();
	for(; iter != keyTypes_.end(); ++iter)
	{
		retstr += iter->first;
		retstr += "(";
		retstr += iter->second->dataType->aliasName();
		retstr += "), ";
	}
	
	if(retstr.size() > 0)
		retstr.erase(retstr.size() - 2, 2);

	return retstr;
}

//-------------------------------------------------------------------------------------
bool FixedDictType::initialize(XML* xml, TiXmlNode* node, ADataTypes *dataTypes)
{
	TiXmlNode* propertiesNode = xml->enterNode(node, "Properties");
	if(propertiesNode == NULL)
	{
		return false;
	}

	std::string strType = "", typeName = "";

	XML_FOR_BEGIN(propertiesNode)
	{
		typeName = xml->getKey(propertiesNode);

		TiXmlNode* typeNode = xml->enterNode(propertiesNode->FirstChild(), "Type");
		TiXmlNode* PersistentNode = xml->enterNode(propertiesNode->FirstChild(), "Persistent");
		
		bool persistent = true;
		if(PersistentNode)
		{

		}

		if(typeNode)
		{
			strType = xml->getValStr(typeNode);

			if(strType == "ARRAY")
			{
				FixedArrayType* dataType = new FixedArrayType();
				DictItemDataTypePtr pDictItemDataType(new DictItemDataType());
				pDictItemDataType->dataType = dataType;

				if(dataType->initialize(xml, typeNode, dataTypes))
				{
					DATATYPE_UID uid = dataType->id();
					//EntityDef::md5().append((void*)&uid, sizeof(DATATYPE_UID));
					//EntityDef::md5().append((void*)strType.c_str(), (int)strType.size());
					//EntityDef::md5().append((void*)typeName.c_str(), (int)typeName.size());

					keyTypes_.push_back(std::pair< std::string, DictItemDataTypePtr >(typeName, pDictItemDataType));

					if(dataType->getDataType()->type() == DATA_TYPE_MAILBOX)
					{
						persistent = false;
						//EntityDef::md5().append((void*)&persistent, sizeof(bool));
					}

					pDictItemDataType->persistent = persistent;
					dataTypes->addDataType(std::string("_") + genUUID64() + typeName, dataType);
				}
				else
				{
					return false;
				}
			}
			else
			{
				DataType* dataType = dataTypes->getDataType(strType);
				DictItemDataTypePtr pDictItemDataType(new DictItemDataType());
				pDictItemDataType->dataType = dataType;

				if(dataType != NULL)
				{
					DATATYPE_UID uid = dataType->id();
					//EntityDef::md5().append((void*)&uid, sizeof(DATATYPE_UID));
					//EntityDef::md5().append((void*)strType.c_str(), (int)strType.size());
					//EntityDef::md5().append((void*)typeName.c_str(), (int)typeName.size());

					keyTypes_.push_back(std::pair< std::string, DictItemDataTypePtr >(typeName, pDictItemDataType));
					
					if(dataType->type() == DATA_TYPE_MAILBOX)
					{
						persistent = false;
						//EntityDef::md5().append((void*)&persistent, sizeof(bool));
					}

					pDictItemDataType->persistent = persistent;
				}
				else
				{
					return false;
				}
			}
		}
	}
	XML_FOR_END(propertiesNode);

	TiXmlNode* implementedByNode = xml->enterNode(node, "implementedBy");
	if(implementedByNode)
	{
		strType = xml->getValStr(implementedByNode);

		if(g_componentType == CELLAPP_TYPE || g_componentType == BASEAPP_TYPE ||
				g_componentType == CLIENT_TYPE)
		{
			if(strType.size() > 0 && !loadImplModule(strType))
				return false;

			moduleName_ = strType;
		}

		//if(strType.size() > 0)
			//EntityDef::md5().append((void*)strType.c_str(), (int)strType.size());
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool FixedDictType::loadImplModule(std::string moduleName)
{

	std::vector<std::string> res_;
	kbe_split<char>(moduleName, '.', res_);
	
	if(res_.size() != 2)
	{
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
}
