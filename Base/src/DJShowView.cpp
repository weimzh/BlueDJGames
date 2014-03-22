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

#include "DJShowView.h"
#include "DJRemoteToLocalFile.h"
#include "DJGraphicsAnimationItem.h"
#include "DJGameUser.h"
#include "Utility.h"

static	uint	KEEP_XML_MAX_SHOWID	= 255;
static	int		SHOW_WIDTH			= 72;
static 	int		SHOW_HEIGHT			= 122;

DJShowItem::DJShowItem( DJShowView *showView, const QString& name, DJGameUser *user, double x, double y, double z, bool isAnimated, const QMatrix& matrix )
	:m_showView(showView),m_name(name),m_x(x),m_y(y),m_z(z),m_isAnimated(isAnimated),m_matrix(matrix)
{
	djDebug() << "DJShowItem constructor" << name << x << y << z << isAnimated<<matrix;
	
	if ( m_name.isEmpty() ) {
		quint8 gender	= 0;
		if ( user )
			gender	= user->gender();
		if ( DJGAME_GENDER_MALE == gender ) {
			m_localImagePath	= ":/BaseRes/image/hall/ShowMale.gif";
		}else if ( DJGAME_GENDER_FEMALE == gender ) {
			m_localImagePath	= ":/BaseRes/image/hall/ShowFemale.gif";
		}else {
			m_localImagePath	= ":/BaseRes/image/hall/ShowSecret.gif";
		}
	}else {
		m_localImagePath	= QString("images/avatars/") + m_name;
	}
	m_item	= 0;
	
	createItem();
	
	m_hasStartedTimer	= false;
	m_lifeTime	= 0;
}
DJShowItem::~DJShowItem()
{
	djDebug() << "DJShowItem destructor";
	delete m_item;
	m_item	= 0;
}
void DJShowItem::description()
{
	djDebug() << "m_name=" << m_name << "x=" << m_x << "y=" << m_y << "z=" << m_z << "animated=" << m_isAnimated;
}
void DJShowItem::createItem()
{
	djDebug() << "DJShowItem::createItem" << m_localImagePath;
	if ( !QFile::exists(m_localImagePath) ) {
		djDebug() << "file is not exists";
		QUrl	url( HTTP_WWW_HOST + m_localImagePath );
		DJRemoteToLocalFile	*r2lItem	= new DJRemoteToLocalFile( url, m_localImagePath, false, this );
    	connect( r2lItem, SIGNAL(localFileDone(const QString&)), SLOT(showImageDone()) );
		return;
	}
	if ( 0 != m_item )
		return;
	
	//clear dx , dy 
	if ( m_isAnimated ) {
		//create animated item
		djDebug() << "create animated item";
		m_item	= new DJGraphicsAnimationItem( m_localImagePath, 0, m_showView->scene() );
	}else {
		//create static item
		djDebug() << "create static item";
		m_item	= new QGraphicsPixmapItem( QPixmap(m_localImagePath), 0, m_showView->scene() );
		m_item->setTransformationMode( Qt::SmoothTransformation );
	}
	m_item->setMatrix( m_matrix );
	m_item->setPos( m_x, m_y );
	m_item->setZValue( m_z );
	m_item->hide();
}
void DJShowItem::show()
{
	if ( m_item )
		m_item->show();
}
void DJShowItem::hide()
{
	if ( m_item )
		m_item->hide();
}
bool DJShowItem::isReady() const
{
	return 0 != m_item;
}
void DJShowItem::setLifeTime( int second )
{
	if ( !m_hasStartedTimer ) {
		m_hasStartedTimer	= true;
		startTimer( 1000 );
	}
	m_lifeTime	= second;
}
void DJShowItem::setAlignment( Qt::AlignmentFlag flag )
{
}
void DJShowItem::timerEvent( QTimerEvent * event )
{
	if ( m_lifeTime > 0 ) {
		m_lifeTime--;
		if ( 0 == m_lifeTime ) {
			hide();
		}
	}
}
void DJShowItem::showImageDone()
{
	createItem();
	emit itemIsReady();
}
///////////////////////////////////////////////////////////////////////////////////////
DJShowView::DJShowView( DJGameUser* user, QWidget* parent )
	:QGraphicsView(parent),m_user(user)
{	
	setRealSize( SHOW_WIDTH,SHOW_HEIGHT );
	m_alignment	= 0;
	
	QGraphicsScene	*scene	= new QGraphicsScene(this);
	setScene( scene );
	
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//setViewportMargins(0,0,0,0);
			
	resize( SHOW_WIDTH,SHOW_HEIGHT );
	
	setUser( user );
}

