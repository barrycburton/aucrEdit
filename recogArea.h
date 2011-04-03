//recogArea.h

#ifndef RECOGAREA_H
#define RECOGAREA_H

#include <qwidget.h>
#include <qpainter.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qpe/qpeapplication.h>
#include <wchar.h>
#include <stdlib.h>
#include "AUCR.h"
#include "aucrEdit.h"


class recogArea : public QWidget
{
    Q_OBJECT

public slots:
	void setAlphabet( AUCR_Alphabet_Ptr newalf );
	void setStart( bool newstart );
	void setText();
	void setClear( bool );
	
signals: 
	void found( wchar_t );       

public:
		recogArea( QWidget *parent, const char *name );
		~recogArea();
		bool started();
		QSize sizeHint() const;
		QSizePolicy sizePolicy() const;
        
private:
		void mousePressEvent( QMouseEvent* );
		void mouseMoveEvent( QMouseEvent* );
		void mouseReleaseEvent( QMouseEvent* );
      void paintEvent( QPaintEvent *e = 0  );
      bool mousePressed;
		bool myStart;
		bool clear;
		bool begin;
		bool graffiti;
		AUCR_Error error;
		AUCR_Alphabet_Ptr myAlf;
      AUCR_Coordinate_Ptr myCords;
		QPixmap* myPixmap;
      int numCords;
		int n;
		int xMax;
		int xMin;
		int xOld;
		wchar_t uni;
};

#endif //RECOGAREA_H
