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


#ifndef KBE_SCRIPT_DEF_MODULE_H
#define KBE_SCRIPT_DEF_MODULE_H

#include "common.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif

#include "method.h"
#include "property.h"
#include "detaillevel.h"
#include "volatileinfo.h"
#include "xml.h"


namespace KBEngine{

    /**
      描述一个脚本def模块
      */
    class ScriptDefModule 
    {
        public:
            typedef std::map<std::string, PropertyDescription*> PROPERTYDESCRIPTION_MAP;
            typedef std::map<std::string, MethodDescription*> METHODDESCRIPTION_MAP;
            typedef std::map<ENTITY_PROPERTY_UID, PropertyDescription*> PROPERTYDESCRIPTION_UIDMAP;
            typedef std::map<ENTITY_METHOD_UID, MethodDescription*> METHODDESCRIPTION_UIDMAP;
            typedef std::map<ENTITY_DEF_ALIASID, PropertyDescription*> PROPERTYDESCRIPTION_ALIASMAP;
            typedef std::map<ENTITY_DEF_ALIASID, MethodDescription*> METHODDESCRIPTION_ALIASMAP;

            ScriptDefModule(std::string name, ENTITY_SCRIPT_UID utype);
            ~ScriptDefModule();

            void finalise(void);
            void onLoaded(void);


            INLINE ENTITY_SCRIPT_UID getUType(void);
            INLINE ENTITY_DEF_ALIASID getAliasID(void);
            void setUType(ENTITY_SCRIPT_UID utype);

            INLINE DetailLevel& getDetailLevel(void);
            INLINE VolatileInfo* getPVolatileInfo(void);

            INLINE void setCell(bool have);
            INLINE void setBase(bool have);
            INLINE void setClient(bool have);

            INLINE bool hasCell(void) const;
            INLINE bool hasBase(void) const;
            INLINE bool hasClient(void) const;

            PropertyDescription* findCellPropertyDescription(const char* attrName);
            PropertyDescription* findBasePropertyDescription(const char* attrName);
            PropertyDescription* findClientPropertyDescription(const char* attrName);
            PropertyDescription* findPersistentPropertyDescription(const char* attrName);
            PropertyDescription* findPropertyDescription(const char* attrName, COMPONENT_TYPE componentType);

            PropertyDescription* findCellPropertyDescription(ENTITY_PROPERTY_UID utype);
            PropertyDescription* findBasePropertyDescription(ENTITY_PROPERTY_UID utype);
            PropertyDescription* findClientPropertyDescription(ENTITY_PROPERTY_UID utype);
            PropertyDescription* findPersistentPropertyDescription(ENTITY_PROPERTY_UID utype);
            PropertyDescription* findPropertyDescription(ENTITY_PROPERTY_UID utype, COMPONENT_TYPE componentType);

            PropertyDescription* findAliasPropertyDescription(ENTITY_DEF_ALIASID aliasID);
            MethodDescription* findAliasMethodDescription(ENTITY_DEF_ALIASID aliasID);

            INLINE PROPERTYDESCRIPTION_MAP& getCellPropertyDescriptions();
            INLINE PROPERTYDESCRIPTION_MAP& getCellPropertyDescriptionsByDetailLevel(int8 detailLevel);
            INLINE PROPERTYDESCRIPTION_MAP& getBasePropertyDescriptions();
            INLINE PROPERTYDESCRIPTION_MAP& getClientPropertyDescriptions();
            INLINE PROPERTYDESCRIPTION_MAP& getPersistentPropertyDescriptions();

            INLINE PROPERTYDESCRIPTION_UIDMAP& getCellPropertyDescriptions_uidmap();
            INLINE PROPERTYDESCRIPTION_UIDMAP& getBasePropertyDescriptions_uidmap();
            INLINE PROPERTYDESCRIPTION_UIDMAP& getClientPropertyDescriptions_uidmap();
            INLINE PROPERTYDESCRIPTION_UIDMAP& getPersistentPropertyDescriptions_uidmap();

            ScriptDefModule::PROPERTYDESCRIPTION_MAP& getPropertyDescrs();

            bool addPropertyDescription(const char* attrName, 
                    PropertyDescription* propertyDescription, 
                    COMPONENT_TYPE componentType);


            MethodDescription* findCellMethodDescription(const char* attrName);
            MethodDescription* findCellMethodDescription(ENTITY_METHOD_UID utype);
            bool addCellMethodDescription(const char* attrName, MethodDescription* methodDescription);
            INLINE METHODDESCRIPTION_MAP& getCellMethodDescriptions(void);

            MethodDescription* findBaseMethodDescription(const char* attrName);
            MethodDescription* findBaseMethodDescription(ENTITY_METHOD_UID utype);
            bool addBaseMethodDescription(const char* attrName, MethodDescription* methodDescription);
            INLINE METHODDESCRIPTION_MAP& getBaseMethodDescriptions(void);

            MethodDescription* findClientMethodDescription(const char* attrName);
            MethodDescription* findClientMethodDescription(ENTITY_METHOD_UID utype);
            bool addClientMethodDescription(const char* attrName, MethodDescription* methodDescription);
            INLINE METHODDESCRIPTION_MAP& getClientMethodDescriptions(void);

