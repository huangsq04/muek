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


#ifndef KBE_DATA_TYPES_H
#define KBE_DATA_TYPES_H

#include "common.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif
#include "datatype.h"
#include "xml.h"	

namespace KBEngine{
    typedef DataType * DataTypePtr;

	class ADataTypes
	{
	public:
		typedef std::map<std::string, DataTypePtr> DATATYPE_MAP;
		typedef std::map<DATATYPE_UID, DataType*> UID_DATATYPE_MAP;

		ADataTypes();
		virtual ~ADataTypes();

		bool initialize(std::string file);
		void finalise(void);

		bool addDataType(std::string name, DataType* dataType);
		bool addDataType(DATATYPE_UID uid, DataType* dataType);
		void delDataType(std::string name);

		DataType* getDataType(std::string name);
		DataType* getDataType(const char* name);
		DataType* getDataType(DATATYPE_UID uid);

		bool loadAlias(std::string& file);

		const DATATYPE_MAP& dataTypes() { return dataTypes_; }
		const UID_DATATYPE_MAP& uid_dataTypes() { return uid_dataTypes_; }
		std::string  DataTypeToUE4Struct();
		static std::string  GetDataTypeUE4Type(DataType* dataType);
		static std::string  GetDataBaseTypeName(DataType* dataType);

		static bool IsBluePrintType(DataType* dataType);
		static bool IsUe4Type(DataType* dataType);

		static bool GenDefineList(DataType* dataType, std::vector<std::string> &dlist);
	protected:

		DATATYPE_MAP dataTypes_;
		DATATYPE_MAP dataTypesLowerName_;
		UID_DATATYPE_MAP uid_dataTypes_;

	};

}
#endif // KBE_DATA_TYPES_H
