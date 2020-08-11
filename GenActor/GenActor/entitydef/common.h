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


#ifndef KBENGINE_DEF_COMMON_H
#define KBENGINE_DEF_COMMON_H

#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
//#include <objbase.h>

#define INLINE    inline
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
// 常规的buf长度
#define MAX_BUF 256

namespace KBEngine {

    typedef signed __int64											int64;
    typedef signed __int32											int32;
    typedef signed __int16											int16;
    typedef signed __int8											int8;
    typedef unsigned __int64										uint64;
    typedef unsigned __int32										uint32;
    typedef unsigned __int16										uint16;
    typedef unsigned __int8											uint8;

    // 属性和方法的UID类别
    typedef uint16 ENTITY_PROPERTY_UID;
    typedef uint16 ENTITY_METHOD_UID;
    typedef uint16 ENTITY_SCRIPT_UID;
    typedef uint16 DATATYPE_UID;
    typedef uint8  DATATYPE;
    typedef uint8  ENTITY_DEF_ALIASID;
    template <class TYPE>
        inline std::string val2str(const TYPE& value)
        {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }
     typedef struct _KGUID {
			unsigned int  Data1;
			unsigned short Data2;
			unsigned short Data3;
			unsigned char  Data4[8];
		} KGUID;
    inline std::string genUUID64()
    {
        char buffer[64] = { 0 };
		KGUID guid;
		guid.Data1 = rand();
		guid.Data2 = rand();
		guid.Data3 = rand();

        _snprintf_s(buffer, sizeof(buffer),
                "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
                guid.Data1, guid.Data2, guid.Data3,
                guid.Data4[0], guid.Data4[1], guid.Data4[2],
                guid.Data4[3], guid.Data4[4], guid.Data4[5],
                guid.Data4[6], guid.Data4[7]);
        return std::string(buffer);
    }
    // entity的数据传输特性标记 
    enum EntityDataFlags
    {
        ED_FLAG_UNKOWN = 0x00000000, // 未定义
        ED_FLAG_CELL_PUBLIC = 0x00000001, // 相关所有cell广播
        ED_FLAG_CELL_PRIVATE = 0x00000002, // 当前cell
        ED_FLAG_ALL_CLIENTS = 0x00000004, // cell广播与所有客户端
        ED_FLAG_CELL_PUBLIC_AND_OWN = 0x00000008, // cell广播与自己的客户端
        ED_FLAG_OWN_CLIENT = 0x00000010, // 当前cell和客户端
        ED_FLAG_BASE_AND_CLIENT = 0x00000020, // base和客户端
        ED_FLAG_BASE = 0x00000040, // 当前base
        ED_FLAG_OTHER_CLIENTS = 0x00000080, // cell广播和其他客户端
    };

    std::string entityDataFlagsToString(uint32 flags);

#define ED_FLAG_ALL  ED_FLAG_CELL_PUBLIC | ED_FLAG_CELL_PRIVATE | ED_FLAG_ALL_CLIENTS \
    | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OWN_CLIENT |	\
    ED_FLAG_BASE_AND_CLIENT | ED_FLAG_BASE | ED_FLAG_OTHER_CLIENTS

    // 相当于对entity数据传输类别的一个总体的定义 
    enum EntityDataFlagRelation
    {
        // 所有与baseapp有关系的标志
        ENTITY_BASE_DATA_FLAGS = ED_FLAG_BASE | ED_FLAG_BASE_AND_CLIENT,
        // 所有与cellapp有关系的标志
        ENTITY_CELL_DATA_FLAGS = ED_FLAG_CELL_PUBLIC | ED_FLAG_CELL_PRIVATE | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
        // 所有与client有关系的标志
        ENTITY_CLIENT_DATA_FLAGS = ED_FLAG_BASE_AND_CLIENT | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
        // 所有需要广播给其他cellapp的标志
        ENTITY_BROADCAST_CELL_FLAGS = ED_FLAG_CELL_PUBLIC | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS,
        // 所有需要广播给其他客户端(不包括自己的)的标志
        ENTITY_BROADCAST_OTHER_CLIENT_FLAGS = ED_FLAG_OTHER_CLIENTS | ED_FLAG_ALL_CLIENTS,
        // 所有需要广播给自己的客户端的标志
        ENTITY_BROADCAST_OWN_CLIENT_FLAGS = ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OWN_CLIENT | ED_FLAG_BASE_AND_CLIENT,
    };
    enum ENTITY_MAILBOX_TYPE
    {
        MAILBOX_TYPE_CELL = 0,
        MAILBOX_TYPE_BASE = 1,
        MAILBOX_TYPE_CLIENT = 2,
        MAILBOX_TYPE_CELL_VIA_BASE = 3,
        MAILBOX_TYPE_BASE_VIA_CELL = 4,
        MAILBOX_TYPE_CLIENT_VIA_CELL = 5,
        MAILBOX_TYPE_CLIENT_VIA_BASE = 6,
    };
    enum COMPONENT_TYPE
    {
        UNKNOWN_COMPONENT_TYPE = 0,
        DBMGR_TYPE = 1,
        LOGINAPP_TYPE = 2,
        BASEAPPMGR_TYPE = 3,
        CELLAPPMGR_TYPE = 4,
        CELLAPP_TYPE = 5,
        BASEAPP_TYPE = 6,
        CLIENT_TYPE = 7,
        MACHINE_TYPE = 8,
        CONSOLE_TYPE = 9,
        LOGGER_TYPE = 10,
        BOTS_TYPE = 11,
        WATCHER_TYPE = 12,
        INTERFACES_TYPE = 13,
        COMPONENT_END_TYPE = 14,
    };
    const COMPONENT_TYPE ENTITY_MAILBOX_COMPONENT_TYPE_MAPPING[] =
    {
        CELLAPP_TYPE,
        BASEAPP_TYPE,
        CLIENT_TYPE,
        BASEAPP_TYPE,
        CELLAPP_TYPE,
        CELLAPP_TYPE,
        BASEAPP_TYPE,
    };
    // 属性的lod广播级别范围的定义 
    typedef uint8 DETAIL_TYPE;
#define DETAIL_LEVEL_NEAR													0	// lod级别：近						
#define DETAIL_LEVEL_MEDIUM													1	// lod级别：中
#define DETAIL_LEVEL_FAR													2	// lod级别：远	

