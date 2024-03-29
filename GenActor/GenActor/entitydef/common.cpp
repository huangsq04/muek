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
#include "common.h"
namespace KBEngine{

ENTITYFLAGMAP g_entityFlagMapping;
//-------------------------------------------------------------------------------------

std::string entityDataFlagsToString(uint32 flags)
{
	if(flags == ED_FLAG_CELL_PUBLIC)
		return "CELL_PUBLIC";

	if(flags == ED_FLAG_CELL_PRIVATE)
		return "CELL_PRIVATE";

	if(flags == ED_FLAG_ALL_CLIENTS)
		return "ALL_CLIENTS";

	if(flags == ED_FLAG_CELL_PUBLIC_AND_OWN)
		return "CELL_PUBLIC_AND_OWN";

	if(flags == ED_FLAG_OWN_CLIENT)
		return "OWN_CLIENT";

	if(flags == ED_FLAG_BASE_AND_CLIENT)
		return "BASE_AND_CLIENT";

	if(flags == ED_FLAG_BASE)
		return "BASE";

	if(flags == ED_FLAG_OTHER_CLIENTS)
		return "OTHER_CLIENTS";

	return "UNKOWN";
}


}
