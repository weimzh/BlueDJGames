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

#include "DJStyle.h"
#include "DJToolButton.h"
#include "DJGlobal.h"

#ifndef Q_WS_MAC
void djDrawWinButton ( QPainter * p, QRect &rect, const QPalette & pal, bool sunken,int h=2);
void djDrawHeader( const DJStyle *style, QPainter *p,QStyleOptionHeader *opt);
#endif

DJStyle::DJStyle( const QString& stylePath )
	: BASE_STYLE(),m_stylePath(stylePath)
{
	QPixmap	pix( m_stylePath + "PushButtonMid.png" );
	if ( !pix.isNull() )
		m_pbHeight	= pix.height();
	else
		m_pbHeight	= 0;
}
DJStyle::~DJStyle()
{
}

#ifndef Q_WS_MAC
void DJStyle::drawPrimitive(PrimitiveElement element,
                                       const QStyleOption *option,
                                       QPainter *painter,
                                       const QWidget *widget) const
{
    switch (element) {
    case PE_PanelButtonCommand: {
    	//djDebug() << "PE_PanelButtonTool";
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            //bool hover = (button->state & State_Enabled) && (button->state & State_MouseOver);

            const QPushButton *pushButton = qobject_cast<const QPushButton *>(widget);
            if ( pushButton && !pushButton->isFlat() ) {
                bool down = pushButton && ((button->state & State_Sunken) || (button->state & State_On));
    
    			//int height	= button->rect.height();
    			
                QPixmap left, right, mid;
                if (down) {
                    left 	= QPixmap( m_stylePath + "PushButtonPressedLeft.png");
                    right	= QPixmap( m_stylePath + "PushButtonPressedRight.png");
                    mid		= QPixmap( m_stylePath + "PushButtonPressedMid.png");
                } else {
                    left 	= QPixmap( m_stylePath + "PushButtonLeft.png");
                    right 	= QPixmap( m_stylePath + "PushButtonRight.png");
                    mid 	= QPixmap( m_stylePath + "PushButtonMid.png");
                }
                
                painter->drawPixmap(button->rect.topLeft(), left);
                painter->drawTiledPixmap(QRect(button->rect.x() + left.width(),
                                               button->rect.y(),
                                               button->rect.width() - left.width() - right.width(),
                                               left.height()),
                                         mid);
                painter->drawPixmap(button->rect.x() + button->rect.width() - right.width(),
                                    button->rect.y(),
                                    right);
                
            	//if (hover)
                	//painter->fillRect(widget->rect().adjusted(3,5,-3,-5), QColor(31,127,31));
            	return;
            }
        }
    	break;
	}
	case PE_FrameDefaultButton :
		return;
	case PE_FrameFocusRect:
		return;
	case PE_PanelButtonTool: {
		if ( widget->inherits("DJToolButton") ) {
			DJToolButton *djtb	= (DJToolButton*)widget;
			QIcon icon	= djtb->backgroundIcon();
			QSize size	= djtb->iconSize();
			QPixmap pixOn	= icon.pixmap( size, QIcon::Normal, QIcon::On );
			QPixmap pixOff	= icon.pixmap( size, QIcon::Normal, QIcon::Off );
				
			if ( option->state & State_Raised || option->state & State_Off )
				painter->drawPixmap( option->rect, pixOff );
			else if ( option->state & State_Sunken || option->state & State_On )
				painter->drawPixmap( option->rect, pixOn );
			return;
		}
	}
    default:
        break;
    }
    BASE_STYLE::drawPrimitive(element, option, painter, widget);
}
void djDrawHeader( const DJStyle *style, QPainter *p,QStyleOptionHeader *opt)
{
	if (opt->state & (QStyle::State_Raised | QStyle::State_Sunken) )
	{
		djDrawWinButton(p,opt->rect,opt->palette,false);//opt->state & QStyle::State_Sunken);
	}else
	{
		p->fillRect(opt->rect,opt->palette.brush(QPalette::Button));;
	}
	opt->textAlignment &= 0x0F;
	opt->textAlignment |= (Qt::AlignVCenter);
	QTextOption to(opt->textAlignment);
	
//	QRect textrect=QRect(x,y,rect.width()+2,rect.height());
	p->setPen(QPen(opt->palette.color(QPalette::ButtonText))); 
	p->drawText(opt->rect,opt->text,to);
	
	if (opt->sortIndicator != QStyleOptionHeader::None) 
	{
		int x,y;
		QRect rect = opt->fontMetrics.boundingRect(opt->text);
		switch(opt->textAlignment & 0x0F)
		{
			case Qt::AlignHCenter :
				x = ((opt->rect.width()-rect.width())>>1);
				break;
			case Qt::AlignLeft :
				x = 2;
				break;
			default:
				x	= 2;
				break;
		}
		if(x<2)
			x = 2;
		x += opt->rect.left();
	
		QPixmap pix;
    	if (opt->sortIndicator == QStyleOptionHeader::SortUp) 
		{
			pix = QPixmap(style->stylePath() + "ArrowUp.png");
    	}else
		{
			pix = QPixmap(style->stylePath() + "ArrowDown.png");
		}
		x += rect.width()+2;
		y = ((opt->rect.height() - pix.height())>>1)+opt->rect.top();
		p->drawPixmap(x,y,pix);
	}		
}


