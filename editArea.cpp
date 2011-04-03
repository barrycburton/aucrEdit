//editArea.cpp

#include "editArea.h"

editArea::editArea( QWidget *parent, const char *name ) : QWidget( parent, name )
{
	myCharacter = NULL;
	myStart = false;
	dirty = false;
	charChanged = false;
	setMouseTracking( TRUE );
	repaint( TRUE );
	error = AUCR_INTERPOLATED_CHARACTER_Init( 0x0001, 64, &myInterpolated );
	if( error != AUCR_ERR_SUCCESS )
	{
		aucrEdit::errorDialog( error );
		qApp->closeAllWindows();
	}
	numCords = 1000;
	myCords = (AUCR_Coordinate_Ptr)malloc( numCords * sizeof( AUCR_Coordinate ) );
	if( myCords == NULL )
	{
		error = AUCR_ERR_NO_MEMORY;
		aucrEdit::errorDialog( error );
		qApp->closeAllWindows();
	}
	timer = new QTimer( this );
	connect( timer, SIGNAL(timeout()), this, SLOT(dispLetter()) ); 
}

editArea::~editArea()
{
	free( myCords );
	AUCR_INTERPOLATED_CHARACTER_Release( &myInterpolated );
}

void editArea::dispLetter( AUCR_Character_Ptr newchar )
{
	myCharacter = newchar;
	charChanged = true;
	this->dispLetter();
}

void editArea::dispLetter( void )
{
	if( !mousePressed )
	{
		this->paintEvent();
	}
}

void editArea::clear()
{
	QPainter painter( this );
	painter.setBrush( Qt::white );
	painter.setPen( Qt::black );
	painter.drawRect( 0, 0, this->width(), this->height() );
	painter.setPen( QPen( Qt::black, 1, DashDotLine ) );
	painter.drawLine( 0, this->height() - 24, this->width(), this->height() - 24 );
	painter.end();
}

void editArea::paintEvent( QPaintEvent *e )
{
	if( e ){}
	int i;
	clear();

	if( myCharacter != NULL && myAlf != NULL )
	{
		if( charChanged )
		{
			error = AUCR_INTERPOLATED_CHARACTER_From_Character(
				myCharacter, myAlf, this->width(), this->height(), 15, myInterpolated );
			if( error != AUCR_ERR_SUCCESS )
			{
				aucrEdit::errorDialog( error );
				qApp->closeAllWindows();		
			}
			charChanged = false;
		}
		
		QPainter painter( this );
		painter.setPen( QPen( Qt::black, 1, SolidLine ) );
		painter.setBrush( Qt::darkGray );
		painter.drawEllipse( myInterpolated->coordinates[0].x - 2, this->height() - myInterpolated->coordinates[0].y - 2, 4, 4 );		
		painter.setPen( QPen( Qt::darkGray, 2, SolidLine ) );		
		for( i = 1; i <= myInterpolated->num_directional_codes; i++ )
		{
			painter.drawLine( myInterpolated->coordinates[i-1].x, this->height() - myInterpolated->coordinates[i-1].y, 
				myInterpolated->coordinates[i].x, this->height() - myInterpolated->coordinates[i].y );
		}
		painter.end();
	}
}

void editArea::mousePressEvent( QMouseEvent* e )
{
	if( myStart )
	{
		mousePressed = TRUE;
		clear();
		n = 0;
		myCords[n].x=(e->pos()).x();
		myCords[n].y=(this->height() - (e->pos()).y() );
		n++;
		QPainter painter( this );
		painter.setPen( QPen( Qt::black, 1, SolidLine ) );
		painter.setBrush( Qt::darkGray );
		painter.drawEllipse( (e->pos()).x() - 1, (e->pos()).y() - 1, 2, 2 );
		painter.end();
	}
}

void editArea::mouseReleaseEvent( QMouseEvent* e )
{
	if( mousePressed && myStart )
	{
		mousePressed = FALSE;
		myCords[n].x=(e->pos()).x();
		myCords[n].y=(this->height() - (e->pos()).y() );
	
		QPainter painter( this );
		painter.setPen( QPen( Qt::darkGray, 1, SolidLine ) );
		painter.drawLine( myCords[n-1].x, this->height() - myCords[n-1].y, (e->pos()).x(), (e->pos()).y() );
		painter.end();
	
		n++;

		if( myCharacter == NULL )
		{
			error = AUCR_ALPHABET_Add_Raw( myCurrent, n, myCords, myAlf);
			if( error != AUCR_ERR_SUCCESS )
			{
				aucrEdit::errorDialog( error );
				qApp->closeAllWindows();						
			}

			dirty = true;
			charChanged = true;
			
			myCharacter = myAlf->characters_ptr_ptr[myAlf->num_characters - 1];
		}
		else
		{
			error = AUCR_INTERPOLATED_CHARACTER_From_Raw(
				n, myCords, myInterpolated );
			if( error != AUCR_ERR_SUCCESS )
			{
				aucrEdit::errorDialog( error );
				qApp->closeAllWindows();							
			}

			dirty = true;
			charChanged = true;
			
			error = AUCR_CHARACTER_From_Interpolated(
				myInterpolated, myAlf, myCharacter );
			if( error != AUCR_ERR_SUCCESS )
			{
				aucrEdit::errorDialog( error );
				qApp->closeAllWindows();							
			}
		}

		//timer calls dispLetter when it times out [.5 second]
		timer->start( 500, TRUE );
	}
}

void editArea::mouseMoveEvent( QMouseEvent* e )
{
	if ( mousePressed && myStart )
	{
		myCords[n].x=(e->pos()).x();
		myCords[n].y=(this->height() - (e->pos()).y() );
		QPainter painter( this );
		painter.setPen( QPen( Qt::darkGray, 1, SolidLine ) );
		painter.drawLine( myCords[n - 1].x, this->height() - myCords[n - 1].y, (e->pos()).x(), (e->pos()).y() );
		painter.end();
		n++;
		if( numCords <= n + 2 )
		{
			numCords *= 2;

			myCords = (AUCR_Coordinate_Ptr)realloc( myCords, numCords * sizeof( AUCR_Coordinate ) );

			if( myCords == NULL )
			{
				error = AUCR_ERR_NO_MEMORY;
				aucrEdit::errorDialog( error );
				qApp->closeAllWindows();			
			}
		}

	}
}

void editArea::setCurrent( wchar_t newchar )
{
	myCurrent = newchar;
}

void editArea::setStart( bool newstart )
{
	myStart = newstart;
}

void editArea::setAlphabet( AUCR_Alphabet_Ptr newalf )
{
	myAlf = newalf;
	dirty = false;
	charChanged = true;
	if( myAlf->num_directional_codes != myInterpolated->num_directional_codes )
	{
		if( myInterpolated != NULL )
		{
			AUCR_INTERPOLATED_CHARACTER_Release( &myInterpolated );
			myInterpolated = NULL;
		}
		error = AUCR_INTERPOLATED_CHARACTER_Init( 0x0001, myAlf->num_directional_codes, &myInterpolated );
		if( error != AUCR_ERR_SUCCESS )
		{

			aucrEdit::errorDialog( error );
			qApp->closeAllWindows();
		}
	}
}

bool editArea::isDirty()
{
	return dirty;
}

QSize editArea::sizeHint() const
{
	return QSize( 72, 72 );
}

QSizePolicy editArea::sizePolicy() const
{
	return QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}
