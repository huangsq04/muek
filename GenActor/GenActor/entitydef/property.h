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


#ifndef KBENGINE_DEF_PROPERTY_H
#define KBENGINE_DEF_PROPERTY_H

#include "common.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif

#include "fixedarray.h"
#include "fixeddict.h"
#include "datatype.h"

namespace KBEngine{

    class RefCountable;
    class PropertyDescription 
    {
        public:	
            PropertyDescription(ENTITY_PROPERTY_UID utype, 
                    std::string dataTypeName, 
                    std::string name, 
                    uint32 flags, 
                    bool isPersistent, 
                    DataType* dataType, 
                    bool isIdentifier, 
                    std::string indexType,
                    uint32 databaseLength, 
                    std::string defaultStr, 
                    DETAIL_TYPE detailLevel);

            virtual ~PropertyDescription();
			void SetImplements(const std::string &Implements) { Implements_ = Implements; };
			const std::string &GetImplements() { return Implements_; };
            /** 
              ��ȡ������Ե�detailLevel 
              */
            INLINE int8 getDetailLevel(void) const;

            /** 
              �Ƿ���һ�����浽���ݿ��е����� 
              */
            INLINE bool isPersistent(void) const;

            /** 
              ��ȡ������Ե�������� 
              */
            INLINE DataType* getDataType(void) const;

            /** 
              ��ȡ���Եı�־ cell_public�� 
              */
            INLINE uint32 getFlags(void) const;

            /** 
              ��ȡ�������� 
              */
            INLINE const char* getName(void) const;

            /** 
              ��ȡ�ַ������������ UINT32, BAG..
              */
            INLINE const char* getDataTypeName(void) const;

            /** 
              ��ȡ��ʼֵ�ַ���
              */
            INLINE const char* getDefaultValStr(void) const;

            /** 
              ���Ե�������� ���������ϴ���ʶ�� 
              */
            INLINE ENTITY_PROPERTY_UID getUType(void) const;

            /** 
              ��ȡ�����������
              */
            INLINE const char* indexType(void) const;

            /** 
              ����id�� ����¶�ķ������߹㲥�������ܸ���С��255ʱ
              ���ǲ�ʹ��utype��ʹ��1�ֽڵ�aliasID������
              */
            INLINE int16 aliasID() const;
            INLINE uint8 aliasIDAsUint8() const;
            INLINE void aliasID(int16 v);

            /** 
              �����������Ϊ������ 
              */
            INLINE void setIdentifier(bool isIdentifier);

            /** 
              ����������������ݿ��еĳ��� 
              */
            INLINE void setDatabaseLength(uint32 databaseLength);
            INLINE uint32 getDatabaseLength() const;

            /** 
              ��ȡ�������������def�ļ��б������Ĭ��ֵ 
              */
            //PyObject* newDefaultVal(void);

            /** 
              ������������������� 
              */
            static uint32 getDescriptionCount(void){ return propertyDescriptionCount_; }
            static void resetDescriptionCount(void){ propertyDescriptionCount_ = 0; }

            /** 
              �������Ͳ���һ������ʵ�� 
              */
            static PropertyDescription* createDescription(ENTITY_PROPERTY_UID utype, 
                    std::string& dataTypeName, 
                    std::string& name,
                    uint32 flags, 
                    bool isPersistent, 
                    DataType* dataType, 
                    bool isIdentifier, 
                    std::string indexType,
                    uint32 databaseLength,
                    std::string& defaultStr, 
                    DETAIL_TYPE detailLevel);


            INLINE bool hasCell(void) const;
            INLINE bool hasBase(void) const;
            INLINE bool hasClient(void) const;

        protected:	
            static uint32				propertyDescriptionCount_;						// ���е���������������	
            std::string					name_;											// ������Ե�����
            std::string					dataTypeName_;									// ������Ե��ַ������������
            uint32						flags_;											// ������Ե�һЩ��־  ���� cell_public
            bool						isPersistent_;									// �Ƿ���һ���洢�����ݿ������
            DataType*					dataType_;										// ������Ե��������
            bool						isIdentifier_;									// �Ƿ���һ��������
            uint32						databaseLength_;								// ������������ݿ��еĳ���
            ENTITY_PROPERTY_UID			utype_;											// ������Ե�������� ���������ϴ���ʶ��
            std::string					defaultValStr_;									// ������Ե�Ĭ��ֵ
            DETAIL_TYPE					detailLevel_;									// ������Ե�lod���鼶�� ��common�е�:���Ե�lod�㲥����Χ�Ķ���
            int16						aliasID_;										// ����id�� ����¶�ķ������߹㲥�������ܸ���С��255ʱ�� ���ǲ�ʹ��utype��ʹ��1�ֽڵ�aliasID������
            std::string					indexType_;										// ���Ե��������UNIQUE, INDEX���ֱ��Ӧ�����á�Ψһ��������ͨ����
			std::string                 Implements_;									//�����Ǹ��ӿ�
    };

