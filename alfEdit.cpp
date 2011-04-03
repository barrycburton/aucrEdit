//alfEdit.cpp

#include "alfEdit.h"

alfEdit::alfEdit( QWidget* parent, const char* name ) : QWidget( parent, name )
{	
	int i;
	state = 0;
	current = '\0';
	start = false;
	myAlf = NULL;

	qtext[0][0] = QString( "0" );
	qtext[0][1] = QString( "1" );
	qtext[0][2] = QString( "2" );
	qtext[0][3] = QString( "3" );
	qtext[0][4] = QString( "4" );
	qtext[0][5] = QString( "5" );
	qtext[0][6] = QString( "6" );
	qtext[0][7] = QString( "7" );
	qtext[0][8] = QString( "8" );
	qtext[0][9] = QString( "9" );
	qtext[0][10] = QString( "a" );
	qtext[0][11] = QString( "b" );
	qtext[0][12] = QString( "c" );
	qtext[0][13] = QString( "d" );
	qtext[0][14] = QString( "e" );
	qtext[0][15] = QString( "f" );
	qtext[0][16] = QString( "g" );
	qtext[0][17] = QString( "h" );
	qtext[0][18] = QString( "i" );
	qtext[0][19] = QString( "j" );
	qtext[0][20] = QString( "k" );
	qtext[0][21] = QString( "l" );
	qtext[0][22] = QString( "m" );
	qtext[0][23] = QString( "n" );
	qtext[0][24] = QString( "o" );
	qtext[0][25] = QString( "p" );
	qtext[0][26] = QString( "q" );
	qtext[0][27] = QString( "r" );
	qtext[0][28] = QString( "s" );
	qtext[0][29] = QString( "t" );
	qtext[0][30] = QString( "u" );
	qtext[0][31] = QString( "v" );
	qtext[0][32] = QString( "w" );
	qtext[0][33] = QString( "x" );
	qtext[0][34] = QString( "y" );
	qtext[0][35] = QString( "z" );
	qtext[0][36] = QString( "`" );
	qtext[0][37] = QString( "-" );
	qtext[0][38] = QString( "=" );
	qtext[0][39] = QString( "Bksp" );
	qtext[0][40] = QString( "Tab" );
	qtext[0][41] = QString( "[" );
	qtext[0][42] = QString( "]" );
	qtext[0][43] = QString( "\\" );
	qtext[0][44] = QString( ";" );
	qtext[0][45] = QString( "\'" );
	qtext[0][46] = QString( "Enter" );
	qtext[0][47] = QString( "," );
	qtext[0][48] = QString( "." );
	qtext[0][49] = QString( "/" );
	qtext[0][50] = QString( "Space" );
	
	qtext[1][0] = QString( ")" );												 
	qtext[1][1] = QString( "!" );
	qtext[1][2] = QString( "@" );
	qtext[1][3] = QString( "#" );
	qtext[1][4] = QString( "$" );
	qtext[1][5] = QString( "%" );
	qtext[1][6] = QString( "^" );
	qtext[1][7] = QString( "&" );
	qtext[1][8] = QString( "*" );
	qtext[1][9] = QString( "(" );
	qtext[1][10] = QString( "A" );
	qtext[1][11] = QString( "B" );
	qtext[1][12] = QString( "C" );
	qtext[1][13] = QString( "D" );
	qtext[1][14] = QString( "E" );
	qtext[1][15] = QString( "F" );
	qtext[1][16] = QString( "G" );
	qtext[1][17] = QString( "H" );
	qtext[1][18] = QString( "I" );
	qtext[1][19] = QString( "J" );
	qtext[1][20] = QString( "K" );
	qtext[1][21] = QString( "L" );
	qtext[1][22] = QString( "M" );
	qtext[1][23] = QString( "N" );
	qtext[1][24] = QString( "O" );
	qtext[1][25] = QString( "P" );	
	qtext[1][26] = QString( "Q" );
	qtext[1][27] = QString( "R" );
	qtext[1][28] = QString( "S" );
	qtext[1][29] = QString( "T" );
	qtext[1][30] = QString( "U" );
	qtext[1][31] = QString( "V" );
	qtext[1][32] = QString( "W" );
	qtext[1][33] = QString( "X" );
	qtext[1][34] = QString( "Y" );
	qtext[1][35] = QString( "Z" );
	qtext[1][36] = QString( "~" );
	qtext[1][37] = QString( "_" );
	qtext[1][38] = QString( "+" );
	qtext[1][39] = QString( "Bksp" );
	qtext[1][40] = QString( "Tab" );
	qtext[1][41] = QString( "{" );
	qtext[1][42] = QString( "}" );
	qtext[1][43] = QString( "|" );
	qtext[1][44] = QString( ":" );
	qtext[1][45] = QString( "\"" );
	qtext[1][46] = QString( "Enter" );
	qtext[1][47] = QString( "<" );
	qtext[1][48] = QString( ">" );
	qtext[1][49] = QString( "?" );
	qtext[1][50] = QString( "Space" );

	
	unicode[0][0] = L'0';
	unicode[0][1] = L'1';
	unicode[0][2] = L'2';
	unicode[0][3] = L'3';
	unicode[0][4] = L'4';
	unicode[0][5] = L'5';
	unicode[0][6] = L'6';
	unicode[0][7] = L'7';
	unicode[0][8] = L'8';
	unicode[0][9] = L'9';
	unicode[0][10] = L'a';
	unicode[0][11] = L'b';
	unicode[0][12] = L'c';
	unicode[0][13] = L'd';
	unicode[0][14] = L'e';
	unicode[0][15] = L'f';
	unicode[0][16] = L'g';
	unicode[0][17] = L'h';
	unicode[0][18] = L'i';
	unicode[0][19] = L'j';
	unicode[0][20] = L'k';
	unicode[0][21] = L'l';
	unicode[0][22] = L'm';
	unicode[0][23] = L'n';
	unicode[0][24] = L'o';
	unicode[0][25] = L'p';
	unicode[0][26] = L'q';
	unicode[0][27] = L'r';
	unicode[0][28] = L's';
	unicode[0][29] = L't';
	unicode[0][30] = L'u';
	unicode[0][31] = L'v';
	unicode[0][32] = L'w';
	unicode[0][33] = L'x';
	unicode[0][34] = L'y';
	unicode[0][35] = L'z';
	unicode[0][36] = L'`';
	unicode[0][37] = L'-';
	unicode[0][38] = L'=';
	unicode[0][39] = L'\b';
	unicode[0][40] = L'\t';
	unicode[0][41] = L'[';
	unicode[0][42] = L']';
	unicode[0][43] = L'\\';
	unicode[0][44] = L';';
	unicode[0][45] = L'\'';
	unicode[0][46] = L'\r';
	unicode[0][47] = L',';
	unicode[0][48] = L'.';
	unicode[0][49] = L'/';
	unicode[0][50] = L' ';
	
	unicode[1][0] = L')';
	unicode[1][1] = L'!';
	unicode[1][2] = L'@';
	unicode[1][3] = L'#';
	unicode[1][4] = L'$';
	unicode[1][5] = L'%';
	unicode[1][6] = L'^';
	unicode[1][7] = L'&';
	unicode[1][8] = L'*';
	unicode[1][9] = L'(';
	unicode[1][10] = L'A';
	unicode[1][11] = L'B';
	unicode[1][12] = L'C';
	unicode[1][13] = L'D';
	unicode[1][14] = L'E';
	unicode[1][15] = L'F';
	unicode[1][16] = L'G';
	unicode[1][17] = L'H';
	unicode[1][18] = L'I';
	unicode[1][19] = L'J';
	unicode[1][20] = L'K';
	unicode[1][21] = L'L';
	unicode[1][22] = L'M';
	unicode[1][23] = L'N';
	unicode[1][24] = L'O';
	unicode[1][25] = L'P';
	unicode[1][26] = L'Q';
	unicode[1][27] = L'R';
	unicode[1][28] = L'S';
	unicode[1][29] = L'T';
	unicode[1][30] = L'U';
	unicode[1][31] = L'V';
	unicode[1][32] = L'W';
	unicode[1][33] = L'X';
	unicode[1][34] = L'Y';
	unicode[1][35] = L'Z';
	unicode[1][36] = L'~';
	unicode[1][37] = L'_';
	unicode[1][38] = L'+';
	unicode[1][39] = L'\b';
	unicode[1][40] = L'\t';
	unicode[1][41] = L'{';
	unicode[1][42] = L'}';
	unicode[1][43] = L'|';
	unicode[1][44] = L':';
	unicode[1][45] = L'\"';
	unicode[1][46] = L'\r';
	unicode[1][47] = L'<';
	unicode[1][48] = L'>';
	unicode[1][49] = L'?';
	unicode[1][50] = L' ';

	mainLay = new QVBoxLayout( this ); 
	mainLay->setSpacing( 2 );
	mainLay->setMargin( 5 );


	topLab = new QLabel( QString( "Character Editor" ), this, "topLab" );
	mainLay->addWidget( topLab );

	
	topLay = new QHBoxLayout();
	topLay->setSpacing( 5 );
	topLay->setMargin( 0 );
	mainLay->addLayout( topLay );	
	
	entry1 = new editArea( this, "entry1" );
	topLay->addWidget( entry1 );

	entry2 = new editArea( this, "entry2" );
	topLay->addWidget( entry2 );

	entry3 = new editArea( this, "entry3" );
	topLay->addWidget( entry3 );

	topShim = new QSpacerItem( 10, 5, QSizePolicy::Expanding, QSizePolicy::Expanding );
	mainLay->addItem( topShim );

	middleLay = new QHBoxLayout();
	middleLay->setSpacing( 5 );
	middleLay->setMargin( 0 );
	mainLay->addLayout( middleLay );
	
	currentLab = new QLabel( QString( "Current Character: "), this, "currentLab" );
	middleLay->addWidget( currentLab );

	charLab = new QLabel( QString(" "), this, "charLab" );
	middleLay->addWidget( charLab );

	middleShim = new QSpacerItem( 10, 5, QSizePolicy::Expanding, QSizePolicy::Expanding );
	mainLay->addItem( middleShim );
		
	keyboard = new QButtonGroup( "keyboard" );
	keyboard->setExclusive( TRUE );
		 
	QFont keyboard_font( keyboard->font() );
	keyboard_font.setFamily( "adobe-helvetica" );
	keyboard_font.setPointSize( 8 );
	keyboard->setFont( keyboard_font );
	
	for( i = 0; i <= 50; i++ )
	{			
		keys[i] = new QPushButton( this, qtext[0][i].latin1() );
		keys[i]->setFixedSize( 15, 15 );
		keys[i]->setText( qtext[0][i] );
		keys[i]->setToggleButton( TRUE );
		keys[i]->setFont( keyboard_font );
		keyboard->insert( keys[i] );
	}
	
	keys[39]->setFixedSize( 30, 15 );
	
	keys[40]->setFixedSize( 23, 15 );
	
	keys[43]->setFixedSize( 22, 15 );
	
	keys[46]->setFixedSize( 30, 15 );
	
	keys[50]->setFixedSize( 93, 15 );
	

	lshift = new QPushButton( this, "lshift" );
	lshift->setFixedSize( 38, 15 );
	lshift->setText( tr( "Shift" ) );
	lshift->setToggleButton( TRUE );
	lshift->setFont( keyboard_font );

	rshift = new QPushButton( this, "rshift" );
	rshift->setFixedSize( 37, 15 );
	rshift->setText( tr( "Shift" ) );
	rshift->setToggleButton( TRUE );
	rshift->setFont( keyboard_font );

	caps = new QPushButton( this, "caps" );
	caps->setFixedSize( 30, 15 );
	caps->setText( tr( "Caps" ) );
	caps->setToggleButton( TRUE );
	caps->setFont( keyboard_font );
	
	keyLshim = new QSpacerItem( 59, 15, QSizePolicy::Fixed, QSizePolicy::Fixed );
	keyRshim = new QSpacerItem( 73, 15, QSizePolicy::Fixed, QSizePolicy::Fixed );
	
	keyLay = new QVBoxLayout();
	keyLay->setSpacing( 1 );
	keyLay->setMargin( 0 );
	mainLay->addLayout( keyLay );  
	
	row1Lay = new QHBoxLayout();
	row1Lay->setSpacing( 0 );
	row1Lay->setMargin( 0 );
	keyLay->addLayout( row1Lay );
		
	row2Lay = new QHBoxLayout();
	row2Lay->setSpacing( 0 );
	row2Lay->setMargin( 0 );	
	keyLay->addLayout( row2Lay );
		
	row3Lay = new QHBoxLayout();
	row3Lay->setSpacing( 0 );
	row3Lay->setMargin( 0 );	
	keyLay->addLayout( row3Lay ); 
		
	row4Lay = new QHBoxLayout();
	row4Lay->setSpacing( 0 );
	row4Lay->setMargin( 0 );	
	keyLay->addLayout( row4Lay );
		
	row5Lay = new QHBoxLayout();
	row5Lay->setSpacing( 0 );
	row5Lay->setMargin( 0 );	
	keyLay->addLayout( row5Lay ); 					

	row1Lay->addWidget( keys[36] );
	row1Lay->addWidget( keys[1] );
	row1Lay->addWidget( keys[2] );
	row1Lay->addWidget( keys[3] );
	row1Lay->addWidget( keys[4] );
	row1Lay->addWidget( keys[5] );
	row1Lay->addWidget( keys[6] );
	row1Lay->addWidget( keys[7] );
	row1Lay->addWidget( keys[8] );
	row1Lay->addWidget( keys[9] );
	row1Lay->addWidget( keys[0] );
	row1Lay->addWidget( keys[37] );
	row1Lay->addWidget( keys[38] );
	row1Lay->addWidget( keys[39] );
	 
	
	row2Lay->addWidget( keys[40] );
	row2Lay->addWidget( keys[26] );
	row2Lay->addWidget( keys[32] );
	row2Lay->addWidget( keys[14] );
	row2Lay->addWidget( keys[27] );
	row2Lay->addWidget( keys[29] );
	row2Lay->addWidget( keys[34] );
	row2Lay->addWidget( keys[30] );
	row2Lay->addWidget( keys[18] );
	row2Lay->addWidget( keys[24] );
	row2Lay->addWidget( keys[25] );
	row2Lay->addWidget( keys[41] );
	row2Lay->addWidget( keys[42] );
	row2Lay->addWidget( keys[43] );

	
	row3Lay->addWidget( caps );
	row3Lay->addWidget( keys[10] );
	row3Lay->addWidget( keys[28] );
	row3Lay->addWidget( keys[13] );
	row3Lay->addWidget( keys[15] );
	row3Lay->addWidget( keys[16] );
	row3Lay->addWidget( keys[17] );
	row3Lay->addWidget( keys[19] );
	row3Lay->addWidget( keys[20] );
	row3Lay->addWidget( keys[21] );
	row3Lay->addWidget( keys[44] );
	row3Lay->addWidget( keys[45] );  
	row3Lay->addWidget( keys[46] );


	row4Lay->addWidget( lshift );
	row4Lay->addWidget( keys[35] );
	row4Lay->addWidget( keys[33] );
	row4Lay->addWidget( keys[12] );
	row4Lay->addWidget( keys[31] );
	row4Lay->addWidget( keys[11] );
	row4Lay->addWidget( keys[23] );
	row4Lay->addWidget( keys[22] );
	row4Lay->addWidget( keys[47] );
	row4Lay->addWidget( keys[48] );
	row4Lay->addWidget( keys[49] );
	row4Lay->addWidget( rshift );
 

	row5Lay->addItem( keyLshim );
	row5Lay->addWidget( keys[50] );
	row5Lay->addItem( keyRshim );
			
	bottomShim = new QSpacerItem( 10, 5, QSizePolicy::Expanding, QSizePolicy::Expanding );
	mainLay->addItem( bottomShim );			


	// signals and slots connections
	connect( keyboard, SIGNAL( clicked( int ) ), this, SLOT( setCurrent( int ) ) );
	connect( lshift, SIGNAL( toggled( bool ) ), this, SLOT( setShift( bool ) ) );
	connect( rshift, SIGNAL( toggled( bool ) ), this, SLOT( setShift( bool ) ) );
	connect( caps, SIGNAL( toggled( bool ) ), this, SLOT( setCaps() ) );
	
	
	//setup keys for entry
	keys[26]->setFocus();
	keys[26]->animateClick();
	
}

