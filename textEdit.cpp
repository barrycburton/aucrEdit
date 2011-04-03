//textEdit.cpp

#include "textEdit.h"

MyMultiLineEdit::MyMultiLineEdit( QWidget* parent, const char* name ) : QMultiLineEdit( parent, name )
{}

bool MyMultiLineEdit::myHasMarkedText()
{
	return this->hasMarkedText();
} 

void MyMultiLineEdit::myDel()
{
	this->del();
}

void MyMultiLineEdit::setFound( wchar_t uni )
{
	if( uni == L'\b' )
	{
		this->backspace();
	}
	else if( uni == L'\r' )
	{
		this->newLine();
	}
	else
	{
		this->insert( QString( QChar( uni ) ) );
	}
}

void MyMultiLineEdit::myEnd()
{
	this->setCursorPosition( this->numLines() - 1, this->lineLength( this->numLines() - 1 ) );
}

textEdit::textEdit( QWidget* parent, const char* name ) : QWidget( parent, name )
{		
	mainLay = new QVBoxLayout( this ); 
	mainLay->setSpacing( 2 );
	mainLay->setMargin( 3 );
	
	//text box
	textbox = new MyMultiLineEdit ( this, "textbox" );
	textbox->setUndoEnabled( TRUE );
	textbox->setWordWrap( QMultiLineEdit::WidgetWidth );
	textbox->setWrapPolicy( QMultiLineEdit::AtWhiteSpace );
	mainLay->addWidget( textbox );
	
	//spacer
	topShim = new QSpacerItem( 10, 5, QSizePolicy::Expanding, QSizePolicy::Fixed );		
	mainLay->addItem( topShim );
		
	//draw area label
	textLab = new QLabel( QString( "Handwriting Area" ), this, "textLab" );
	mainLay->addWidget( textLab );
	
	//draw area
	entry = new recogArea( this, "entry" );
	mainLay->addWidget( entry );
	
	//spacer
	bottomShim = new QSpacerItem( 10, 5, QSizePolicy::Expanding, QSizePolicy::Fixed );		
	mainLay->addItem( bottomShim );
	
	//signals and slots connections
	connect( entry, SIGNAL( found( wchar_t ) ), textbox, SLOT( setFound( wchar_t ) ) );
	
	//set focus
	textbox->setFocus();
	
}

textEdit::~textEdit()
{
	delete textbox;
	delete entry;
}

void textEdit::setText( QString newtext )
{
	textbox->setText( newtext );
	textbox->myEnd();
	textbox->setEdited( false );
	entry->setText();
}

QMultiLineEdit* textEdit::getMyMultiLineEdit()
{
	return textbox;
}

recogArea* textEdit::getRecogArea()
{
	return entry;
}

QSize textEdit::sizeHint() const
{
	return QSize( 210, 290 );
}

QSizePolicy textEdit::sizePolicy() const
{
	return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}
