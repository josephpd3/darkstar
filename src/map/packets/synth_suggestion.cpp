/*
===========================================================================

Copyright (c) 2010-2015 Darkstar Dev Teams

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

This file is part of DarkStar-server source code.

===========================================================================
*/
#include <unordered_map>

#include "../../common/socket.h"
#include "../map.h"

#include "synth_suggestion.h"


CSynthSuggestionPacket::CSynthSuggestionPacket(uint32 synthID)
{
	this->type = 0x31;
	this->size = 0x1A;

	uint16 itemAmount;
	std::unordered_map<uint16, int>::const_iterator columnAmountByteIter;
	int columnAmountByte;

	const char* fmtQuery =

		"SELECT KeyItem, Wood, Smith, Gold, Cloth, Leather, Bone, Alchemy, Cook, Crystal, \
			Result, Ingredient1, Ingredient2, Ingredient3, Ingredient4, Ingredient5, Ingredient6, Ingredient7, Ingredient8 \
		FROM synth_recipes \
		WHERE ID = %u \
		LIMIT 1";

	int32 ret = Sql_Query(
		SqlHandle,
		fmtQuery,
		synthID);

	static const std::unordered_map<uint16, int> ingredientColumnToAmountByteMap = {
		{11, 0x20},
		{12, 0x22},
		{13, 0x24},
		{14, 0x26},
		{15, 0x28},
		{16, 0x2A},
		{17, 0x2C},
		{18, 0x2E}
	};

	if (ret != SQL_ERROR &&
		Sql_NumRows(SqlHandle) != 0 &&
		Sql_NextRow(SqlHandle) == SQL_SUCCESS)
	{
		ref<uint16>(0x04) = Sql_GetUIntData(SqlHandle,10);
		//words 0x06, 0x08, 0x0A are subcraft number
		ref<uint16>(0x0C) = Sql_GetUIntData(SqlHandle,9);
		ref<uint16>(0x0E) = Sql_GetUIntData(SqlHandle,0);
		ref<uint16>(0x10) = Sql_GetUIntData(SqlHandle,11);
		ref<uint16>(0x12) = Sql_GetUIntData(SqlHandle,12);
		ref<uint16>(0x14) = Sql_GetUIntData(SqlHandle,13);
		ref<uint16>(0x16) = Sql_GetUIntData(SqlHandle,14);
		ref<uint16>(0x18) = Sql_GetUIntData(SqlHandle,15);
		ref<uint16>(0x1A) = Sql_GetUIntData(SqlHandle,16);
		ref<uint16>(0x1C) = Sql_GetUIntData(SqlHandle,17);
		ref<uint16>(0x1E) = Sql_GetUIntData(SqlHandle,18);
		// words 0x20 through 0x2E are the quantity per material
		for (uint16 col = 11; col < 19; col++)
		{
			itemAmount = 0;
			columnAmountByteIter = ingredientColumnToAmountByteMap.find(col);
			columnAmountByte = (*columnAmountByteIter).second;

			if (Sql_GetUIntData(SqlHandle, col) == 0)
				continue;
			else
				itemAmount++;

			uint16 ingredientID = Sql_GetUIntData(SqlHandle, (size_t)col);

			for (uint16 other_col = 11; other_col < 19; other_col++)
			{
				if (ingredientID == Sql_GetUIntData(SqlHandle, (size_t)other_col))
					// Skip this item if we've seen it in a lesser indexed column
					if (col > other_col)
						continue;
					itemAmount++;
			}
			ref<uint16>(columnAmountByte) = itemAmount;
		}
		//
		ref<uint16>(0x30) = 0x01;
	}
}
