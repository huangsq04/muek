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


#ifndef KBENGINE_DEF_METHOD_H
#define KBENGINE_DEF_METHOD_H

#include "common.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif

#include "datatype.h"
#include "datatypes.h"


namespace KBEngine{

    class MethodDescription
    {
        public:	
            MethodDescription(ENTITY_METHOD_UID utype, int32 domain,
                    std::string name, 
                    bool isExposed = false);

            virtual ~MethodDescription();

            INLINE const char* getName(void) const;

            INLINE ENTITY_METHOD_UID getUType(void) const;
            INLINE void setUType(ENTITY_METHOD_UID muid);

            static uint32 getDescriptionCount(void){ return methodDescriptionCount_; }
            static void resetDescriptionCount(void){ methodDescriptionCount_ = 0; }

            INLINE bool isExposed(void) const;

            void setExposed(void);

            bool pushArgType(DataType* dataType, const char *argName = NULL);

            INLINE std::vector<DataType*>& getArgTypes(void);
			INLINE std::vector<std::string>& getArgNames(void);
            size_t getArgSize(void);

            INLINE void currCallerID(int32 eid);

            INLINE int32 domain() const;

            INLINE bool isClient() const;
            INLINE bool isCell() const;
            INLINE bool isBase() const;

            /** 
              ����id�� ����¶�ķ������߹㲥�������ܸ���С��255ʱ
              ���ǲ�ʹ��utype��ʹ��1�ֽڵ�aliasID������
              */
            INLINE int16 aliasID() const;
            INLINE uint8 aliasIDAsUint8() const;
            INLINE void aliasID(int16 v);
			void SetImplements(const std::string &Implements) { Implements_ = Implements; };
			const std::string &GetImplements() { return Implements_; };

			void SetAuthority(int s) { Authority_ = s; };
			int GetAuthority() { return Authority_; };

        protected:
            static uint32							methodDescriptionCount_;					// ���е���������������

            int32							methodDomain_;

            std::string								name_;										// �������������
            ENTITY_METHOD_UID						utype_;										// ���������������� ���������ϴ���ʶ��

            std::vector<DataType*>					argTypes_;									// ������ԵĲ�������б�
			std::vector<std::string>				argName_;									// ������ԵĲ��������б�

            bool									isExposed_;									// �Ƿ���һ����¶����

            int32								currCallerID_;								// ��ǰ������������ĵ�����ID, �ṩ��¶��������ʱ���ű��жϵ���Դ��ֹ����

            int16									aliasID_;									// ����id�� ����¶�ķ������߹㲥�������ܸ���С��255ʱ�� ���ǲ�ʹ��utype��ʹ��1�ֽڵ�aliasID������
			std::string                         Implements_;									//�����Ǹ��ӿ�
			int                                 Authority_;                                     //�������ִ��Ȩ��Ĭ��0��1����ʾֻ���ڷ�����������ʹ��Call+���������е���
			                                                                                    //2:�������������¶���
    };


    INLINE const char* MethodDescription::getName(void) const
    {
        return name_.c_str();
    };

    INLINE ENTITY_METHOD_UID MethodDescription::getUType(void) const
    {
        return utype_;
    }

    INLINE void MethodDescription::setUType(ENTITY_METHOD_UID muid)
    {
        utype_ = muid;
    }

    INLINE bool MethodDescription::isExposed(void) const
    {
        return isExposed_;
    }

    INLINE std::vector<DataType*>& MethodDescription::getArgTypes(void)
    {
        return argTypes_;
    }
	INLINE std::vector<std::string>& MethodDescription::getArgNames(void)
	{
		return argName_;
	}
    INLINE void MethodDescription::currCallerID(int32 eid)
    {
        currCallerID_ = eid;
    }

    INLINE int16 MethodDescription::aliasID() const
    {
        return aliasID_;
    }

    INLINE uint8 MethodDescription::aliasIDAsUint8() const
    {
        return (uint8)aliasID_;
    }

    INLINE void MethodDescription::aliasID(int16 v)
    {
        aliasID_ = v;
    }

    INLINE int32 MethodDescription::domain() const
    {
        return methodDomain_;
    }

    INLINE bool MethodDescription::isClient() const
    {
        return !isCell() && !isBase();
    }

    INLINE bool MethodDescription::isCell() const
    {
        return methodDomain_ == CELLAPP_TYPE;
    }

    INLINE bool MethodDescription::isBase() const
    {
        return methodDomain_ == BASEAPP_TYPE;
    }



}

#endif // KBENGINE_DEF_METHOD_H
