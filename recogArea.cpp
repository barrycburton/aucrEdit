//recogArea.cpp

#include "recogArea.h"

recogArea::recogArea( QWidget *parent, const char *name ) : QWidget( parent, name )
{
	myStart = false;
	clear = true;
	begin = true;
	graffiti = false;
	numCords = 1000;
	xMax = 0;
	xMin = 0;
	myPixmap = NULL;
	myCords = (AUCR_Coordinate_Ptr)malloc( numCords * sizeof( AUCR_Coordinate ) );
	if( myCords == NULL )
	{
		error = AUCR_ERR_NO_MEMORY;
		aucrEdit::errorDialog( error );
		qApp->closeAllWindows();
	}
	
	setMouseTracking( TRUE );
	repaint( TRUE );
}

recogArea::~recogArea()
{
	free( myCords );
	delete myPixmap;
}

void recogArea::setAlphabet( AUCR_Alphabet_Ptr newalf )
{
	myAlf = newalf;
	if( !begin )
	{
		clear = true;
		paintEvent();
	}
	this->setStart( true );
}

void recogArea::setText()
{
	if( !begin )
	{
		clear = true;
		paintEvent();
	}
}

void recogArea::setStart( bool newstart )
{
	if( ( myAlf != NULL ) && newstart )
	{
		myStart = true;
	}
	else
	{
		myStart = false;
	}
}

void recogArea::setClear( bool newGraf )
{
	graffiti = newGraf;
}

bool recogArea::started()
{
	return ( myStart && ( myAlf->num_characters != 0 ) );
}

void recogArea::paintEvent( QPaintEvent *e )
{
	if( e )
	{}
	
	if( clear )
	{
		if( myPixmap != NULL )
		{
			delete myPixmap;
		}
		myPixmap = new QPixmap( this->width(), this->height() );
		QPainter bPainter;
		bPainter.begin( myPixmap );
		bPainter.setBrush( Qt::white );
		bPainter.setPen( QPen( Qt::black, 1, SolidLine ) );
		bPainter.drawRect( 0, 0, this->width(), this->height() );
		bPainter.setPen( QPen( Qt::black, 1, DashDotLine ) );
		bPainter.drawLine( 0, this->height() - 20, this->width(), this->height() - 20 );
		bPainter.end();
		clear = false;
		begin = false;
	}
	
	//copy pixmap to screen
	bitBlt( this, 0, 0, myPixmap );
}

void recogArea::mousePressEvent( QMouseEvent* e )
{
	if( started() )
	{
		mousePressed = TRUE;
		if( graffiti || ( (e->pos()).x() < xOld ) )
		{
			clear = true;
			paintEvent();
			xOld = 0;
		}
		xMax = (e->pos()).x();
		xMin = (e->pos()).x();
		n = 0;
		myCords[n].x=(e->pos()).x();
		myCords[n].y=(this->height() - (e->pos()).y() );
		n++;
		QPainter wPainter, bPainter;
		wPainter.begin( this );
		bPainter.begin( myPixmap );
		wPainter.setPen( QPen( Qt::darkGray, 1, SolidLine ) );
		bPainter.setPen( QPen( Qt::darkGray, 1, SolidLine ) );
		wPainter.drawPoint( e->pos() );
		bPainter.drawPoint( e->pos() );
		wPainter.end();
		bPainter.end();
	}
}

void recogArea::mouseReleaseEvent( QMouseEvent* e )
{
	int i;
	
	if( mousePressed && myStart )
	{
		mousePressed = FALSE;
		
		if( (e->pos()).x() > xMax )
		{
			xMax = (e->pos()).x();
		}
		if( (e->pos()).x() < xMin )
		{
			xMin = (e->pos()).x();
		}
		
		myCords[n].x=(e->pos()).x();
		myCords[n].y=(this->height() - (e->pos()).y() );
		if( (e->pos()).x() < xOld )
		{
			clear = true;
			paintEvent();
			QPainter wPainter, bPainter;
			wPainter.begin( this );
			bPainter.begin( myPixmap );
			wPainter.setPen( QPen( Qt::darkGray, 1, SolidLine ) );
			bPainter.setPen( QPen( Qt::darkGray, 1, SolidLine ) );
			for( i = 1; i <= n; i++ )
			{
				wPainter.drawLine( myCords[i-1].x, this->height() - myCords[i-1].y, myCords[i].x, this->height() - myCords[i].y );
				bPainter.drawLine( myCords[i-1].x, this->height() - myCords[i-1].y, myCords[i].x, this->height() - myCords[i].y );
			}
			wPainter.end();		
			bPainter.end();
		}
		else
		{
			QPainter wPainter, bPainter;
			wPainter.begin( this );
			bPainter.begin( myPixmap );
			wPainter.setPen( QPen( Qt::darkGray, 1, SolidLine ) );
			bPainter.setPen( QPen( Qt::darkGray, 1, SolidLine ) );	
			wPainter.drawLine( myCords[n-1].x, this->height() - myCords[n-1].y, (e->pos()).x(), (e->pos()).y() );
			bPainter.drawLine( myCords[n-1].x, this->height() - myCords[n-1].y, (e->pos()).x(), (e->pos()).y() );
			wPainter.end();		
			bPainter.end();
		}
		
		xOld = ( xMax - ( ( xMax - xMin ) / 4 ) );
	
		n++;

		error = AUCR_ALPHABET_Recognize_From_Raw( n,	myCords,	myAlf, &uni );

		if( error != AUCR_ERR_SUCCESS )
		{
			aucrEdit::errorDialog( error );
			qApp->closeAllWindows();						
		}
		else
		{
			emit found( uni );
		}
	}
}

void recogArea::mouseMoveEvent( QMouseEvent* e )
{
	if ( mousePressed && myStart )
	{
		if( (e->pos()).x() > xMax )
		{
			xMax = (e->pos()).x();
		}
		if( (e->pos()).x() < xMin )
		{
			xMin = (e->pos()).x();
		}
		
		myCords[n].x=(e->pos()).x();
		myCords[n].y=(this->height() - (e->pos()).y() );
		QPainter wPainter, bPainter;
		wPainter.begin( this );
		bPainter.begin( myPixmap );
		wPainter.setPen( QPen( Qt::darkGray, 1, SolidLine ) );
		bPainter.setPen( QPen( Qt::darkGray, 1, SolidLine ) );
		wPainter.drawLine( myCords[n - 1].x, this->height() - myCords[n - 1].y, (e->pos()).x(), (e->pos()).y() );
		bPainter.drawLine( myCords[n - 1].x, this->height() - myCords[n - 1].y, (e->pos()).x(), (e->pos()).y() );
		wPainter.end();		
		bPainter.end();
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

QSize recogArea::sizeHint() const
{
	return QSize( 216, 72 );
}

QSizePolicy recogArea::sizePolicy() const
{
	return QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}
