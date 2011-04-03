//editArea.h

#ifndef EDITAREA_H
#define EDITAREA_H

#include <qwidget.h>
#include <qpainter.h>
#include <qevent.h>
#include <qtimer.h>
#include <qpe/qpeapplication.h>
#include <wchar.h>
#include <stdlib.h>
#include "AUCR.h"
#include "aucrEdit.h"


class editArea : public QWidget
{
    Q_OBJECT

public slots:
	void dispLetter( AUCR_Character_Ptr );
	void dispLetter();
	void clear();
	void paintEvent( QPaintEvent *e = 0  );
	void setCurrent( wchar_t );
	void setStart( bool );
	void setAlphabet( AUCR_Alphabet_Ptr );
	
signals:        

public:
	editArea( QWidget *parent, const char *name );
	~editArea();
	bool isDirty();
	QSize sizeHint() const;
	QSizePolicy sizePolicy() const;
        
private:
	void mousePressEvent( QMouseEvent* );
	void mouseMoveEvent( QMouseEvent* );
	void mouseReleaseEvent( QMouseEvent* );
	bool mousePressed;
	bool dirty;
	bool charChanged;
	AUCR_Character_Ptr myCharacter;
	AUCR_Interpolated_Character_Ptr myInterpolated;
	wchar_t myCurrent;
	bool myStart;
	AUCR_Alphabet_Ptr myAlf;
	AUCR_Coordinate_Ptr myCords;
	AUCR_Error error;
	int numCords;
	int n;
	QTimer * timer;
};

#endif //EDITAREA_H