            MethodDescription* findMethodDescription(const char* attrName, COMPONENT_TYPE componentType);
            MethodDescription* findMethodDescription(ENTITY_METHOD_UID utype, COMPONENT_TYPE componentType);

            INLINE METHODDESCRIPTION_MAP& getBaseExposedMethodDescriptions(void);
            INLINE METHODDESCRIPTION_MAP& getCellExposedMethodDescriptions(void);

            INLINE const char* getName();

            void autoMatchCompOwn();

            INLINE bool isPersistent() const;

            void c_str();

			/**
			* 产生定义的json字符串
			*
			* @return json字符串
			*/
            std::string ToJson();

			/**
			* 产生def定义的entity属性,客户端方法,base方法,cell方法
			*
			* @param vl 接口列表
			* @param vm 接口字典，存放创建这个接口的C++代码
			* @return c++代码
			*/
			std::string  DefineToUE4Object(std::vector<std::string> &vl, std::map<std::string, std::string> &vm);

			/**
			* 产生调用服务器base和cell方法
			*
			* @param method 方法描述
			* @return 给定方法定义的c++代码
			*/
			std::string   DefineToUE4AuthorityCall(MethodDescription *method);
			/**
			* 产生调用服务器base和cell方法
			*
			* @param method 方法描述
			* @param type base方法0,cell方法1
			* @return 给定方法定义的c++代码
			*/
			std::string  DefineToUE4Method(MethodDescription *method, int type);
			/**
			* 产生客户端函数代码
			*
			* @param method 方法描述
			* @param type 1:蓝图方法 0:非蓝图方法
			* @return 给定方法定义的c++代码
			*/
			std::string  DefineToUE4ClientMethod(MethodDescription *method, int type); 

			/**
			* 判断是否可以产生蓝图方法
			*
			* @return 蓝图类型true或false
			*/
			bool  IsUE4BluePrintMethod(MethodDescription *method);
			/**
			* 产生客户端函数调用代码
			*
			* @param method 方法描述
			* @return 给定方法定义的c++代码
			*/
			std::string  ClientMethodCall(MethodDescription *method);
			/**
			* 根据模块名称产生创建UObject的头c++代码
			*
			* @param name 模块名称如果是Entity表示定义的实体其他表示接口
			* @return c++代码
			*/
			std::string  InitCreateUobjectHead(const std::string &name);

            INLINE bool usePropertyDescrAlias() const;
            INLINE bool useMethodDescrAlias() const;

			std::string CreateDerivedClass(const std::string &derivedname);
			std::string CreateDerivedClassInterface(const std::string &derivedname, const std::string &interfacename);

			bool HasInterface(const std::string &Interfacename);

			std::string  GetUE4Property(const std::string &name);
			void setue4class(const char *v)
			{
				ue4class_ = v;
			}
			const std::string &getue4class() { return ue4class_;  }

        protected:

            // 数字类别  主要用于方便查找和网络间传输识别这个脚本模块
            ENTITY_SCRIPT_UID					uType_;

            // 这个脚本所有的存储到db的属性
            PROPERTYDESCRIPTION_MAP				persistentPropertyDescr_;

            // 这个脚本cell部分所拥有的所有属性描述
            PROPERTYDESCRIPTION_MAP				cellPropertyDescr_;

            // cell近中远级别属性描述
            PROPERTYDESCRIPTION_MAP				cellDetailLevelPropertyDescrs_[3];

            // 这个脚本base部分所拥有的属性描述
            PROPERTYDESCRIPTION_MAP				basePropertyDescr_;

            // 这个脚本client部分所拥有的属性描述
            PROPERTYDESCRIPTION_MAP				clientPropertyDescr_;

            // 这个脚本所拥有的属性描述uid映射
            PROPERTYDESCRIPTION_UIDMAP			persistentPropertyDescr_uidmap_;
            PROPERTYDESCRIPTION_UIDMAP			cellPropertyDescr_uidmap_;
            PROPERTYDESCRIPTION_UIDMAP			basePropertyDescr_uidmap_;
            PROPERTYDESCRIPTION_UIDMAP			clientPropertyDescr_uidmap_;

            // 这个脚本所拥有的属性描述aliasID映射
            PROPERTYDESCRIPTION_ALIASMAP		propertyDescr_aliasmap_;

            // 这个脚本所拥有的方法描述
            METHODDESCRIPTION_MAP				methodCellDescr_;
            METHODDESCRIPTION_MAP				methodBaseDescr_;
            METHODDESCRIPTION_MAP				methodClientDescr_;

            METHODDESCRIPTION_MAP				methodBaseExposedDescr_;
            METHODDESCRIPTION_MAP				methodCellExposedDescr_;

            // 这个脚本所拥有的方法描述uid映射
            METHODDESCRIPTION_UIDMAP			methodCellDescr_uidmap_;
            METHODDESCRIPTION_UIDMAP			methodBaseDescr_uidmap_;
            METHODDESCRIPTION_UIDMAP			methodClientDescr_uidmap_;

