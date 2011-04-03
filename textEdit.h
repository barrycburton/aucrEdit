//textEdit.h

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <qwidget.h>
#include <qevent.h>
#include <qlayout.h>
#include <qabstractlayout.h>
#include <qlabel.h>
#include <qstring.h>
#include <qmultilineedit.h>
#include <wchar.h>
#include <stdlib.h>
#include "recogArea.h"

class recogArea;

class MyMultiLineEdit : public QMultiLineEdit
{
	Q_OBJECT
	
public:
	MyMultiLineEdit( QWidget* parent, const char* name );
	bool myHasMarkedText();
	
public slots:
	void myEnd();
	void myDel();
	void setFound( wchar_t );
	
};


class textEdit : public QWidget
{ 
	Q_OBJECT

public slots:
	void setText( QString );
	
public:
	textEdit( QWidget* parent = 0, const char* name = 0 );
	~textEdit();
	QMultiLineEdit* getMyMultiLineEdit();
	recogArea* getRecogArea();
	QSize sizeHint() const;
	QSizePolicy sizePolicy() const;

private:			   
	recogArea* entry;
	QLabel* textLab;
	MyMultiLineEdit* textbox;	
	QVBoxLayout* mainLay;
	QSpacerItem* topShim;
	QSpacerItem* bottomShim;
	
};

#endif //TEXTEDIT_H
