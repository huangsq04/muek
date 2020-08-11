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
#include "scriptdef_module.h"
#include "entitydef.h"
#include "datatypes.h"
#include "common.h"
#include <sstream>

//#include "common/smartpointer.h"
#include "entity_mailbox.h"

static char *g_EntitySuffix = "Entity";
static char *g_EntityInterfaceSuffix = "EI";

namespace KBEngine{

//-------------------------------------------------------------------------------------
ScriptDefModule::ScriptDefModule(std::string name, ENTITY_SCRIPT_UID utype):
uType_(utype),
persistentPropertyDescr_(),
cellPropertyDescr_(),
basePropertyDescr_(),
clientPropertyDescr_(),
persistentPropertyDescr_uidmap_(),
cellPropertyDescr_uidmap_(),
basePropertyDescr_uidmap_(),
clientPropertyDescr_uidmap_(),
propertyDescr_aliasmap_(),
methodCellDescr_(),
methodBaseDescr_(),
methodClientDescr_(),
methodBaseExposedDescr_(),
methodCellExposedDescr_(),
methodCellDescr_uidmap_(),
methodBaseDescr_uidmap_(),
methodClientDescr_uidmap_(),
methodDescr_aliasmap_(),
hasCell_(false),
hasBase_(false),
hasClient_(false),
pVolatileinfo_(new VolatileInfo()),
name_(name),
ue4class_("AActor"),
usePropertyDescrAlias_(false),
useMethodDescrAlias_(false)
{
	//EntityDef::md5().append((void*)name.c_str(), (int)name.size());
}

//-------------------------------------------------------------------------------------
ScriptDefModule::~ScriptDefModule()
{

}

//-------------------------------------------------------------------------------------
void ScriptDefModule::finalise(void)
{
	PROPERTYDESCRIPTION_MAP::iterator iter1 = cellPropertyDescr_.begin();
	//for(; iter1 != cellPropertyDescr_.end(); ++iter1)
	//	iter1->second->decRef();
	
	cellPropertyDescr_.clear();

	iter1 = basePropertyDescr_.begin();
	//for(; iter1 != basePropertyDescr_.end(); ++iter1)
	//	iter1->second->decRef();

	basePropertyDescr_.clear();

	iter1 = clientPropertyDescr_.begin();
	//for(; iter1 != clientPropertyDescr_.end(); ++iter1)
	//	iter1->second->decRef();

	clientPropertyDescr_.clear();

	METHODDESCRIPTION_MAP::iterator iter2 = methodCellDescr_.begin();
	//for(; iter2 != methodCellDescr_.end(); ++iter2)
	//	SAFE_RELEASE(iter2->second);
		
	methodCellDescr_.clear();

	METHODDESCRIPTION_MAP::iterator iter3 = methodBaseDescr_.begin();
	//for(; iter3 != methodBaseDescr_.end(); ++iter3)
	//	SAFE_RELEASE(iter3->second);
	
	methodBaseDescr_.clear();

	METHODDESCRIPTION_MAP::iterator iter4 = methodClientDescr_.begin();
	//for(; iter4 != methodClientDescr_.end(); ++iter4)
	//	SAFE_RELEASE(iter4->second);

	methodClientDescr_.clear();
}

//-------------------------------------------------------------------------------------
void ScriptDefModule::onLoaded(void)
{
	//if(EntityDef::entitydefAliasID())
	{
		int aliasID = ENTITY_BASE_PROPERTY_ALIASID_MAX;
		PROPERTYDESCRIPTION_MAP::iterator iter1 = cellPropertyDescr_.begin();
		/*
		for(; iter1 != cellPropertyDescr_.end(); ++iter1)
		{
			if(iter1->second->hasClient())
			{
				propertyDescr_aliasmap_[aliasID] = iter1->second;
				iter1->second->aliasID(aliasID++);
			}
		}

		iter1 = basePropertyDescr_.begin();
		for(; iter1 != basePropertyDescr_.end(); ++iter1)
		{
			if(iter1->second->hasClient())
			{
				propertyDescr_aliasmap_[aliasID] = iter1->second;
				iter1->second->aliasID(aliasID++);
			}
		}
		*/
		iter1 = clientPropertyDescr_.begin();
		for(; iter1 != clientPropertyDescr_.end(); ++iter1)
		{
			if(iter1->second->hasClient())
			{
				propertyDescr_aliasmap_[aliasID] = iter1->second;
				iter1->second->aliasID(aliasID++);
			}
		}
		
		if(aliasID > 255)
		{
			iter1 = cellPropertyDescr_.begin();
			for(; iter1 != cellPropertyDescr_.end(); ++iter1)
			{
				if(iter1->second->hasClient())
				{
					iter1->second->aliasID(-1);
				}
			}

			iter1 = basePropertyDescr_.begin();
			for(; iter1 != basePropertyDescr_.end(); ++iter1)
			{
				if(iter1->second->hasClient())
				{
					iter1->second->aliasID(-1);
				}
			}

			iter1 = clientPropertyDescr_.begin();
			for(; iter1 != clientPropertyDescr_.end(); ++iter1)
			{
				if(iter1->second->hasClient())
				{
					iter1->second->aliasID(-1);
				}
			}

			propertyDescr_aliasmap_.clear();
		}
		else
		{
			usePropertyDescrAlias_ = true;
		}

		aliasID = 0;

		METHODDESCRIPTION_MAP::iterator iter2 = methodClientDescr_.begin();
		for(; iter2 != methodClientDescr_.end(); ++iter2)
		{
			methodDescr_aliasmap_[aliasID] = iter2->second;
			iter2->second->aliasID(aliasID++);
		}

		if(aliasID > 255)
		{
			METHODDESCRIPTION_MAP::iterator iter2 = methodClientDescr_.begin();
			for(; iter2 != methodClientDescr_.end(); ++iter2)
			{
				iter2->second->aliasID(-1);
				methodDescr_aliasmap_.clear();
			}
		}
		else
		{
			useMethodDescrAlias_ = true;
		}
	}

}

//-------------------------------------------------------------------------------------
void ScriptDefModule::c_str()
{


}

//-------------------------------------------------------------------------------------
void ScriptDefModule::setUType(ENTITY_SCRIPT_UID utype)
{ 
	uType_ = utype; 
	//EntityDef::md5().append((void*)&uType_, sizeof(ENTITY_SCRIPT_UID));
}

/*
void ScriptDefModule::addSmartUTypeToStream(MemoryStream* pStream)
{
	if(EntityDef::scriptModuleAliasID())
		(*pStream) << getAliasID();
	else
		(*pStream) << getUType();
}
*/
void ScriptDefModule::autoMatchCompOwn()
{

}

//-------------------------------------------------------------------------------------
bool ScriptDefModule::addPropertyDescription(const char* attrName, 
										  PropertyDescription* propertyDescription, 
										  COMPONENT_TYPE componentType)
{
	PropertyDescription* f_propertyDescription = NULL;
	PROPERTYDESCRIPTION_MAP*  propertyDescr;
	PROPERTYDESCRIPTION_UIDMAP*  propertyDescr_uidmap;

	switch(componentType)
	{
	case CELLAPP_TYPE:
			f_propertyDescription = findCellPropertyDescription(attrName);
			propertyDescr = &getCellPropertyDescriptions();
			propertyDescr_uidmap = &getCellPropertyDescriptions_uidmap();
			
			// 判断他们是什么级别的属性， 将其保存到对应detailLevel的地方
			if((propertyDescription->getFlags() & ENTITY_CLIENT_DATA_FLAGS) > 0){
				cellDetailLevelPropertyDescrs_[propertyDescription->getDetailLevel()][attrName] = propertyDescription;
			}

			setCell(true);
			break;
	case BASEAPP_TYPE:
			f_propertyDescription = findBasePropertyDescription(attrName);
			propertyDescr = &getBasePropertyDescriptions();
			propertyDescr_uidmap = &getBasePropertyDescriptions_uidmap();
			setBase(true);
			break;
	default:
			f_propertyDescription = findClientPropertyDescription(attrName);
			propertyDescr = &getClientPropertyDescriptions();
			propertyDescr_uidmap = &getClientPropertyDescriptions_uidmap();
			setClient(true);
			break;
	};

	if(f_propertyDescription)
	{
		return false;
	}

	(*propertyDescr)[attrName] = propertyDescription;
	(*propertyDescr_uidmap)[propertyDescription->getUType()] = propertyDescription;
	//propertyDescription->incRef();


	// 判断是否是存储属性， 是就存储到persistentPropertyDescr_
	if(propertyDescription->isPersistent())
	{
		PROPERTYDESCRIPTION_MAP::const_iterator pciter = 
			persistentPropertyDescr_.find(attrName);

		if(pciter == persistentPropertyDescr_.end())
		{
			persistentPropertyDescr_[attrName] = propertyDescription;
			persistentPropertyDescr_uidmap_[propertyDescription->getUType()] = propertyDescription;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------
PropertyDescription* ScriptDefModule::findCellPropertyDescription(const char* attrName)
{
	PROPERTYDESCRIPTION_MAP::iterator iter = cellPropertyDescr_.find(attrName);
	if(iter == cellPropertyDescr_.end())
	{
		return NULL;
	}
	return iter->second;
}

//-------------------------------------------------------------------------------------
PropertyDescription* ScriptDefModule::findBasePropertyDescription(const char* attrName)
{
	PROPERTYDESCRIPTION_MAP::iterator iter = basePropertyDescr_.find(attrName);
	if(iter == basePropertyDescr_.end())
	{
		return NULL;
	}
	return iter->second;
}

//-------------------------------------------------------------------------------------
PropertyDescription* ScriptDefModule::findClientPropertyDescription(const char* attrName)
{
	PROPERTYDESCRIPTION_MAP::iterator iter = clientPropertyDescr_.find(attrName);
	if(iter == clientPropertyDescr_.end())
	{
		return NULL;
	}
	return iter->second;
}

//-------------------------------------------------------------------------------------
PropertyDescription* ScriptDefModule::findPersistentPropertyDescription(const char* attrName)
{
	PROPERTYDESCRIPTION_MAP::iterator iter = persistentPropertyDescr_.find(attrName);
	if(iter == persistentPropertyDescr_.end())
	{
		return NULL;
	}
	return iter->second;
}

//-------------------------------------------------------------------------------------
PropertyDescription* ScriptDefModule::findCellPropertyDescription(ENTITY_PROPERTY_UID utype)
{
	PROPERTYDESCRIPTION_UIDMAP::iterator iter = cellPropertyDescr_uidmap_.find(utype);

	if(iter == cellPropertyDescr_uidmap_.end())
	{
		return NULL;
	}

	return iter->second;
}

//-------------------------------------------------------------------------------------
PropertyDescription* ScriptDefModule::findBasePropertyDescription(ENTITY_PROPERTY_UID utype)
{
	PROPERTYDESCRIPTION_UIDMAP::iterator iter = basePropertyDescr_uidmap_.find(utype);

	if(iter == basePropertyDescr_uidmap_.end())
	{
		return NULL;
	}

	return iter->second;
}

//-------------------------------------------------------------------------------------
PropertyDescription* ScriptDefModule::findClientPropertyDescription(ENTITY_PROPERTY_UID utype)
{
	PROPERTYDESCRIPTION_UIDMAP::iterator iter = clientPropertyDescr_uidmap_.find(utype);

	if(iter == clientPropertyDescr_uidmap_.end())
	{
		return NULL;
	}

	return iter->second;
}

//-------------------------------------------------------------------------------------
PropertyDescription* ScriptDefModule::findPersistentPropertyDescription(ENTITY_PROPERTY_UID utype)
{
	PROPERTYDESCRIPTION_UIDMAP::iterator iter = persistentPropertyDescr_uidmap_.find(utype);

	if(iter == persistentPropertyDescr_uidmap_.end())
	{
		return NULL;
	}

	return iter->second;
}

//-------------------------------------------------------------------------------------
PropertyDescription* ScriptDefModule::findPropertyDescription(const char* attrName, 
															  COMPONENT_TYPE componentType)
{
	switch(componentType)
	{
	case CELLAPP_TYPE:
			return findCellPropertyDescription(attrName);
			break;
	case BASEAPP_TYPE:
			return findBasePropertyDescription(attrName);
			break;
	default:
			return findClientPropertyDescription(attrName);
			break;
	};

	return NULL;
}

//-------------------------------------------------------------------------------------
PropertyDescription* ScriptDefModule::findPropertyDescription(ENTITY_PROPERTY_UID utype, 
															  COMPONENT_TYPE componentType)
{
	switch(componentType)
	{
	case CELLAPP_TYPE:
			return findCellPropertyDescription(utype);
			break;
	case BASEAPP_TYPE:
			return findBasePropertyDescription(utype);
			break;
	default:
			return findClientPropertyDescription(utype);
			break;
	};

	return NULL;
}

//-------------------------------------------------------------------------------------
MethodDescription* ScriptDefModule::findMethodDescription(const char* attrName, 
														  COMPONENT_TYPE componentType)
{
	switch(componentType)
	{
	case CELLAPP_TYPE:
			return findCellMethodDescription(attrName);
			break;
	case BASEAPP_TYPE:
			return findBaseMethodDescription(attrName);
			break;
	default:
			return findClientMethodDescription(attrName);
			break;
	};

	return NULL;
}

//-------------------------------------------------------------------------------------
MethodDescription* ScriptDefModule::findMethodDescription(ENTITY_METHOD_UID utype, 
														  COMPONENT_TYPE componentType)
{
	switch(componentType)
	{
	case CELLAPP_TYPE:
			return findCellMethodDescription(utype);
			break;
	case BASEAPP_TYPE:
			return findBaseMethodDescription(utype);
			break;
	default:
			return findClientMethodDescription(utype);
			break;
	};

	return NULL;
}

//-------------------------------------------------------------------------------------
MethodDescription* ScriptDefModule::findCellMethodDescription(const char* attrName)
{
	METHODDESCRIPTION_MAP::iterator iter = methodCellDescr_.find(attrName);
	if(iter == methodCellDescr_.end())
	{
		return NULL;
	}

	return iter->second;
}

//-------------------------------------------------------------------------------------
MethodDescription* ScriptDefModule::findCellMethodDescription(ENTITY_METHOD_UID utype)
{
	METHODDESCRIPTION_UIDMAP::iterator iter = methodCellDescr_uidmap_.find(utype);
	if(iter == methodCellDescr_uidmap_.end())
	{
		return NULL;
	}

	return iter->second;
}

//-------------------------------------------------------------------------------------
PropertyDescription* ScriptDefModule::findAliasPropertyDescription(ENTITY_DEF_ALIASID aliasID)
{
	PROPERTYDESCRIPTION_ALIASMAP::iterator iter = propertyDescr_aliasmap_.find(aliasID);

	if(iter == propertyDescr_aliasmap_.end())
	{
		return NULL;
	}

	return iter->second;
}

//-------------------------------------------------------------------------------------
MethodDescription* ScriptDefModule::findAliasMethodDescription(ENTITY_DEF_ALIASID aliasID)
{
	METHODDESCRIPTION_ALIASMAP::iterator iter = methodDescr_aliasmap_.find(aliasID);
	if(iter == methodDescr_aliasmap_.end())
	{
		return NULL;
	}

	return iter->second;
}

//-------------------------------------------------------------------------------------
bool ScriptDefModule::addCellMethodDescription(const char* attrName, 
											   MethodDescription* methodDescription)
{
	MethodDescription* f_methodDescription = findCellMethodDescription(attrName);
	if(f_methodDescription)
	{
		return false;
	}
	
	setCell(true);
	methodCellDescr_[attrName] = methodDescription;
	methodCellDescr_uidmap_[methodDescription->getUType()] = methodDescription;

	if(methodDescription->isExposed())
		methodCellExposedDescr_[attrName] = methodDescription;

	return true;
}

//-------------------------------------------------------------------------------------
MethodDescription* ScriptDefModule::findBaseMethodDescription(const char* attrName)
{
	METHODDESCRIPTION_MAP::iterator iter = methodBaseDescr_.find(attrName);
	if(iter == methodBaseDescr_.end())
	{
		return NULL;
	}
	return iter->second;
}

//-------------------------------------------------------------------------------------
MethodDescription* ScriptDefModule::findBaseMethodDescription(ENTITY_METHOD_UID utype)
{
	METHODDESCRIPTION_UIDMAP::iterator iter = methodBaseDescr_uidmap_.find(utype);
	if(iter == methodBaseDescr_uidmap_.end())
	{
		return NULL;
	}
	return iter->second;
}

//-------------------------------------------------------------------------------------
bool ScriptDefModule::addBaseMethodDescription(const char* attrName, 
											   MethodDescription* methodDescription)
{
	MethodDescription* f_methodDescription = findBaseMethodDescription(attrName);
	if(f_methodDescription)
	{
		return false;
	}
	
	setBase(true);
	methodBaseDescr_[attrName] = methodDescription;
	methodBaseDescr_uidmap_[methodDescription->getUType()] = methodDescription;

	if(methodDescription->isExposed())
		methodBaseExposedDescr_[attrName] = methodDescription;

	return true;
}

//-------------------------------------------------------------------------------------
MethodDescription* ScriptDefModule::findClientMethodDescription(const char* attrName)
{
	METHODDESCRIPTION_MAP::iterator iter = methodClientDescr_.find(attrName);
	if(iter == methodClientDescr_.end())
	{
		return NULL;
	}
	return iter->second;
}

//-------------------------------------------------------------------------------------
MethodDescription* ScriptDefModule::findClientMethodDescription(ENTITY_METHOD_UID utype)
{
	METHODDESCRIPTION_UIDMAP::iterator iter = methodClientDescr_uidmap_.find(utype);
	if(iter == methodClientDescr_uidmap_.end())
	{
		return NULL;
	}
	return iter->second;
}

//-------------------------------------------------------------------------------------
bool ScriptDefModule::addClientMethodDescription(const char* attrName, 
												 MethodDescription* methodDescription)
{
	MethodDescription* f_methodDescription = findClientMethodDescription(attrName);
	if(f_methodDescription)
	{
		return false;
	}

	setClient(true);
	methodClientDescr_[attrName] = methodDescription;
	methodClientDescr_uidmap_[methodDescription->getUType()] = methodDescription;
	return true;
}

//-------------------------------------------------------------------------------------
ScriptDefModule::PROPERTYDESCRIPTION_MAP& ScriptDefModule::getPropertyDescrs()
{
	ScriptDefModule::PROPERTYDESCRIPTION_MAP* lpPropertyDescrs = NULL;	

	switch(g_componentType)	
	{					
		case CELLAPP_TYPE:
			lpPropertyDescrs = &getCellPropertyDescriptions();
			break;
		case BASEAPP_TYPE:
			lpPropertyDescrs = &getBasePropertyDescriptions();
			break;	
		default:
			lpPropertyDescrs = &getClientPropertyDescriptions();
			break;	
	};
	
	return *lpPropertyDescrs;	
}
std::string ScriptDefModule::GetUE4Property(const std::string &name)
{
	if (findCellPropertyDescription(name.c_str()) == NULL)
		return "UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = \"Entity\")";
	else
		return "UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = \"Entity\")";
}
/**
* 产生定义的json字符串
*
* @return json字符串
*/
std::string ScriptDefModule::ToJson()
{
	std::string retType = "'" + name_ + "':{";
	PROPERTYDESCRIPTION_MAP::iterator iter1 = clientPropertyDescr_.begin();
	for (; iter1 != clientPropertyDescr_.end(); ++iter1)
	{
		retType = retType + "'" + iter1->second->getName() + "':" + iter1->second->getDataType()->ToJson()+ ",";
	}
	retType = retType.substr(0, retType.size() - 1);
	retType = retType + "}";
	return retType;
}



/**
* 判断是否可以产生蓝图方法
*
* @return 蓝图类型true或false
*/
bool  ScriptDefModule::IsUE4BluePrintMethod(MethodDescription *method)
{
	size_t size = method->getArgSize();
	for (uint32 i = 0; i < size; i++)
	{
		DataType* dt = method->getArgTypes()[i];
		if (dt->type() == DATA_TYPE_FIXEDARRAY)
		{
			KBEngine::FixedArrayType* arraytype = (KBEngine::FixedArrayType*)(dt);
			if (arraytype->getDataType()->type() == DATA_TYPE_FIXEDDICT)
			{
				return true;
			}
			else
			{
				if (!ADataTypes::IsBluePrintType(arraytype->getDataType()))
					return false;
			}
		}
		else if (dt->type() == DATA_TYPE_FIXEDDICT)
		{
			return true;
		}
		else if (!ADataTypes::IsBluePrintType(dt))
		{
			return false;
		}
	}
	return true;
}
/**
* UE4授权方法调用
* @param method 方法描述
* @return 给定方法定义的c++代码
*/
std::string  ScriptDefModule::DefineToUE4AuthorityCall(MethodDescription *method)
{
	std::stringstream ss;
	ss << "    void Call" << method->getName() << "(EKBEFunctionFlags flag,";
	std::string writestream = "";
	size_t size = method->getArgSize();
	for (uint32 i = 0; i < size; i++)
	{
		DataType* dt = method->getArgTypes()[i];
		std::string TypeName = ADataTypes::GetDataTypeUE4Type(dt);
		std::stringstream sargname;
		std::string sArgName = method->getArgNames()[i];
		sargname << sArgName;
		std::string ArgName(sargname.str());
		if (TypeName == "ARRAY")
		{
			FixedArrayType* dataType = (FixedArrayType*)dt;
			std::string subType = ADataTypes::GetDataTypeUE4Type(dataType->getDataType());
			if (subType == "FIXED_DICT")
			{
				subType = std::string("F") + dataType->getDataType()->aliasName();
				writestream = writestream + "        BundlePtr->WriteInt32(" + ArgName + ".Num());\n";
				writestream = writestream + "        for(int i = 0; i < " + ArgName + ".Num(); i++)\n";
				writestream = writestream + "            " + ArgName + "[i].AddToStream(BundlePtr);\n";

			}
			else
			{
				writestream = writestream + "        BundlePtr->WriteInt32(" + ArgName + ".Num());\n";
				writestream = writestream + "        for(int i = 0; i < " + ArgName + ".Num(); i++)\n";
				writestream = writestream + "            BundlePtr->Write" + ADataTypes::GetDataBaseTypeName(dataType->getDataType()) + "(" + ArgName + "[i]);\n";
			}
			TypeName = "TArray<" + subType + ">";
			ss << "const " << TypeName << " &" << ArgName;
		}
		else if (TypeName == "FIXED_DICT")
		{
			ss << "const F" << dt->aliasName() << " &" << ArgName;
		}
		else
		{
			if (TypeName == "FString" || TypeName == "TArray<uint8>")
				ss << "const " << TypeName << " &" << ArgName;
			else
				ss << "const " << TypeName << " " << ArgName;
			std::string wType = TypeName;
			if (TypeName == "FVector")
			{
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".X);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".Y);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".Z);\n";
			}
			else if (TypeName == "FVector2D")
			{
				writestream = writestream + "        BundlePtr->WriteUint32(2);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".X);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".Y);\n";
			}
			else if (TypeName == "FVector4")
			{
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".X);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".Y);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".Z);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".W);\n";
			}
			else
				writestream = writestream + "        BundlePtr->Write" + ADataTypes::GetDataBaseTypeName(dt) + "(" + ArgName + ");\n";
		}
		if (i != size - 1)
			ss << ",";
	}

