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


#ifndef KBE_ENTITYDEF_H
#define KBE_ENTITYDEF_H

#include "common.h"
//#include "common/md5.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif

#include "method.h"	
#include "property.h"
#include "xml.h"	

namespace KBEngine{

    class ScriptDefModule;
    typedef ScriptDefModule * ScriptDefModulePtr;

	class UEntityDef
	{
	public:
		typedef std::vector<ScriptDefModulePtr> SCRIPT_MODULES;
		typedef std::map<std::string, ENTITY_SCRIPT_UID> SCRIPT_MODULE_UID_MAP;

		UEntityDef();
		~UEntityDef();

		/**
		初始化
		*/
		bool initialize(const std::string &resbasepath);

		bool finalise(bool isReload = false);

		void reload(bool fullReload);
		/**
		* 产生定义的json字符串
		*
		* @return json字符串
		*/
		std::string ToJson();

		/**
		* 根据def定义的Alias产生虚幻的结构体
		*
		* @return 结构体的c++代码
		*/
		std::string  AliasToUE4Struct();
		/**
		* 根据def定义的entity产生虚幻的UObject对象代码
		*
		* @return UObject对象代码c++代码
		*/
		std::string  DefineToUE4Object();

		/**
		* 产生继承类，只处理基类函数。
		*
		* @return string
		*/
		std::string  CreateDerivedClass(const char *classname, const char * derivedname);

		bool loadAllDefDescriptions(const std::string& moduleName,
			XML* defxml,
			TiXmlNode* defNode,
			ScriptDefModule* pScriptModule,
			const std::string &Implements = "Entity");

		bool loadDefPropertys(const std::string& moduleName,
			XML* xml,
			TiXmlNode* defPropertyNode,
			ScriptDefModule* pScriptModule,
			const std::string &Implements = "Entity");

		bool loadDefCellMethods(const std::string& moduleName,
			XML* xml,
			TiXmlNode* defMethodNode,
			ScriptDefModule* pScriptModule,
			const std::string &Implements);

		bool loadDefBaseMethods(const std::string& moduleName,
			XML* xml,
			TiXmlNode* defMethodNode,
			ScriptDefModule* pScriptModule,
			const std::string &Implements);

		bool loadDefClientMethods(const std::string& moduleName,
			XML* xml,
			TiXmlNode* defMethodNode,
			ScriptDefModule* pScriptModule,
			const std::string &Implements);

		bool loadInterfaces(const std::string& defFilePath,
			const std::string& moduleName,
			XML* defxml,
			TiXmlNode* defNode,
			ScriptDefModule* pScriptModule);

		bool loadParentClass(const std::string& defFilePath,
			const std::string& moduleName,
			XML* defxml,
			TiXmlNode* defNode,
			ScriptDefModule* pScriptModule);

		bool loadDefInfo(const std::string& defFilePath,
			const std::string& moduleName,
			XML* defxml,
			TiXmlNode* defNode,
			ScriptDefModule* pScriptModule);

		bool loadDetailLevelInfo(const std::string& defFilePath,
			const std::string& moduleName,
			XML* defxml,
			TiXmlNode* defNode,
			ScriptDefModule* pScriptModule);

		bool loadVolatileInfo(const std::string& defFilePath,
			const std::string& moduleName,
			XML* defxml,
			TiXmlNode* defNode,
			ScriptDefModule* pScriptModule);


		/**
		检查脚本模块中被定义的属性是否合法
		*/
		bool validDefPropertyName(ScriptDefModule* pScriptModule, const std::string& name);

		/**
		通过标记来寻找到对应的脚本模块对象
		*/
		ScriptDefModule* findScriptModule(ENTITY_SCRIPT_UID utype);
		ScriptDefModule* findScriptModule(const char* scriptName);
		ScriptDefModule* findOldScriptModule(const char* scriptName);

		const SCRIPT_MODULES& getScriptModules() {
			return UEntityDef::ScriptModules_;
		}

		//static KBE_MD5& md5(){ return __md5; }

		bool initializeWatcher();

		std::string GenClassActorCharacter(const std::string &name, const std::string &uclass);

		void SetProjectName(const std::string &name);
		std::string &GetProjectName() { return UE4ProjectName_; };

		static std::string  UE4ProjectAPIName_;
	private:
		SCRIPT_MODULES ScriptModules_;					// 所有的扩展脚本模块都存储在这里
		SCRIPT_MODULES OldScriptModules_;			    // reload时旧的模块会放到这里用于判断

		SCRIPT_MODULE_UID_MAP ScriptTypeMappingUType_;  // 脚本类别映射utype
		SCRIPT_MODULE_UID_MAP OldScriptTypeMappingUType_;   // reload时旧的脚本类别映射utype

		ADataTypes *DataTypes_;

		std::string  UE4ProjectName_;
		//static KBE_MD5 __md5;									// defs-md5

	};
}

#endif // KBE_ENTITYDEF_H