alfEdit::~alfEdit()
{
	delete entry1;
	delete entry2;	
	delete entry3;
}

editArea* alfEdit::getEditArea1()
{
	return entry1;
}

editArea* alfEdit::getEditArea2()
{
	return entry2;
}

editArea* alfEdit::getEditArea3()
{
	return entry3;
}

void alfEdit::update()
{
	int i;
	if( lshift->isOn() && caps->isOn() )
	{
		for( i = 0; i <= 9; i++ )
		{
			keys[i]->setText( qtext[1][i] );
		}
		for( i = 10; i <= 35; i++ )
		{
			keys[i]->setText( qtext[0][i] );
		}
		for( i = 36; i <= 50; i++ )
		{
			keys[i]->setText( qtext[1][i] );
		}		
	}
	else if( caps->isOn() )
	{
		for( i = 0; i <= 9; i++ )
		{
			keys[i]->setText( qtext[0][i] );
		}
		for( i = 10; i <= 35; i++ )
		{
			keys[i]->setText( qtext[1][i] );
		}
		for( i = 36; i <= 50; i++ )
		{
			keys[i]->setText( qtext[0][i] );
		}	
	}
	else
	{
		for( i = 0; i <= 50; i++ )
		{
			keys[i]->setText( qtext[state][i] );
		}
	}
}