	ss
		<< ")\n"
		<< "    {\n"
		<< "        FBundle* BundlePtr = new FBundle();\n"
		<< "        BundlePtr->WriteInt32(EntityID);\n";
	if(useMethodDescrAlias())
		ss  << "        BundlePtr->WriteUint8(" << method->aliasID() << ");\n";
	else
		ss << "        BundlePtr->WriteUint16(" << method->aliasID() << ");\n";
	ss  << writestream 
		<< "        OnDefindCallFunction(flag, BundlePtr);\n"
	    << "        delete BundlePtr;\n    }\n";
	return ss.str();
}
/**
* 产生调用服务器base和cell方法
*
* @param method 方法描述
* @param type 0:base方法,1:cell方法
* @return 给定方法定义的c++代码
*/
std::string  ScriptDefModule::DefineToUE4Method(MethodDescription *method, int type)
{
	std::stringstream ss;
	ss << "    void " << method->getName() << "(";
	std::string writestream = "";
	size_t size = method->getArgSize();
	for (uint32 i = 0; i < size; i++)
	{
		DataType* dt = method->getArgTypes()[i];
		std::string TypeName = ADataTypes::GetDataTypeUE4Type(dt);
		std::stringstream sargname;
		std::string sArgName = method->getArgNames()[i];
		sargname << sArgName;
		std::string ArgName(sargname.str());
		if (TypeName == "ARRAY")
		{
			FixedArrayType* dataType = (FixedArrayType*)dt;
			std::string subType = ADataTypes::GetDataTypeUE4Type(dataType->getDataType());
			if (subType == "FIXED_DICT")
			{
				subType = std::string("F") + dataType->getDataType()->aliasName();
				writestream = writestream + "        BundlePtr->WriteInt32(" + ArgName + ".Num());\n";
				writestream = writestream + "        for(int i = 0; i < " + ArgName + ".Num(); i++)\n";
				writestream = writestream + "            " + ArgName + "[i].AddToStream(BundlePtr);\n";

			}
			else
			{
				writestream = writestream + "        BundlePtr->WriteInt32(" + ArgName + ".Num());\n";
				writestream = writestream + "        for(int i = 0; i < " + ArgName + ".Num(); i++)\n";
				writestream = writestream + "            BundlePtr->Write" + ADataTypes::GetDataBaseTypeName(dataType->getDataType()) + "(" + ArgName + "[i]);\n";
			}
			TypeName = "TArray<" + subType + ">";
			ss << "const " << TypeName << " &" << ArgName ;
		}
		else if (TypeName == "FIXED_DICT")
		{
			ss << "const F" << dt->aliasName() << " &" << ArgName ;
		}
		else
		{
			if(TypeName == "FString" || TypeName == "TArray<uint8>")
				ss << "const " << TypeName << " &" << ArgName ;
			else
				ss << "const " << TypeName << " " << ArgName;
			std::string wType = TypeName;
			if (TypeName == "FVector")
			{
				//writestream = writestream + "        BundlePtr->WriteUint32(3);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".X);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".Y);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".Z);\n";
			}
			else if (TypeName == "FVector2D")
			{
				writestream = writestream + "        BundlePtr->WriteUint32(2);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".X);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".Y);\n";
			}
			else if (TypeName == "FVector4")
			{
				//writestream = writestream + "        BundlePtr->WriteUint32(3);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".X);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".Y);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".Z);\n";
				writestream = writestream + "        BundlePtr->WriteFloat(" + ArgName + ".W);\n";
			}
			else
				writestream = writestream + "        BundlePtr->Write" + ADataTypes::GetDataBaseTypeName(dt) + "(" + ArgName + ");\n";			
		}
		if(i != size - 1)
			ss << ",";
	}

	ss << ")";
	ss << "\n    {\n";
	if (type == 0)
		ss << "        FBundle* BundlePtr = Entity->BaseMailbox()->NewMail();\n";
	else
		ss << "        FBundle* BundlePtr = Entity->CellMailbox()->NewMail();\n";

	//调用base或cell方法方法是UINT16
	ss << "        BundlePtr->WriteUint16(" << method->getUType() << ");\n";


	if (type == 0 )
		ss <<  writestream  << "        Entity->BaseMailbox()->PostMail(NULL);\n    }\n";
	else if (type == 1)
		ss << writestream << "        Entity->CellMailbox()->PostMail(NULL);\n    }\n";
	else
		ss << ";\n";
	return ss.str();
}

