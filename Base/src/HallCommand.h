//****************************************************************************
//Copyright (C) 2005-2006 Beijing BlueDJ Technology Co.,Ltd. All rights reserved.

//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program (in the file LICENSE.GPL); if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

//Please visit http://www.bluedj.com for more infomation about us.
//Contact us at ggwizard@gmail.com or darkdong@gmail.com.
//****************************************************************************/

#ifndef HALLCOMMAND_H
#define HALLCOMMAND_H

enum HallCommand {
	HALL_COMMAND_SEND_REQUEST,
	HALL_COMMAND_GAME_QUITTED,
	HALL_COMMAND_REMOVE_ACL,
	HALL_COMMAND_USER_ONOFF_LINE,
	HALL_COMMAND_USER_CONTACT_TYPE,
	HALL_COMMAND_USER_ADD_CONTACT,
	HALL_COMMAND_USER_REMOVE_CONTACT,
	HALL_COMMAND_USER_LITERAL_CHAT,
	HALL_COMMAND_CHANGE_SERVER_USERS,
	HALL_COMMAND_RECONNECT,
	HALL_COMMAND_TREE_CONTAINER,
	HALL_COMMAND_TAB_CONTAINER,
	HALL_COMMAND_MAIN_WINDOW,
	HALL_COMMAND_CLOSE_MUTEX_HANDLE,
	HALL_COMMAND_SELF_LOGIN,
};
#endif
