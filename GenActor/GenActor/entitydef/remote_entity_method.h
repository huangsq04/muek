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


#ifndef KBENGINE_REMOTE_ENTITY_METHOD_H
#define KBENGINE_REMOTE_ENTITY_METHOD_H

#include "common.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif
#include "datatype.h"
#include "datatypes.h"



namespace KBEngine{

    class MethodDescription;

    class RemoteEntityMethod
    {

        public:	

            virtual ~RemoteEntityMethod();

            const char* getName(void) const;

            MethodDescription* getDescription(void) const
            { 
                return methodDescription_; 
            }



        protected:	
            MethodDescription*		methodDescription_;					// 这个方法的描述
    };
}

#endif // KBENGINE_REMOTE_ENTITY_METHOD_H