/**
* 产生客户端函数代码
*
* @param method 方法描述
* @param type 1:蓝图方法 0:非蓝图方法
* @return 给定方法定义的c++代码
*/
std::string  ScriptDefModule::DefineToUE4ClientMethod(MethodDescription *method, int type)
{
	std::stringstream ss;
	if(type == 1)
	    ss << "    void " << method->getName() << "(";
	else
		ss << "    virtual void " << method->getName() << "_Implementation(";
	std::string writestream = "    ";
	size_t size = method->getArgSize();
	for (uint32 i = 0; i < size; i++)
	{
		DataType* dt = method->getArgTypes()[i];
		std::string sArgName = method->getArgNames()[i];
		std::string TypeName = ADataTypes::GetDataTypeUE4Type(dt);
		std::stringstream sargname;
		sargname << sArgName;
		std::string ArgName(sargname.str());
		if (TypeName == "ARRAY")
		{
			FixedArrayType* dataType = (FixedArrayType*)dt;
			std::string subType = ADataTypes::GetDataTypeUE4Type(dataType->getDataType());
			if (subType == "FIXED_DICT")
			{
				subType = std::string("F") + dataType->getDataType()->aliasName();
			}
			TypeName = "TArray<" + subType + ">";
			ss << "const " << TypeName << " &" << ArgName;
		}
		else if (TypeName == "FIXED_DICT")
		{
			ss << "const F" << dt->aliasName() << " &" << ArgName;
		}
		else if (TypeName == "FString" || TypeName == "TArray<uint8>")
		{

			ss << "const " << TypeName << " &" << ArgName;

		}
		else if (TypeName == "FVector")
		{
			ss << "const " << TypeName << " &" << ArgName;
		}
		else if (TypeName == "FVector2D")
		{
			ss << "const " << TypeName << " &" << ArgName;
		}
		else if (TypeName == "FVector4")
		{
			ss << "const " << TypeName << " &" << ArgName;
		}
		else
		{
			ss << "const " << TypeName << " " << ArgName;
		}
		if (i != size - 1)
			ss << ",";
	}
	if (type == 1)
		ss << ");\n";
	else
		ss << "){};\n";

	return ss.str();
}

