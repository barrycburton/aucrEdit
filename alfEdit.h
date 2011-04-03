//alfEdit.h

#ifndef ALFEDIT_H
#define ALFEDIT_H

#include <qwidget.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qevent.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qstring.h>
#include <qabstractlayout.h>
#include <wchar.h>
#include <stdlib.h>
#include "AUCR.h"
#include "editArea.h"

class editArea;

class alfEdit : public QWidget
{ 
	Q_OBJECT

public slots:
	void setCurrent( int n );
	void setShift( bool on );
	void setCaps();
	void dispCurrent();
	void setAlphabet( AUCR_Alphabet_Ptr alf );
	
signals:	

public:
	alfEdit( QWidget* parent = 0, const char* name = 0 );
	~alfEdit();
	void update();
	editArea* getEditArea1();	
	editArea* getEditArea2();
	editArea* getEditArea3();
	void currentChanged( wchar_t myUnicodeChar );
	void startChanged( bool myStartedYet );
	QSize sizeHint() const;
	QSizePolicy sizePolicy() const;

private:		 
	editArea* entry1;
	editArea* entry2;
	editArea* entry3;
	
	QVBoxLayout* mainLay;
	QHBoxLayout* topLay;
	QHBoxLayout* middleLay;
	QVBoxLayout* keyLay;
	QHBoxLayout* row1Lay;
	QHBoxLayout* row2Lay;
	QHBoxLayout* row3Lay;
	QHBoxLayout* row4Lay;
	QHBoxLayout* row5Lay;
	
	QLabel* topLab;
	QLabel* currentLab;
	QLabel* charLab;
	
	QSpacerItem* keyLshim;
	QSpacerItem* keyRshim;
	
	QSpacerItem* topShim;
	QSpacerItem* middleShim;
	QSpacerItem* bottomShim;
	
	QPushButton* last;
	QPushButton* lshift;
	QPushButton* rshift;
	QPushButton* caps;
	QPushButton* keys[51];
	
	QButtonGroup* keyboard;
	
	wchar_t unicode[2][51];
	QString qtext[2][51];

	int state;
	AUCR_Alphabet_Ptr myAlf;
	wchar_t current;
	bool start;
	
};

#endif //ALFEDIT_H
