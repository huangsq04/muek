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
#include "datatypes.h"
#include<algorithm>
#include <sstream>
namespace KBEngine{

    ADataTypes::ADataTypes()
    {
    }
    ADataTypes::~ADataTypes()
    {
        finalise();
    }
    void ADataTypes::finalise(void)
    {
        uid_dataTypes_.clear();
        dataTypesLowerName_.clear();
        dataTypes_.clear();
    }

    bool ADataTypes::initialize(std::string file)
    {
        // 初始化一些基础类别
        addDataType("UINT8",	new IntType<uint8>);
        addDataType("UINT16",	new IntType<uint16>);
        addDataType("UINT64",	new UInt64Type);
        addDataType("UINT32",	new UInt32Type);

        addDataType("INT8",		new IntType<int8>);
        addDataType("INT16",	new IntType<int16>);
        addDataType("INT32",	new IntType<int32>);
        addDataType("INT64",	new Int64Type);

        addDataType("STRING",	new StringType);
        addDataType("UNICODE",	new UnicodeType);
        addDataType("FLOAT",	new FloatType);
        addDataType("DOUBLE",	new DoubleType);
        addDataType("PYTHON",	new UnicodeType);
        addDataType("PY_DICT",	new UnicodeType);
        addDataType("PY_TUPLE",	new UnicodeType);
        addDataType("PY_LIST",	new UnicodeType);
        addDataType("MAILBOX",	new MailboxType);
        addDataType("BLOB",		new BlobType);

        addDataType("VECTOR2",	new VectorType(2));
        addDataType("VECTOR3",	new VectorType(3));
        addDataType("VECTOR4",	new VectorType(4));
        return loadAlias(file);
    }

    bool ADataTypes::loadAlias(std::string& file)
    {
        TiXmlNode* node = NULL;
        XML *xml = new XML(file.c_str());

        if(xml == NULL || !xml->isGood())
            return false;

        node = xml->getRootNode();

        if(node == NULL)
        {
            return true;
        }

        XML_FOR_BEGIN(node)
        {
            std::string type = "";
            std::string aliasName = xml->getKey(node);
            TiXmlNode* childNode = node->FirstChild();

            if(childNode != NULL)
            {
                type = xml->getValStr(childNode);
                if(type == "FIXED_DICT")
                {
                    FixedDictType* fixedDict = new FixedDictType;

                    if(fixedDict->initialize(xml, childNode, this))
                    {
                        addDataType(aliasName, fixedDict);
                    }
                    else
                    {		
                        delete fixedDict;
                        return false;
                    }
                }
                else if(type == "ARRAY")
                {
                    FixedArrayType* fixedArray = new FixedArrayType;

                    if(fixedArray->initialize(xml, childNode,this))
                    {
                        addDataType(aliasName, fixedArray);
                    }
                    else
                    {				
                        delete fixedArray;
                        return false;
                    }
                }
                else
                {
                    DataType* dataType = getDataType(type);
                    if(dataType == NULL)
                    {			
                        return false;
                    }

                    addDataType(aliasName, dataType);
                }
            }
        }
        XML_FOR_END(node);
        delete xml;
        return true;
    }

    bool ADataTypes::addDataType(std::string name, DataType* dataType)
    {
        dataType->aliasName(name);
        std::string lowername = name;
        std::transform(lowername.begin(), lowername.end(), lowername.begin(), tolower);	

        DATATYPE_MAP::iterator iter = dataTypesLowerName_.find(lowername);
        if (iter != dataTypesLowerName_.end())
        { 
            return false;
        }

        dataTypes_[name] = dataType;
        dataTypesLowerName_[lowername] = dataType;
        uid_dataTypes_[dataType->id()] = dataType;

        return true;
    }

    bool ADataTypes::addDataType(DATATYPE_UID uid, DataType* dataType)
    {
        UID_DATATYPE_MAP::iterator iter = uid_dataTypes_.find(uid);
        if (iter != uid_dataTypes_.end())
        {
            return false;
        }
        uid_dataTypes_[uid] = dataType;
        return true;
    }

    void ADataTypes::delDataType(std::string name)
    {
        DATATYPE_MAP::iterator iter = dataTypes_.find(name);
        if (iter == dataTypes_.end())
        {
        }
        else
        {
            uid_dataTypes_.erase(iter->second->id());
            dataTypes_.erase(iter);
            dataTypesLowerName_.erase(iter);
        }
    }

