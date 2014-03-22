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

static const char *gFanNames[]={
	QT_TRANSLATE_NOOP("MahjongFanName","Unknown"),
	QT_TRANSLATE_NOOP("MahjongFanName","Unknown"),
	QT_TRANSLATE_NOOP("MahjongFanName","Unknown"),
	QT_TRANSLATE_NOOP("MahjongFanName","Unknown"),
	QT_TRANSLATE_NOOP("MahjongFanName","Unknown"),
	QT_TRANSLATE_NOOP("MahjongFanName","Unknown"),
	QT_TRANSLATE_NOOP("MahjongFanName","Unknown"),
	QT_TRANSLATE_NOOP("MahjongFanName","Tian Hu"),
	QT_TRANSLATE_NOOP("MahjongFanName","Di Hu"),
	QT_TRANSLATE_NOOP("MahjongFanName","Ren Hu"),
    QT_TRANSLATE_NOOP("MahjongFanName","Da 4 Xi"),
    QT_TRANSLATE_NOOP("MahjongFanName","Da 3 Yuan"),
    QT_TRANSLATE_NOOP("MahjongFanName","Lv 1 Se"),
    QT_TRANSLATE_NOOP("MahjongFanName","Hong Kong Que"),
    QT_TRANSLATE_NOOP("MahjongFanName","9 Lian Bao Deng"),
    QT_TRANSLATE_NOOP("MahjongFanName","4 Gang"),
    QT_TRANSLATE_NOOP("MahjongFanName","Lian 7 Dui"),
    QT_TRANSLATE_NOOP("MahjongFanName","13 1"),
    QT_TRANSLATE_NOOP("MahjongFanName","Qing 1 9"),
    QT_TRANSLATE_NOOP("MahjongFanName","Xiao 4 Xi"),
    QT_TRANSLATE_NOOP("MahjongFanName","Xiao 3 Yuan"),
    QT_TRANSLATE_NOOP("MahjongFanName","Zi 1 Se"),
    QT_TRANSLATE_NOOP("MahjongFanName","4 An Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","1 Se 2 Long Hui"),
    QT_TRANSLATE_NOOP("MahjongFanName","1 Se 4 Tong Shun"),
    QT_TRANSLATE_NOOP("MahjongFanName","1 Se 4 Jie Gao"),
    QT_TRANSLATE_NOOP("MahjongFanName","1 Se 4 Bu Gao"),
    QT_TRANSLATE_NOOP("MahjongFanName","3 Gang"),
    QT_TRANSLATE_NOOP("MahjongFanName","Hun 1 9"),
    QT_TRANSLATE_NOOP("MahjongFanName","7 Dui"),
    QT_TRANSLATE_NOOP("MahjongFanName","7 Xing Bu Kao"),
    QT_TRANSLATE_NOOP("MahjongFanName","Quan 2 Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","Qing 1 Se"),
    QT_TRANSLATE_NOOP("MahjongFanName","1 Se 3 Tong Shun"),
    QT_TRANSLATE_NOOP("MahjongFanName","1 Se 3 Jie Gao"),
    QT_TRANSLATE_NOOP("MahjongFanName","Quan Da"),
    QT_TRANSLATE_NOOP("MahjongFanName","Quan Zhong"),
    QT_TRANSLATE_NOOP("MahjongFanName","Quan Xiao"),
    QT_TRANSLATE_NOOP("MahjongFanName","Qing Long"),
    QT_TRANSLATE_NOOP("MahjongFanName","3 Se 2 Long Hui"),
    QT_TRANSLATE_NOOP("MahjongFanName","1 Se 3 Bu Gao"),
    QT_TRANSLATE_NOOP("MahjongFanName","Quan Dai 5"),
    QT_TRANSLATE_NOOP("MahjongFanName","3 Tong Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","3 An Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","Quan Bu Kao"),
    QT_TRANSLATE_NOOP("MahjongFanName","Zu He Long"),
    QT_TRANSLATE_NOOP("MahjongFanName"," > 5"),
    QT_TRANSLATE_NOOP("MahjongFanName"," < 5"),
    QT_TRANSLATE_NOOP("MahjongFanName","3 Feng Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","Hua Long"),
    QT_TRANSLATE_NOOP("MahjongFanName","Tui Bu Dao"),
    QT_TRANSLATE_NOOP("MahjongFanName","3 Se 3 Tong Shun"),
    QT_TRANSLATE_NOOP("MahjongFanName","3 Se 3 Jie Gao"),
    QT_TRANSLATE_NOOP("MahjongFanName","Wu Fan Hu"),
    QT_TRANSLATE_NOOP("MahjongFanName","Miao Shou Hui Chun"),
    QT_TRANSLATE_NOOP("MahjongFanName","Hai Di Lao Yue"),
    QT_TRANSLATE_NOOP("MahjongFanName","Gang Shang Kai Hua"),
    QT_TRANSLATE_NOOP("MahjongFanName","Qiang Gang Hu"),
    QT_TRANSLATE_NOOP("MahjongFanName","Peng Peng Hu"),
    QT_TRANSLATE_NOOP("MahjongFanName","Hun 1 Se"),
    QT_TRANSLATE_NOOP("MahjongFanName","3 Se 3 Bu Gao"),
    QT_TRANSLATE_NOOP("MahjongFanName","5 Men Qi"),
    QT_TRANSLATE_NOOP("MahjongFanName","Quan Qiu Ren"),
    QT_TRANSLATE_NOOP("MahjongFanName","2 An Gang"),
    QT_TRANSLATE_NOOP("MahjongFanName","2 Jian Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","Quan Dai 1"),
    QT_TRANSLATE_NOOP("MahjongFanName","Bu Qiu Ren"),
    QT_TRANSLATE_NOOP("MahjongFanName","2 Ming Gang"),
    QT_TRANSLATE_NOOP("MahjongFanName","Hu Jue Zhang"),
    QT_TRANSLATE_NOOP("MahjongFanName","Jian Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","Quan Feng Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","Men Feng Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","Men Qian Qing"),
    QT_TRANSLATE_NOOP("MahjongFanName","Ping Hu"),
    QT_TRANSLATE_NOOP("MahjongFanName","4 Gui 1"),
    QT_TRANSLATE_NOOP("MahjongFanName","2 Tong Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","2 An Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","An Gang"),
    QT_TRANSLATE_NOOP("MahjongFanName","Duan 1 9"),
    QT_TRANSLATE_NOOP("MahjongFanName","YI Ban Gao"),
    QT_TRANSLATE_NOOP("MahjongFanName","Xi Xiang Feng"),
    QT_TRANSLATE_NOOP("MahjongFanName","Lian 6"),
    QT_TRANSLATE_NOOP("MahjongFanName","Lao Shao Fu"),
    QT_TRANSLATE_NOOP("MahjongFanName","1 9 Ke"),
    QT_TRANSLATE_NOOP("MahjongFanName","Ming Gang"),
    QT_TRANSLATE_NOOP("MahjongFanName","Que 1 Men"),
    QT_TRANSLATE_NOOP("MahjongFanName","Wu Zi"),
    QT_TRANSLATE_NOOP("MahjongFanName","Bian Zhang"),
    QT_TRANSLATE_NOOP("MahjongFanName","Kan Zhang"),
    QT_TRANSLATE_NOOP("MahjongFanName","Dan Diao"),
    QT_TRANSLATE_NOOP("MahjongFanName","Zi Mo"),
    QT_TRANSLATE_NOOP("MahjongFanName","Hua Pai")
};
