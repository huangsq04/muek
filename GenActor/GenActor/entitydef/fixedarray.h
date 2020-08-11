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


#ifndef _FIXED_ARRAY_TYPE_H
#define _FIXED_ARRAY_TYPE_H
#include <string>
#include "datatype.h"


namespace KBEngine{

    class FixedArray 
    {		
        public:	
            FixedArray(DataType* dataType);
            FixedArray(DataType* dataType, std::string& strInitData);
            virtual ~FixedArray();

            const DataType* getDataType(void){ return _dataType; }

            /** 
              初始化固定数组
              */
            void initialize(std::string strInitData);

        protected:
            FixedArrayType* _dataType;
    } ;

}
#endif
