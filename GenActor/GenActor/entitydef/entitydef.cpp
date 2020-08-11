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
#include "entitydef.h"
#include "scriptdef_module.h"
#include "datatypes.h"
#include "common.h"
#include <sstream>
#include "entity_mailbox.h"
#include <algorithm>


namespace KBEngine{

bool g_isReload = false;

// 方法产生时自动产生utype用的
ENTITY_METHOD_UID g_methodUtypeAuto = 1;
std::vector<ENTITY_METHOD_UID> g_methodCusUtypes;																									
std::string  UEntityDef::UE4ProjectAPIName_ ;
ENTITY_PROPERTY_UID auto_puid = 1;
std::vector<ENTITY_PROPERTY_UID> puids;

UEntityDef::UEntityDef()
{
}
UEntityDef::~UEntityDef()
{
	UEntityDef::finalise();
}
bool UEntityDef::finalise(bool isReload)
{
	PropertyDescription::resetDescriptionCount();
	MethodDescription::resetDescriptionCount();

	//EntityDef::__md5.clear();
	g_methodUtypeAuto = 1;

	auto_puid = 1;
	puids.clear();

	if (!isReload)
	{
		std::vector<ScriptDefModulePtr>::iterator iter = ScriptModules_.begin();
		for (; iter != ScriptModules_.end(); ++iter)
		{
			(*iter)->finalise();
		}

		iter = OldScriptModules_.begin();
		for (; iter != OldScriptModules_.end(); ++iter)
		{
			(*iter)->finalise();
		}

		OldScriptModules_.clear();
		OldScriptTypeMappingUType_.clear();
	}

	ScriptModules_.clear();
	ScriptTypeMappingUType_.clear();
	g_methodCusUtypes.clear();
	delete DataTypes_;
	return true;
}

void UEntityDef::reload(bool fullReload)
{
	g_isReload = true;
	if (fullReload)
	{
		OldScriptModules_.clear();
		OldScriptTypeMappingUType_.clear();

		std::vector<ScriptDefModulePtr>::iterator iter = ScriptModules_.begin();
		for (; iter != ScriptModules_.end(); ++iter)
		{
			OldScriptModules_.push_back((*iter));
			OldScriptTypeMappingUType_[(*iter)->getName()] = (*iter)->getUType();
		}

		bool ret = finalise(true);

		ret = initialize("");

	}
}

bool UEntityDef::initialize(const std::string &resbasepath)
{
	g_entityFlagMapping["CELL_PUBLIC"] = ED_FLAG_CELL_PUBLIC;
	g_entityFlagMapping["CELL_PRIVATE"] = ED_FLAG_CELL_PRIVATE;
	g_entityFlagMapping["ALL_CLIENTS"] = ED_FLAG_ALL_CLIENTS;
	g_entityFlagMapping["CELL_PUBLIC_AND_OWN"] = ED_FLAG_CELL_PUBLIC_AND_OWN;
	g_entityFlagMapping["BASE_AND_CLIENT"] = ED_FLAG_BASE_AND_CLIENT;
	g_entityFlagMapping["BASE"] = ED_FLAG_BASE;
	g_entityFlagMapping["OTHER_CLIENTS"] = ED_FLAG_OTHER_CLIENTS;
	g_entityFlagMapping["OWN_CLIENT"] = ED_FLAG_OWN_CLIENT;

	std::string entitiesFile = resbasepath + "entities.xml";
	std::string defFilePath = resbasepath + "entity_defs/";
	ENTITY_SCRIPT_UID utype = 1;

	// 初始化数据类别
	DataTypes_ = new ADataTypes();
	DataTypes_->initialize(defFilePath + "types.xml");

	XML *xml = new XML();
	if (!xml->openSection(entitiesFile.c_str()))
		return false;

	// 获得entities.xml根节点, 如果没有定义一个entity那么直接返回true
	TiXmlNode* node = xml->getRootNode();
	if (node == NULL)
		return true;

	// 开始遍历所有的entity节点
	XML_FOR_BEGIN(node)
	{
		std::string moduleName = xml->getKey(node);
		ScriptTypeMappingUType_[moduleName] = utype;
		ScriptDefModule* pScriptModule = new ScriptDefModule(moduleName, utype++);
		const char* uec = ((TiXmlElement *)node)->Attribute("ue4class");
		if (uec != NULL)
		{
			pScriptModule->setue4class(uec);
		}
		ScriptModules_.push_back(pScriptModule);

		std::string deffile = defFilePath + moduleName + ".def";
		XML *defxml = new XML();

		if (!defxml->openSection(deffile.c_str()))
			return false;

		TiXmlNode* defNode = defxml->getRootNode();
		if (defNode == NULL)
		{
			// root节点下没有子节点了
			continue;
		}

		// 加载def文件中的定义
		if (!loadDefInfo(defFilePath, moduleName, defxml, defNode, pScriptModule))
		{
			return false;
		}

		// 尝试在主entity文件中加载detailLevel数据
		if (!loadDetailLevelInfo(defFilePath, moduleName, defxml, defNode, pScriptModule))
		{
			return false;
		}

		pScriptModule->onLoaded();
		delete defxml;
	}
	XML_FOR_END(node);

	//EntityDef::md5().final();
	delete xml;
	return true;
}

bool UEntityDef::loadDefInfo(const std::string& defFilePath,
	const std::string& moduleName,
	XML* defxml,
	TiXmlNode* defNode,
	ScriptDefModule* pScriptModule)
{
	if (!loadAllDefDescriptions(moduleName, defxml, defNode, pScriptModule))
	{
		return false;
	}

	// 遍历所有的interface， 并将他们的方法和属性加入到模块中
	if (!loadInterfaces(defFilePath, moduleName, defxml, defNode, pScriptModule))
	{
		return false;
	}

	// 加载父类所有的内容
	if (!loadParentClass(defFilePath, moduleName, defxml, defNode, pScriptModule))
	{
		return false;
	}

	// 尝试加载detailLevel数据
	if (!loadDetailLevelInfo(defFilePath, moduleName, defxml, defNode, pScriptModule))
	{
		return false;
	}

	// 尝试加载VolatileInfo数据
	if (!loadVolatileInfo(defFilePath, moduleName, defxml, defNode, pScriptModule))
	{
		return false;
	}

	return true;
}

bool UEntityDef::loadDetailLevelInfo(const std::string& defFilePath,
	const std::string& moduleName,
	XML* defxml,
	TiXmlNode* defNode,
	ScriptDefModule* pScriptModule)
{
	TiXmlNode* detailLevelNode = defxml->enterNode(defNode, "DetailLevels");
	if (detailLevelNode == NULL)
		return true;

	DetailLevel& dlInfo = pScriptModule->getDetailLevel();

	TiXmlNode* node = defxml->enterNode(detailLevelNode, "NEAR");
	TiXmlNode* radiusNode = defxml->enterNode(node, "radius");
	TiXmlNode* hystNode = defxml->enterNode(node, "hyst");
	if (node == NULL || radiusNode == NULL || hystNode == NULL)
	{
		return false;
	}

	dlInfo.level[DETAIL_LEVEL_NEAR].radius = (float)defxml->getValFloat(radiusNode);
	dlInfo.level[DETAIL_LEVEL_NEAR].hyst = (float)defxml->getValFloat(hystNode);

	node = defxml->enterNode(detailLevelNode, "MEDIUM");
	radiusNode = defxml->enterNode(node, "radius");
	hystNode = defxml->enterNode(node, "hyst");
	if (node == NULL || radiusNode == NULL || hystNode == NULL)
	{
		return false;
	}

	dlInfo.level[DETAIL_LEVEL_MEDIUM].radius = (float)defxml->getValFloat(radiusNode);

	dlInfo.level[DETAIL_LEVEL_MEDIUM].radius += dlInfo.level[DETAIL_LEVEL_NEAR].radius +
		dlInfo.level[DETAIL_LEVEL_NEAR].hyst;

	dlInfo.level[DETAIL_LEVEL_MEDIUM].hyst = (float)defxml->getValFloat(hystNode);

	node = defxml->enterNode(detailLevelNode, "FAR");
	radiusNode = defxml->enterNode(node, "radius");
	hystNode = defxml->enterNode(node, "hyst");
	if (node == NULL || radiusNode == NULL || hystNode == NULL)
	{
		return false;
	}

	dlInfo.level[DETAIL_LEVEL_FAR].radius = (float)defxml->getValFloat(radiusNode);

	dlInfo.level[DETAIL_LEVEL_FAR].radius += dlInfo.level[DETAIL_LEVEL_MEDIUM].radius +
		dlInfo.level[DETAIL_LEVEL_MEDIUM].hyst;

	dlInfo.level[DETAIL_LEVEL_FAR].hyst = (float)defxml->getValFloat(hystNode);

	return true;

}

bool UEntityDef::loadVolatileInfo(const std::string& defFilePath,
	const std::string& moduleName,
	XML* defxml,
	TiXmlNode* defNode,
	ScriptDefModule* pScriptModule)
{
	TiXmlNode* pNode = defxml->enterNode(defNode, "Volatile");
	if (pNode == NULL)
		return true;

	VolatileInfo* pVolatileInfo = pScriptModule->getPVolatileInfo();

	TiXmlNode* node = defxml->enterNode(pNode, "position");
	if (node)
	{
		pVolatileInfo->position((float)defxml->getValFloat(node));
	}
	else
	{
		if (defxml->hasNode(pNode, "position"))
			pVolatileInfo->position(VolatileInfo::ALWAYS);
		else
			pVolatileInfo->position(-1.f);
	}

	node = defxml->enterNode(pNode, "yaw");
	if (node)
	{
		pVolatileInfo->yaw((float)defxml->getValFloat(node));
	}
	else
	{
		if (defxml->hasNode(pNode, "yaw"))
			pVolatileInfo->yaw(VolatileInfo::ALWAYS);
		else
			pVolatileInfo->yaw(-1.f);
	}

	node = defxml->enterNode(pNode, "pitch");
	if (node)
	{
		pVolatileInfo->pitch((float)defxml->getValFloat(node));
	}
	else
	{
		if (defxml->hasNode(pNode, "pitch"))
			pVolatileInfo->pitch(VolatileInfo::ALWAYS);
		else
			pVolatileInfo->pitch(-1.f);
	}

	node = defxml->enterNode(pNode, "roll");
	if (node)
	{
		pVolatileInfo->roll((float)defxml->getValFloat(node));
	}
	else
	{
		if (defxml->hasNode(pNode, "roll"))
			pVolatileInfo->roll(VolatileInfo::ALWAYS);
		else
			pVolatileInfo->roll(-1.f);
	}

	return true;
}

bool UEntityDef::loadInterfaces(const std::string& defFilePath,
	const std::string& moduleName,
	XML* defxml,
	TiXmlNode* defNode,
	ScriptDefModule* pScriptModule)
{
	TiXmlNode* implementsNode = defxml->enterNode(defNode, "Implements");
	if (implementsNode == NULL)
		return true;

	XML_FOR_BEGIN(implementsNode)
	{
		TiXmlNode* interfaceNode = defxml->enterNode(implementsNode, "Interface");
		std::string interfaceName = defxml->getKey(interfaceNode);
		std::string interfacefile = defFilePath + "interfaces/" + interfaceName + ".def";
		XML *interfaceXml = new XML();
		if (!interfaceXml->openSection(interfacefile.c_str()))
			return false;

		TiXmlNode* interfaceRootNode = interfaceXml->getRootNode();
		if (interfaceRootNode == NULL)
		{
			return true;
		}

		if (!loadAllDefDescriptions(moduleName, interfaceXml, interfaceRootNode, pScriptModule, interfaceName))
		{
			return false;
		}

		// 尝试加载detailLevel数据
		if (!loadDetailLevelInfo(defFilePath, moduleName, interfaceXml, interfaceRootNode, pScriptModule))
		{
			return false;
		}

		// 遍历所有的interface， 并将他们的方法和属性加入到模块中
		if (!loadInterfaces(defFilePath, moduleName, interfaceXml, interfaceRootNode, pScriptModule))
		{
			return false;
		}
		delete interfaceXml;
	}
	XML_FOR_END(implementsNode);

	return true;
}

bool UEntityDef::loadParentClass(const std::string& defFilePath,
	const std::string& moduleName,
	XML* defxml,
	TiXmlNode* defNode,
	ScriptDefModule* pScriptModule)
{
	TiXmlNode* parentClassNode = defxml->enterNode(defNode, "Parent");
	if (parentClassNode == NULL)
		return true;

	std::string parentClassName = defxml->getKey(parentClassNode);
	std::string parentClassfile = defFilePath + parentClassName + ".def";

	XML *parentClassXml = new XML();
	if (!parentClassXml->openSection(parentClassfile.c_str()))
		return false;

	TiXmlNode* parentClassdefNode = parentClassXml->getRootNode();
	if (parentClassdefNode == NULL)
	{
		// root节点下没有子节点了
		return true;
	}

	// 加载def文件中的定义
	if (!loadDefInfo(defFilePath, parentClassName, parentClassXml, parentClassdefNode, pScriptModule))
	{
		return false;
	}
	delete parentClassXml;
	return true;
}

bool UEntityDef::loadAllDefDescriptions(const std::string& moduleName,
	XML* defxml,
	TiXmlNode* defNode,
	ScriptDefModule* pScriptModule,
	const std::string &Implements)
{
	// 加载属性描述
	if (!loadDefPropertys(moduleName, defxml, defxml->enterNode(defNode, "Properties"), pScriptModule, Implements))
		return false;

	// 加载cell方法描述
	if (!loadDefCellMethods(moduleName, defxml, defxml->enterNode(defNode, "CellMethods"), pScriptModule, Implements))
	{
		return false;
	}

	// 加载base方法描述
	if (!loadDefBaseMethods(moduleName, defxml, defxml->enterNode(defNode, "BaseMethods"), pScriptModule, Implements))
	{
		return false;
	}

	// 加载client方法描述
	if (!loadDefClientMethods(moduleName, defxml, defxml->enterNode(defNode, "ClientMethods"), pScriptModule, Implements))
	{
		return false;
	}

	return true;
}

bool UEntityDef::validDefPropertyName(ScriptDefModule* pScriptModule, const std::string& name)
{
	int i = 0;
	while (true)
	{
		std::string limited = ENTITY_LIMITED_PROPERTYS[i];

		if (limited == "")
			break;

		if (name == limited)
			return false;

		++i;
	};

	return true;
}

bool UEntityDef::loadDefPropertys(const std::string& moduleName,
	XML* xml,
	TiXmlNode* defPropertyNode,
	ScriptDefModule* pScriptModule,
	const std::string &Implements)
{
	if (defPropertyNode)
	{
		XML_FOR_BEGIN(defPropertyNode)
		{
			ENTITY_PROPERTY_UID			futype = 0;
			uint32						flags = 0;
			int32						hasBaseFlags = 0;
			int32						hasCellFlags = 0;
			int32						hasClientFlags = 0;
			DataType*					dataType = NULL;
			bool						isPersistent = false;
			bool						isIdentifier = false;		// 是否是一个索引键
			uint32						databaseLength = 0;			// 这个属性在数据库中的长度
			std::string					indexType;
			DETAIL_TYPE					detailLevel = DETAIL_LEVEL_FAR;
			std::string					detailLevelStr = "";
			std::string					strType;
			std::string					strisPersistent;
			std::string					strFlags;
			std::string					strIdentifierNode;
			std::string					defaultStr;
			std::string					name = "";

			name = xml->getKey(defPropertyNode);
			if (!validDefPropertyName(pScriptModule, name))
			{
				return false;
			}

			TiXmlNode* flagsNode = xml->enterNode(defPropertyNode->FirstChild(), "Flags");
			if (flagsNode)
			{
				strFlags = xml->getValStr(flagsNode);
				std::transform(strFlags.begin(), strFlags.end(), strFlags.begin(), toupper);

				ENTITYFLAGMAP::iterator iter = g_entityFlagMapping.find(strFlags.c_str());
				if (iter == g_entityFlagMapping.end())
				{
					return false;
				}

				flags = iter->second;
				hasBaseFlags = flags & ENTITY_BASE_DATA_FLAGS;
				if (hasBaseFlags > 0)
					pScriptModule->setBase(true);

				hasCellFlags = flags & ENTITY_CELL_DATA_FLAGS;
				if (hasCellFlags > 0)
					pScriptModule->setCell(true);

				hasClientFlags = flags & ENTITY_CLIENT_DATA_FLAGS;
				if (hasClientFlags > 0)
					pScriptModule->setClient(true);

				if (hasBaseFlags <= 0 && hasCellFlags <= 0)
				{
					return false;
				}
			}


			TiXmlNode* persistentNode = xml->enterNode(defPropertyNode->FirstChild(), "Persistent");
			if (persistentNode)
			{
				strisPersistent = xml->getValStr(persistentNode);

				std::transform(strisPersistent.begin(), strisPersistent.end(),
					strisPersistent.begin(), tolower);

				if (strisPersistent == "true")
					isPersistent = true;
			}

			TiXmlNode* typeNode = xml->enterNode(defPropertyNode->FirstChild(), "Type");
			if (typeNode)
			{
				strType = xml->getValStr(typeNode);

				if (strType == "ARRAY")
				{
					FixedArrayType* dataType1 = new FixedArrayType();
					if (dataType1->initialize(xml, typeNode, DataTypes_))
						dataType = dataType1;
					else
						return false;
				}
				else
				{
					dataType = DataTypes_->getDataType(strType);
				}

				if (dataType == NULL)
				{
					return false;
				}
			}

			TiXmlNode* indexTypeNode = xml->enterNode(defPropertyNode->FirstChild(), "Index");
			if (indexTypeNode)
			{
				indexType = xml->getValStr(indexTypeNode);

				std::transform(indexType.begin(), indexType.end(),
					indexType.begin(), toupper);
			}


			TiXmlNode* identifierNode = xml->enterNode(defPropertyNode->FirstChild(), "Identifier");
			if (identifierNode)
			{
				strIdentifierNode = xml->getValStr(identifierNode);
				std::transform(strIdentifierNode.begin(), strIdentifierNode.end(),
					strIdentifierNode.begin(), tolower);

				if (strIdentifierNode == "true")
					isIdentifier = true;
			}

			TiXmlNode* databaseLengthNode = xml->enterNode(defPropertyNode->FirstChild(), "DatabaseLength");
			if (databaseLengthNode)
			{
				databaseLength = xml->getValInt(databaseLengthNode);
			}

			TiXmlNode* defaultValNode =
				xml->enterNode(defPropertyNode->FirstChild(), "Default");

			if (defaultValNode)
			{
				defaultStr = xml->getValStr(defaultValNode);
			}

			TiXmlNode* detailLevelNode =
				xml->enterNode(defPropertyNode->FirstChild(), "DetailLevel");

			if (detailLevelNode)
			{
				detailLevelStr = xml->getValStr(detailLevelNode);
				if (detailLevelStr == "FAR")
					detailLevel = DETAIL_LEVEL_FAR;
				else if (detailLevelStr == "MEDIUM")
					detailLevel = DETAIL_LEVEL_MEDIUM;
				else if (detailLevelStr == "NEAR")
					detailLevel = DETAIL_LEVEL_NEAR;
				else
					detailLevel = DETAIL_LEVEL_FAR;
			}

			TiXmlNode* utypeValNode =
				xml->enterNode(defPropertyNode->FirstChild(), "Utype");

			if (utypeValNode)
			{
				int iUtype = xml->getValInt(utypeValNode);
				futype = iUtype;

				if (iUtype != int(futype))
				{
					return false;
				}

				puids.push_back(futype);
			}
			else
			{
				while (true)
				{
					futype = auto_puid++;
					std::vector<ENTITY_PROPERTY_UID>::iterator iter =
						std::find(puids.begin(), puids.end(), futype);

					if (iter == puids.end())
						break;
				}
			}

			// 产生一个属性描述实例
			PropertyDescription* propertyDescription = PropertyDescription::createDescription(futype, strType,
				name, flags, isPersistent,
				dataType, isIdentifier, indexType,
				databaseLength, defaultStr,
				detailLevel);

			bool ret = true;
			propertyDescription->SetImplements(Implements);
			// 添加到模块中
			if (hasCellFlags > 0)
				ret = pScriptModule->addPropertyDescription(name.c_str(),
					propertyDescription, CELLAPP_TYPE);

			if (hasBaseFlags > 0)
				ret = pScriptModule->addPropertyDescription(name.c_str(),
					propertyDescription, BASEAPP_TYPE);

			if (hasClientFlags > 0)
				ret = pScriptModule->addPropertyDescription(name.c_str(),
					propertyDescription, CLIENT_TYPE);

			if (!ret)
			{

			}
		}
		XML_FOR_END(defPropertyNode);
	}

	return true;
}

bool UEntityDef::loadDefCellMethods(const std::string& moduleName,
	XML* xml,
	TiXmlNode* defMethodNode,
	ScriptDefModule* pScriptModule,
	const std::string &Implements)
{
	if (defMethodNode)
	{
		XML_FOR_BEGIN(defMethodNode)
		{
			std::string name = xml->getKey(defMethodNode);
			MethodDescription* methodDescription = new MethodDescription(0, CELLAPP_TYPE, name);
			TiXmlNode* argNode = defMethodNode->FirstChild();
			methodDescription->SetImplements(Implements);
			const char* Authority = ((TiXmlElement *)defMethodNode)->Attribute("Authority");
			if (Authority != NULL)
			{
				methodDescription->SetAuthority(atoi(Authority));
			}
			// 可能没有参数
			if (argNode)
			{
				int argp = 0;
				XML_FOR_BEGIN(argNode)
				{
					std::string argType = xml->getKey(argNode);

					if (argType == "Exposed")
					{
						methodDescription->setExposed();
					}
					else if (argType == "Arg")
					{
						std::stringstream pArgName;
						TiXmlElement *pe = (TiXmlElement *)argNode;
						if (pe != NULL)
						{
							const char* pname = pe->Attribute("Name");
							if (pname != NULL)
							{
								pArgName << pname;
							}
						}
						if (pArgName.str().size() == 0)
						{
							pArgName << "arg" << argp;
						}
							
						argp++;
						std::string argName(pArgName.str());
						DataType* dataType = NULL;
						TiXmlNode* typeNode = argNode->FirstChild();
						std::string strType = xml->getValStr(typeNode);

						if (strType == "ARRAY")
						{
							FixedArrayType* dataType1 = new FixedArrayType();
							if (dataType1->initialize(xml, typeNode, DataTypes_))
								dataType = dataType1;
						}
						else
						{
							dataType = DataTypes_->getDataType(strType);
						}

						if (dataType == NULL)
						{

							return false;
						}

						methodDescription->pushArgType(dataType, argName.c_str());
					}
					else if (argType == "Utype")
					{
						TiXmlNode* typeNode = argNode->FirstChild();

						int iUtype = xml->getValInt(typeNode);
						ENTITY_METHOD_UID muid = iUtype;

						if (iUtype != int(muid))
						{
							return false;
						}

						methodDescription->setUType(muid);
					}
				}
				
				XML_FOR_END(argNode);
			}

			// 如果配置中没有设置过utype, 则产生
			if (methodDescription->getUType() <= 0)
			{
				ENTITY_METHOD_UID muid = 0;
				while (true)
				{
					muid = g_methodUtypeAuto++;
					std::vector<ENTITY_METHOD_UID>::iterator iterutype =
						std::find(g_methodCusUtypes.begin(), g_methodCusUtypes.end(), muid);

					if (iterutype == g_methodCusUtypes.end())
					{
						break;
					}
				}

				methodDescription->setUType(muid);
			}

			pScriptModule->addCellMethodDescription(name.c_str(), methodDescription);
		}
		XML_FOR_END(defMethodNode);
	}

	return true;
}

bool UEntityDef::loadDefBaseMethods(const std::string& moduleName, XML* xml,
	TiXmlNode* defMethodNode, ScriptDefModule* pScriptModule, const std::string &Implements)
{
	if (defMethodNode)
	{
		XML_FOR_BEGIN(defMethodNode)
		{
			std::string name = xml->getKey(defMethodNode);
			MethodDescription* methodDescription = new MethodDescription(0, BASEAPP_TYPE, name);
			TiXmlNode* argNode = defMethodNode->FirstChild();
			methodDescription->SetImplements(Implements);
			// 可能没有参数
			if (argNode)
			{
				int argp = 0;
				XML_FOR_BEGIN(argNode)
				{
					std::string argType = xml->getKey(argNode);

					if (argType == "Exposed")
					{
						methodDescription->setExposed();
					}
					else if (argType == "Arg")
					{

						std::stringstream pArgName;
						TiXmlElement *pe = (TiXmlElement *)argNode;
						if (pe != NULL)
						{
							const char* pname = pe->Attribute("Name");
							if (pname != NULL)
							{
								pArgName << pname;
							}
						}
						if (pArgName.str().size() == 0)
						{
							pArgName << "arg"<< argp;
						}
						
						argp++;
						std::string argName(pArgName.str());

						DataType* dataType = NULL;
						TiXmlNode* typeNode = argNode->FirstChild();
						std::string strType = xml->getValStr(typeNode);

						if (strType == "ARRAY")
						{
							FixedArrayType* dataType1 = new FixedArrayType();
							if (dataType1->initialize(xml, typeNode, DataTypes_))
								dataType = dataType1;
						}
						else
						{
							dataType = DataTypes_->getDataType(strType);
						}

						if (dataType == NULL)
						{
							return false;
						}

						methodDescription->pushArgType(dataType, argName.c_str());
					}
					else if (argType == "Utype")
					{
						TiXmlNode* typeNode = argNode->FirstChild();

						int iUtype = xml->getValInt(typeNode);
						ENTITY_METHOD_UID muid = iUtype;

						if (iUtype != int(muid))
						{
							return false;
						}

						methodDescription->setUType(muid);
					}
				}
				
				XML_FOR_END(argNode);
			}

			// 如果配置中没有设置过utype, 则产生
			if (methodDescription->getUType() <= 0)
			{
				ENTITY_METHOD_UID muid = 0;
				while (true)
				{
					muid = g_methodUtypeAuto++;
					std::vector<ENTITY_METHOD_UID>::iterator iterutype =
						std::find(g_methodCusUtypes.begin(), g_methodCusUtypes.end(), muid);

					if (iterutype == g_methodCusUtypes.end())
					{
						break;
					}
				}

				methodDescription->setUType(muid);
			}

			pScriptModule->addBaseMethodDescription(name.c_str(), methodDescription);
		}
		XML_FOR_END(defMethodNode);
	}

	return true;
}

bool UEntityDef::loadDefClientMethods(const std::string& moduleName, XML* xml,
	TiXmlNode* defMethodNode, ScriptDefModule* pScriptModule, const std::string &Implements)
{
	if (defMethodNode)
	{
		XML_FOR_BEGIN(defMethodNode)
		{
			std::string name = xml->getKey(defMethodNode);
			MethodDescription* methodDescription = new MethodDescription(0, CLIENT_TYPE, name);
			methodDescription->SetImplements(Implements);
			TiXmlNode* argNode = defMethodNode->FirstChild();
			const char* Authority = ((TiXmlElement *)defMethodNode)->Attribute("Authority");
			if (Authority != NULL)
			{
				methodDescription->SetAuthority(atoi(Authority));
			}
				
			// 可能没有参数
			if (argNode)
			{
				int argp = 0;
				XML_FOR_BEGIN(argNode)
				{
					std::string argType = xml->getKey(argNode);

					if (argType == "Arg")
					{

						std::stringstream pArgName;

						TiXmlElement *pe = (TiXmlElement *)argNode;
						if (pe != NULL)
						{
							const char* pname = pe->Attribute("Name");
							if (pname != NULL)
							{
								pArgName << pname;
							}
						}
						if (pArgName.str().size() == 0)
						{
							pArgName << "arg"<< argp;
						}
						argp++;
						std::string argName(pArgName.str());
						DataType* dataType = NULL;
						TiXmlNode* typeNode = argNode->FirstChild();
						std::string strType = xml->getValStr(typeNode);

						if (strType == "ARRAY")
						{
							FixedArrayType* dataType1 = new FixedArrayType();
							if (dataType1->initialize(xml, typeNode, DataTypes_))
								dataType = dataType1;
						}
						else
						{
							dataType = DataTypes_->getDataType(strType);
						}

						if (dataType == NULL)
						{
							return false;
						}

						methodDescription->pushArgType(dataType, argName.c_str());
					}
					else if (argType == "Utype")
					{
						TiXmlNode* typeNode = argNode->FirstChild();

						int iUtype = xml->getValInt(typeNode);
						ENTITY_METHOD_UID muid = iUtype;

						if (iUtype != int(muid))
						{
							return false;
						}

						methodDescription->setUType(muid);
					}
				}
				
				XML_FOR_END(argNode);
			}

			// 如果配置中没有设置过utype, 则产生
			if (methodDescription->getUType() <= 0)
			{
				ENTITY_METHOD_UID muid = 0;
				while (true)
				{
					muid = g_methodUtypeAuto++;
					std::vector<ENTITY_METHOD_UID>::iterator iterutype =
						std::find(g_methodCusUtypes.begin(), g_methodCusUtypes.end(), muid);

					if (iterutype == g_methodCusUtypes.end())
					{
						break;
					}
				}

				methodDescription->setUType(muid);
			}

			pScriptModule->addClientMethodDescription(name.c_str(), methodDescription);
		}
		XML_FOR_END(defMethodNode);
	}

	return true;
}

ScriptDefModule* UEntityDef::findScriptModule(ENTITY_SCRIPT_UID utype)
{
	// utype 最小为1
	if (utype == 0 || utype >= ScriptModules_.size() + 1)
	{
		return NULL;
	}

	return ScriptModules_[utype - 1];
}

ScriptDefModule* UEntityDef::findScriptModule(const char* scriptName)
{
	std::map<std::string, ENTITY_SCRIPT_UID>::iterator iter =
		ScriptTypeMappingUType_.find(scriptName);

	if (iter == ScriptTypeMappingUType_.end())
	{
		return NULL;
	}

	return findScriptModule(iter->second);
}

ScriptDefModule* UEntityDef::findOldScriptModule(const char* scriptName)
{
	std::map<std::string, ENTITY_SCRIPT_UID>::iterator iter =
		OldScriptTypeMappingUType_.find(scriptName);

	if (iter == OldScriptTypeMappingUType_.end())
	{
		return NULL;
	}

	if (iter->second >= OldScriptModules_.size() + 1)
	{
		return NULL;
	}

	return OldScriptModules_[iter->second - 1];

}

bool UEntityDef::initializeWatcher()
{
	return true;
}
std::string UEntityDef::ToJson()
{
	std::string retType = "{";
	for (int i = 0; i < ScriptModules_.size(); i++)
	{
		retType = retType + ScriptModules_[i]->ToJson() + ",";
	}
	retType = retType.substr(0, retType.size() - 1);
	retType = retType + "}";
	return retType;
}
/**
* 根据def定义的Alias产生虚幻的结构体
*
* @return 结构体的c++代码
*/
std::string  UEntityDef::AliasToUE4Struct()
{
	return DataTypes_->DataTypeToUE4Struct();
}
/**
* 生成ActorCharacter类代码
*
* @return 结构体的c++代码
*/
std::string UEntityDef::GenClassActorCharacter(const std::string &name, const std::string &uclass)
{
	std::stringstream ret;
	ret
		<< "UCLASS(BlueprintType, Blueprintable)\n"
		<< "class " << UEntityDef::UE4ProjectAPIName_ << " A" << name << " : public " << uclass << "\n"
		<< "{\n"
		<< "    GENERATED_BODY()\n"
		<< "public:\n"
		<< "    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = \"Entity\", meta = (AllowPrivateAccess = \"true\"))\n"
		<< "    class U" << name << "Entity* Entity;\n"
		<< "    A" << name << "()\n"
		<< "    {\n"
		<< "        __GENERATED_ENTITY_CHARACTER_INIT(" << name << ")\n"
		<< "    };\n"
		<< "    void PostInitializeComponents()\n"
		<< "    {\n"
		<< "        Super::PostInitializeComponents();\n"
		//<< "        InitChangelistMgr(TEXT(\"" << name << "\"), Entity);\n"
		<< "    }\n"
		<< "    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const\n"
		<< "    {\n"
		<< "        Super::GetLifetimeReplicatedProps(OutLifetimeProps);\n"
		<< "        DOREPLIFETIME(A" << name << ", Entity);\n"
		<< "    }\n"
		<< "    virtual void ReplaceEntityComponent(UEntity *EntityPtr)\n"
		<< "    {\n"
		<< "        Entity = Cast<U" << name << "Entity>(EntityPtr);\n"
		<< "    }\n"
		<< "    virtual UEntity *ComponentEntity() { return Entity;	};\n"
		<< "};\n";
	return ret.str();
}
/**
* 根据def定义的entity产生虚幻的UObject对象代码
*
* @return UObject对象代码c++代码
*/
std::string  UEntityDef::DefineToUE4Object()
{
	std::stringstream retType;
	std::stringstream defineentityobject;
	std::vector<std::string> InterfaceNameList;
	std::map<std::string,std::string> InterfaceCreateMap;
	std::stringstream ClassTypeToName;
	std::stringstream ClassActorCharacter;
	ClassTypeToName << "inline void GetClassNameFromType(TMap<int,FString> &EntityClassIndex)\n"
		<< " {\n";

	retType << "#pragma once\n";
	retType << "#include \"" << GetProjectName() << ".h\"\n";
	retType << "#include \"Entity.h\"\n";
	retType << "#include \"Alias.h\"\n";
	retType << "#include \"DefineEntity.generated.h\"\n";
	retType << "#pragma optimize(\"\", off)\n";

	for (int i = 0; i < ScriptModules_.size(); i++)
	{
		if (!ScriptModules_[i]->hasClient())
			continue;

		defineentityobject << ScriptModules_[i]->DefineToUE4Object(InterfaceNameList, InterfaceCreateMap) + "\n";
		ClassTypeToName << "    EntityClassIndex.Add(" << ScriptModules_[i]->getUType() << ", TEXT(\"" << ScriptModules_[i]->getName() << "\"));\n";
		ClassActorCharacter << GenClassActorCharacter(ScriptModules_[i]->getName(), ScriptModules_[i]->getue4class());
	}
	for (int i = 0; i < InterfaceNameList.size(); i++)
	{
		retType << InterfaceCreateMap.find(InterfaceNameList[i])->second;
	}

	ClassTypeToName << " }\n";
	retType << defineentityobject.str();
	//retType << ClassActorCharacter.str();
	retType << ClassTypeToName.str() << "\n";
	retType << "#pragma optimize(\"\", on)\n";
	return retType.str();
}
std::string  UEntityDef::CreateDerivedClass(const char *classname, const char * derivedname)
{
	std::stringstream retType;
	std::stringstream defineentityobject;
	std::vector<std::string> InterfaceNameList;
	std::map<std::string, std::string> InterfaceCreateMap;

	retType << "#pragma once\n";
	retType << "#include \"DemoDefineEntity.h\"\n";
	retType << "#include \""<< derivedname <<".generated.h\"\n";
	retType << "#pragma optimize(\"\", off)\n";
	for (int i = 0; i < ScriptModules_.size(); i++)
	{
		if (!ScriptModules_[i]->hasClient())
			continue;
		if (ScriptModules_[i]->getName() == std::string(classname))
		{
			defineentityobject << ScriptModules_[i]->CreateDerivedClass(derivedname) + "\n";
			retType << defineentityobject.str();
			retType << "#pragma optimize(\"\", on)\n";
			return retType.str();
		}
	}
	for (int i = 0; i < ScriptModules_.size(); i++)
	{
		if (!ScriptModules_[i]->hasClient())
			continue;
		if (ScriptModules_[i]->HasInterface(classname))
		{
			defineentityobject << ScriptModules_[i]->CreateDerivedClassInterface(derivedname, classname) + "\n";
			retType << defineentityobject.str();
			retType << "#pragma optimize(\"\", on)\n";
			return retType.str();
		}
	}
	return "";

}
void UEntityDef::SetProjectName(const std::string &name)
{
	UE4ProjectName_ = name;
	UEntityDef::UE4ProjectAPIName_ = UE4ProjectName_ + "_API";
	std::transform(UEntityDef::UE4ProjectAPIName_.begin(), 
		UEntityDef::UE4ProjectAPIName_.end(),
		UEntityDef::UE4ProjectAPIName_.begin(), std::toupper);
}
}
