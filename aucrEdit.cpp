//aucrEdit.cpp

#include "aucrEdit.h"
#include <qaccel.h>

MyDialog::MyDialog( QWidget * parent=0, const char * name=0, bool modal=FALSE, WFlags f=0 ) : QDialog( parent, name, modal, f )
{

}

MyDialog::~MyDialog()
{

}

void MyDialog::save()
{
	this->done( 1 );
}

void MyDialog::cancel()
{
	this->done( 0 );
}

void MyDialog::discard()
{
	this->done( 2 );
}


MyFileSelector::MyFileSelector( const QString & f, QWidget * parent, const char * name = 0, 
											bool newVisible = TRUE, bool closeVisible = TRUE ) : 
											QHBox( parent, name, Qt::WType_Modal ) 
{
	fileBrowse = new FileSelector( f, this, "fileBrowse", newVisible, closeVisible );
	connect( fileBrowse, SIGNAL( fileSelected( const DocLnk & ) ), this, SIGNAL( fileSelected( const DocLnk & ) ) );
	connect( fileBrowse, SIGNAL( newSelected( const DocLnk & ) ), this, SIGNAL( newSelected( const DocLnk & ) ) );
	connect( fileBrowse, SIGNAL( closeMe() ), this, SIGNAL( closeMe() ) );
}

MyFileSelector::~MyFileSelector()
{
	delete fileBrowse;
}

QSize MyFileSelector::sizeHint() const
{
	return QSize( 200, 200 );
}