/**
* 产生客户端函数调用代码
*
* @param method 方法描述
* @return 给定方法定义的c++代码
*/
std::string  ScriptDefModule::ClientMethodCall(MethodDescription *method)
{
	std::stringstream ss;
	std::string args = "";
	ss << "            case " << method->aliasID()  << ":\n" << "            {\n";
	size_t size = method->getArgSize();
	for (uint32 i = 0; i < size; i++)
	{
		DataType* dt = method->getArgTypes()[i];
		std::string TypeName = ADataTypes::GetDataTypeUE4Type(dt);
		char buf[16] = { 0 };
		std::stringstream sargname;
		std::string sArgName = method->getArgNames()[i];
		sargname << sArgName;
		std::string ArgName(sargname.str());
		args = args + std::string(sargname.str()) + ",";
		if (TypeName == "ARRAY")
		{
			FixedArrayType* dataType = (FixedArrayType*)dt;
			std::string subType = ADataTypes::GetDataTypeUE4Type(dataType->getDataType());
			std::string aliasName = dataType->getDataType()->aliasName();
			if (subType == "FIXED_DICT")
			{
				TypeName = "TArray<F" + aliasName + ">";
			}
			else
				TypeName = "TArray<" + subType + ">";


			ss << "               " << TypeName + " " << ArgName + ";\n"
			   << "               int " << ArgName << "size = Stream.ReadInt32();\n"
			   << "               for (int i = 0; i < " << ArgName << "size; i++)\n"
			   << "               {\n";

			if (subType == "FIXED_DICT")
			{
				subType = dataType->getDataType()->aliasName();
				ss << "                   F" << subType << " Value;\n"
				   << "                   Value.CreateFromStream(&Stream);\n";
			}
			else
			{
				ss << "                   " 
					<< subType 
					<< " value =  Stream.Read" + ADataTypes::GetDataBaseTypeName(dataType->getDataType()) + "();\n";
			}

			ss << "                   " << ArgName << ".Add(Value);\n"   << "               }\n";

		}
		else if (TypeName == "FIXED_DICT")
		{
			ss << "               F" << dt->aliasName() << " " << ArgName << ";\n"
			   << "               " << ArgName << ".CreateFromStream(&Stream);\n";
		}
		else
		{
			ss << "               " << TypeName << " " << ArgName << ";\n";
			TypeName[0] = TypeName[0] - 0x20;
			ss << "               " << ArgName << "= Stream.Read" + ADataTypes::GetDataBaseTypeName(dt) + "();\n";
		}
	}
	if (size != 0)
	{
		args = args.substr(0, args.size() - 1);
	}
	ss << "               " << std::string(method->getName()) << "(" << args << ");\n"
	   //<< "               " << std::string(method->getName()) << "_Implementation(" << args << ");\n"
		<< "               break;\n"
		<< "            }\n";

	return ss.str();
}

