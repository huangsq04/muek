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


#ifndef KBE_ENTITY_CELL_BASE_CLIENT__MAILBOX_H
#define KBE_ENTITY_CELL_BASE_CLIENT__MAILBOX_H

#include "common.h"
namespace KBEngine{

    class ScriptDefModule;

    class MethodDescription;

    class EntityMailbox
    {
        public:


            EntityMailbox(ScriptDefModule* pScriptModule,int32 componentID, 
                    int32 eid, ENTITY_MAILBOX_TYPE type);

            ~EntityMailbox();

            void c_str(char* s, size_t size);

            void reload();

            typedef std::vector<EntityMailbox*> MAILBOXS;
            static MAILBOXS mailboxs;

        private:


        protected:
            std::string								scriptModuleName_;

            // 该entity所使用的脚本模块对象
            ScriptDefModule*						pScriptModule_;	

            void _setATIdx(MAILBOXS::size_type idx) { 
                atIdx_ = idx; 
            }

            MAILBOXS::size_type	atIdx_;
    };

}
#endif // KBE_ENTITY_CELL_BASE_CLIENT__MAILBOX_H