    DataType* ADataTypes::getDataType(std::string name)
    {
        DATATYPE_MAP::iterator iter = dataTypes_.find(name);
        if (iter != dataTypes_.end()) 
            return iter->second;

        return NULL;
    }
    DataType* ADataTypes::getDataType(const char* name)
    {
        DATATYPE_MAP::iterator iter = dataTypes_.find(name);
        if (iter != dataTypes_.end()) 
            return iter->second;

        return NULL;
    }
    DataType* ADataTypes::getDataType(DATATYPE_UID uid)
    {
        UID_DATATYPE_MAP::iterator iter = uid_dataTypes_.find(uid);
        if (iter != uid_dataTypes_.end()) 
            return iter->second;
        return NULL;
    }
	std::string  ADataTypes::GetDataBaseTypeName(DataType* dataType)
	{
		if (dataType->type() == DATA_TYPE_DIGIT)
		{
			std::string type = "undefine";
			if (dataType->getName() == std::string("FLOAT"))
			{
				return "Float";
			}
			else if (dataType->getName() == std::string("DOUBLE"))
			{
				return "Double";
			}
			else if (dataType->getName() == std::string("INT32"))
			{
				return "Int32";
			}
			else if (dataType->getName() == std::string("UINT32"))
			{
				return "Uint32";
			}
			else if (dataType->getName() == std::string("INT8"))
			{
				return "Int8";
			}
			else if (dataType->getName() == std::string("UINT8"))
			{
				return "Uint8";
			}
			else if (dataType->getName() == std::string("INT16"))
			{
				return "Int16";
			}
			else if (dataType->getName() == std::string("UINT16"))
			{
				return "Uint16";
			}
			else if (dataType->getName() == std::string("INT64"))
			{
				return "Int64";
			}
			else if (dataType->getName() == std::string("UINT64"))
			{
				return "Uint64";
			}
			return type;
		}
		else if (dataType->type() == DATA_TYPE_STRING)
		{
			return "String";
		}
		else if (dataType->type() == DATA_TYPE_BLOB ||
			dataType->type() == DATA_TYPE_UNICODE )
		{
			return "Blob";
		}
		else if (dataType->type() == DATA_TYPE_VECTOR)
		{
			KBEngine::VectorType* vdatatype = (KBEngine::VectorType*)(dataType);
			if (vdatatype->GetElemCount() == 2)
			{
				return "FVector2D";
			}
			if (vdatatype->GetElemCount() == 3)
			{
				return "FVector";
			}
			if (vdatatype->GetElemCount() == 4)
			{
				return "FVector4";
			}
		}
		return dataType->getName();
	}
	bool ADataTypes::IsUe4Type(DataType* dataType)
	{
		if (dataType->type() == DATA_TYPE_PYTHON
			|| dataType->type() == DATA_TYPE_MAILBOX
			|| dataType->type() == DATA_TYPE_PYDICT
			|| dataType->type() == DATA_TYPE_PYTUPLE
			|| dataType->type() == DATA_TYPE_PYLIST
			)
		{
			return false;
		}
		return true;
	}
	bool ADataTypes::IsBluePrintType(DataType* dataType)
	{
		if (dataType->type() == DATA_TYPE_DIGIT)
		{
			if (dataType->getName() == std::string("INT8"))
			{
				return false;
			}
			else if (dataType->getName() == std::string("UINT8"))
			{
				return true;
			}
			else if (dataType->getName() == std::string("INT16"))
			{
				return false;
			}
			else if (dataType->getName() == std::string("UINT16"))
			{
				return false;
			}
			else if (dataType->getName() == std::string("INT32"))
			{
				return true;
			}
			else if (dataType->getName() == std::string("UINT32"))
			{
				return false;
			}
			else if (dataType->getName() == std::string("INT64"))
			{
				return false;
			}
			else if (dataType->getName() == std::string("UINT64"))
			{
				return false;
			}
			else if (dataType->getName() == std::string("FLOAT"))
			{
				return true;
			}
			else if (dataType->getName() == std::string("DOUBLE"))
			{
				return false;
			}
			return false;
		}
		else if (dataType->type() == DATA_TYPE_STRING ||
			dataType->type() == DATA_TYPE_BLOB ||
			dataType->type() == DATA_TYPE_UNICODE )
		{
			return true;
		}
		else if (dataType->type() == DATA_TYPE_VECTOR)
		{
			return true;
		}
		return false;
	}
    std::string  ADataTypes::GetDataTypeUE4Type(DataType* dataType)
    {
        if (dataType->type() == DATA_TYPE_DIGIT)
        {
            std::string type = "undefine";
			if (dataType->getName() == std::string("INT8"))
			{
				return "int8";
			}
			else if (dataType->getName() == std::string("UINT8"))
			{
				return "uint8";
			}
			else if (dataType->getName() == std::string("INT16"))
			{
				return "int16";
			}
			else if (dataType->getName() == std::string("UINT16"))
			{
				return "uint16";
			}
			else if (dataType->getName() == std::string("INT32"))
			{
				return "int32";
			}
			else if (dataType->getName() == std::string("UINT32"))
			{
				return "uint32";
			}
			else if (dataType->getName() == std::string("INT64"))
			{
				return "int64";
			}
			else if (dataType->getName() == std::string("UINT64"))
			{
				return "uint64";
			}
            else if (dataType->getName() == std::string("FLOAT"))
            {
                return "float";
            }
			else if (dataType->getName() == std::string("DOUBLE"))
			{
				return "double";
			}
            return type;
        }
        else if (dataType->type() == DATA_TYPE_STRING )
		{
			return "FString";
		}
		else if (dataType->type() == DATA_TYPE_BLOB ||
                dataType->type() == DATA_TYPE_UNICODE)
        {
            return "TArray<uint8>";
        }
        else if (dataType->type() == DATA_TYPE_VECTOR)
        {
            KBEngine::VectorType* vdatatype = (KBEngine::VectorType*)(dataType);
            if (vdatatype->GetElemCount() == 2)
            {
                return "FVector2D";
            }
            if (vdatatype->GetElemCount() == 3)
            {
                return "FVector";
            }
            if (vdatatype->GetElemCount() == 4)
            {
                return "FVector4";
            }
        }
        return dataType->getName();
    }