void alfEdit::setCurrent( int n )
{
	int tempState;
	
	if( !start && myAlf != NULL )
	{
		start = true;
		this->startChanged( start );
	}
	
	if( lshift->isOn() && caps->isOn() )
	{
		tempState = ( ( n < 10 ) || ( n > 35 ) ) ? 1 : 0;
	}
	else if( caps->isOn() )
	{
		tempState = ( ( n < 10 ) || ( n > 35 ) ) ? 0 : 1;
	}
	else
	{
		tempState = state;
	}
	
	if( current != unicode[tempState][n] )
	{
		current = unicode[tempState][n];
		this->currentChanged( current );
	}
	
	charLab->setText( qtext[tempState][n] );	
	last = (QPushButton*)keyboard->find( n );
	if( lshift->isOn() )
	{
		lshift->setOn( false );
		rshift->setOn( false );
		last->setOn( false );
		state = 0;
		this->update();
	}
	
	if( myAlf != NULL )
	{
		this->dispCurrent();
	}
}
	
void alfEdit::dispCurrent()
{  
	/*search through alphabet, looking for all instances of current character*/
	int i, found = 0;
	if( myAlf != NULL )
	{
		for( i = 0; i < myAlf->num_characters; i++ )
		{
			if( current == myAlf->characters_ptr_ptr[i]->unicode )
			{
				if( found == 2 )
				{
					entry3->dispLetter( myAlf->characters_ptr_ptr[i] );
					found++;
					break;
				}
				else if( found == 1 )
				{
					entry2->dispLetter( myAlf->characters_ptr_ptr[i] );
					found++;
				}
				else if( found == 0 )
				{
					entry1->dispLetter( myAlf->characters_ptr_ptr[i] );
					found++;
				}
			}
		}
	}
	
	if( found == 0 )
	{
		entry1->dispLetter( NULL );
		entry2->dispLetter( NULL );
		entry3->dispLetter( NULL );
	}
	else if( found == 1 )
	{
		entry2->dispLetter( NULL );
		entry3->dispLetter( NULL );
	}
	else if( found == 2 )
	{
		entry3->dispLetter( NULL );
	}
}