void djDrawWinButton ( QPainter * p, QRect &rect, const QPalette & pal,bool sunken,int h)
{
	QPen uppen,downpen;
	p->fillRect(rect,pal.brush(QPalette::Button));
	if(!sunken)
	{
		uppen = QPen(pal.brush(QPalette::Light),h );
		downpen = QPen(pal.brush(QPalette::Dark),h);
	}else
	{
		downpen = QPen(pal.brush(QPalette::Light),h);
		uppen = QPen(pal.brush(QPalette::Dark),h);
	}
	p->setPen(uppen);
	p->drawLine(rect.topRight(),rect.topLeft());
	p->drawLine(rect.topLeft(),rect.bottomLeft());
	
	p->setPen(downpen);
	p->drawLine(rect.bottomLeft(),rect.bottomRight());
	p->drawLine(rect.bottomRight(),rect.topRight());
}

void DJStyle::drawControl(ControlElement element,
                                     const QStyleOption *option,
                                     QPainter *painter,
                                     const QWidget *widget) const
{
    switch (element) {

    case CE_ScrollBarAddLine :
    case CE_ScrollBarSubLine : {
    	const QStyleOptionSlider *sliderOption = qstyleoption_cast<const QStyleOptionSlider *>(option);
    	QStyleOptionSlider myoption = *sliderOption;
    	
    	djDrawWinButton(painter,myoption.rect,myoption.palette,myoption.state & State_Sunken,1);
    	
    	QPixmap pix;
    	int x,y;

    	if ( Qt::Vertical == myoption.orientation )
    	{
    		if(element == CE_ScrollBarAddLine)
    			pix = QPixmap(m_stylePath + "ArrowDown.png");
    		else 
    			pix = QPixmap(m_stylePath + "ArrowUp.png");
    	}else
    	{
    		if(element == CE_ScrollBarAddLine)
    			pix = QPixmap(m_stylePath + "ArrowRight.png");
    		else 
    			pix = QPixmap(m_stylePath + "ArrowLeft.png");
    	}
    	x = ( (myoption.rect.width()-pix.width())>>1) + myoption.rect.left();
    	y = ((myoption.rect.height()-pix.height())>>1) + myoption.rect.top();
    	painter->drawPixmap( x,y,pix);
    	
    	break;
    }
    case CE_ScrollBarAddPage:
    case CE_ScrollBarSubPage: {
    	QStyleOption myoption = *option;
    	painter->fillRect(myoption.rect,myoption.palette.brush(QPalette::Background));
    	break;
    }
    case CE_ScrollBarSlider:
    {
    	QStyleOption myoption = *option;
    	painter->fillRect(myoption.rect,myoption.palette.brush(QPalette::Button));
    	break;
    }
    case CE_ScrollBarFirst:
    	break;
    case CE_ScrollBarLast:
    	break;

    case CE_Header: {
        const QStyleOptionHeader *headerOption = qstyleoption_cast<const QStyleOptionHeader *>(option);
        if ( headerOption ) {
        	QStyleOptionHeader myHeaderOption = *headerOption;
        	if ( QPalette::Disabled != myHeaderOption.palette.currentColorGroup() ) {

			QPointF ps = myHeaderOption.rect.topLeft();
			QPointF pe = myHeaderOption.rect.bottomLeft();
        		QLinearGradient linearGradient( ps,pe  );
        		linearGradient.setColorAt(0, QColor(172,206,247));
        		linearGradient.setColorAt(0.618, QColor(99,169,255));
        		linearGradient.setColorAt(1, QColor(172,206,247));
			linearGradient.setSpread(QGradient::ReflectSpread);

    			myHeaderOption.palette.setBrush(myHeaderOption.palette.currentColorGroup(), QPalette::Button, linearGradient );
    			
			myHeaderOption.palette.setColor(myHeaderOption.palette.currentColorGroup(),QPalette::ButtonText,QColor(0,0,255));
			myHeaderOption.textAlignment = Qt::AlignHCenter;

			QBrush brightb=QBrush(QColor(247,254,255));
			QBrush darkb=QBrush(QColor(0,0,255));

			myHeaderOption.palette.setBrush(myHeaderOption.palette.currentColorGroup(), QPalette::Light, brightb );
			myHeaderOption.palette.setBrush(myHeaderOption.palette.currentColorGroup(), QPalette::Dark, darkb );
			myHeaderOption.palette.setBrush(myHeaderOption.palette.currentColorGroup(), QPalette::Shadow, darkb );

			return djDrawHeader( this, painter,&myHeaderOption);
			
    		}
    		BASE_STYLE::drawControl(element, &myHeaderOption, painter, widget);
        }else {
    		BASE_STYLE::drawControl(element, option, painter, widget);
    	}
    	break;
    }
    case CE_TabBarTab: {
    	//djDebug() << "CE_TabBarTab";
    	const QStyleOptionTab *tabOption = qstyleoption_cast<const QStyleOptionTab *>(option);
    	if ( tabOption ) {
    		QStyleOptionTab myTabOption = *tabOption;
        	
        	QLinearGradient linearGradient( myTabOption.rect.topLeft(), myTabOption.rect.bottomLeft() );
        	linearGradient.setColorAt(0, Qt::white);
        	linearGradient.setColorAt(0.618, QColor(99,169,255));
        	linearGradient.setColorAt(1, QColor(172,206,247));
    			
        	myTabOption.palette.setBrush( QPalette::Window, linearGradient );
        		
        	BASE_STYLE::drawControl(element, &myTabOption, painter, widget);
    	}else
    		BASE_STYLE::drawControl(element, option, painter, widget);
    	break;
    }
	case CE_Splitter :  {
		QStyleOption myOption = *option;

		if (!(option->state & State_Horizontal)) {
			QLinearGradient linearGradient( myOption.rect.topLeft(), myOption.rect.bottomLeft() );
			linearGradient.setColorAt(0, QColor(195,251,255));
        		linearGradient.setColorAt(0.5, QColor(61,172,239));
        		linearGradient.setColorAt(1, QColor(195,251,255));
    			
				painter->fillRect( option->rect, linearGradient );
		}else
		{
			QLinearGradient linearGradient( myOption.rect.topLeft(), myOption.rect.topRight() );
			linearGradient.setColorAt(0, QColor(195,251,255));
        		linearGradient.setColorAt(0.5, QColor(61,172,239));
        		linearGradient.setColorAt(1, QColor(195,251,255));
    			
				painter->fillRect( option->rect, linearGradient );
		}
		return;
    }
    default:
        BASE_STYLE::drawControl(element, option, painter, widget);
        break;
    }
}