	bool ADataTypes::GenDefineList(DataType* dataType, std::vector<std::string> &dlist)
	{
		KBEngine::FixedDictType* fdatatype = (KBEngine::FixedDictType*)(dataType);

		FixedDictType::FIXEDDICT_KEYTYPE_MAP& keyTypes = fdatatype->getKeyTypes();
		FixedDictType::FIXEDDICT_KEYTYPE_MAP::iterator itor = keyTypes.begin();
		for (; itor != keyTypes.end(); ++itor)
		{
			if (itor->second->dataType->type() == DATA_TYPE_FIXEDARRAY)
			{
				KBEngine::FixedArrayType* arraytype = (KBEngine::FixedArrayType*)(itor->second->dataType);
				if (arraytype->getDataType()->type() == DATA_TYPE_FIXEDDICT)
				{
					std::string typeName = arraytype->getDataType()->aliasName();
					GenDefineList(arraytype->getDataType(), dlist);
					if (std::find(dlist.begin(), dlist.end(), typeName) == dlist.end())
						dlist.push_back(typeName);

				}
				else
				{
					std::string typeName = ADataTypes::GetDataTypeUE4Type(arraytype->getDataType());
					//printf("%s\n", typeName.c_str());
				}
			}
			else if (itor->second->dataType->type() == DATA_TYPE_FIXEDDICT)
			{
				std::string typeName = itor->second->dataType->aliasName();
				GenDefineList(itor->second->dataType, dlist);
				if (std::find(dlist.begin(), dlist.end(), typeName) == dlist.end())
					dlist.push_back(typeName);
			}
			else
			{
				//printf("%s:%s\n", ADataTypes::GetDataTypeUE4Type(itor->second->dataType).c_str(), itor->first.c_str());
			}
				
		}

		return true;
	}
	std::string  ADataTypes::DataTypeToUE4Struct()
	{
		std::vector<std::string> blist;
		DATATYPE_MAP::iterator it = dataTypes_.begin();
		for (; it != dataTypes_.end(); ++it)
		{
			//if (std::string("HIGH_BUY_BACK_DATA") == it->second->aliasName() 
			//	 || std::string("CHAT_DATA") == it->second->aliasName() 
			//	|| std::string("CHAT_DATA") == it->second->aliasName()
			//	)
			{
				if (it->second->type() == DATA_TYPE_FIXEDDICT)
				{
					GenDefineList(it->second, blist);
					if (std::find(blist.begin(), blist.end(), it->second->aliasName()) == blist.end())
						blist.push_back(it->second->aliasName());
				}
			}
		}

		std::stringstream ue4struct;
		ue4struct << "#pragma once\n";
		ue4struct << "#include \"Entity.h\"\n";
		ue4struct << "#include \"Alias.generated.h\"\n";
		//ue4struct << "#define MAILBOX int32\n";


		for (int i = 0; i < blist.size(); i++)			
		{
			DATATYPE_MAP::iterator iter = dataTypes_.find(blist[i]);
			if (iter == dataTypes_.end())
				continue;
			if (iter->second->type() == DATA_TYPE_FIXEDDICT)
			{
				//std::string th = iter->second->aliasName();
				//if ("SPAWN_POINT_DICT" != th)
				//	continue;
				std::stringstream aliasfixeddict;
				std::string createFromStream = "\npublic: \n    void CreateFromStream(FMemoryStream* Stream){\n";
				std::string addToStream = "\n    void AddToStream(FBundle *Stream) const{\n";

				aliasfixeddict << "USTRUCT(BlueprintType)\nstruct F" +
					std::string(iter->second->aliasName()) + "{\n    GENERATED_USTRUCT_BODY()\n";
				KBEngine::FixedDictType* fdatatype = (KBEngine::FixedDictType*)(iter->second);

				FixedDictType::FIXEDDICT_KEYTYPE_MAP& keyTypes = fdatatype->getKeyTypes();
				FixedDictType::FIXEDDICT_KEYTYPE_MAP::iterator itor = keyTypes.begin();
				int OnelyKBE = 0;
				for (; itor != keyTypes.end(); ++itor)
				{
					if (itor->second->dataType->type() == DATA_TYPE_FIXEDARRAY)
					{
						KBEngine::FixedArrayType* arraytype = (KBEngine::FixedArrayType*)(itor->second->dataType);
						if (arraytype->getDataType()->type() == DATA_TYPE_FIXEDDICT)
						{
							std::string typeName = arraytype->getDataType()->aliasName();
							aliasfixeddict << "\n    UPROPERTY(EditAnywhere, BlueprintReadWrite)\n    TArray<F" <<
								typeName + "> " << itor->first + ";";

							createFromStream = createFromStream + \
								"        " + itor->first + ".Empty();\n"
								"        int Array" + itor->first + "Num = Stream->ReadUint32();\n" +
								"        while (Array" + itor->first + "Num > 0){\n" + 
								"            Array" + itor->first + "Num--;\n" +
								"            F" + typeName + " Value;\n" + 
								"            Value.CreateFromStream(Stream);\n" + 
								"            " + itor->first + ".Add(Value);\n" + 
								"        }\n";

							addToStream = addToStream + \
								"        Stream->WriteUint32(" + itor->first + ".Num());\n" + \
								"        for (int32 i = 0; i < " + itor->first + ".Num(); i++) {\n" + \
								"            " + itor->first + "[i].AddToStream(Stream);\n" + \
								"        }\n";
						}
						else
						{
							std::string typeName = ADataTypes::GetDataTypeUE4Type(arraytype->getDataType());
							aliasfixeddict << "\n    UPROPERTY(EditAnywhere, BlueprintReadWrite)\n    TArray<" <<
								typeName + "> " << itor->first << ";";

							createFromStream = createFromStream + \
								"        " + itor->first + ".Empty();\n"
								"        int Array" + itor->first + "Num = Stream->ReadUint32();\n" +
								"        while (Array" + itor->first + "Num > 0){\n" +
								"            Array" + itor->first + "Num--;\n" +
								"            " + typeName + " value = Stream->Read" + GetDataBaseTypeName(arraytype->getDataType())+ "();\n" +
								"            " + itor->first + ".Add(value);\n" +
								"        }\n";

							addToStream = addToStream + \
								"        Stream->WriteUint32(" + itor->first + ".Num());\n" + \
								"        for (int32 i = 0; i < " + itor->first + ".Num(); i++) {\n" + \
								"            Stream->Write" + GetDataBaseTypeName(arraytype->getDataType()) + "(" + itor->first + "[i]);\n" + \
								"        }\n";

						}

					}
					else if (itor->second->dataType->type() == DATA_TYPE_FIXEDDICT)
					{
						std::string typeName = itor->second->dataType->aliasName();
						aliasfixeddict << "\n    UPROPERTY(EditAnywhere, BlueprintReadWrite)";
						aliasfixeddict << "\n    F" << typeName << " " + itor->first << ";";

						createFromStream = createFromStream + "        " + itor->first + ".CreateFromStream(Stream);\n";
						addToStream = addToStream + "        " + itor->first + ".AddToStream(Stream);\n";

					}
					else
					{
						if(!ADataTypes::IsUe4Type(itor->second->dataType))
						{
							OnelyKBE = 1;
							continue;
						}
						if (ADataTypes::IsBluePrintType(itor->second->dataType))
						{
							aliasfixeddict << "\n    UPROPERTY(EditAnywhere, BlueprintReadWrite)";

						}
						aliasfixeddict << "\n    " << ADataTypes::GetDataTypeUE4Type(itor->second->dataType) << " " + itor->first << ";";

						createFromStream = createFromStream + "        " + itor->first + \
							"= Stream->Read" + GetDataBaseTypeName(itor->second->dataType) + "();\n";

						addToStream = addToStream + \
						"        Stream->Write" + \
						GetDataBaseTypeName(itor->second->dataType) + "(" + itor->first + ");\n";

					}

				}
				if (OnelyKBE == 0)
				{
					ue4struct << aliasfixeddict.str();
					ue4struct << createFromStream << "    }\n" << addToStream << "    }\n";
					ue4struct << " \n}; \n";
				}
			}
		}
		return ue4struct.str();
	}
}