    class FixedDictDescription : public PropertyDescription
    {
        public:	
            FixedDictDescription(ENTITY_PROPERTY_UID utype, 
                    std::string dataTypeName,
                    std::string name, 
                    uint32 flags, 
                    bool isPersistent, 
                    DataType* dataType, 
                    bool isIdentifier, 
                    std::string indexType,
                    uint32 databaseLength, 
                    std::string defaultStr, 
                    DETAIL_TYPE detailLevel);

            virtual ~FixedDictDescription();


            typedef std::vector<std::pair<std::string, PropertyDescription * > > CHILD_PROPERTYS;

        protected:
            CHILD_PROPERTYS childPropertys_;
    };

    class ArrayDescription : public PropertyDescription
    {
        public:	
            ArrayDescription(ENTITY_PROPERTY_UID utype, 
                    std::string dataTypeName, 
                    std::string name, 
                    uint32 flags, 
                    bool isPersistent, 
                    DataType* dataType, 
                    bool isIdentifier,
                    std::string indexType,
                    uint32 databaseLength, 
                    std::string defaultStr, 
                    DETAIL_TYPE detailLevel);

            virtual ~ArrayDescription();


        protected:	
    };

    class VectorDescription : public PropertyDescription
    {
        public:	
            VectorDescription(ENTITY_PROPERTY_UID utype, 
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
                    uint8 elemCount);

            virtual ~VectorDescription();

        protected:	
            uint8 elemCount_;
    };

    INLINE int8 PropertyDescription::getDetailLevel(void) const
    {
        return detailLevel_;
    }

    INLINE bool PropertyDescription::isPersistent(void) const
    {
        return isPersistent_;
    };

    INLINE DataType* PropertyDescription::getDataType(void) const
    {
        return dataType_;
    };

    INLINE uint32 PropertyDescription::getFlags(void) const
    {
        return flags_;
    };

    INLINE const char* PropertyDescription::getName(void) const
    {
        return name_.c_str();
    };

    INLINE const char* PropertyDescription::getDataTypeName(void) const
    {
        return dataTypeName_.c_str();
    }

    INLINE const char* PropertyDescription::getDefaultValStr(void) const
    {
        return defaultValStr_.c_str();
    }

    INLINE ENTITY_PROPERTY_UID PropertyDescription::getUType(void) const
    {
        return utype_;
    }

    INLINE void PropertyDescription::setIdentifier(bool isIdentifier)
    {
        isIdentifier_ = isIdentifier;
    }

    INLINE void PropertyDescription::setDatabaseLength(uint32 databaseLength)
    {
        databaseLength_ = databaseLength;
    }

    INLINE uint32 PropertyDescription::getDatabaseLength() const
    {
        return databaseLength_;
    }

    INLINE int16 PropertyDescription::aliasID() const
    {
        return aliasID_;
    }

    INLINE uint8 PropertyDescription::aliasIDAsUint8() const
    {
        return (uint8)aliasID_;
    }

    INLINE void PropertyDescription::aliasID(int16 v)
    {
        aliasID_ = v;
    }

    INLINE const char* PropertyDescription::indexType(void) const
    {
        return indexType_.c_str();
    }

    INLINE bool PropertyDescription::hasCell(void) const
    {
        return (flags_ & ENTITY_CELL_DATA_FLAGS) > 0;
    }

    INLINE bool PropertyDescription::hasBase(void) const
    {
        return (flags_ & ENTITY_BASE_DATA_FLAGS) > 0;
    }

    INLINE bool PropertyDescription::hasClient(void) const
    {
        return (flags_ & ENTITY_CLIENT_DATA_FLAGS) > 0;
    }

}

#endif // KBENGINE_DEF_PROPERTY_H

