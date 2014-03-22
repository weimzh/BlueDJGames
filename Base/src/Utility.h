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

#ifndef UTILITY_H
#define UTILITY_H

#include <QtGui>
#include <QtSvg>
#include "DJBaseGlobal.h"
#include "protocol.h"

#define	MAX_SUPPORTED_LOCALE	2

struct LocaleInfo {
	QString languageName;
	QString	localeName;
};

DJ_BASE_EXPORT QByteArray MakeGeneralData( quint8 type, quint8 subType, quint8 language, const QByteArray& data = QByteArray()  );

DJ_BASE_EXPORT	inline quint8 hibyte( quint16 word )
{
	return word >> 8;
}
DJ_BASE_EXPORT	inline quint8 lobyte( quint16 word )
{
	return word;
}
DJ_BASE_EXPORT	inline quint16 makeint16( quint8 hi8, quint8 lo8 )
{
	return hi8 << 8 | lo8;
}
DJ_BASE_EXPORT	inline quint32 makeint32( quint16 hi16, quint16 lo16 )
{
	return hi16 << 16 | lo16;
}

DJ_BASE_EXPORT	quint32 letoh4( const QByteArray& buf );
DJ_BASE_EXPORT	quint16 letoh2( const QByteArray& buf );

DJ_BASE_EXPORT	inline quint32 letoh4( const void* buf )
{ 
	return letoh4( QByteArray::fromRawData( reinterpret_cast<const char *>(buf), 4 ) ); 
}
DJ_BASE_EXPORT	inline quint32 letoh4( quint32 leInt32 )
{
	return letoh4( &leInt32 );
}
DJ_BASE_EXPORT	inline quint16 letoh2( const void* buf )
{
	return letoh2( QByteArray::fromRawData( reinterpret_cast<const char *>(buf), 2 ) );
}
DJ_BASE_EXPORT	inline quint16 letoh2( quint16 leInt16 )
{
	return letoh2( &leInt16 );
}

DJ_BASE_EXPORT QByteArray htoa4( quint32 h, QDataStream::ByteOrder bo );
DJ_BASE_EXPORT QByteArray htoa2( quint16 h, QDataStream::ByteOrder bo );

DJ_BASE_EXPORT	inline void setle4( void* buf, quint32 value )
{
	memcpy( buf, htoa4( value, QDataStream::LittleEndian ).data(), 4 );
}
DJ_BASE_EXPORT	inline void setle2( void* buf, quint16 value )
{
	memcpy( buf, htoa2( value, QDataStream::LittleEndian ).data(), 2 );
}

DJ_BASE_EXPORT void PosByAlign( QPoint& pos, Qt::Alignment align, const QSize& size, qreal angle = 0 );
DJ_BASE_EXPORT void ScalePixmap( QPixmap& pix, qreal scale );
DJ_BASE_EXPORT QPixmap CreateConjointPixmap( const QList<QPixmap>& pixmaps, const QSize& unitSize = QSize() );
DJ_BASE_EXPORT QPixmap SvgRender2Pixmap( QSvgRenderer *renderer );
DJ_BASE_EXPORT bool 	GetSettingSound();
DJ_BASE_EXPORT QString GetSettingLocaleName();
DJ_BASE_EXPORT QString FindSupportedLocaleName();
DJ_BASE_EXPORT QString MostAppropriateSupportedLocaleName( const QString& localeName );

DJ_BASE_EXPORT quint8	GetSettingLanguageCode();
DJ_BASE_EXPORT quint8	LocaleNameToLanguageCode( const QString& localeName );
DJ_BASE_EXPORT uint GetSettingMaxImageSize();
DJ_BASE_EXPORT void GetSettingMinMaxSentences( int & min, int & max );

DJ_BASE_EXPORT QByteArray	crypt( const QByteArray& buf );

DJ_BASE_EXPORT bool GetSettingUseProxy();
DJ_BASE_EXPORT int GetSettingProxyType();
DJ_BASE_EXPORT QVariant GetSettingProxy( int proxyType );
DJ_BASE_EXPORT void GetSettingProxyAuth( QString& username, QString& password );
DJ_BASE_EXPORT void DebugBuffer( const QByteArray& buf, int length = -1 );
DJ_BASE_EXPORT void DebugBuffer( const char* buf, int length );
DJ_BASE_EXPORT bool InterpretText( QString& text );
DJ_BASE_EXPORT bool RemoveHtmlTag( QString& text );
DJ_BASE_EXPORT bool RemoveMemberPrefix( QString& text );
DJ_BASE_EXPORT void PaintBackground( QWidget* widget, const QPoint& start, const QPoint& final);
DJ_BASE_EXPORT void GetSettingGameSetup( const QString& gameName, quint8& speed, bool& refuseSameIp, quint8& breakRate, qint32& minScore, qint32& maxScore, quint16& maxUnwelcome );
DJ_BASE_EXPORT void SetSettingGameSetup( const QString& gameName, quint8 speed, bool refuseSameIp, quint8 breakRate, qint32 minScore, qint32 maxScore, quint16 maxUnwelcome );
DJ_BASE_EXPORT QBrush GetSettingGameDesktopBrush( const QString& gameName );
DJ_BASE_EXPORT void SetSettingGameDesktopBrush( const QString& gameName, const QBrush& brush );
DJ_BASE_EXPORT void ConvertSeconds2HMS( int totalSeconds, int& hours, int& minutes, int& seconds );
#endif
