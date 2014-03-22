#ifndef DJTABWIDGET_H
#define DJTABWIDGET_H

#include <QtGui>

class DJTabWidget : public QTabWidget
{
	Q_OBJECT
public:
	DJTabWidget( QWidget * parent = 0 );
	virtual ~DJTabWidget();
	
	virtual void mouseReleaseEvent( QMouseEvent * e );
signals:
	void rightClicked(int);
};

#endif
