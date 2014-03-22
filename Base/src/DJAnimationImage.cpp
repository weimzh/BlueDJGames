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

#include "DJAnimationImage.h"

static const int DEFAULT_ANIMATE_PERIOD			= 100;// 0.1 second

DJAnimationImage::DJAnimationImage( QObject* parent )
	:QObject(parent)
{
	djDebug()<<"DJAnimationImage constructor";
	m_currentFrame	= -1;
	
	m_timer	= new QTimer( this );
    connect(m_timer,SIGNAL(timeout()),this,SLOT(handleAnimation()));
}
DJAnimationImage::DJAnimationImage( const QString& file, QObject* parent )
	:QObject(parent)
{
	djDebug()<<"DJAnimationImage constructor";
	m_currentFrame	= -1;
	
	m_timer	= new QTimer( this );
    connect(m_timer,SIGNAL(timeout()),this,SLOT(handleAnimation()));
    
	setImagePath( file );
}
DJAnimationImage::~DJAnimationImage()
{
	djDebug()<<"DJAnimationImage destructor";
}
void DJAnimationImage::setImagePath( const QString& file )
{
	if ( QFile::exists( file ) ) {
		djDebug()<<"image file exists";
		QImageReader	imageReader( file );
    	QImage	image	= imageReader.read();
    	while( !image.isNull() ) {
    		m_images << image;
    		djDebug()<<"next image delay="<<imageReader.nextImageDelay();
    		m_delays << imageReader.nextImageDelay();
    		imageReader.jumpToNextImage();
    		image	= imageReader.read();
    	}
    	if ( !m_images.isEmpty() ) {
    		djDebug() << "m_images has frames" << m_images.size();
    		m_currentFrame	= 0;
    	}
	}
}
bool DJAnimationImage::isValid() const
{
	return !m_images.isEmpty();
}
bool DJAnimationImage::isAnimation() const
{
	return m_images.size() > 1;
}
void DJAnimationImage::start()
{
	if ( -1 != m_currentFrame ) {
		m_timer->start( 0 );
	}
}
void DJAnimationImage::stop()
{
	m_timer->stop();
}
QImage DJAnimationImage::image() const
{
	if ( -1 != m_currentFrame ) {
		return m_images.at(m_currentFrame);
	}else
		return QImage();
}
void DJAnimationImage::handleAnimation()
{
	emit imageChanged();
	
	int delay	= m_delays.value( m_currentFrame, -1 );
	if ( -1 == delay )
   		delay	= DEFAULT_ANIMATE_PERIOD;
   	m_currentFrame++;
	if ( m_currentFrame >= m_images.size() ) {
		m_currentFrame	= 0;
	}
   	m_timer->start( delay );
}