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

#include "DJSymbol.h"

static const int	PLAYER_SYMBOL_WIDGET		=	1;
static const int	MEMBER_SYMBOL_WIDGET		=	2;

/////////////////////////////////////////////////////////////////
DJSymbolTable::DJSymbolTable( int type, QWidget* parent, const QSize symbolSize )
	:QTableWidget(parent),m_type(type)
{	
	djDebug() << "DJSymbolTable constructor";
	setSelectionMode( QAbstractItemView::NoSelection );
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
	setGridStyle( Qt::NoPen );
	setShowGrid( false );
	setSymbolSize( symbolSize );
	
	switch ( m_type ) {
	case PLAYER_SYMBOL_WIDGET: {
		setDirPath( ":/BaseRes/image/face" );
		createSymbolItems();
		break;
	}
	case MEMBER_SYMBOL_WIDGET: {
		setDirPath( "images/member" );
		createSymbolItems();
		break;
	}
	default:
		djDebug() << "symbol type error";	
		break;
	}
	
	connect( this, SIGNAL(itemClicked(QTableWidgetItem *)),SLOT(handleItemClicked(QTableWidgetItem *)));
	connect( this, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),SLOT(handleItemDoubleClicked(QTableWidgetItem *)));
	
	djDebug() << "DJSymbolTable constructor end";
}
DJSymbolTable::~DJSymbolTable()
{
}
void DJSymbolTable::setSymbolSize( const QSize& size )
{
	m_symbolSize	= size;
	//setIconSize( size );
	setIconSize( m_symbolSize - QSize(4,4) );
}
void DJSymbolTable::setDirPath( const QString& dirPath )
{
	m_dirPath	= dirPath;
}
void DJSymbolTable::createSymbolItems()
{	
	djDebug() << "createSymbolItems";
	clear();
	m_symbolItems.clear();
	
	QDir d( m_dirPath );
	QFileInfoList infoes;
	
	switch ( m_type ) {
	case PLAYER_SYMBOL_WIDGET: {
		d.setSorting(QDir::Name);
		infoes	= d.entryInfoList(QDir::Files);
		break;
	}
	case MEMBER_SYMBOL_WIDGET: {
		d.setSorting(QDir::Name | QDir::DirsFirst);
		infoes	= d.entryInfoList();
		break;
	}
	default:
		djDebug()<<"symbol type error";
		break;
	}
	
	foreach( QFileInfo info, infoes ) {
    	QString	path	= info.filePath();
    	//djDebug() << path;
    	QTableWidgetItem* item;
    	QPixmap	pix;
    	if ( info.isDir() ) {
    		QString dirName	= path.section('/',-1,-1);
    		QString	parentDirName	= path.section('/',-2,-2);
    		
    		if ( dirName == "." ) {
    			continue;
    		}else if ( dirName == ".." ) {
    			if ( parentDirName == "member" )
    				continue;
    			item	= new QTableWidgetItem;
    			item->setToolTip( tr("up") );
    			pix.load( ":/BaseRes/image/hall/folderup.png" );
    		}else {
    			QString name	= info.baseName();
    			item	= new QTableWidgetItem;
    			item->setToolTip( name );
    			pix.load( ":/BaseRes/image/hall/folder.png" );
    			QPainter p( &pix );
    			p.drawText( 2, m_symbolSize.height()/2 + 2, name );
    		}
    	}else {
    		item	= new QTableWidgetItem;
    		item->setToolTip( info.baseName() );
    		pix.load( path );
    	}
    	//djDebug() << "setpix";
    	if ( pix.isNull() ) {
    		item->setIcon( QPixmap( ":/BaseRes/image/hall/break.png") );
    	}else {
    		item->setIcon( pix );
    	}
    	item->setText( path );
    	m_symbolItems << item;
    	//djDebug() << "setpix end";
    }
    djDebug() << "createSymbolItems end";
}
void DJSymbolTable::setSymbolItems( int rows, int columns )
{
	djDebug() << "setSymbolItems" << rows << columns;
	
	setRowCount( rows );
	setColumnCount( columns );
	
	QHeaderView* header;
	header	= verticalHeader();
	for ( int i = 0; i < header->count(); i++ ) {
		header->resizeSection( i, m_symbolSize.height() );
	}
	header->hide();
	header	= horizontalHeader();
	for ( int i = 0; i < header->count(); i++ ) {
		header->resizeSection( i, m_symbolSize.width() );
	}
	header->hide();
	
	int index	= 0;
	for ( int r = 0; r < rows; r++ ) {
		for (int c	= 0; c < columns; c++ ) {
			if ( index < m_symbolItems.size() ) {
				setItem( r, c, m_symbolItems.at(index++) );
			}
		}
	}
	djDebug() << "setSymbolItems end";
}
void DJSymbolTable::reload()
{
	djDebug() << "reload";
	createSymbolItems();
	resizeEvent( 0 );
	djDebug() << "reload end";
}
void DJSymbolTable::handleItemClicked( QTableWidgetItem * item )
{	
	if ( !item ) {
		return;
	}
	QString name	= item->text();
	djDebug() << "name = " << name;
	
	switch ( m_type ) {
	case PLAYER_SYMBOL_WIDGET: {
		emit playerSymbolSelected( name.section('/',-1,-1).section('.',0,0) );
		break;
	}
	case MEMBER_SYMBOL_WIDGET: {
		QFileInfo info( name );
		if ( info.isFile() )
			emit memberSymbolSelected( name.section('/',1,-1) );
		break;
	}
	default:
		djDebug() <<"symbol type error";
		break;
	}
}
void DJSymbolTable::handleItemDoubleClicked( QTableWidgetItem * item )
{
	djDebug() << "handleItemDoubleClicked" << m_type;
	
	if ( !item ) {
		return;
	}
	QString name	= item->text();
	djDebug() << "name = " << name;
	
	switch ( m_type ) {
	case PLAYER_SYMBOL_WIDGET: {
		break;
	}
	case MEMBER_SYMBOL_WIDGET: {
		QFileInfo info( name );
		if ( info.isDir() ) {
			QString dirPath	= QDir::cleanPath( name );
			djDebug() << "dirPath" << dirPath;
			setDirPath( dirPath );
			reload();
		}
		break;
	}
	default:
		djDebug() <<"symbol type error";
		break;
	}
	djDebug() << "handleItemDoubleClicked end";
}
void DJSymbolTable::resizeEvent( QResizeEvent * event )
{	
	djDebug() << "DJSymbolTable::resizeEvent" << "dirpath=" << m_dirPath;
	if ( event )
		QTableWidget::resizeEvent(event);
	
	int w	= width() - verticalScrollBar()->width();
	int h	= height();
	if ( w <= 0 || h <= 0 )
		return;
		
	int columns	= w / m_symbolSize.width();
	
	if ( 0 == columns )  {
		columns = 1;
	}
	djDebug() << "item size = " << m_symbolItems.size();
	int rows	= ( m_symbolItems.size() + columns - 1) / columns;
	
	djDebug() << "rows=" << rows << "columns" << columns << "oldrow" << rowCount() << "oldcolumn" << columnCount();
	//djDebug() << "items = " << m_symbolItems.size();
	if ( rows && columns ) {
		if ( (rows != rowCount() && columns != columnCount()) || 0 == event ) {
			//if ( event )
				createSymbolItems();
			setSymbolItems( rows, columns );
		}
	}
	djDebug() << "DJSymbolTable::resizeEvent end";
}
////////////////////////////////////////////////////////////////////////////////////////
DJSymbol::DJSymbol( QWidget * parent )
	:QTabWidget( parent )
{
	setTabPosition( QTabWidget::South );
	
	m_playerSymbolWidget	= new DJSymbolTable( PLAYER_SYMBOL_WIDGET );
	addTab( m_playerSymbolWidget, tr("player") );
	connect(m_playerSymbolWidget, SIGNAL(playerSymbolSelected(const QString&)),SIGNAL(playerSymbolSelected(const QString&)));
	
	m_memberSymbolWidget	= new DJSymbolTable( MEMBER_SYMBOL_WIDGET );
	addTab( m_memberSymbolWidget, tr("member") );
	connect(m_memberSymbolWidget, SIGNAL(memberSymbolSelected(const QString&)),SIGNAL(memberSymbolSelected(const QString&)));
}
DJSymbol::~DJSymbol()
{
}
void DJSymbol::removeMemberWidget()
{
	djDebug() << "removeMemberWidget";
	
	m_memberSymbolWidget->hide();
	int index	= indexOf( m_memberSymbolWidget );
	if ( -1 != index )
		removeTab( index );
}
void DJSymbol::reloadMemberWidget()
{
	djDebug() << "reloadMemberWidget";
	
	m_memberSymbolWidget->reload();
}