QSize DJStyle::sizeFromContents(ContentsType type, const QStyleOption *option,
                                    const QSize &size, const QWidget *widget) const
{
    QSize newSize = BASE_STYLE::sizeFromContents(type, option, size, widget);
	
    switch (type) {

    case CT_PushButton:
    	if ( 0 != m_pbHeight )
        	newSize.setHeight(m_pbHeight);
        break;
    default:
        break;
    }
    return newSize;
}
QPixmap DJStyle::standardPixmap ( StandardPixmap standardPixmap, const QStyleOption * option, const QWidget * widget ) const
{
	switch (standardPixmap){
	case SP_ToolBarHorizontalExtensionButton:
		return QPixmap(m_stylePath + "DoubleArrowRight.png");
	default:
		return BASE_STYLE::standardPixmap( standardPixmap, option, widget);
	}
}
#endif

void DJStyle::drawComplexControl ( ComplexControl control,
							const QStyleOptionComplex * option, 
							QPainter * painter, 
							const QWidget * widget ) const
{
	switch( control ){
#ifndef Q_WS_MAC		
	case CC_ScrollBar: {
		//djDebug() << "CC_ScrollBar";
		const QStyleOptionSlider *sliderOption = qstyleoption_cast<const QStyleOptionSlider *>(option);
        if ( sliderOption ) {
        	//djDebug() << "sliderOption" << option->rect.topLeft() << option->rect.bottomRight();
        	QStyleOptionSlider mySliderOption = *sliderOption;
        	if ( QPalette::Disabled != mySliderOption.palette.currentColorGroup() ) {
        		QPoint endPoint;
        		if ( Qt::Vertical == mySliderOption.orientation ) {
        			endPoint	= mySliderOption.rect.topRight();
        		}else {
        			endPoint	= mySliderOption.rect.bottomLeft();
        		}
        		QLinearGradient linearGradient( mySliderOption.rect.topLeft(), endPoint );
        		linearGradient.setColorAt(0, QColor(186,247,255) );
        		linearGradient.setColorAt(0.5, QColor(99,169,255) );
        		linearGradient.setColorAt(1, QColor(186,247,255) );
    			mySliderOption.palette.setBrush( QPalette::Button, linearGradient );

				QBrush shadowb=QBrush(QColor(192,217,255));
				mySliderOption.palette.setBrush(mySliderOption.palette.currentColorGroup(), QPalette::Background, shadowb );
					
				BASE_STYLE::drawComplexControl(control, &mySliderOption, painter, widget);
				return;
        	}
        }
		break;
	}
#endif
#ifdef Q_WS_MAC
	case CC_ToolButton : {
		const QStyleOptionToolButton *toolbutton
                = qstyleoption_cast<const QStyleOptionToolButton *>(option);
		if(toolbutton && widget->inherits("DJToolButton") )
		{
			QStyleOptionToolButton mytoolbutton = *toolbutton;
			
			DJToolButton *djtb	= (DJToolButton*)widget;
			QIcon icon	= djtb->backgroundIcon();
			QSize size	= djtb->iconSize();
			QPixmap pixOn	= icon.pixmap( size, QIcon::Normal, QIcon::On );
			QPixmap pixOff	= icon.pixmap( size, QIcon::Normal, QIcon::Off );
				
			if ( (option->state & State_Raised || option->state & State_Off) && !pixOff.isNull()) {
				painter->drawPixmap( mytoolbutton.rect.center().x()-(pixOff.width()>>1),
									 mytoolbutton.rect.center().y()-(pixOff.height()>>1),
									 pixOff );
			}else if ( (option->state & State_Sunken || option->state & State_On) && !pixOn.isNull() ) {
				painter->drawPixmap( mytoolbutton.rect.center().x()-(pixOn.width()>>1),
									 mytoolbutton.rect.center().y()-(pixOn.height()>>1),
									 pixOn );
			}
				
			QPixmap pix	= mytoolbutton.icon.pixmap( mytoolbutton.iconSize );
			int x	= mytoolbutton.rect.center().x()-(pix.width()>>1);
			int	y	= mytoolbutton.rect.center().y()-(pix.height()>>1);
			if ( mytoolbutton.toolButtonStyle == Qt::ToolButtonTextUnderIcon ) {
				djDebug()<<"ToolButtonTextUnderIcon";
				painter->drawText(mytoolbutton.rect,Qt::AlignHCenter | Qt::AlignBottom, mytoolbutton.text);
				x	= mytoolbutton.rect.center().x()-(pix.width()>>1);
				y	= 0;
			}
			if ( mytoolbutton.toolButtonStyle == Qt::ToolButtonTextBesideIcon ) {
				djDebug()<<"ToolButtonTextBesideIcon";
				painter->drawText(mytoolbutton.rect,Qt::AlignRight | Qt::AlignVCenter, mytoolbutton.text);
				x	= 0;
				y	= 0;
			}
			if(mytoolbutton.state & QStyle::State_Sunken)
			{
				x++;
				y++;
			}
			painter->drawPixmap( x,y,pix );
			return;
		}
		break;
	}
#endif
	default:
		break;
	}
	BASE_STYLE::drawComplexControl(control, option, painter, widget);
}