            METHODDESCRIPTION_ALIASMAP			methodDescr_aliasmap_;

            // 是否有cell部分等
            bool								hasCell_;
            bool								hasBase_;
            bool								hasClient_;

            // entity的详情级别数据
            DetailLevel							detailLevel_;
            VolatileInfo*						pVolatileinfo_;

            // 这个模块的名称
            std::string							name_;

            std::string							ue4class_;

            bool								usePropertyDescrAlias_;
            bool								useMethodDescrAlias_;
    };

    INLINE DetailLevel& ScriptDefModule::getDetailLevel(void)
    {
        return detailLevel_;
    }

    //-------------------------------------------------------------------------------------
    INLINE VolatileInfo* ScriptDefModule::getPVolatileInfo(void)
    {
        return pVolatileinfo_;
    }

    //-------------------------------------------------------------------------------------
    INLINE void ScriptDefModule::setCell(bool have)
    {
        hasCell_ = have;
    }

    //-------------------------------------------------------------------------------------
    INLINE void ScriptDefModule::setBase(bool have)
    {
        hasBase_ = have;
    }

    //-------------------------------------------------------------------------------------
    INLINE void ScriptDefModule::setClient(bool have)
    {
        hasClient_ = have;
    }

    //-------------------------------------------------------------------------------------
    INLINE bool ScriptDefModule::hasCell(void) const
    {
        return hasCell_;
    }

    //-------------------------------------------------------------------------------------
    INLINE bool ScriptDefModule::hasBase(void) const
    {
        return hasBase_;
    }

    //-------------------------------------------------------------------------------------
    INLINE bool ScriptDefModule::hasClient(void) const
    {
        return hasClient_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::PROPERTYDESCRIPTION_MAP& ScriptDefModule::getCellPropertyDescriptions()
    {
        return cellPropertyDescr_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::PROPERTYDESCRIPTION_MAP& ScriptDefModule::getCellPropertyDescriptionsByDetailLevel(int8 detailLevel)
    {
        return cellDetailLevelPropertyDescrs_[detailLevel];
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::PROPERTYDESCRIPTION_MAP& ScriptDefModule::getBasePropertyDescriptions()
    {
        return basePropertyDescr_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::PROPERTYDESCRIPTION_MAP& ScriptDefModule::getClientPropertyDescriptions()
    {
        return clientPropertyDescr_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::PROPERTYDESCRIPTION_MAP& ScriptDefModule::getPersistentPropertyDescriptions()
    {
        return persistentPropertyDescr_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::PROPERTYDESCRIPTION_UIDMAP& ScriptDefModule::getCellPropertyDescriptions_uidmap()
    {
        return cellPropertyDescr_uidmap_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::PROPERTYDESCRIPTION_UIDMAP& ScriptDefModule::getBasePropertyDescriptions_uidmap()
    {
        return basePropertyDescr_uidmap_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::PROPERTYDESCRIPTION_UIDMAP& ScriptDefModule::getClientPropertyDescriptions_uidmap()
    {
        return clientPropertyDescr_uidmap_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::PROPERTYDESCRIPTION_UIDMAP& ScriptDefModule::getPersistentPropertyDescriptions_uidmap()
    {
        return persistentPropertyDescr_uidmap_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::METHODDESCRIPTION_MAP& ScriptDefModule::getCellMethodDescriptions(void)
    {
        return methodCellDescr_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::METHODDESCRIPTION_MAP& ScriptDefModule::getBaseMethodDescriptions(void)
    {
        return methodBaseDescr_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::METHODDESCRIPTION_MAP& ScriptDefModule::getClientMethodDescriptions(void)
    {
        return methodClientDescr_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::METHODDESCRIPTION_MAP& ScriptDefModule::getBaseExposedMethodDescriptions(void)
    {
        return methodBaseExposedDescr_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ScriptDefModule::METHODDESCRIPTION_MAP& ScriptDefModule::getCellExposedMethodDescriptions(void)
    {
        return methodCellExposedDescr_;
    }

    //-------------------------------------------------------------------------------------
    INLINE const char* ScriptDefModule::getName()
    {
        return name_.c_str();
    }

    //-------------------------------------------------------------------------------------
    INLINE bool ScriptDefModule::isPersistent() const
    {
        return persistentPropertyDescr_uidmap_.size() > 0;
    }

    //-------------------------------------------------------------------------------------
    INLINE bool ScriptDefModule::usePropertyDescrAlias() const
    {
        return usePropertyDescrAlias_;
    }

    //-------------------------------------------------------------------------------------
    INLINE bool ScriptDefModule::useMethodDescrAlias() const
    {
        return useMethodDescrAlias_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ENTITY_SCRIPT_UID ScriptDefModule::getUType(void)
    {
        return uType_;
    }

    //-------------------------------------------------------------------------------------
    INLINE ENTITY_DEF_ALIASID ScriptDefModule::getAliasID(void)
    {
        return (ENTITY_DEF_ALIASID)uType_;
    }

}

#endif // KBE_SCRIPT_DEF_MODULE_H

