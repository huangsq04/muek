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
		��ʼ��
		*/
		bool initialize(const std::string &resbasepath);

		bool finalise(bool isReload = false);

		void reload(bool fullReload);
		/**
		* ���������json�ַ���
		*
		* @return json�ַ���
		*/
		std::string ToJson();

		/**
		* ����def�����Alias������õĽṹ��
		*
		* @return �ṹ���c++����
		*/
		std::string  AliasToUE4Struct();
		/**
		* ����def�����entity������õ�UObject�������
		*
		* @return UObject�������c++����
		*/
		std::string  DefineToUE4Object();

		/**
		* �����̳��ֻ࣬������ຯ����
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
		���ű�ģ���б�����������Ƿ�Ϸ�
		*/
		bool validDefPropertyName(ScriptDefModule* pScriptModule, const std::string& name);

		/**
		ͨ�������Ѱ�ҵ���Ӧ�Ľű�ģ�����
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
		SCRIPT_MODULES ScriptModules_;					// ���е���չ�ű�ģ�鶼�洢������
		SCRIPT_MODULES OldScriptModules_;			    // reloadʱ�ɵ�ģ���ŵ����������ж�

		SCRIPT_MODULE_UID_MAP ScriptTypeMappingUType_;  // �ű����ӳ��utype
		SCRIPT_MODULE_UID_MAP OldScriptTypeMappingUType_;   // reloadʱ�ɵĽű����ӳ��utype

		ADataTypes *DataTypes_;

		std::string  UE4ProjectName_;
		//static KBE_MD5 __md5;									// defs-md5

	};
}

#endif // KBE_ENTITYDEF_H