    typedef std::map<std::string, EntityDataFlags> ENTITYFLAGMAP;
    extern ENTITYFLAGMAP g_entityFlagMapping;										// entity 的flag字符串映射表



#define DATA_TYPE_UNKONWN		0
#define DATA_TYPE_FIXEDARRAY	1
#define DATA_TYPE_FIXEDDICT		2
#define DATA_TYPE_STRING		3
#define DATA_TYPE_DIGIT			4
#define DATA_TYPE_BLOB			5
#define DATA_TYPE_PYTHON		6
#define DATA_TYPE_VECTOR		7
#define DATA_TYPE_UNICODE		8
#define DATA_TYPE_MAILBOX		9
#define DATA_TYPE_PYDICT		10
#define DATA_TYPE_PYTUPLE		11
#define DATA_TYPE_PYLIST		12

    // 对entity的一些系统级别的可变属性进行编号以便网络传输时进行辨别
    enum ENTITY_BASE_PROPERTY_UTYPE
    {
        ENTITY_BASE_PROPERTY_UTYPE_POSITION_XYZ = 1,
        ENTITY_BASE_PROPERTY_UTYPE_DIRECTION_ROLL_PITCH_YAW = 2,
        ENTITY_BASE_PROPERTY_UTYPE_SPACEID = 3,
    };

    // 对entity的一些系统级别的可变属性进行编号以便网络传输时进行辨别
    enum ENTITY_BASE_PROPERTY_ALIASID
    {
        ENTITY_BASE_PROPERTY_ALIASID_POSITION_XYZ = 0,
        ENTITY_BASE_PROPERTY_ALIASID_DIRECTION_ROLL_PITCH_YAW = 1,
        ENTITY_BASE_PROPERTY_ALIASID_SPACEID = 2,
        ENTITY_BASE_PROPERTY_ALIASID_MAX = 3,
    };

    // 被限制的系统属性，def中不允许定义
    const char ENTITY_LIMITED_PROPERTYS[][32] =
    {
        "id",
        "position",
        "direction",
        "spaceID",
        "autoLoad",
        "cell",
        "base",
        "client",
        "cellData",
        "className",
        "databaseID",
        "isDestroyed",
        "shouldAutoArchive",
        "shouldAutoBackup",
        "__ACCOUNT_NAME__",
        "__ACCOUNT_PASSWORD__",
        "clientAddr",
        "entitiesEnabled",
        "hasClient",
        "roundTripTime",
        "timeSinceHeardFromClient",
        "allClients",
        "hasWitness",
        "isWitnessed",
        "otherClients",
        "topSpeed",
        "topSpeedY",
        "",
    };


    template<typename T>
        inline void kbe_split(const std::basic_string<T>& s, T c, std::vector< std::basic_string<T> > &v)
        {
            if (s.size() == 0)
                return;

            typename std::basic_string< T >::size_type i = 0;
            typename std::basic_string< T >::size_type j = s.find(c);

            while (j != std::basic_string<T>::npos)
            {
                std::basic_string<T> buf = s.substr(i, j - i);

                if (buf.size() > 0)
                    v.push_back(buf);

                i = ++j; j = s.find(c, j);
            }

            if (j == std::basic_string<T>::npos)
            {
                std::basic_string<T> buf = s.substr(i, s.length() - i);
                if (buf.size() > 0)
                    v.push_back(buf);
            }
        }
    extern COMPONENT_TYPE g_componentType;
	
}
#endif // KBENGINE_DEF_COMMON_H