/**
* 根据模块名称产生创建UObject的头c++代码
*
* @param name 模块名称如果是Entity表示定义的实体其他表示接口
* @return c++代码
*/
std::string  ScriptDefModule::InitCreateUobjectHead(const std::string &name)
{
	std::string modename = name;
	std::string uobjectname = name;
		
	if (name == "Entity")
	{
		modename = name_;
		uobjectname = modename + g_EntitySuffix;
	}
	else
	{
		modename = modename + g_EntityInterfaceSuffix;
		uobjectname = modename;
	}
	std::stringstream pret;
	pret << "UCLASS(BlueprintType, Blueprintable)\nclass " << UEntityDef::UE4ProjectAPIName_ << "  U" <<
		uobjectname << ": public UEntity\n{\n    GENERATED_BODY()\npublic:\n";
	pret << "    virtual void Init()\n";
	pret << "    {\n";
	pret << "    	Cell = NewObject<U" << modename << "CellMethod>();\n";
	pret << "    	Cell->Entity = this;\n";
	pret << "    	Base = NewObject<U" << modename << "BaseMethod>();\n";
	pret << "    	Base->Entity = this;\n";
	pret << "    	SetClassName(TEXT(\"" << modename << "\"));\n";
	//if (name == "Entity")
    //		pret << "    	InitInterface(this);\n";
	pret << "    }\n";
	pret << "    virtual bool UsePropertyDescrAlias() { return "<< usePropertyDescrAlias() <<"; };\n";
	pret << "    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = \"Entity\")\n";
	pret << "    U" << modename << "BaseMethod *Base;\n";
	pret << "    virtual UObject *GetBase() { return Base; };\n";
	pret << "    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = \"Entity\")\n";
	pret << "    U" << modename << "CellMethod *Cell;\n";
	pret << "    virtual UObject *GetCell() { return Cell; };\n";


	return pret.str();
}