void alfEdit::setShift( bool on )
{
	if( on )
	{
		if( !lshift->isOn() )
		{ 
			lshift->setOn( true );
		}
		if( !rshift->isOn() )
		{
			rshift->setOn( true );
		}
		state = 1;
	}
	else
	{
		if( lshift->isOn() )
		{
			lshift->setOn( false );
		}
		if( rshift->isOn() )
		{
			rshift->setOn( false );
		}
		state = 0;
	}
	last->setOn( false );
	this->update();	  
}

void alfEdit::setCaps()
{
	last->setOn( false );
	this->update();
}

void alfEdit::setAlphabet( AUCR_Alphabet_Ptr alf )
{
	if( ( myAlf == NULL ) && ( current != '\0' ) && ( alf != NULL ) )
	{
		start = true;
		this->startChanged( start );
	}
	else if( start && ( alf == NULL ) )
	{
		start = false;
		this->startChanged( start );
	}

	myAlf = alf;

	this->dispCurrent();
}

void alfEdit::currentChanged( wchar_t myUnicodeChar )
{
	entry1->setCurrent( myUnicodeChar );
	entry2->setCurrent( myUnicodeChar );	
	entry3->setCurrent( myUnicodeChar );	

	return;
}

void alfEdit::startChanged( bool myStartedYet )
{
	entry1->setStart( myStartedYet );
	entry2->setStart( myStartedYet );
	entry3->setStart( myStartedYet );

	return;
}

QSize alfEdit::sizeHint() const
{
	return QSize( 210, 290 );
}

QSizePolicy alfEdit::sizePolicy() const
{
	return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}