DJShowView::~DJShowView()
{
	//all items on canvas should have been deleted
	clearAllItems();
}
void DJShowView::setUser( DJGameUser* user )
{
	djDebug()<<"DJShowView::setUser"<<user;
	m_user	= user;
	quint32 showId	= 0;
	if ( user )
		showId	= user->showId();
	clearAllItems();
	
	createDefaultItems();
	showDefaultShow();
	
	if ( 0 != showId ) {
		QString path	= "xml/show/1%1.xml";
		if ( user && DJGAME_GENDER_FEMALE == user->gender() )
			path	= "xml/show/2%1.xml";
			
    	QString	xmlPath	= path.arg( showId, 8, 16, QChar('0') );
    	QFile	file( xmlPath );
    	if ( file.exists() ) {
    		createCustomItems( xmlPath );
    	}else {
        	QUrl	remoteUrl( HTTP_WWW_HOST + xmlPath );
        	if ( showId > KEEP_XML_MAX_SHOWID ) {
#ifdef DJ_OEM
				return;
#else        		
        		xmlPath.clear();
#endif        		
        	}
        	if ( m_showXmls.contains( showId ) ) {
        		xmlPath	= m_showXmls.value( showId );
        		createCustomItems( xmlPath );
        	}else {
        		DJRemoteToLocalFile	*r2lShowXml	= new DJRemoteToLocalFile( remoteUrl, xmlPath, false, this );
        		connect( r2lShowXml, SIGNAL(localFileDone(const QString&)), SLOT(showXmlDone(const QString&)) );
        	}
    	}
	}
}
QSize DJShowView::sizeHint() const
{
	djDebug() << "sizehint" << m_realSize;
	return m_realSize;
}
void DJShowView::setViewAlignment( Qt::Alignment alignment )
{
	m_alignment	= alignment;
}
void DJShowView::setVirtualPos( const QPoint& vpos )
{
	djDebug()<<"DJShowView::setVirtualPos"<<vpos;
	m_virtualPos	= vpos;
}
/*
void DJShowView::setRealPos( const QPoint& pos )
{
	int x	= pos.x();
	int y	= pos.y();
	
	PosByAlign( x, y, m_alignment, size() );
	move( x, y );
}
*/
void DJShowView::adjustPos( const QMatrix& m )
{	
	QPoint	pos	= m_virtualPos;
	PosByAlign( pos, m_alignment, m_realSize );
	pos	= m.map( pos );
	move( pos );
}
void DJShowView::setRealSize( const QSize& size )
{
	m_realSize	= size;
}
void DJShowView::setRealSize( int width, int height )
{
	m_realSize.setWidth( width );
	m_realSize.setHeight( height );
}
void DJShowView::clearAllItems()
{
	djDebug() << "DJShowView::clearAllItems";
	djDebug() << "m_defaultItems.size()" << m_defaultItems.size();
	qDeleteAll( m_defaultItems );
	m_defaultItems.clear();
	djDebug() << "m_customItems.size()" << m_customItems.size();
	qDeleteAll( m_customItems );
	m_customItems.clear();
	djDebug() << "m_effectItems.size()" << m_effectItems.size();
	qDeleteAll( m_effectItems );
	m_effectItems.clear();
}
void DJShowView::createDefaultItems()
{
	djDebug() << "DJShowView::createDefaultItems";
	qDeleteAll( m_defaultItems );
	m_defaultItems.clear();
	
	DJShowItem	*item	= new DJShowItem( this, QString(), m_user, 0, 0, 100, false );
	m_defaultItems << item;
}
void DJShowView::showDefaultShow()
{
	foreach( DJShowItem *item, m_defaultItems ) {
		item->show();
	}
}
void DJShowView::hideDefaultShow()
{
	foreach( DJShowItem *item, m_defaultItems ) {
		item->hide();
	}
}
void DJShowView::createCustomItems( const QString& xmlPath )
{
	djDebug() << "DJShowView::createCustomItems";
	
	qDeleteAll( m_customItems );
	m_customItems.clear();
	
	QFile file( xmlPath );
	QDomDocument	domDocument;
	domDocument.setContent( &file, true );
	QDomElement domRoot = domDocument.documentElement();
	if ( domRoot.tagName() != "djshow" )
		return;
	/*	
	QDomElement domSize = domRoot.firstChildElement("size");
	int width	= SHOW_WIDTH;
	int height	= SHOW_HEIGHT;
	if ( !domSize.isNull() ) {
		width	= domSize.firstChildElement("width").text().toInt();
		height	= domSize.firstChildElement("height").text().toInt();
	}
	djDebug() << "resize canvas";
	canvas()->resize(width,height);
	*/
	QDomElement domItems = domRoot.firstChildElement("items");
	if ( domItems.isNull() )
		return;
	QDomElement domItem = domItems.firstChildElement();
	while( !domItem.isNull() ) {
		djDebug() << "domItem" << domItem.tagName();
		QString name	= domItem.firstChildElement("filename").text();
		double	wscale	= domItem.firstChildElement("wscale").text().toDouble();
		double	hscale	= domItem.firstChildElement("hscale").text().toDouble();
		double	x	= domItem.firstChildElement("x").text().toDouble();
		double	y	= domItem.firstChildElement("y").text().toDouble();
		double	z	= domItem.firstChildElement("z").text().toDouble();
		bool	isAnimated	= domItem.firstChildElement("animated").text().toUInt();
		if ( !name.isEmpty() ) {
			QMatrix m;
			if ( 0 != wscale && 0 != hscale ) {				
				m.setMatrix( wscale,0,0,hscale,0,0);
			}
			DJShowItem	*item	= new DJShowItem( this, name, m_user, x, y, z, isAnimated, m);
			connect( item, SIGNAL(itemIsReady()), SLOT(customItemIsReady()) );
			m_customItems << item;
		}
		domItem	= domItem.nextSiblingElement();
	}
	
	//foreach( DJShowItem *item, m_customItems ) {
		//item->description();
	//}
	customItemIsReady();
}
void DJShowView::showCustomShow()
{	
	foreach( DJShowItem *item, m_customItems ) {
		item->show();
	}
}
void DJShowView::hideCustomShow()
{
	foreach( DJShowItem *item, m_customItems ) {
		item->hide();
	}
} 
bool DJShowView::isAllCustomItemsReady()
{
	bool	isReady	= true;
	foreach( DJShowItem *item, m_customItems ) {
		if ( !item->isReady() ) {
			isReady	= false;
		}
	}
	return isReady;
}
void DJShowView::addEffect( const QString& name, int second )
{
	djDebug() << "DJShowView::addEffect" << name;
	if ( name.isEmpty() )
		return;
	DJShowItem*	item;
	if ( m_effectItems.contains( name ) ) {
		item	= m_effectItems.value( name );
	}else {
		item	= new DJShowItem( this, name, m_user, 0, 0, 1000, true );
		m_effectItems.insert( name, item );
	}
	item->setLifeTime( second );
	item->show();
}
void DJShowView::resizeEvent( QResizeEvent * event )
{
	djDebug() << "DJShowView::resizeEvent";
	QGraphicsView::resizeEvent( event );
	//NOTE: canvas size should be visible width height not width height	
	//canvas()->resize( visibleWidth(), visibleHeight() );
	//m_resizeTimer->start();
	//fitInView( sceneRect() );
}
void DJShowView::mouseReleaseEvent( QMouseEvent * e )
{
	djDebug() << "DJShowView::mouseReleaseEvent";
	QGraphicsView::mouseReleaseEvent(e);
		
	if ( Qt::RightButton == e->button() ) {
		if ( m_user ) {
			djDebug()<<"emit rightClicked";
			emit rightClicked( m_user );
		}
	}
}
void DJShowView::showXmlDone( const QString& filePath )
{
	djDebug()<<"showXmlDone"<<filePath;
	if ( m_user && m_user->showId() ) {
		m_showXmls.insert( m_user->showId(), filePath );
	}
	createCustomItems( filePath );
}
void DJShowView::customItemIsReady()
{
	if ( isAllCustomItemsReady() ) {
		hideDefaultShow();
		showCustomShow();
	}
}