/**
* 产生def定义的entity属性,客户端方法,base方法,cell方法
*
* @param vl 接口列表 
* @param vm 接口字典，存放创建这个接口的C++代码
* @return c++代码
*/
std::string  ScriptDefModule::DefineToUE4Object(std::vector<std::string> &vl, std::map<std::string, std::string> &vm)
{
	std::stringstream ret;
	std::vector<std::string> bvl;
	std::map<std::string, std::string> bvmret;
	std::map<std::string, std::string> bvmrp;
	ret << InitCreateUobjectHead("Entity");
	bvmret.insert(std::pair<std::string, std::string>("Entity", ret.str()));

	//属性
	std::stringstream ssUpdatePropertys;
	ssUpdatePropertys 
		<< "    virtual FString OnUpdateProperty(uint16 UType, FMemoryStream& Stream)\n"
		<< "    {\n"
	    << "        FString Command = TEXT(\"\");\n";

	//替换属性
	std::stringstream ssGetLifetimeReplicatedProps;
	ssGetLifetimeReplicatedProps 
		<< "    bool IsSupportedForNetworking() const	{ return true;	}\n"
		<< "    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const\n"
		<< "    {\n"
	    << "        Super::GetLifetimeReplicatedProps(OutLifetimeProps);\n";

	std::stringstream ssUpdatePropertysToCell;
	ssUpdatePropertysToCell
		<< "    virtual void UpdatePropertysToCell(uint16 changed, Bundle* BundlePtr)\n"
		<< "    {\n"
		<< "         switch (changed) {\n";



	ssUpdatePropertys 
		<< "        switch(UType){\n"
		<< "            case 0:\n"
		<< "                __SET_ENTITY_POSITION();\n"
		<< "                 break;\n"
		<< "            case 1:\n"
		<< "                __SET_ENTITY_DIRECTION();\n"
		<< "                break;\n"
		<< "            case 2:\n"
		<< "                SpaceID = Stream.ReadInt32();\n"
		<< "                break;\n";

	bvmrp.insert(std::pair<std::string, std::string>("Entity", ssUpdatePropertys.str()));

	PROPERTYDESCRIPTION_MAP::iterator iter1 = clientPropertyDescr_.begin();
	int  repidx = 2;
	for (; iter1 != clientPropertyDescr_.end(); ++iter1)
	{
		bool iscellproperty = findCellPropertyDescription(iter1->second->getName()) != NULL;

		std::stringstream propertystream;
		std::stringstream updatepropstream;
		std::stringstream replicatedpropsstream;
		std::stringstream propertystocellstream;

		std::string TypeName = ADataTypes::GetDataTypeUE4Type(iter1->second->getDataType());
		if (iscellproperty)
		{
			replicatedpropsstream << "        DOREPLIFETIME(U"<< name_ << "Entity, " << iter1->second->getName() << ");\n";
			repidx++;
		}

	
		if (TypeName == "ARRAY")//列表类型子属性可以是字典和基础类型但是不可以包括列表
		{
			FixedArrayType* dataType = (FixedArrayType*)iter1->second->getDataType();

			std::string subType = ADataTypes::GetDataTypeUE4Type(dataType->getDataType());
			std::string dictType = subType;
			if (subType == "FIXED_DICT")
			{
				dictType = std::string("F") + dataType->getDataType()->aliasName();
			}
			TypeName = "TArray<" + dictType + ">";

			propertystream 
				<< "    " << GetUE4Property(iter1->second->getName()) << "\n"
				<< "    " << TypeName << " " << iter1->second->getName() << ";\n";


			updatepropstream 
				<< "            case " << iter1->second->aliasID() << ":\n"				 
				<< "            {\n"				
				<< "                int size = Stream.ReadInt32();\n"						
				<< "                " << iter1->second->getName() << ".Reset();\n"			    
				<< "                for (int i = 0; i < size; i++)\n"				 
				<< "                {\n"				
				<< "                    " << dictType << " value;\n";
			   
			if (iscellproperty)
			{
				propertystocellstream
					<< "         case " << repidx << ":\n"
					<< "         {\n";

				propertystocellstream << "             BundlePtr->WriteUint16(" << iter1->second->getUType() << ");\n";
				propertystocellstream << "             int size = " << iter1->second->getName() << ".Num();\n"
					<< "             BundlePtr->writeUint32(size);\n"
					<< "             for (int i = 0; i < size; i++)\n"
					<< "             {\n";

			}
			if (subType == "FIXED_DICT")
			{
				updatepropstream 
					<< "                    value.CreateFromStream(&Stream);\n";
				if (iscellproperty)
				{
					propertystocellstream
						<< "                 " << iter1->second->getName() << "[i].AddToStream(BundlePtr); \n"
						<< "             break;\n";
				}
			}
			else
			{
				std::string btype = ADataTypes::GetDataBaseTypeName(dataType->getDataType());
				updatepropstream 
					<< "                    value = Stream.Read" << btype << "(); \n";
				if (iscellproperty)
				{
					propertystocellstream
						<< "                 BundlePtr->Write" << btype << "(" << iter1->second->getName() << "[i]);\n";
				}
		    }

			updatepropstream 
				<< "                    " << iter1->second->getName() << ".Add(value);\n"
				<< "                }\n"
				<< "                Command = FString::Printf(TEXT(\"OnRep_" << iter1->second->getName() << "\"));\n"
				<< "                break;\n"
				<< "            }\n";
			if (iscellproperty)
			{
				propertystocellstream 
					<< "             }\n"
					<< "             break;\n"
					<< "          }\n";
			}

		}
		else if (TypeName == "FIXED_DICT") //字典类型
		{
			propertystream 
				<< "    " << GetUE4Property(iter1->second->getName()) <<"\n"
				<< "    F" << iter1->second->getDataType()->aliasName() << " " << iter1->second->getName() << ";\n";

			updatepropstream 
				<< "            case " << iter1->second->aliasID() << ":\n"
				<< "                " << iter1->second->getName() << ".CreateFromStream(&Stream);\n"
				<< "                Command = FString::Printf(TEXT(\"OnRep_" << iter1->second->getName() << "\"));\n"
				<< "                break;\n";
			if (iscellproperty)
			{
				propertystocellstream
					<< "         case " << repidx << ":\n";

				propertystocellstream << "             BundlePtr->WriteUint16(" << iter1->second->getUType() << ");\n";

				propertystocellstream << "             " << iter1->second->getName() << ".AddToStream(BundlePtr);\n"
					<< "             break;\n";
			}
		}
		else //基础类型
		{
			if (ADataTypes::IsBluePrintType(iter1->second->getDataType()))	
				propertystream 
				<< "    " << GetUE4Property(iter1->second->getName())  <<"\n";
					
			propertystream 
				<< "    " << TypeName << " " << iter1->second->getName() << ";\n";

			std::string btype = ADataTypes::GetDataBaseTypeName(iter1->second->getDataType());

			updatepropstream 
				<< "            case " << iter1->second->aliasID() << ":\n"
				<< "                " << iter1->second->getName() << "= Stream.Read" << btype << "();\n"
				<< "                Command = FString::Printf(TEXT(\"OnRep_" << iter1->second->getName() << "\"));\n"
				<< "                break;\n";

			if (iscellproperty)
			{
				propertystocellstream
					<< "         case " << repidx << ":\n";
				propertystocellstream << "             BundlePtr->WriteUint16(" << iter1->second->getUType() << ");\n";

				propertystocellstream << "             BundlePtr->Write" << btype << "(" << iter1->second->getName()  << ");\n"
					<< "             break;\n";
			}

		}

		if (bvmret.find(iter1->second->GetImplements()) == bvmret.end())
		{
			std::stringstream pret;
			std::stringstream pup;
			pret << InitCreateUobjectHead(iter1->second->GetImplements());
			pup 
				<< "    virtual void OnUpdatePropertys(const FString &name, const FVariant &newVal, const FVariant &oldVal)\n"
				<< "    {\n"
				<< "    	UEntity::OnUpdateOldPropertys(name, newVal, oldVal);\n"
				<< "        if(name == \"\"){ return; }\n";
			bvmret.insert(std::pair<std::string, std::string>(iter1->second->GetImplements(), pret.str()));
			bvmrp.insert(std::pair<std::string, std::string>(iter1->second->GetImplements(), pup.str()));

		}

		bvmret.find(iter1->second->GetImplements())->second = bvmret.find(iter1->second->GetImplements())->second + propertystream.str();
		bvmrp.find(iter1->second->GetImplements())->second = bvmrp.find(iter1->second->GetImplements())->second + updatepropstream.str();

		if (iter1->second->GetImplements() == "Entity")
		{
			ret << propertystream.str();
			ssUpdatePropertys << updatepropstream.str();
			ssGetLifetimeReplicatedProps << replicatedpropsstream.str();
			ssUpdatePropertysToCell << propertystocellstream.str();
		}
	}
	ssGetLifetimeReplicatedProps 
		<< "    }\n";
	if(repidx != 2)
    		ret << ssGetLifetimeReplicatedProps.str();

	ssUpdatePropertysToCell
		<< "         default:\n"
		<< "             break;\n"
		<< "         }\n"
		<< "     }\n";
	//if (repidx != 2)
	//	ret << ssUpdatePropertysToCell.str();

	ssUpdatePropertys
		<< "        }\n"
		//<< "        OnUpdatePropertysInterface(UType, Stream);\n"
		<< "        return Command;\n"
	    << "    }\n";

	ret << ssUpdatePropertys.str() << "\n";

	for (std::map<std::string, std::string>::iterator itor = bvmret.begin(); itor != bvmret.end(); ++itor)
	{
		itor->second = itor->second + bvmrp.find(itor->first)->second + "    }\n\n";
	}

	//客户端方法
	std::stringstream clientMethodCall;
	std::stringstream clientMethod;
	std::map<std::string, std::string> bclientMethodCall;
	std::map<std::string, std::string> bclientMethod;

	clientMethodCall << "    virtual void OnRemoteMethodCall(FMemoryStream& Stream)\n"
		<< "    {\n";
		if(useMethodDescrAlias())
			clientMethodCall << "    	    uint16 AliasID = Stream.ReadUint8();\n";
		else
			clientMethodCall << "    	    uint16 AliasID = Stream.ReadUint16();\n";
		clientMethodCall << "            switch(AliasID){\n"
		<< "            case 0xFFFF:\n"
		<< "                 break;\n";
	METHODDESCRIPTION_MAP::iterator clitetMethodItor = methodClientDescr_.begin();
	for (; clitetMethodItor != methodClientDescr_.end(); ++clitetMethodItor)
	{
		MethodDescription *method = clitetMethodItor->second;
		if (bvmret.find(method->GetImplements()) == bvmret.end())
		{
			std::stringstream pret;
			pret << InitCreateUobjectHead(method->GetImplements());
			bvmret.insert(std::pair<std::string, std::string>(method->GetImplements(), pret.str()));
		}
		if (bclientMethodCall.find(method->GetImplements()) == bclientMethodCall.end())
		{
			std::stringstream s;
			s << "    virtual void OnRemoteMethodCall(FMemoryStream& Stream)\n"
			  << "    {\n"
			  << "        uint16 AliasID = Stream.ReadUint8();\n"
			  << "        switch(AliasID){\n"
			  << "        case 0xFFFF:\n"
			  << "            break;\n";

			bclientMethodCall.insert(std::pair<std::string, std::string>(method->GetImplements(), s.str()));
			bclientMethod.insert(std::pair<std::string, std::string>(method->GetImplements(), ""));
		}

		if (IsUE4BluePrintMethod(method))
		{
			std::string UE4UFUNCTION = "";
			if (method->GetAuthority() == 1)
			{
				UE4UFUNCTION = "    UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly,Category = \"Entity | Server\")\n";
			}
			else if(method->GetAuthority() == 2)
			{
				UE4UFUNCTION = "    UFUNCTION(BlueprintNativeEvent, Category = \"Entity | Redirect\")\n";
			}
			else
			{
				UE4UFUNCTION = "    UFUNCTION(BlueprintNativeEvent, Category = \"Entity | Client\")\n";
			}
			std::stringstream ducm;
			ducm << UE4UFUNCTION << DefineToUE4ClientMethod(method, 1);
			ducm << DefineToUE4ClientMethod(method, 0);

			/*
			if (method->GetAuthority() == 1)
			{
				std::string UE4UFUNCTIONCALL = "    UFUNCTION(BlueprintCallable, meta = (DisplayName = \"Call" + std::string(method ->getName()) + "\"), Category = \"Entity | Server\")\n";
				
				ducm << UE4UFUNCTIONCALL << DefineToUE4AuthorityCall(method);
			}
			else if (method->GetAuthority() == 2)
			{
				ducm 
					<< "    virtual void " << method->getName() << "_Implementation(const TArray<uint8> &arg0)\n"
					<< "    {\n"
					<< "        RedirectCallFunction(arg0);\n"
					<< "    }\n";
			}
			*/
			
			bclientMethod.find(method->GetImplements())->second = bclientMethod.find(method->GetImplements())->second + ducm.str();
			if (method->GetImplements() == "Entity")
			{
				clientMethod << ducm.str();
			}
		}
		else
		{
			std::string ducm = DefineToUE4ClientMethod(method, 0);
			bclientMethod.find(method->GetImplements())->second = bclientMethod.find(method->GetImplements())->second + ducm;
			if (method->GetImplements() == "Entity")
				clientMethod << ducm;
		}	
		std::string cmc = ClientMethodCall(method);
		bclientMethodCall.find(method->GetImplements())->second = bclientMethodCall.find(method->GetImplements())->second + cmc;

		if(method->GetImplements() == "Entity")
			clientMethodCall << cmc;
	}
	clientMethodCall << "        }\n";
	for (std::map<std::string, std::string>::iterator itor = bvmret.begin(); itor != bvmret.end(); ++itor)
	{
		if (bclientMethod.find(itor->first) != bclientMethod.end())
		{
			itor->second = itor->second + bclientMethod.find(itor->first)->second;
		}
		if (bclientMethodCall.find(itor->first) != bclientMethodCall.end())
		{
			itor->second = itor->second + bclientMethodCall.find(itor->first)->second;
			itor->second = itor->second + "    }\n";
		}

		itor->second = itor->second + "};\n";
	}
	clientMethodCall << "      }\n";
	ret << clientMethod.str() << clientMethodCall.str() << "\n";

	//base方法
	std::stringstream sbasemethod;
	std::map<std::string, std::string> bsbasemethod;
	METHODDESCRIPTION_MAP::iterator baseMethodItor = methodBaseDescr_.begin();
	for (; baseMethodItor != methodBaseDescr_.end(); ++baseMethodItor)
	{
		MethodDescription *method = baseMethodItor->second;
		if (!method->isExposed())
			continue;

		std::string basemethod = DefineToUE4Method(method, 0);
		if (method->GetImplements() == "Entity")
		{
			if (IsUE4BluePrintMethod(method))
				sbasemethod << "    UFUNCTION(BlueprintCallable, Category = \"Entity | Base\")\n" << basemethod;
			else
				sbasemethod << basemethod;
		}
		if (bsbasemethod.find(method->GetImplements()) == bsbasemethod.end())
		{
			bsbasemethod.insert(std::pair<std::string, std::string>(method->GetImplements(), ""));
		}
		if (IsUE4BluePrintMethod(method))
			bsbasemethod.find(method->GetImplements())->second = bsbasemethod.find(method->GetImplements())->second + \
			"    UFUNCTION(BlueprintCallable, Category = \"Entity | Base\")\n" + basemethod;
		else
			bsbasemethod.find(method->GetImplements())->second = bsbasemethod.find(method->GetImplements())->second +  basemethod;
	
	}
	//cell方法
	std::stringstream scellmethod;
	std::map<std::string, std::string> bscellmethod;
	METHODDESCRIPTION_MAP::iterator cellMethodItor = methodCellDescr_.begin();
	for (; cellMethodItor != methodCellDescr_.end(); ++cellMethodItor)
	{
		MethodDescription *method = cellMethodItor->second;
		if (!method->isExposed())
			continue;

		std::string cellmethod = DefineToUE4Method(method, 1);
		if (method->GetImplements() == "Entity")
		{			
			if (IsUE4BluePrintMethod(method))
				scellmethod << "    UFUNCTION(BlueprintCallable, Category = \"Entity | Cell\")\n" << cellmethod;
			else
				scellmethod << cellmethod;
		}
		if (bscellmethod.find(method->GetImplements()) == bscellmethod.end())
		{
			bscellmethod.insert(std::pair<std::string, std::string>(method->GetImplements(), ""));
		}
		if (IsUE4BluePrintMethod(method))
		    bscellmethod.find(method->GetImplements())->second = bscellmethod.find(method->GetImplements())->second +\
			"    UFUNCTION(BlueprintCallable, Category = \"Entity | Cell\")\n" + cellmethod;
		else
			bscellmethod.find(method->GetImplements())->second = bscellmethod.find(method->GetImplements())->second + cellmethod;	
	
		if (method->GetAuthority() == 2)
		{
			
			ret << "    virtual void " << method->getName() << "(const uint8 arg0, const int32 arg1, const TArray<uint8> &arg2)\n"
				<< "    {\n"
				<< "    	Cell->"<< method->getName() << "(arg0, arg1, arg2);\n"
				<< "    };\n";
		}
	}
	std::stringstream createstream;
	std::map<std::string, std::string> bcreatestream;
	createstream << "\nUCLASS(BlueprintType, Blueprintable)\nclass " << UEntityDef::UE4ProjectAPIName_ << " U" <<
		name_ << "CellMethod: public UEntityServerMethod\n{\n    GENERATED_BODY()\npublic:\n" << scellmethod.str()
		<< "};\n";

	createstream << "\nUCLASS(BlueprintType, Blueprintable)\nclass " << UEntityDef::UE4ProjectAPIName_ << " U" <<
		name_ << "BaseMethod: public UEntityServerMethod\n{\n    GENERATED_BODY()\npublic:\n" << sbasemethod.str()
		<< "};\n";

	createstream << ret.str();

	std::stringstream uinterface;
	std::stringstream  InitInterface;
	std::stringstream  OnRemoteMethodCallInterface;
	std::stringstream  OnUpdatePropertysInterface;

	for (std::map<std::string, std::string>::iterator itor = bvmret.begin(); itor != bvmret.end(); ++itor)
	{
		std::string cellmothod = "\nUCLASS(BlueprintType, Blueprintable)\nclass " + UEntityDef::UE4ProjectAPIName_ + " U";
		if(itor->first != "Entity")
			cellmothod = cellmothod + itor->first + std::string(g_EntityInterfaceSuffix) + "CellMethod: public UEntityServerMethod\n{\n    GENERATED_BODY()\npublic:\n";
		else
			cellmothod = cellmothod + name_ + "CellMethod: public UEntityServerMethod\n{\n    GENERATED_BODY()\npublic:\n";
		if (bscellmethod.find(itor->first) != bscellmethod.end())
		{
			cellmothod = cellmothod + bscellmethod.find(itor->first)->second;
		}
		cellmothod = cellmothod + "};\n";
		std::string basemothod = "\nUCLASS(BlueprintType, Blueprintable)\nclass " + UEntityDef::UE4ProjectAPIName_ + " U";
		if (itor->first != "Entity")
			basemothod = basemothod + itor->first + std::string(g_EntityInterfaceSuffix) + "BaseMethod: public UEntityServerMethod\n{\n    GENERATED_BODY()\npublic:\n";
		else
			basemothod = basemothod + name_ + "BaseMethod: public UEntityServerMethod\n{\n    GENERATED_BODY()\npublic:\n";
		if (bsbasemethod.find(itor->first) != bsbasemethod.end())
		{
			basemothod = basemothod  + bsbasemethod.find(itor->first)->second;
		}
		basemothod = basemothod + "};\n";

		itor->second = cellmothod + basemothod + itor->second;

		if (itor->first != "Entity")
		{
			uinterface << "    "<< GetUE4Property(iter1->second->getName()) <<"\n    U"
				       << itor->first << g_EntityInterfaceSuffix << " *" << itor->first << ";\n";
			InitInterface << "        " << itor->first << " = (U" << itor->first << g_EntityInterfaceSuffix
				          << " *)DefineEntitiesUtils::CreateClientEntityObject(entity, \"" << itor->first << "\", 0);\n";
			InitInterface << "        if(" << itor->first << "!=nullptr)\n";
			InitInterface << "            " << itor->first << "->Init(entity);\n";
			OnRemoteMethodCallInterface << "        if(" << itor->first << "!=nullptr)\n";
			OnRemoteMethodCallInterface << "            " << itor->first << "->OnRemoteMethodCall(name,args);\n";
			OnUpdatePropertysInterface << "        if(" << itor->first << "!=nullptr)\n";
			OnUpdatePropertysInterface << "            " << itor->first << "->OnUpdatePropertys(name,newVal,oldVal);\n";
		}

	}
	for (std::map<std::string, std::string>::iterator itor = bvmret.begin(); itor != bvmret.end(); ++itor)
	{
		if (itor->first == "Entity")
			continue;
		if (std::find(vl.begin(), vl.end(), itor->first) == vl.end())
		{
			vl.push_back(itor->first);
			vm.insert(std::pair < std::string, std::string>(itor->first, itor->second));
		}
	}
	std::string fcre = bvmret.find("Entity")->second;
	fcre = fcre.erase(fcre.size() - 3);
	fcre = fcre + uinterface.str();
	fcre = fcre + "    virtual void InitInterface(UEntity *EntityPtr){\n";
	fcre = fcre + InitInterface.str() +  "    }\n";
	fcre = fcre + "    virtual void OnRemoteMethodCallInterface(uint16 AliasID, FMemoryStream& Stream){\n";
	fcre = fcre + OnRemoteMethodCallInterface.str() + "    }\n";
	fcre = fcre + "    virtual void OnUpdatePropertysInterface(uint16 UType, FMemoryStream& Stream){\n";
	fcre = fcre + OnUpdatePropertysInterface.str() + "    }\n";
	fcre = fcre + "};\n";

	/*
	createstream << uinterface.str() 
		         << "    virtual void InitInterface(UEntity *EntityPtr){\n"
		         << InitInterface.str() 
		         << "    }\n" 
		         << "    virtual void OnRemoteMethodCallInterface(uint16 AliasID, FMemoryStream& Stream){\n"
		         << OnRemoteMethodCallInterface.str() 
		         << "    }\n"
		         << "    virtual void OnUpdatePropertysInterface(uint16 UType, FMemoryStream& Stream){\n"
		         << OnUpdatePropertysInterface.str() 
		         << "    }\n";
	*/
	createstream << "};\n";
	return createstream.str();
}

