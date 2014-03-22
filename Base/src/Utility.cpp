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

#include <math.h>
#include "Utility.h"

LocaleInfo supportedLocaleInfos[MAX_SUPPORTED_LOCALE]	= {
	{"English","en"},
	{ QString::fromUtf8("\xe7\xae\x80\xe4\xbd\x93\xe4\xb8\xad\xe6\x96\x87"),"zh_CN"},
	//{"TraditionalChinese","zh_TW"},
	//{"Korean","ko"}
};

QByteArray MakeGeneralData( quint8 type, quint8 subType, quint8 language, const QByteArray& data  )
{
	QByteArray	buf( sizeof(DJGameProtocol) + data.size(), 0 );
	DJGameProtocol *protocol	= reinterpret_cast<DJGameProtocol *>(buf.data());
	protocol->chTotalLen	= buf.size();
	protocol->chType	= type;
	protocol->chSubType	= subType;
	protocol->chLanguage= language;
	memcpy( protocol->chBuf, data.data(), data.size() );
	return buf;
}
quint32 letoh4( const QByteArray& buf )
{
	quint32	h;
	QDataStream	s( buf );
	s.setByteOrder( QDataStream::LittleEndian );
	s >> h;
	return h;
}
quint16 letoh2( const QByteArray& buf )
{
	quint16	h;
	QDataStream	s( buf );
	s.setByteOrder( QDataStream::LittleEndian );
	s >> h;
	return h;
}
QByteArray htoa4( quint32 h, QDataStream::ByteOrder order )
{
	QByteArray buf( 4, 0 );
	QDataStream	s( &buf, QIODevice::ReadWrite );
	s.setByteOrder( order );
	s << h;
	return buf;
}
QByteArray htoa2( quint16 h, QDataStream::ByteOrder order )
{
	QByteArray buf( 2, 0 );
	QDataStream	s( &buf, QIODevice::ReadWrite );
	s.setByteOrder( order );
	s << h;
	return buf;
}
void PosByAlign( QPoint& pos, Qt::Alignment align, const QSize& size, qreal angle )
{
	int w	= size.width();
	int h	= size.height();
	
	if ( 0 != angle ) {
		qreal pi = 3.14;
		qreal a	= pi/180 * angle;
	
		int w1	= static_cast<int>(size.width() * cos(a));
		int w2	= static_cast<int>(size.height() * sin(a));
		if ( w1 < 0 )
			w1	= -w1;
		if ( w2 < 0 )
			w2	= -w2;
		w	= w1 + w2;
		
		int h1	= static_cast<int>(size.width() * sin(a));
		int h2	= static_cast<int>(size.height() * cos(a));
		if ( h1 < 0 )
			h1	= -h1;
		if ( h2 < 0 )
			h2	= -h2;
		h	= h1 + h2;
	}
	
	if( align & Qt::AlignRight ) {
		pos.rx() -= w;
	}else if( align & Qt::AlignHCenter ) {
		pos.rx() -= w >> 1;
	}
	if( align & Qt::AlignBottom ) {
		pos.ry() -= h;
	}else if( align & Qt::AlignVCenter ) {
		pos.ry() -= h >> 1;
	}
}
void ScalePixmap( QPixmap& pix, qreal scale )
{
	if ( pix.isNull() )
		return;
	QSize size	= pix.size();
	size *= scale;
	pix	= pix.scaled(size,Qt::KeepAspectRatio,Qt::SmoothTransformation);
}
QPixmap CreateConjointPixmap( const QList<QPixmap>& pixmaps, const QSize& unitSize )
{
	if ( pixmaps.isEmpty() )
		return QPixmap();
	int width	= unitSize.width();
	int	height	= unitSize.height();
	if ( !unitSize.isValid() ) {
		width	= pixmaps.first().size().width();
		height	= pixmaps.first().size().height();
	}
	QPixmap conjointPixmap( width * pixmaps.size(), height );
	conjointPixmap.fill( Qt::black );
	QBitmap	mask	= conjointPixmap.createMaskFromColor( Qt::black );
	conjointPixmap.setMask( mask );
	
	QPainter painter( &conjointPixmap );
		
	int offsetX	= 0;
	foreach( QPixmap pix, pixmaps ) {
		pix	= pix.scaled( width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
		painter.drawPixmap( offsetX, 0, pix );
		offsetX	+= width;
	}
	return conjointPixmap;
}
QPixmap SvgRender2Pixmap( QSvgRenderer *renderer )
{
	if ( 0 == renderer )
		return QPixmap();
	QPixmap pix( renderer->defaultSize() );
	pix.fill( Qt::black );
	QBitmap	mask	= pix.createMaskFromColor( Qt::black );
	pix.setMask( mask );
	QPainter painter( &pix );
	renderer->render( &painter );
	return pix;
}
bool GetSettingSound()
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_SOUND;
	QString key	= keys.join("/");
	
    if ( settings.contains( key ) ) {
    	return settings.value( key ).toBool();
    }else {
    	return true;
    }
}
//string is the locale name or "auto"
QString GetSettingLocaleName()
{
	QSettings settings;
	
    QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_LANGUAGE;
	QString key	= keys.join("/");
	
    if ( settings.contains( key ) ) {
    	return settings.value( key ).toString();
    }else
    	return QLocale::system().name();
}
QString FindSupportedLocaleName()
{
	QString localeName	= GetSettingLocaleName();
	return MostAppropriateSupportedLocaleName( localeName );
}
QString MostAppropriateSupportedLocaleName( const QString& name )
{
	QStringList	localeNames;
	for ( uint i = 0; i < MAX_SUPPORTED_LOCALE; i++ ) {
		localeNames << supportedLocaleInfos[i].localeName;
	}
	//find equal to
	foreach( QString localeName, localeNames ) {
		if ( name == localeName )
			return localeName;
	}
	//find starts with
	foreach( QString localeName, localeNames ) {
		if ( localeName.startsWith(name) )
			return localeName;
	}
	
	if ( name.contains('_') ) {
		QString language	= name.section('_',0,0);
		//find equal to
		foreach( QString localeName, localeNames ) {
			if ( language == localeName )
				return localeName;
		}
		//find starts with
		foreach( QString localeName, localeNames ) {
			if ( localeName.startsWith(language) )
				return localeName;
		}
	}
	return QString();
}
quint8	GetSettingLanguageCode()
{
	QString localeName	= GetSettingLocaleName();
	return LocaleNameToLanguageCode( localeName );
}
quint8	LocaleNameToLanguageCode( const QString& localeName )
{
	if ( localeName == "zh_CN" )
		return DJGAME_LANGUAGE_SC;
	else if ( localeName == "zh_TW" )
		return DJGAME_LANGUAGE_TC;
	else if ( localeName.startsWith( "zh" ) )
		return DJGAME_LANGUAGE_SC;
	else 
		return DJGAME_LANGUAGE_EN;
}
uint GetSettingMaxImageSize()
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_MAX_IMAGE_SIZE;
	QString key	= keys.join("/");
	
    if ( settings.contains( key ) ) {
    	return settings.value( key ).toUInt();
    }else {
    	return 50;
    }
}
void GetSettingMinMaxSentences( int & min, int & max )
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_MIN_SENTENCES;
	QString key	= keys.join("/");
	
    if ( settings.contains( key ) ) {
    	min	= settings.value( key ).toInt();
    }else {
    	min = 100;
    }
    
    keys.clear();
    keys << KEY_HALL << KEY_SETUP << KEY_MAX_SENTENCES;
	key	= keys.join("/");
	
	if ( settings.contains( key ) ) {
    	max	= settings.value( key ).toInt();
    }else {
    	max	= 500;
    }
}
QByteArray	crypt( const QByteArray& buf )
{
	static const char seed[]	= {0x14,0x15,0x92,0x65,0x02,0xc5,0x0d,0x24,0xe8,0x63,0x29,0x9c,0x1e,0xf4,0x6b,0x25,0x67};

	QByteArray	crypted( buf );
	int	len	= sizeof(seed)/sizeof(char);
	
	for ( int i = 0; i < buf.size(); i++ ) {
		crypted[i]	= buf.at(i) ^ seed[i%len];
	}
	return crypted;
}
bool GetSettingUseProxy()
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_USE_PROXY;
	QString key	= keys.join("/");
	
    return settings.value( key ).toBool();
}
int GetSettingProxyType()
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_PROXY_TYPE;
	QString key	= keys.join("/");
	
	if ( settings.contains( key ) ) {
    	return settings.value( key ).toInt();
    }else {
    	return -1;
    }
}
QVariant GetSettingProxy( int proxyType )
{
	QSettings settings;
	QList<QVariant> nulldata;
	nulldata << QVariant() << QVariant();
		
	QStringList keys;
	switch( proxyType ) {
	case 0://socks5
		keys << KEY_HALL << KEY_SETUP << KEY_SOCKS5_PROXY;
		break;
	case 1://socks4
		keys << KEY_HALL << KEY_SETUP << KEY_SOCKS4_PROXY;
		break;
	case 2://http
		keys << KEY_HALL << KEY_SETUP << KEY_HTTP_PROXY;
		break;
	default:
		return nulldata;
	}
	QString key	= keys.join("/");
	
	if ( settings.contains( key ) ) {
    	return settings.value( key );
    }else {
    	return nulldata;
    }
}
void GetSettingProxyAuth( QString& username, QString& password )
{
	QSettings settings;
	
	QStringList keys;
	keys << KEY_HALL << KEY_SETUP << KEY_AUTH_USERNAME;
	QString key	= keys.join("/");
	
    username	= settings.value( key ).toString();
    
    keys.clear();
   	keys << KEY_HALL << KEY_SETUP << KEY_AUTH_PASSWORD;
	key	= keys.join("/");
	
	password	= QString::fromUtf8( crypt(settings.value( key ).toByteArray()) );
}
void DebugBuffer( const QByteArray& buf, int length )
{
#ifdef DJ_DEBUG_OUTPUT
	int len;
	if ( -1 == length )
		len	= buf.size();
	else
		len	= length;
		
	QString	string;
	QTextStream	stream(&string);
	stream.setFieldWidth(2);
	stream.setPadChar('0');
	
	for(int i = 0; i < len; i++) {
		stream << hex << (quint8)buf.at(i) << "  ";
	}
	djDebug() << string;
#endif
}
void DebugBuffer( const char* buf, int length )
{
	DebugBuffer( QByteArray( buf,length ) );
}

