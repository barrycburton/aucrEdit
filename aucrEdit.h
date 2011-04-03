//aucrEdit.h

#ifndef AUCREDIT_H
#define AUCREDIT_H

#include <qwidget.h>
#include <qevent.h>
#include <qstring.h>
#include <qmainwindow.h>
#include <qwidgetstack.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qtooltip.h>
#include <qmessagebox.h>
#include <qclipboard.h>
#include <qhbox.h>
#include <qlist.h>
#include <qpe/qpeapplication.h>
#include <qpe/applnk.h> 
#include <qpe/fileselector.h>
#include <qpe/global.h>
#include <qpe/config.h>
#include <qpe/filemanager.h>
#include <wchar.h>
#include <stdlib.h>
#include "AUCR.h"
#include "textEdit.h"
#include "alfEdit.h"
#include "recogArea.h"
#include "editArea.h"

class textEdit;
class alfEdit;
class recogArea;
class editArea;
class MyMultiLineEdit;

class MyDialog : public QDialog
{
	Q_OBJECT
	
public slots:
	void save();
	void cancel();
	void discard();

signals:

public:	
	MyDialog( QWidget * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0 );
	~MyDialog();

private:

};


class MyFileSelector : public QHBox
{
	Q_OBJECT
	
public slots:
    void reread();

signals:
    void fileSelected( const DocLnk & );
    void newSelected( const DocLnk & );
    void closeMe();

public:
	MyFileSelector( const QString & f, QWidget * parent, const char * name = 0,
							bool newVisible = TRUE, bool closeVisible = TRUE );
	~MyFileSelector();
	QSize sizeHint() const;
	QSizePolicy sizePolicy() const;	

private:
	FileSelector* fileBrowse;

};


class aucrEdit : public QMainWindow
{
    Q_OBJECT

public slots:
	void setDocument( const QString& applnk_filename );
	void newText();
	void openText();
	void openText( const DocLnk& );
	bool saveText();
	bool saveAsText();
	bool changedTextSave();
	void callCloseText();
	void newAlphabet();
	void openAlphabet();
	void openAlphabet( const DocLnk& );
	bool saveAlphabet();
	bool saveAsAlphabet();
	bool changedAlphabetSave();
	void callCloseAlphabet();	
	void cut();
	void copy();
	void paste();
	void deleteMe();
	void selectAll();
	void viewText();
	void viewAlphabet();
	void setClear();
	void aboutQtopia();
	void aboutAUCR();
	void usingaucrEdit();
	void checkFileMenu();
	void checkEditMenu();
	void checkViewMenu();

signals:      

	
public:
	aucrEdit( QWidget *parent, const char *name, WFlags f );
	~aucrEdit();
	void closeEvent( QCloseEvent * e );
	static QString nameDialog( QString type, QString init );
	static int saveDialog( QString type );
	static void errorDialog( AUCR_Error type );
	bool alfDirty();
	bool textDirty();
	void alfChanged( AUCR_Alphabet_Ptr thisAlf );
	void textChanged( QString thisText );
	QSize sizeHint() const;
	QSizePolicy sizePolicy() const;

private:
	QMenuBar* myMenu;
	QPopupMenu* fileMenu;
	QPopupMenu* editMenu;
	QPopupMenu* viewMenu;
	QPopupMenu* helpMenu;
	QWidgetStack* myStack;
	
	textEdit* TextWidget;
	alfEdit* AlfWidget;
	DocLnk* myAlfFile;
	DocLnk* myTextFile;
	AUCR_Alphabet_Ptr myAlf;
	AUCR_Error error;
	QString* myTextString;
	
	MyMultiLineEdit* textbox;
	recogArea* entry;
	editArea* entry1;
	editArea* entry2;
	editArea* entry3;
	
	FileManager* fm;
	MyFileSelector* whichAlfFile;
	MyFileSelector* whichTextFile;
	Config* defConfig;
};

#endif //AUCREDIT_H