std::string ScriptDefModule::CreateDerivedClass(const std::string &derivedname)
{
	std::stringstream clientMethod;
	clientMethod << "UCLASS(BlueprintType, Blueprintable)\n";
	clientMethod << "class " << UEntityDef::UE4ProjectAPIName_ << " U"<< derivedname << " : public U" << name_ << g_EntitySuffix << "{\n";
	clientMethod << "    GENERATED_BODY()\n";
	clientMethod << "public:\n";
	METHODDESCRIPTION_MAP::iterator clitetMethodItor = methodClientDescr_.begin();
	for (; clitetMethodItor != methodClientDescr_.end(); ++clitetMethodItor)
	{
		MethodDescription *method = clitetMethodItor->second;
		if (IsUE4BluePrintMethod(method))
		{
			std::string ducm = "    UFUNCTION(BlueprintNativeEvent, Category = \"Entity | Client\")\n" + DefineToUE4ClientMethod(method, 1);
			if (method->GetImplements() == "Entity")
				clientMethod << ducm;
		}
		else
		{
			std::string ducm = DefineToUE4ClientMethod(method, 0);
			if (method->GetImplements() == "Entity")
				clientMethod << ducm;
		}

	}
	return clientMethod.str() + "};\n";
}
bool ScriptDefModule::HasInterface(const std::string &Interfacename)
{
	METHODDESCRIPTION_MAP::iterator clitetMethodItor = methodClientDescr_.begin();
	for (; clitetMethodItor != methodClientDescr_.end(); ++clitetMethodItor)
	{
		MethodDescription *method = clitetMethodItor->second;
		if (method->GetImplements() == Interfacename)
		{
			return true;
		}
	}
	return false;
}
std::string ScriptDefModule::CreateDerivedClassInterface(const std::string &derivedname, const std::string &interfacename)
{
	std::stringstream clientMethod;
	clientMethod << "UCLASS(BlueprintType, Blueprintable)\n";
	clientMethod << "class " << UEntityDef::UE4ProjectAPIName_ << " U" << derivedname << " : public U" << interfacename << g_EntityInterfaceSuffix << "{\n";
	clientMethod << "    GENERATED_BODY()\n";
	clientMethod << "public:\n";

	METHODDESCRIPTION_MAP::iterator clitetMethodItor = methodClientDescr_.begin();
	for (; clitetMethodItor != methodClientDescr_.end(); ++clitetMethodItor)
	{
		MethodDescription *method = clitetMethodItor->second;
		if (IsUE4BluePrintMethod(method))
		{
			std::string ducm = "    UFUNCTION(BlueprintNativeEvent, Category = \"Entity | Client\")\n" + DefineToUE4ClientMethod(method, 1);
			if (method->GetImplements() == interfacename)
				clientMethod << ducm;
		}
		else
		{
			std::string ducm = DefineToUE4ClientMethod(method, 0);
			if (method->GetImplements() == interfacename)
				clientMethod << ducm;
		}
	}
	return clientMethod.str() + "};\n";
}
//-------------------------------------------------------------------------------------
}
