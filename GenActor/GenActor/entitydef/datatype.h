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


#ifndef KBE_DATA_TYPE_H
#define KBE_DATA_TYPE_H


#include "common.h"	
#include "xml.h"	

namespace KBEngine{
	class ADataTypes;
    class DataType 
    {
        public:	
            DataType(DATATYPE_UID did = 0);
            virtual ~DataType();	

            static bool finalise();


            virtual bool initialize(XML* xml, TiXmlNode* node, ADataTypes *dataTypes);

            virtual const char* getName(void) const = 0;

            INLINE int32 id() const;

            INLINE void aliasName(std::string aliasName);
            INLINE const char* aliasName(void) const;

            virtual DATATYPE type() const{ return DATA_TYPE_UNKONWN; }

            virtual std::string ToJson() { return "'" + aliasName_ + "'"; };
            virtual const char* UE4Type() const { return ""; }

        protected:
            DATATYPE_UID id_;
            std::string aliasName_;
    };

    template <typename SPECIFY_TYPE>
        class IntType : public DataType
    {
        protected:
        public:	
            IntType(DATATYPE_UID did = 0);
            virtual ~IntType();	
            const char* getName(void) const{ return "INT";}
            const char* UE4Type() const { return "int"; }
            virtual DATATYPE type() const{ return DATA_TYPE_DIGIT; }
    };


    template <>
        inline const char* IntType<uint8>::getName(void) const
        {
            return "UINT8";
        }

    template <>
        inline const char* IntType<uint16>::getName(void) const
        {
            return "UINT16";
        }

    template <>
        inline const char* IntType<uint32>::getName(void) const
        {
            return "UINT32";
        }

    template <>
        inline const char* IntType<int8>::getName(void) const
        {
            return "INT8";
        }

    template <>
        inline const char* IntType<int16>::getName(void) const
        {
            return "INT16";
        }

    template <>
        inline const char* IntType<int32>::getName(void) const
        {
            return "INT32";
        }

    class UInt64Type : public DataType
    {
        protected:
        public:	
            UInt64Type(DATATYPE_UID did = 0);
            virtual ~UInt64Type();	
            const char* getName(void) const{ return "UINT64";}

            virtual DATATYPE type() const{ return DATA_TYPE_DIGIT; }
    };

    class UInt32Type : public DataType
    {
        protected:
        public:	
            UInt32Type(DATATYPE_UID did = 0);
            virtual ~UInt32Type();	
            const char* getName(void) const{ return "UINT32";}

            virtual DATATYPE type() const{ return DATA_TYPE_DIGIT; }
    };

    class Int64Type : public DataType
    {
        protected:
        public:	
            Int64Type(DATATYPE_UID did = 0);
            virtual ~Int64Type();	

            const char* getName(void) const{ return "INT64";}

            virtual DATATYPE type() const{ return DATA_TYPE_DIGIT; }
    };

    class FloatType : public DataType
    {
        protected:
        public:	
            FloatType(DATATYPE_UID did = 0);
            virtual ~FloatType();	

            const char* getName(void) const{ return "FLOAT";}

            virtual DATATYPE type() const{ return DATA_TYPE_DIGIT; }
    };

    class DoubleType : public DataType
    {
        protected:
        public:	
            DoubleType(DATATYPE_UID did = 0);
            virtual ~DoubleType();	

            const char* getName(void) const{ return "DOUBLE";}

            virtual DATATYPE type() const{ return DATA_TYPE_DIGIT; }
    };

    class VectorType : public DataType
    {
        public:	
            VectorType(uint32 elemCount, DATATYPE_UID did = 0);
            virtual ~VectorType();	
            const char* getName(void) const{ return name_.c_str();}

            virtual DATATYPE type() const{ return DATA_TYPE_VECTOR; }
            uint32 GetElemCount() { return  elemCount_;	};

        protected:
            std::string name_;
            uint32 elemCount_;
    };

    class StringType : public DataType
    {
        protected:
        public:	
            StringType(DATATYPE_UID did = 0);
            virtual ~StringType();	
            const char* getName(void) const{ return "STRING";}

            virtual DATATYPE type() const{ return DATA_TYPE_STRING; }
    };