bool InterpretText( QString& text )
{
	bool ring	= false;
	
	while( -1 != text.indexOf("{{") && -1 != text.indexOf("}}") ) {
		int	begin	= text.indexOf("{{");
		int	end		= text.indexOf("}}");
		
		int lenSentence	= end + 2 - begin;
		QString sentence	= text.mid( begin, lenSentence );
		
		int	lenContent	= lenSentence - 4;
		QString content	= text.mid( begin + 2, lenContent );
		
		//djDebug() << "sentence = " << sentence << "content" << content;
		if ( content.startsWith("member/", Qt::CaseInsensitive) ) {
			djDebug() << "startsWith member"<<content;
			QByteArray buf	= content.toUtf8();
			for (int i = 0; i < buf.size(); ++i ) {
				quint8 ch	= buf.at(i);
				if ( ch >= 0x80 ) {
					djDebug()<<"ch is not ascii"<<i<<hex<<ch;
					buf.truncate(i);
					break;
				}
			}
			content	= QString( buf );
			djDebug() << "content="<<content;
			
			QString imageHtml	= QString("<img src=%1>");
    		QString	destPath	= QString("images/") + content;
    		if ( QFile::exists( destPath ) ) {
    			text.replace( sentence, imageHtml.arg( destPath ) );
    		}else {
    			text.replace( sentence, imageHtml.arg( QString(HTTP_WWW_HOST) + destPath ) );
    		}    		
		}else {
			//it is face image
			QString	facename	= content.section(",",0,0);
    		if ( facename == "ring" )
    			ring	= true;
    		//djDebug() << facename << size;
    		uint	size	= content.section(",",-1,-1).toUInt();
    		if ( 0 == size || size > 96 )
    			size	= 32;
    		if ( !facename.contains('.') )
    			facename	+= ".png";
    		QString imageHtml	= QString("<img src=images/%1 width=%2 height=%3>")
    										.arg( facename )
    										.arg( size )
    										.arg( size );

    		text.replace( sentence, imageHtml );
		}
	}
	
	return ring;
}
bool RemoveHtmlTag( QString& text )
{	
	//djDebug() << "RemoveHtmlTag" << text;
	bool result	= false;
	int begin	= text.indexOf("<");
	if ( -1 == begin )
		return result;
	while (  -1 != begin ) {
		//djDebug() << "begin index=" << begin;
		result	= true;
		int end	= text.indexOf( ">", begin );
		//djDebug() << "end index=" << end;
		if ( -1 == end ) {
			//end tag not found,remove all
			text.truncate( begin );
			//djDebug() << "RemoveHtmlTag finish" << text;
			break;
		} else {
			text	= text.remove( begin, end - begin + 1 );
			//djDebug() << "after remove text = " << text;
			begin	= text.indexOf("<",end);
		}
	}
	return result;
}
bool RemoveMemberPrefix( QString& text )
{
	djDebug() << "RemoveMemberPrefix" << text;
	bool result	= false;
	
	int begin	= text.indexOf("{{");
	if ( -1 == begin )
		return result;
	while (  -1 != begin ) {
		djDebug() << "begin index=" << begin;
		int end	= text.indexOf("}}",begin);
		djDebug() << "end index=" << end;
		if ( -1 == end ) 
			break;
			
		int lenSentence	= end + 2 - begin;
    	QString sentence	= text.mid( begin, lenSentence );
    	
    	int	lenContent	= lenSentence - 4;
    	QString content	= text.mid( begin + 2, lenContent );
    	
    	djDebug() << "sentence = " << sentence << "content" << content;
    	
    	if ( content.startsWith( "member/", Qt::CaseInsensitive) ) {
    		text	=  text.remove( begin, end - begin + 2 );
    		djDebug() << "after remove text = " << text;
    		result	= true;
    		begin	= text.indexOf("{{");
    	}else {
    		//skip to next {{
    		begin	= text.indexOf("{{",end);
    	}
	}
	return result;
}
void PaintBackground( QWidget* widget, const QPoint& start, const QPoint& final)
{
	QPalette palette	= widget->palette();
	QLinearGradient linearGradient( start, final );
	linearGradient.setSpread( QGradient::ReflectSpread );
	linearGradient.setColorAt(0, QColor(224,238,255) );
	linearGradient.setColorAt(1, QColor(144,194,255) );
	palette.setBrush(QPalette::Base, linearGradient );
	widget->setPalette( palette );
}
void GetSettingGameSetup( const QString& gameName, quint8& speed, bool& refuseSameIp, quint8& breakRate, qint32& minScore, qint32& maxScore, quint16& maxUnwelcome )
{
	QSettings settings;
	settings.beginGroup( gameName );
	settings.beginGroup( KEY_SETUP );
	
	speed	= settings.value( KEY_SPEED ).toUInt();
	refuseSameIp	= settings.value( KEY_REFUSE_SAMEIP ).toBool();
	breakRate	= settings.value( KEY_BREAK_RATE ).toUInt();
	minScore	= settings.value( KEY_MIN_SCORE ).toInt();
	maxScore	= settings.value( KEY_MAX_SCORE ).toInt();
	maxScore	= settings.value( KEY_MAX_SCORE ).toInt();
	maxUnwelcome	= settings.value( KEY_MAX_UNWELCOME ).toUInt();
	
	settings.endGroup();
	settings.endGroup();
}
void SetSettingGameSetup( const QString& gameName, quint8 speed, bool refuseSameIp, quint8 breakRate, qint32 minScore, qint32 maxScore, quint16 maxUnwelcome )
{
	QSettings settings;
	settings.beginGroup( gameName );
	settings.beginGroup( KEY_SETUP );
	
	settings.setValue( KEY_SPEED, speed );
	settings.setValue( KEY_REFUSE_SAMEIP, refuseSameIp );
	settings.setValue( KEY_BREAK_RATE, breakRate );
	settings.setValue( KEY_MIN_SCORE, minScore );
	settings.setValue( KEY_MAX_SCORE, maxScore );
	settings.setValue( KEY_MAX_UNWELCOME, maxUnwelcome );
	
	settings.endGroup();
	settings.endGroup();
}
QBrush GetSettingGameDesktopBrush( const QString& gameName )
{
	QSettings settings;
	settings.beginGroup( gameName );
	QVariant variant	= settings.value( KEY_GAME_DESKTOP_BRUSH );
	settings.endGroup();
	if ( !variant.isValid() ) {
		return QColor(75,130,170);
	}
	QBrush brush	= variant.value<QBrush>();
	return brush;
}
void SetSettingGameDesktopBrush( const QString& gameName, const QBrush& brush )
{
	QSettings settings;
	settings.beginGroup( gameName );
	QVariant variant	= brush;
	settings.setValue( KEY_GAME_DESKTOP_BRUSH, variant );
	settings.endGroup();
}
void ConvertSeconds2HMS( int totalSeconds, int& hours, int& minutes, int& seconds )
{
	hours	= totalSeconds / 3600;
	totalSeconds	-= hours * 3600;
	minutes	= totalSeconds / 60;
	totalSeconds	-= minutes * 60;
	seconds	= totalSeconds;
}