QSizePolicy MyFileSelector::sizePolicy() const
{
	return QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

void MyFileSelector::reread()
{
	fileBrowse->reread();
}


aucrEdit::aucrEdit( QWidget* parent, const char* name, WFlags f ) : QMainWindow( parent, name, ( f || Qt::WType_TopLevel ) )
{
	int i;
	myMenu = new QMenuBar( this, "myMenu" );
	fileMenu = new QPopupMenu( this, "fileMenu" );
	fileMenu->insertItem( "New Text", this, SLOT( newText() ), 0, -1, 0 );
	fileMenu->insertItem( "New Alphabet", this, SLOT( newAlphabet() ), 0, -1, 1 );
	fileMenu->insertItem( "Open Text...", this, SLOT( openText() ), 0, -1, 2 );
	fileMenu->insertItem( "Open Alphabet...", this, SLOT( openAlphabet() ), 0, -1, 3 ); 
	fileMenu->insertItem( "Save Text", this, SLOT( saveText() ), 0, -1, 4 );
	fileMenu->insertItem( "Save Alphabet", this, SLOT( saveAlphabet() ), 0, -1, 5 );
	fileMenu->insertItem( "Save Text As...", this, SLOT( saveAsText() ), 0, -1, 6 );
	fileMenu->insertItem( "Save Alphabet As...", this, SLOT( saveAsAlphabet() ), 0, -1, 7 );
	fileMenu->insertItem( "Quit", qApp, SLOT( closeAllWindows() ), 0, -1, 8 );
	connect( fileMenu, SIGNAL( aboutToShow() ), this, SLOT( checkFileMenu() ) );
	myMenu->insertItem( "File", fileMenu, -1, 0 );

	editMenu = new QPopupMenu( this, "editMenu" );
	editMenu->insertItem( "Cut", this, SLOT( cut() ), 0, -1, 0 );
	editMenu->insertItem( "Copy", this, SLOT( copy() ), 0, -1, 1 );
	editMenu->insertItem( "Paste", this, SLOT( paste() ), 0, -1, 2 );
	editMenu->insertItem( "Delete", this, SLOT( deleteMe() ), 0, -1, 3 ); 
	editMenu->insertItem( "Select All", this, SLOT( selectAll() ), 0, -1, 4 );
	connect( editMenu, SIGNAL( aboutToShow() ), this, SLOT( checkEditMenu() ) );
	myMenu->insertItem( "Edit", editMenu, -1, 1 );
	
	viewMenu = new QPopupMenu( this, "viewMenu" );
	viewMenu->insertItem( "Text Editor", this, SLOT( viewText() ), 0, -1, 0 );
	viewMenu->insertItem( "Alphabet Editor", this, SLOT( viewAlphabet() ), 0, -1, 1 );
	viewMenu->insertItem( "Always Clear", this, SLOT( setClear() ), 0, -1, 2 );
	viewMenu->setCheckable( true );
	connect( viewMenu, SIGNAL( aboutToShow() ), this, SLOT( checkViewMenu() ) );
	myMenu->insertItem( "View", viewMenu, -1, 2 );
	
	helpMenu = new QPopupMenu( this, "helpMenu" );
	helpMenu->insertItem( "about Qtopia...", this, SLOT( aboutQtopia() ), 0, -1, 0 );
	helpMenu->insertItem( "about AUCR...", this, SLOT( aboutAUCR() ), 0, -1, 1 );
	helpMenu->insertItem( "using aucrEdit...", this, SLOT( usingaucrEdit() ), 0, -1, 2 );
	myMenu->insertItem( "Help", helpMenu, -1, 3 );
	
	myMenu->setSeparator( QMenuBar::InWindowsStyle );
		
	myStack = new QWidgetStack( this, "myStack" );
	TextWidget = new textEdit( myStack, "TextWidget" );
	AlfWidget = new alfEdit( myStack, "AlfWidget" );
	myStack->addWidget( TextWidget, 0 );
	myStack->addWidget( AlfWidget, 1 );
	myStack->raiseWidget( TextWidget );
	this->setCentralWidget( myStack );

	entry = TextWidget->getRecogArea();
	textbox = (MyMultiLineEdit*) TextWidget->getMyMultiLineEdit();
	entry1 = AlfWidget->getEditArea1();
	entry2 = AlfWidget->getEditArea2();
	entry3 = AlfWidget->getEditArea3();
	
	myAlf = NULL;
	myAlfFile = NULL;
	myTextFile = NULL;
	
	DocLnk* tempFile;

	defConfig = new Config( "aucrEdit" );
	defConfig->setGroup( "Common" );
	
	if ( defConfig->hasKey( "alphabetFile" ) )
	{
		tempFile = new DocLnk( defConfig->readEntry( "alphabetFile" ) );
		fm = new FileManager();
		if( fm->exists( *tempFile ) )
		{	
			delete fm;
			this->openAlphabet( *tempFile );
		}
		else
		{
			delete fm;
			defConfig->removeEntry( "alphabetFile" );
		}

		delete tempFile;

	}

	if( defConfig->hasKey( "textFile" ) )
	{
		tempFile = new DocLnk( defConfig->readEntry( "textFile" ) );
		fm = new FileManager();
		if( fm->exists( *tempFile ) )
		{
			delete fm;
			this->openText( *tempFile );
		}
		else
		{
			delete fm;
			defConfig->removeEntry( "textFile" );
		}

		delete tempFile;
		
	}

	if( defConfig->hasKey( "alwaysClear" ) && defConfig->readEntry( "alwaysClear" ) == "yes" )
	{
		viewMenu->setItemChecked( viewMenu->idAt( 2 ), false );
		this->setClear();
	}
	else
	{
		viewMenu->setItemChecked( viewMenu->idAt( 2 ), true );
		this->setClear();
	}

	delete defConfig;

	if( myAlfFile == NULL )
	{
		this->newAlphabet();
	}

	if( myTextFile == NULL )
	{
		this->newText();
	}
	
	
	whichTextFile = new MyFileSelector( "text/plain", this, "whichTextFile", true, true );
	connect( whichTextFile, SIGNAL( fileSelected( const DocLnk& ) ), this, SLOT( openText( const DocLnk& ) ) );
	connect( whichTextFile, SIGNAL( closeMe() ), this, SLOT( callCloseText() ) );
	connect( whichTextFile, SIGNAL( newSelected( const DocLnk& ) ), this, SLOT( newText() ) );
	whichTextFile->setCaption( "Open Text File" );
	
	whichAlfFile = new MyFileSelector( "application/x-aucr-alphabet", this, "whichAlfFile", true, true );
	connect( whichAlfFile, SIGNAL( fileSelected( const DocLnk& ) ), this, SLOT( openAlphabet( const DocLnk& ) ) );
	connect( whichAlfFile, SIGNAL( closeMe() ), this, SLOT( callCloseAlphabet() ) );
	connect( whichAlfFile, SIGNAL( newSelected( const DocLnk& ) ), this, SLOT( newAlphabet() ) );
	whichAlfFile->setCaption( "Open Alphabet File" );
	

	bool startOnalf = false;
	for( i = 1; i < qApp->argc(); i++ )
	{
		if( strcmp( qApp->argv()[i], "-alf" ) == 0 )
		{
			this->viewAlphabet();
			startOnalf = true;
			break;
		}
	}
	if( !startOnalf )
	{
		this->viewText();
	}
}

aucrEdit::~aucrEdit()
{
	delete TextWidget;
	delete AlfWidget;
	
	delete myTextFile;
	delete myAlfFile;
	
	if( whichTextFile )
	{
		whichTextFile->close();
		delete whichTextFile;
	}
	
	if( whichAlfFile )
	{
		whichAlfFile->close();
		delete whichAlfFile;
	}
	
	AUCR_ALPHABET_Release( &myAlf );
}

void aucrEdit::closeEvent( QCloseEvent * e )
{
	defConfig = new Config( "aucrEdit" );
	defConfig->setGroup( "Common" );
	if( viewMenu->isItemChecked( viewMenu->idAt( 2 ) ) )
	{
		defConfig->writeEntry( "alwaysClear", "yes" );
	}
	else
	{
		defConfig->writeEntry( "alwaysClear", "no" );
	}	
	delete defConfig;

	if( textDirty() )
	{
		if( !changedTextSave() )
		{
			e->ignore();
			return;
		}
	}
	if( alfDirty() )
	{
		if( !changedAlphabetSave() )
		{
			e->ignore();
			return;
		}
	}
	
	e->accept();
	return;
}

void aucrEdit::setDocument( const QString& applnk_filename )
{
	DocLnk* tempfile = new DocLnk( applnk_filename );
	
	if( tempfile->isValid() )
	{
		if( tempfile->type() == "text/plain" )
		{
			if( textDirty() )
			{
				if( !changedTextSave() )
				{
					return;
				}
			}
			openText( *tempfile );
		}
		else if( tempfile->type() == "application/x-aucr-alphabet" )
		{
			if( this->alfDirty() )
			{
				if( !changedAlphabetSave() )
				{
					return;
				}
			}
			openAlphabet( *tempfile );
		}
	}
	
	delete tempfile;
	
	return;
}

void aucrEdit::alfChanged( AUCR_Alphabet_Ptr thisAlf )
{
	entry1->setAlphabet( thisAlf );
	entry2->setAlphabet( thisAlf );
	entry3->setAlphabet( thisAlf );
	entry->setAlphabet( thisAlf );
	AlfWidget->setAlphabet( thisAlf );

	return;
}

void aucrEdit::textChanged( QString thisText )
{
	TextWidget->setText( thisText );

	return;
}

void aucrEdit::newText()
{
	if( textDirty() )
	{
		if( !changedTextSave() )
		{
			return;
		}
	}
	
	if( myTextFile != NULL )
	{
		delete myTextFile;
		myTextFile = NULL;
	}
	
	emit textChanged( "" );
	
	this->viewText();
}

void aucrEdit::newAlphabet()
{
	AUCR_Directional_Code_Map_Ptr dir;

	if( this->alfDirty() )
	{
		if( !changedAlphabetSave() )
		{
			return;
		}
	}
	
	dir = NULL;
	error = AUCR_DIRECTIONAL_CODE_MAP_Init( 16, &dir );
	if( error != AUCR_ERR_SUCCESS )
	{
		aucrEdit::errorDialog( error );
		qApp->closeAllWindows();		
	}
	
 	dir->x[0] = 490;
	dir->y[0] = -98;
	dir->x[1] = 490;
	dir->y[1] = 98;
	dir->x[2] = 415;
	dir->y[2] = 278;
	dir->x[3] = 278;
	dir->y[3] = 415;
	dir->x[4] = 98;
	dir->y[4] = 490;
	dir->x[5] = -98;
	dir->y[5] = 490;
	dir->x[6] = -278;
	dir->y[6] = 415;
	dir->x[7] = -415;
	dir->y[7] = -278;
 	dir->x[8] = -490;
	dir->y[8] = 98;
	dir->x[9] = -490;
	dir->y[9] = -98;
	dir->x[10] = -415;
	dir->y[10] = -278;		
	dir->x[11] = -278;
	dir->y[11] = -415;
	dir->x[12] = -98;
	dir->y[12] = -490;
	dir->x[13] = 98;
	dir->y[13] = -490;
	dir->x[14] = 278;
	dir->y[14] = -415;
	dir->x[15] = 415;
	dir->y[15] = -278;


	if( myAlf != NULL )
	{
		AUCR_ALPHABET_Release( &myAlf );
	}
	error = AUCR_ALPHABET_Init( dir, 7, 64, &myAlf );
	AUCR_DIRECTIONAL_CODE_MAP_Release( &dir );
	if( error != AUCR_ERR_SUCCESS )
	{
		aucrEdit::errorDialog( error );
		qApp->closeAllWindows();		
	}
	
	myAlf->activity_regions[0].start = 0;
	myAlf->activity_regions[0].stop = 63;
	myAlf->activity_regions[1].start = 0;
	myAlf->activity_regions[1].stop = 31;
	myAlf->activity_regions[2].start = 32;
	myAlf->activity_regions[2].stop = 63;
	myAlf->activity_regions[3].start = 0;
	myAlf->activity_regions[3].stop = 15;
	myAlf->activity_regions[4].start = 16;
	myAlf->activity_regions[4].stop = 31;
	myAlf->activity_regions[5].start = 32;
	myAlf->activity_regions[5].stop = 47;
	myAlf->activity_regions[6].start = 48;
	myAlf->activity_regions[6].stop = 63;
	
	if( myAlfFile != NULL )
	{
		delete myAlfFile;
		myAlfFile = NULL;
	}
	
	emit alfChanged( myAlf );
	
	this->viewAlphabet();
}

void aucrEdit::openText()
{
	whichTextFile->reread();
	whichTextFile->show();
}

void aucrEdit::openText( const DocLnk& textfile )
{	
	if( textDirty() )
	{
		if( !changedTextSave() )
		{
			return;
		}
	}
	
	QString text;
	DocLnk* tempfile = new DocLnk( textfile );
	
	fm = new FileManager();
	fm->loadFile( *tempfile, text );
	myTextString = new QString( text );
	delete fm;
	
	if( myTextFile != NULL )
	{
		delete myTextFile;
	}
	
	myTextFile = new DocLnk( *tempfile );
	delete tempfile;
	myTextFile->setType( "text/plain" );
	myTextFile->setIcon( "aucredittext" );
	myTextFile->writeLink();
	
	this->textChanged( *myTextString );
	
	this->viewText();
}

void aucrEdit::openAlphabet()
{	
	whichAlfFile->reread();
	whichAlfFile->show();
}

void aucrEdit::openAlphabet( const DocLnk& alffile )
{
	if( alfDirty() )
	{
		if( !changedAlphabetSave() )
		{
			return;
		}
	}

	DocLnk* tempfile = new DocLnk( alffile );
	
	if( myAlf != NULL )
	{
		AUCR_ALPHABET_Release( &myAlf );
		myAlf = NULL;
	}

	error = AUCR_ALPHABET_Open_From_File( tempfile->file(), &myAlf );

	if( error != AUCR_ERR_SUCCESS )
	{
		aucrEdit::errorDialog( error );
		qApp->closeAllWindows();		
	}

	if( myAlfFile != NULL )
	{
		delete myAlfFile;
	}

	myAlfFile = new DocLnk( *tempfile );
	delete tempfile;
	myAlfFile->setType( "application/x-aucr-alphabet" );
	myAlfFile->setIcon( "aucreditalphabet" );
	myAlfFile->writeLink();

	this->alfChanged( myAlf );
	
	this->viewAlphabet();

}

bool aucrEdit::saveText()
{
	if( myTextFile == NULL )
	{
		return saveAsText();
	}
	else
	{
		fm = new FileManager();
		if( fm->saveFile( *myTextFile, textbox->text() ) )
		{
			delete fm;
			
			this->textChanged( (QString) textbox->text() );
			
			if( myStack->visibleWidget() == TextWidget )
			{
				this->viewText();
			}
			
			defConfig = new Config( "aucrEdit" );
			defConfig->setGroup( "Common" );
			defConfig->writeEntry( "textFile", myTextFile->linkFile() );
			delete defConfig;
			
			return true;
		}
		delete fm;
	}
	return false;
}

bool aucrEdit::saveAlphabet()
{
	if( myAlfFile == NULL )
	{
		return saveAsAlphabet();
	}
	else
	{
		fm = new FileManager();
		if( fm->saveFile( *myAlfFile, "t  e  m  p" ) )
		{
			delete fm;
			QString tempname = QString( myAlfFile->linkFile() );
			QString tempfile = QString( myAlfFile->file() );
			delete myAlfFile;
			
			error = AUCR_ALPHABET_Save_To_File( tempfile, myAlf );
	
			if( error == AUCR_ERR_SUCCESS )
			{
				myAlfFile = new DocLnk( tempname );
				this->alfChanged( myAlf );
				
				if( myStack->visibleWidget() == AlfWidget )
				{
					this->viewAlphabet();
				}				
				
				defConfig = new Config( "aucrEdit" );
				defConfig->setGroup("Common" );
				defConfig->writeEntry( "alphabetFile", myAlfFile->linkFile() );
				delete defConfig;
				
				return true;
			}
			else
			{
				aucrEdit::errorDialog( error );
				qApp->closeAllWindows();
			}
		}
	}
	return false;
}

bool aucrEdit::saveAsText()
{
	int state;
	QString preName, postName;
	DocLnk* tempFile = NULL;
	defConfig = new Config( "aucrEdit" );
	defConfig->setGroup( "Common" );
	
	if( myTextFile != NULL )
	{
		preName = myTextFile->name();
		state = 0;
	}
	else if( defConfig->hasKey( "textFile" ) )
	{
		tempFile = new DocLnk( defConfig->readEntry( "textFile" ) );
		preName = tempFile->name();
		state = 1;
	}
	else
	{
		preName = "myTextFile";
		state = 2;
	}
	
	postName = aucrEdit::nameDialog( "Text", preName );
	
	if( postName == "" )
	{
		return false;
	}
	else if( postName == preName )
	{
		if( state == 0 )
		{
			return saveText();
		}
		else if( state == 1 )
		{
			if( myTextFile != NULL )
			{
				delete myTextFile;
			}
			myTextFile = new DocLnk( *tempFile );
			delete tempFile;
			return saveText();
		}
	}
	if( myTextFile != NULL )
	{
		delete myTextFile;
		myTextFile = NULL;
	}
	if( tempFile != NULL )
	{
		delete tempFile;
		tempFile = NULL;
	}
	DocLnkSet* textDoc = new DocLnkSet;
	Global::findDocuments( textDoc, "text/plain" );
	QList<DocLnk> textDocList = textDoc->children();
	bool found = false;
	for( tempFile = textDocList.first(); tempFile != 0; tempFile = textDocList.next() )
	{
		if( tempFile->name() == postName )
		{
			found = true;
			break;
		}
	}
	if( found )
	{
		myTextFile = new DocLnk( *tempFile );
	}
	else
	{
		myTextFile = new DocLnk();
		myTextFile->setName( postName );
		myTextFile->setType( "text/plain" );
		myTextFile->setIcon( "aucredittext" );
		myTextFile->file();
		myTextFile->linkFile();
		myTextFile->writeLink();
	}

	delete textDoc;
	
	return saveText();
}

bool aucrEdit::saveAsAlphabet()
{
	int state;
	QString preName, postName;
	DocLnk* tempFile = NULL;
	defConfig = new Config( "aucrEdit" );
	defConfig->setGroup( "Common" );
	
	if( myAlfFile != NULL )
	{
		preName = myAlfFile->name();
		state = 0;
	}
	else if( defConfig->hasKey( "alphabetFile" ) )
	{
		tempFile = new DocLnk( defConfig->readEntry( "alphabetFile" ) );
		preName = tempFile->name();
		state = 1;
	}
	else
	{
		preName = "myAlfFile";
		state = 2;
	}
	
	postName = aucrEdit::nameDialog( "Alphabet", preName );
	
	if( postName == "" )
	{
		return false;
	}
	else if( postName == preName )
	{
		if( state == 0 )
		{
			return saveAlphabet();
		}
		else if( state == 1 )
		{
			if( myAlfFile != NULL )
			{
				delete myAlfFile;
			}
			myAlfFile = new DocLnk( *tempFile );
			delete tempFile;
			return saveAlphabet();
		}
	}
	if( myAlfFile != NULL )
	{
		delete myAlfFile;
		myAlfFile = NULL;
	}
	if( tempFile != NULL )
	{
		delete tempFile;
		tempFile = NULL;
	}
	DocLnkSet* alfDoc = new DocLnkSet;
	Global::findDocuments( alfDoc, "application/x-aucr-alphabet" );
	QList<DocLnk> alfDocList = alfDoc->children();
	bool found = false;
	for( tempFile = alfDocList.first(); tempFile != 0; tempFile = alfDocList.next() )
	{
		if( tempFile->name() == postName )
		{
			found = true;
			break;
		}
	}
	if( found )
	{
		myAlfFile = new DocLnk( *tempFile );
	}
	else
	{
		myAlfFile = new DocLnk();
		myAlfFile->setName( postName );
		myAlfFile->setType( "application/x-aucr-alphabet" );
		myAlfFile->setIcon( "aucreditalphabet" );
		myAlfFile->file();
		myAlfFile->linkFile();
		myAlfFile->writeLink();
	}

	delete alfDoc;
	
	return saveAlphabet();
}

void aucrEdit::callCloseText()
{
	if( whichTextFile != NULL )
	{
		whichTextFile->hide();
	}
}

void aucrEdit::callCloseAlphabet()
{
	if( whichAlfFile != NULL )
	{
		whichAlfFile->hide();
	}
}

bool aucrEdit::changedTextSave()
{	
	int what;
	bool done = false;

	while( !done )
	{
		what = aucrEdit::saveDialog( "Text" );
		if( what == 1 )
		{
			done = this->saveAsText();
		}
		else
		{
			done = true;
		}
	}
	
	if( what == 0 )
	{
		return false;
	}
	
	return true;
}

bool aucrEdit::changedAlphabetSave()
{
	int what;
	bool done = false;

	while( !done )
	{
		what = aucrEdit::saveDialog( "Alphabet" );
		if( what == 1 )
		{
			done = this->saveAsAlphabet();
		}
		else
		{
			done = true;
		}
	}
	
	if( what == 0 )
	{
		return false;
	}
	
	return true;
}

void aucrEdit::cut()
{
	textbox->cut();
}

void aucrEdit::copy()
{
	textbox->copy();
}

void aucrEdit::paste()
{
	textbox->paste();
}

void aucrEdit::deleteMe()
{
	textbox->myDel();
}

void aucrEdit::selectAll()
{
	textbox->selectAll();
}

void aucrEdit::viewText()
{
	myStack->raiseWidget( TextWidget );
	textbox->setFocus();
	
	if( myTextFile == NULL )
	{
		this->setCaption( "aucrEdit:Text" );
	}
	else
	{
		this->setCaption( "aucrEdit:Text-" + myTextFile->name() );
	}
}

void aucrEdit::viewAlphabet()
{
 	myStack->raiseWidget( AlfWidget );
	
	if( myAlfFile == NULL )
	{
		this->setCaption( "aucrEdit:Alphabet" );
	}
	else
	{
		this->setCaption( "aucrEdit:Alphabet-" + myAlfFile->name() );
	}
}

void aucrEdit::setClear()
{
	if( viewMenu->isItemChecked( viewMenu->idAt( 2 ) ) )
	{
		viewMenu->setItemChecked( viewMenu->idAt( 2 ), false );
		entry->setClear( false );
	}
	else
	{
		viewMenu->setItemChecked( viewMenu->idAt( 2 ), true );
		entry->setClear( true );
	}
}

void aucrEdit::aboutQtopia()
{
	QMessageBox::aboutQt ( this, "About Qtopia" );
}

void aucrEdit::aboutAUCR()
{
	Global::execute( "helpbrowser", "aucr.html" );
}

void aucrEdit::usingaucrEdit()
{
	if( myStack->visibleWidget() == AlfWidget )
	{
		Global::execute( "helpbrowser", "editalphabet.html" );
	}
	else
	{
		Global::execute( "helpbrowser", "edittext.html" );	
	}
}

void aucrEdit::checkFileMenu()
{
	if( this->textDirty() )
	{
		fileMenu->setItemEnabled( fileMenu->idAt( 4 ), true );	
	}
	else
	{
		fileMenu->setItemEnabled( fileMenu->idAt( 4 ), false );
	}
	
	if( this->alfDirty() )
	{
		fileMenu->setItemEnabled( fileMenu->idAt( 5 ), true );		
	}
	else
	{
		fileMenu->setItemEnabled( fileMenu->idAt( 5 ), false );
	}
}

void aucrEdit::checkEditMenu()
{
	if( myStack->visibleWidget() != TextWidget )
	{
		editMenu->setItemEnabled( editMenu->idAt( 0 ), false );
		editMenu->setItemEnabled( editMenu->idAt( 1 ), false );
		editMenu->setItemEnabled( editMenu->idAt( 2 ), false );
		editMenu->setItemEnabled( editMenu->idAt( 3 ), false );
		editMenu->setItemEnabled( editMenu->idAt( 4 ), false );		
	}
	else
	{
		if( qApp->clipboard()->text() == NULL )
		{
			editMenu->setItemEnabled( editMenu->idAt( 2 ), false );
		}
		else
		{
			editMenu->setItemEnabled( editMenu->idAt( 2 ), true );		
		}
		
		if( textbox->myHasMarkedText() )
		{
			editMenu->setItemEnabled( editMenu->idAt( 0 ), true );
			editMenu->setItemEnabled( editMenu->idAt( 1 ), true );		
			editMenu->setItemEnabled( editMenu->idAt( 3 ), true );			
		}
		else
		{
			editMenu->setItemEnabled( editMenu->idAt( 0 ), false );
			editMenu->setItemEnabled( editMenu->idAt( 1 ), false );		
			editMenu->setItemEnabled( editMenu->idAt( 3 ), false );		
		}
		editMenu->setItemEnabled( editMenu->idAt( 4 ), true );		
	}
}

void aucrEdit::checkViewMenu()
{
	if( myStack->visibleWidget() == TextWidget )
	{
		viewMenu->setItemEnabled( viewMenu->idAt( 0 ), false );
		viewMenu->setItemEnabled( viewMenu->idAt( 1 ), true );
		viewMenu->setItemEnabled( viewMenu->idAt( 2 ), true );
	}
	else
	{
		viewMenu->setItemEnabled( viewMenu->idAt( 0 ), true );
		viewMenu->setItemEnabled( viewMenu->idAt( 1 ), false );
		viewMenu->setItemEnabled( viewMenu->idAt( 2 ), false );
	}
}

bool aucrEdit::alfDirty()
{
	return ( entry1->isDirty() || entry2->isDirty() || entry3->isDirty() );
}

bool aucrEdit::textDirty()
{
	return textbox->edited();
}

QSize aucrEdit::sizeHint() const
{
    return QSize( 220, 300 );
}

QSizePolicy aucrEdit::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

QString aucrEdit::nameDialog( QString type, QString init )
{
	QString name;
	QDialog *dlg = new QDialog( 0, "fndialog", TRUE );
	dlg->setCaption( "Edit name" );

	QVBoxLayout *dlgLay = new QVBoxLayout( dlg, 6 );
	
	QLabel *svtext = new QLabel( "Name for " + type + " File:", dlg );
	svtext->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
	dlgLay->addWidget( svtext );
	QLineEdit *fnedit = new QLineEdit( init, dlg, "fnedit" );
	dlgLay->addWidget( fnedit );

	QHBoxLayout *buttons = new QHBoxLayout( dlgLay, 6 );
	QPushButton *ok, *cancel;
	ok = new QPushButton( "OK", dlg );
	ok->setDefault(TRUE);
	connect( ok, SIGNAL(clicked()), dlg, SLOT(accept()) );
	
	cancel = new QPushButton( "Cancel", dlg );
	connect( cancel, SIGNAL(clicked()), dlg, SLOT(reject()) );
	
	buttons->addWidget( ok );
	buttons->addWidget( cancel );
	
	fnedit->selectAll();
	fnedit->setFocus();

	int resultCode = dlg->exec();

	if ( resultCode == QDialog::Accepted )
	{
		name = fnedit->text();
	}
	if ( resultCode == QDialog::Rejected )
	{
		name = QString("");
	}
	
	delete dlg;
	
	return name;
}

int aucrEdit::saveDialog( QString type )
{

	MyDialog *dlg = new MyDialog( 0, "svdialog", TRUE );
	dlg->setCaption( "Confirm" );
	
	QVBoxLayout *dlgLay = new QVBoxLayout( dlg, 6 );
	QLabel *svtext = new QLabel( type + " File was modified,\nsave changes?", dlg );
	svtext->setAlignment( Qt::AlignCenter | Qt::AlignVCenter );
	dlgLay->addWidget( svtext );

	QHBoxLayout *buttons = new QHBoxLayout( dlgLay, 6 );
	QPushButton *save, *discard, *cancel;
	
	save = new QPushButton( "Save", dlg );
	connect( save, SIGNAL( clicked() ), dlg, SLOT( save() ) );
	save->setDefault(TRUE);
	
	discard = new QPushButton( "Discard", dlg );
	connect( discard, SIGNAL( clicked() ), dlg, SLOT( discard() ) );
	
	cancel = new QPushButton( "Cancel", dlg );
	connect( cancel, SIGNAL( clicked() ), dlg, SLOT( cancel() ) );
	
	buttons->addWidget( save );
	buttons->addWidget( discard );
	buttons->addWidget( cancel );

	int resultCode = dlg->exec();
	
	delete dlg;
	
	return resultCode;
}

void aucrEdit::errorDialog( AUCR_Error type )
{
	QString error; 
	switch( type )
	{
		case AUCR_ERR_FAILED:
			error = QString( "Error of type\nAUCR_ERR_FAILED.\nThe application must now close." );
			break;
		case AUCR_ERR_NO_MEMORY:
			error = QString( "Error of type\nAUCR_ERR_NO_MEMORY.\nThe application must now close." );
			break;
		case AUCR_ERR_CONFLICTING_PARAMETERS:
			error = QString( "Error of type\nAUCR_ERR_CONFLICTING_PARAMETERS.\nThe application must now close." );
			break;
		default:
			break;
	}
	
	QDialog *dlg = new QDialog( 0, "mdialog", TRUE );
	dlg->setCaption( "Error" );

	QVBoxLayout *dlgLay = new QVBoxLayout( dlg, 6 );
	QLabel *ftext = new QLabel( error, dlg );
	ftext->setAlignment( Qt::AlignCenter | Qt::AlignVCenter );
	dlgLay->addWidget( ftext );

	QPushButton *ok;
	ok = new QPushButton( "OK", dlg );
	connect( ok, SIGNAL(clicked()), dlg, SLOT(accept()) );
	ok->setDefault(TRUE);
	dlgLay->addWidget( ok );
	dlg->exec();
	delete dlg;
}

int main( int argc, char* argv[] )
{
	QPEApplication inter( argc, argv );

	aucrEdit *mainwidget = new aucrEdit( 0, "mainwidget", Qt::WStyle_ContextHelp );

	inter.showMainDocumentWidget( mainwidget );
	
	return inter.exec();
}