    class UnicodeType : public DataType
    {
        protected:
        public:	
            UnicodeType(DATATYPE_UID did = 0);
            virtual ~UnicodeType();	
            const char* getName(void) const{ return "UNICODE";}

            virtual DATATYPE type() const{ return DATA_TYPE_UNICODE; }
    };

    class BlobType : public DataType
    {
        protected:
        public:	
            BlobType(DATATYPE_UID did = 0);
            virtual ~BlobType();	

            const char* getName(void) const{ return "BLOB";}

            virtual DATATYPE type() const{ return DATA_TYPE_BLOB; }
    };

    class MailboxType : public DataType
    {
        protected:
        public:	
            MailboxType(DATATYPE_UID did = 0);
            virtual ~MailboxType();	

            const char* getName(void) const{ return "MAILBOX";}

            virtual DATATYPE type() const{ return DATA_TYPE_MAILBOX; }
    };

    class FixedArrayType : public DataType
    {
        public:	
            FixedArrayType(DATATYPE_UID did = 0);
            virtual ~FixedArrayType();	

            DataType* getDataType(){ return dataType_; }

            bool initialize(XML* xml, TiXmlNode* node, ADataTypes *dataTypes);

            const char* getName(void) const{ return "ARRAY";}

            virtual DATATYPE type() const{ return DATA_TYPE_FIXEDARRAY; }

            virtual std::string ToJson() { return "[" + dataType_->ToJson() + "]"; };

        protected:
            DataType*			dataType_;		// 这个数组所处理的类别
    };

    class FixedDictType : public DataType
    {
        public:
            struct DictItemDataType
            {
                DataType* dataType;

                // 作为一个数据类别在alias中可对dict中的某个项指定是否持久化
                bool persistent;
            };

            typedef std::shared_ptr< DictItemDataType > DictItemDataTypePtr;
            typedef std::vector< std::pair< std::string, DictItemDataTypePtr > > FIXEDDICT_KEYTYPE_MAP;
        public:	
            FixedDictType(DATATYPE_UID did = 0);
            virtual ~FixedDictType();

            /** 
              获得这个固定字典的key类别 
              */	
            FIXEDDICT_KEYTYPE_MAP& getKeyTypes(void){ return keyTypes_; }

            const char* getName(void) const{ return "FIXED_DICT";}

            bool initialize(XML* xml, TiXmlNode* node, ADataTypes *dataTypes);

            /** 
              获得固定字典所有的key名称 
              */
            std::string getKeyNames(void);

            /** 
              获得debug信息，返回固定字典所有的key名称和类型
              */
            std::string debugInfos(void);

            /** 
              加载impl模块
              */
            bool loadImplModule(std::string moduleName);

            virtual DATATYPE type() const{ return DATA_TYPE_FIXEDDICT; }

            std::string& moduleName(){ return moduleName_; }

            virtual std::string ToJson() { 
                std::string retType = "{";
                for (FIXEDDICT_KEYTYPE_MAP::iterator itor = keyTypes_.begin(); itor != keyTypes_.end(); ++itor)
                {
                    retType = retType + "'" + itor->first + "':" + itor->second->dataType->ToJson() + ",";
                }
                retType = retType.substr(0, retType.size() - 1);
                return retType + "}";
            };

        protected:
            // 这个固定字典里的各个key的类型
            FIXEDDICT_KEYTYPE_MAP			keyTypes_;				

            std::string						moduleName_;		
    };


    template class IntType<uint8>;
    template class IntType<uint16>;
    template class IntType<int8>;
    template class IntType<int16>;
    template class IntType<int32>;

    INLINE int32 DataType::id() const
    {
        return id_;
    }

    INLINE void DataType::aliasName(std::string aliasName)
    {
        aliasName_ = aliasName;
    }

    INLINE const char* DataType::aliasName(void) const
    {
        return aliasName_.c_str();
    }

    template <typename SPECIFY_TYPE>
        IntType<SPECIFY_TYPE>::IntType(DATATYPE_UID did) :
            DataType(did)
    {
    }

    template <typename SPECIFY_TYPE>
        IntType<SPECIFY_TYPE>::~IntType()
        {
        }


}


#endif // KBE_DATA_TYPE_H

