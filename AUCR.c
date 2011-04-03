//AUCR.c

#include "AUCR.h"


/* ==============================================
 FUNCTION IMPLEMENTATIONS
 ============================================== */

/*
 *	Name:
 *		AUCR_CHARACTER_Init()
 *	Description:
 *		Initializes a new AUCR_Character instance by allocating appropriate
 *		memories and setting member values.
 *	Parameters:
 *		i_unicode -> the unicode value to be represented by the structure.
 *		i_num_directional_codes -> i_number of directional codes to allocate memory for.
 *		i_num_activity_regions -> i_number of activity regions for to allocate
 *							activity measure memories for.
 *		o_character_ptr <- memory to be populated with the new AUCR_Character.
 *	Preconditions:
 *		i_num_directional_codes and i_num_activity_regions must be positive,
 *		non-zero values. i_unicode should not be 0x0000 which is reserved as an
 *		error character. o_character_ptr must be NULL.
 *	Postconditions:
 *		o_character_ptr is the address of a newly allocated and initialized
 *		AUCR_Character.  Its directional_codes is a new array of
 *		(num_directional_codes + 1) elements. Its activity_measures is a new
 *		array of num_activity_regions elements
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_NO_MEMORY if the structure or array memories could not be allocated
 */
AUCR_Error AUCR_CHARACTER_Init( 
							   wchar_t i_unicode,
							   int i_num_directional_codes,
							   int i_num_activity_regions,
							   AUCR_Character_Ptr * o_character_ptr_ptr )
{
	//check for valid input
	if( ( i_unicode == 0x0000 ) || ( i_num_directional_codes <= 0 ) ||
	   ( i_num_activity_regions <= 0 ) || ( o_character_ptr_ptr == NULL ) ||
	   ( ( * o_character_ptr_ptr ) != NULL ) )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//malloc memory for character struct
	( * o_character_ptr_ptr ) = (AUCR_Character_Ptr)malloc( sizeof( AUCR_Character ) );
	if( ( * o_character_ptr_ptr ) == NULL )
	{
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//malloc memory for directional codes
	( * o_character_ptr_ptr )->directional_codes = 
	(int *)malloc( (i_num_directional_codes ) * sizeof( int ) );
	if( ( * o_character_ptr_ptr )->directional_codes == NULL )
	{
		free( * o_character_ptr_ptr );
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//malloc memory for activity codes
	( * o_character_ptr_ptr )->activity_measures = 
	(int *)malloc( i_num_activity_regions * sizeof( int ) );
	if( ( * o_character_ptr_ptr )->activity_measures == NULL )
	{
		free( * o_character_ptr_ptr );
		free( ( * o_character_ptr_ptr )->directional_codes );
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//memory allocations successful
	//now fill in fields
	( * o_character_ptr_ptr )->unicode = i_unicode;
	( * o_character_ptr_ptr )->num_directional_codes = i_num_directional_codes;
	( * o_character_ptr_ptr )->num_activity_regions = i_num_activity_regions;
	
	//return success
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_INTERPOLATED_CHARACTER_Release()
 *	Description:
 *		Frees all dynamically allocated memories for the AUCR_Interpolated_Character and NULLs
 *		the AUCR_Interpolated_Character_Ptr.
 *	Parameters:
 *		io__interpolatedcharacter_ptr <-> address of the AUCR_Interpolated_Character memory to be freed.
 *	Preconditions:
 *		io__interpolatedcharacter_ptr is a valid address of an AUCR_Interpolated_Character.
 *	Postconditions:
 *		The character's coordinate array is freed followed by the memory for the
 *		character as well.  io_interpolated_character_ptr is NULL.
 *	Returns:
 *		none
 */
void AUCR_CHARACTER_Release( 
							AUCR_Character_Ptr * io_character_ptr_ptr )
{
	//check for valid input
	if( ( io_character_ptr_ptr == NULL ) || ( * io_character_ptr_ptr == NULL ) )
	{
		return;
	}
	
	//free directional code array
	free( ( * io_character_ptr_ptr )->directional_codes );
	
	//free activity measure array
	free( ( * io_character_ptr_ptr )->activity_measures );
	
	//free character structure
	free( * io_character_ptr_ptr );
	
	//set pointer to null
	io_character_ptr_ptr = NULL;
	
	return;
}


/*
 *	Name:
 *		AUCR_CHARACTER_From_Interpolated()
 *	Description:
 *		takes in an interpolated character and an alphabet and forms the character
 *		representation of the interpolated character based on the alphabet.
 *	Parameters:
 *		i_interpolated_character_ptr -> a pointer to an interpolated character
 *		i_alphabet_ptr -> a pointer to an alphabet
 *		io_character_ptr <-> a pointer to a character that we will fill in
 *	Preconditions:
 *		i_interpolated_character_ptr is non NULL and is a pointer to a valid
 *		interpolated character, i_alphabet_ptr is non NULL and is a pointer to
 *		a valid alphabet, io_character_ptr is non NULL and is a pointer to a valid
 *		character, num_directional_codes between the alphabet and the interpolated
 *		are in agreement
 *	Postconditions:
 *		io_character_ptr is populated with the character version of the
 *		interpolated character
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_CONFLICTING_PARAMETERS if all preconditions except the agreement of
 *													num_directional_codes are met
 */
AUCR_Error AUCR_CHARACTER_From_Interpolated(
											AUCR_Interpolated_Character_Ptr i_interpolated_character_ptr,
											AUCR_Alphabet_Ptr i_alphabet_ptr,
											AUCR_Character_Ptr io_character_ptr )
{
	//local variables
	int i, j;
	int code, dx, dy, begin, end, max;
	int * hits;
	
	//check for valid input
	if( i_interpolated_character_ptr == NULL ||
	   i_alphabet_ptr == NULL || io_character_ptr == NULL )
	{
		return( AUCR_ERR_FAILED );
	}
	else if( i_interpolated_character_ptr->num_directional_codes
			!= i_alphabet_ptr->num_directional_codes ||
			i_interpolated_character_ptr->num_directional_codes
			!= io_character_ptr->num_directional_codes ||
			io_character_ptr->num_directional_codes
			!= i_alphabet_ptr->num_directional_codes )
	{
		return( AUCR_ERR_CONFLICTING_PARAMETERS );
	}
	
	hits = (int *)malloc( sizeof( int ) * 
						 ( i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes ) );
	if( hits == NULL )
	{
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//find all directional codes
	for( i = 0; i < i_alphabet_ptr->num_directional_codes; i++ )
	{
		//intialize code to invalid value
		code = -1;
		
		//find change in x
		dx = i_interpolated_character_ptr->coordinates[i + 1].x -
		i_interpolated_character_ptr->coordinates[i].x;
		//find change in y
		dy = i_interpolated_character_ptr->coordinates[i + 1].y -
		i_interpolated_character_ptr->coordinates[i].y;
		
		//setup for search in right halfplane
		if( dx > 0 )
		{
			if( i_alphabet_ptr->directional_code_map_ptr->positive_begin == -1 )
			{
				code = i_alphabet_ptr->directional_code_map_ptr->negative_end;
			}
			begin = i_alphabet_ptr->directional_code_map_ptr->positive_begin;
			end = i_alphabet_ptr->directional_code_map_ptr->positive_end;
		}
		//setup for search in left halfplane
		else
		{
			if( i_alphabet_ptr->directional_code_map_ptr->negative_begin == -1 )
			{
				code = i_alphabet_ptr->directional_code_map_ptr->positive_end;
			}
			begin = i_alphabet_ptr->directional_code_map_ptr->negative_begin;
			end = i_alphabet_ptr->directional_code_map_ptr->negative_end;
		}
		
		if( code == -1 )
		{
			//start at the directional code that begins the quadrant, and advance thru the directional
			//codes until you find one that has slope greater than the slope of the present vector,
			//or until you reach the end of the quadrant
			for ( j = begin; j != (( end + 1 ) %
								   ( i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes ) );
				 j = (( j + 1 ) %
					  ( i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes ) ) )
			{
				if ( dy * i_alphabet_ptr->directional_code_map_ptr->x[j] <
					dx * i_alphabet_ptr->directional_code_map_ptr->y[j] )
				{
					code =
					( ( j + i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes - 1 ) %
					 ( i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes ) );
					break;
				}
			}
		}
		
		//if for loop went until the end of the quadrant,
		//point must be in the last area of the quadrant
		if ( code == -1 )
		{
			code = end;
		}
		//direction is now in code
		
		//store directional code i
		io_character_ptr->directional_codes[i] = code;
	}
	
	//zero out counts for all directions
	for( i = 0; i < 
		i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes; i++ )
	{
		hits[i] = 0;
	}
	//find activity for all regions
	for( i = 0; i < i_alphabet_ptr->num_activity_regions; i++ )
	{
		for( j = i_alphabet_ptr->activity_regions[i].start;
			j <= i_alphabet_ptr->activity_regions[i].stop; j++ )
		{
			hits[io_character_ptr->directional_codes[j]]++;
		}
		max = 0;
		for( j = 0; j < 
			i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes; j++ )
		{
			if( hits[j] > max )
			{
				max = hits[j];
			}
			hits[j] = 0;
		}
		io_character_ptr->activity_measures[i] =
		AUCR_Rounding_Divide( ( AUCR_SCALE *
							   ( 1 + i_alphabet_ptr->activity_regions[i].stop 
								- i_alphabet_ptr->activity_regions[i].start ) ),
							 ( max ) );
	}
	
	free( hits );
	
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_INTERPOLATED_CHARACTER_Init()
 *	Description:
 *		Initializes a new AUCR_Interpolated_Character instance by allocating appropriate
 *		memories and setting member values.
 *	Parameters:
 *		i_unicode -> the unicode value to be represented by the structure.
 *		i_num_directional_codes -> (i_num_directional_codes + 1) elements of the
 *							coordinates vector must have memory allocated.
 *		o_interpolated_character_ptr_ptr <- address of the AUCR_Interpolated_Character_Ptr
 *							to be populated with the new AUCR_Interpolated_Character.
 *	Preconditions:
 *		i_num_directional_codes must be positive and non-zero.  i_unicode should not be
 *		0x0000 which is reserved as an error character.  The AUCR_Interpolated_Character_Ptr
 *		at o_interpolated_character_ptr_ptr must be NULL.
 *	Postconditions:
 *		o_character_ptr_Ptr is the address of a newly allocated and initialized
 *		AUCR_Interpolated_Character_Ptr.  Its coordinates is a new array of
 *		(num_directional_codes + 1) "AUCR_Coordinate"s.
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_NO_MEMORY if the structure or array memories could not be allocated
 */
AUCR_Error AUCR_INTERPOLATED_CHARACTER_Init(
											wchar_t i_unicode,
											int i_num_directional_codes,
											AUCR_Interpolated_Character_Ptr * o_interpolated_character_ptr_ptr )
{
	//check for valid input
	if( ( i_unicode == 0x0000 ) || ( i_num_directional_codes < 1 ) ||
	   ( o_interpolated_character_ptr_ptr == NULL ) ||
	   ( ( * o_interpolated_character_ptr_ptr ) != NULL ) )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//malloc memory for character struct
	( * o_interpolated_character_ptr_ptr ) = 
	( AUCR_Interpolated_Character_Ptr )malloc( sizeof( AUCR_Interpolated_Character ) );
	if( ( * o_interpolated_character_ptr_ptr ) == NULL )
	{
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//malloc memory for coordinates
	( * o_interpolated_character_ptr_ptr )->coordinates = 
	( AUCR_Coordinate_Ptr )malloc( (i_num_directional_codes + 1 ) 
								  * sizeof( AUCR_Coordinate ) );
	if( ( * o_interpolated_character_ptr_ptr )->coordinates == NULL )
	{
		free( * o_interpolated_character_ptr_ptr );
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//memory allocations successful
	//now fill in fields
	( * o_interpolated_character_ptr_ptr )->unicode = i_unicode;
	( * o_interpolated_character_ptr_ptr )->num_directional_codes = 
	i_num_directional_codes;
	
	//return success
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_INTERPOLATED_CHARACTER_Release()
 *	Description:
 *		Frees all dynamically allocated memories for the AUCR_Interpolated_Character and NULLs
 *		the AUCR_Interpolated_Character_Ptr.
 *	Parameters:
 *		io__interpolatedcharacter_ptr <-> address of the AUCR_Interpolated_Character memory to be freed.
 *	Preconditions:
 *		io__interpolatedcharacter_ptr is a valid address of an AUCR_Interpolated_Character.
 *	Postconditions:
 *		The character's coordinate array is freed followed by the memory for the
 *		character as well.  io_interpolated_character_ptr is NULL.
 *	Returns:
 *		none
 */
void AUCR_INTERPOLATED_CHARACTER_Release(
										 AUCR_Interpolated_Character_Ptr * io_interpolated_character_ptr_ptr )
{
	//check for valid input
	if( ( io_interpolated_character_ptr_ptr == NULL ) ||
	   ( ( * io_interpolated_character_ptr_ptr ) == NULL ) )
	{
		return;
	}
	
	//free coordinate array
	free( ( * io_interpolated_character_ptr_ptr )->coordinates );
	
	//free character structure
	free( ( * io_interpolated_character_ptr_ptr ) );
	
	//set pointer to null
	( * io_interpolated_character_ptr_ptr ) = NULL;
	
	return;
	
}


/*
 *	Name:
 *		AUCR_INTERPOLATED_CHARACTER_From_Raw()
 *	Description:
 *		Converts an array of AUCR_Coordinates (captured by a GUI) to an AUCR_Interpolated_Character.
 *	Parameters:
 *		i_num_coordinates -> i_number of coordinates in the i_coordinates array.
 *		i_coordinates -> array of raw pen coordinates
 *		io_interpolated_character_ptr <-> address of the AUCR_Interpolated_Character
 *								to be populated from the raw data
 *	Preconditions:
 *		io_interpolated_character_ptr is a valid address of an initialized AUCR_Interpolated_Character.
 *	Postconditions:
 *		io_interpolated_character_ptr is populated with the interpolated version if the raw
 *		coordinates.
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_NO_MEMORY if the structure or array memories could not be allocated
 */
AUCR_Error AUCR_INTERPOLATED_CHARACTER_From_Raw(
												int i_num_coordinates,
												AUCR_Coordinate * i_coordinates,
												AUCR_Interpolated_Character_Ptr io_interpolated_character_ptr )
{
	//declare variables
	//array of distances for each point
	unsigned int * distances;
	//holds scale factor if needed
	int tempscalar;
	//holds length all segments will be
	int seglength;
	//index of point currently being looked at
	int currentcoord = 1;
	//parameter in parametric equations for doing interpolations
	int t;
	//loop index
	int i;
	
	//check for valid input
	if( io_interpolated_character_ptr == NULL ||
	   i_num_coordinates < 1 )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//get memory for distance array
	distances = (unsigned int *)malloc( i_num_coordinates * sizeof( unsigned int ) );
	if( distances == NULL )
	{
		return( AUCR_ERR_NO_MEMORY );
	}  
	
	distances[0] = 0;
	
	//loop to calculate distances
	for(i = 1; i < i_num_coordinates; i++ )
	{
		distances[i] = distances[i - 1] + AUCR_Integer_Square_Root( 
																   ( ( i_coordinates[i].x - i_coordinates[i - 1].x ) *
																	( i_coordinates[i].x - i_coordinates[i - 1].x ) ) +
																   ( ( i_coordinates[i].y - i_coordinates[i - 1].y ) *
																	( i_coordinates[i].y - i_coordinates[i - 1].y ) ) );
	}
	
	//case 1: input has no distance
	if( distances[i_num_coordinates - 1] < 1 )
	{ 
		//fill in entire coordinate array with zeros
		for( i = 0; i <= io_interpolated_character_ptr->num_directional_codes; i++ )
		{
			io_interpolated_character_ptr->coordinates[i].x = 0;
			io_interpolated_character_ptr->coordinates[i].y = 0;
		}
	}
	
	//case 2: input has distance
	else
	{
		//make sure input has long enough distance
		//
		//the ( io_interpolated_character_ptr->num_directional_codes * AUCR_SCALE * 2 )
		//term is the scaling cut off factor and should have a noticeable impact on 
		//the efficiency and accuracy of the interpolation algorithm.
		//
		//for smaller input devices, it may be better to go ahead and just scale 
		//everything without even checking how much total information we read in.
		//
		//the larger the term is (i.e. increasing the 2 to a 5) should give us a 
		//better interpolation and make the resulting segmented curve smoother, 
		//but on the other hand making it too large makes us run the risk of  
		//overflow in longer cases and will also force us to rescale more often 
		//which may not have adequate run time performance
		//
		if(distances[i_num_coordinates - 1] < 
		   (unsigned int)( io_interpolated_character_ptr->num_directional_codes * AUCR_SCALE * 10 ) )
		{
			//too short, so we will scale
			//scale factor
			tempscalar = AUCR_Rounding_Divide( 
											  io_interpolated_character_ptr->num_directional_codes * AUCR_SCALE * 20,
											  distances[i_num_coordinates - 1] );
			//scale first raw coordinate
			i_coordinates[0].x = tempscalar * i_coordinates[0].x;
			i_coordinates[0].y = tempscalar * i_coordinates[0].y;
			
			//scale all subsequent raw coordinates and distances
			for( i = 1; i < i_num_coordinates; i++ )
			{
				i_coordinates[i].x = tempscalar * i_coordinates[i].x;
				i_coordinates[i].y = tempscalar * i_coordinates[i].y;
				distances[i] = tempscalar * distances[i];
			}
		}
		
		//first interpolated point is the same as first raw point
		io_interpolated_character_ptr->coordinates[0].x = i_coordinates[0].x;
		io_interpolated_character_ptr->coordinates[0].y = i_coordinates[0].y;
		
		//find segment length
		seglength = AUCR_Rounding_Divide( distances[i_num_coordinates - 1],
										 io_interpolated_character_ptr->num_directional_codes );
		
		//interpolate middle points
		for( i = 1; i < io_interpolated_character_ptr->num_directional_codes; i++ )
		{
			//while the total distance from the start to the current coordinate
			//is less than the current multiple of the segment length
			while( ( distances[currentcoord] ) < (unsigned int)( i * seglength ) )
			{
				//move to the next coordinate
				currentcoord++;
			}
			//calculate parameter t that will give us the interpolated point
			//when used in parametric equations for the current segment
			t = AUCR_Rounding_Divide( (unsigned int) ( ( i * seglength ) - 
													  distances[currentcoord - 1] ), AUCR_Rounding_Divide( 
																										  ( distances[currentcoord] -
																										   distances[currentcoord - 1] ), AUCR_SCALE ) );
			//parametric equation for x
			io_interpolated_character_ptr->coordinates[i].x = 
			i_coordinates[currentcoord - 1].x + AUCR_Rounding_Divide( 
																	 ( ( i_coordinates[currentcoord].x -
																		i_coordinates[currentcoord - 1].x ) * t ), AUCR_SCALE );
			//parametric equation for y
			io_interpolated_character_ptr->coordinates[i].y = 
			i_coordinates[currentcoord - 1].y + AUCR_Rounding_Divide( 
																	 ( ( i_coordinates[currentcoord].y -
																		i_coordinates[currentcoord - 1].y ) * t ), AUCR_SCALE );
		}
		
		//last interpolated point is the same as last raw point
		io_interpolated_character_ptr->coordinates[i].x = 
		i_coordinates[i_num_coordinates - 1].x;
		io_interpolated_character_ptr->coordinates[i].y =
		i_coordinates[i_num_coordinates - 1].y;
	}
	
	//return success
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_INTERPOLATED_CHARACTER_From_Character()
 *	Description:
 *		takes in a character and the alphabet that formed the character, as
 *		well as a rectangle (in pixels) and the border inside the rectangle
 *		and returns an interpolated character whose coordinates fill a
 *		rectangle of the given size while always staying a distance of
 *		border pixels from the edge of the rectangle.
 *		note: this function is primarily intended to be used as a mechanism
 *		to get points that can be written to screen, such that a character
 *		may be displayed.  it does not return an interpolated character
 *		that is identical to the interpolated character that was used to
 *		create the character in the alphabet.
 *	Parameters:
 *		i_character_ptr -> a pointer to a character
 *		i_alphabet_ptr -> a pointer to the alphabet that was used to create the character
 *		i_rectangle_width -> the width in pixels of the bounding rectangle for the points
 *										in the interpolated character
 *		i_rectangle_height -> the height in pixels of the bounding rectangle for the points
 *										in the interpolated character
 *		i_rectangle_border -> the closest distance in pixels that any point in the
 *										interpolated character can come to the edge of the
 *										bounding rectangle
 *		io_interpolated charactercharacter_ptr <-> a pointer to an interpolated character
 *				that will be filled in by extrapolating information from the character
 *	Preconditions:
 *		io_interpolated_character_ptr is non NULL and is a pointer to a valid
 *		interpolated character, i_alphabet_ptr is non NULL and is a pointer to
 *		a valid alphabet, io_character_ptr is non NULL and is a pointer to a valid
 *		character, i_rectangle_width is positive, i_rectangle_height is positive,
 *		i_rectangle_border is non negative, and num_directional_codes between
 *		the alphabet, character, and the interpolated character are in agreement
 *	Postconditions:
 *		io_interpolated_character_ptr is populated with the interpolated version of the
 *		character in i_character_ptr
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_CONFLICTING_PARAMETERS if all preconditions except the agreement of
 *													num_directional_codes are met
 */
AUCR_Error AUCR_INTERPOLATED_CHARACTER_From_Character(
													  AUCR_Character_Ptr i_character_ptr,
													  AUCR_Alphabet_Ptr i_alphabet_ptr,
													  int i_rectangle_width,
													  int i_rectangle_height,
													  int i_rectangle_border,
													  AUCR_Interpolated_Character_Ptr io_interpolated_character_ptr )
{
	//local variables
	int i, xoff, yoff, scale, div, nextDirCode;
	int  xmin = 0, ymin = 0, xmax = 0, ymax = 0;
	
	//check for valid input
	if( io_interpolated_character_ptr == NULL ||
	   i_alphabet_ptr == NULL || i_character_ptr == NULL )
	{
		return( AUCR_ERR_FAILED );
	}
	else if( io_interpolated_character_ptr->num_directional_codes
			!= i_alphabet_ptr->num_directional_codes ||
			io_interpolated_character_ptr->num_directional_codes
			!= i_character_ptr->num_directional_codes ||
			i_character_ptr->num_directional_codes
			!= i_alphabet_ptr->num_directional_codes )
	{
		return( AUCR_ERR_CONFLICTING_PARAMETERS );
	}
	
	//start at ( 0, 0 )
	io_interpolated_character_ptr->coordinates[0].x = 0;
	io_interpolated_character_ptr->coordinates[0].y = 0;
	
	//calculate remaining points, using a scale of AUCR_SCALE
	//and an offset of ( 0, 0 )
	for( i = 0; i < i_alphabet_ptr->num_directional_codes; i++ )
	{
		if( i_character_ptr->directional_codes[i] ==
		   ( i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes - 1 ) )
		{ 
			nextDirCode = 0;
		}
		else
		{
			nextDirCode = i_character_ptr->directional_codes[i] + 1;
		}
		div = AUCR_Integer_Square_Root( 
									   ( ( i_alphabet_ptr->directional_code_map_ptr->x[i_character_ptr->directional_codes[i]] +
										  i_alphabet_ptr->directional_code_map_ptr->x[nextDirCode] ) *
										( i_alphabet_ptr->directional_code_map_ptr->x[i_character_ptr->directional_codes[i]] +
										 i_alphabet_ptr->directional_code_map_ptr->x[nextDirCode] ) ) +
									   ( ( i_alphabet_ptr->directional_code_map_ptr->y[i_character_ptr->directional_codes[i]] +
										  i_alphabet_ptr->directional_code_map_ptr->y[nextDirCode] ) *
										( i_alphabet_ptr->directional_code_map_ptr->y[i_character_ptr->directional_codes[i]] +
										 i_alphabet_ptr->directional_code_map_ptr->y[nextDirCode] ) ) );   
		io_interpolated_character_ptr->coordinates[i + 1].x = ( AUCR_Rounding_Divide(
																					 ( i_alphabet_ptr->directional_code_map_ptr->x[i_character_ptr->directional_codes[i]]
																					  + i_alphabet_ptr->directional_code_map_ptr->x[nextDirCode] ) * AUCR_SCALE,
																					 div ) + io_interpolated_character_ptr->coordinates[i].x );
		io_interpolated_character_ptr->coordinates[i + 1].y = ( AUCR_Rounding_Divide(
																					 ( i_alphabet_ptr->directional_code_map_ptr->y[i_character_ptr->directional_codes[i]]
																					  + i_alphabet_ptr->directional_code_map_ptr->y[nextDirCode] ) * AUCR_SCALE,
																					 div ) + io_interpolated_character_ptr->coordinates[i].y );
		if( io_interpolated_character_ptr->coordinates[i + 1].x > xmax )
		{
			xmax = io_interpolated_character_ptr->coordinates[i + 1].x;
		}
		if( io_interpolated_character_ptr->coordinates[i + 1].x < xmin )
		{
			xmin = io_interpolated_character_ptr->coordinates[i + 1].x;
		}
		if( io_interpolated_character_ptr->coordinates[i + 1].y > ymax )
		{
			ymax = io_interpolated_character_ptr->coordinates[i + 1].y;
		}
		if( io_interpolated_character_ptr->coordinates[i + 1].y < ymin )
		{
			ymin = io_interpolated_character_ptr->coordinates[i + 1].y;
		}
	}
	
	//calculate scale and offsets to map points to rectangle 
	if( ( ( ymax - ymin ) * i_rectangle_width ) > ( ( xmax - xmin ) * i_rectangle_height ) )
	{
		//y is the constraining direction
		if( ( ymax - ymin ) < 10 * ( i_rectangle_height - ( 2 * i_rectangle_border ) ) )
		{
			scale = AUCR_Rounding_Divide( 
										 20 * ( i_rectangle_height - ( 2 * i_rectangle_border ) ), ( ymax - ymin ) );
			xmax = xmax * scale;
			ymax = ymax * scale;
			xmin = xmin * scale;
			ymin = ymin * scale;
			for( i = 0; i <= i_alphabet_ptr->num_directional_codes; i++ )
			{
				io_interpolated_character_ptr->coordinates[i].x =
				io_interpolated_character_ptr->coordinates[i].x * scale;
				io_interpolated_character_ptr->coordinates[i].y =
				io_interpolated_character_ptr->coordinates[i].y * scale;
			}
		}
		
		scale = ( i_rectangle_height - ( 2 * i_rectangle_border ) );
		div = ( ymax - ymin );
		
		yoff = i_rectangle_border - AUCR_Rounding_Divide( ymin * scale, div );
		xoff = i_rectangle_border - AUCR_Rounding_Divide( xmin * scale, div )
		+ AUCR_Rounding_Divide( i_rectangle_width - ( 2 * 
													 i_rectangle_border ) - AUCR_Rounding_Divide( 
																								 ( xmax - xmin ) * scale, div ), 2 );
	}
	else
	{
		//x is the constraining direction
		if( ( xmax - xmin ) < 10 * ( i_rectangle_width - ( 2 * i_rectangle_border ) ) )
		{
			scale = AUCR_Rounding_Divide( 
										 20 * ( i_rectangle_width - ( 2 * i_rectangle_border ) ), ( xmax - xmin ) );
			xmax = xmax * scale;
			ymax = ymax * scale;
			xmin = xmin * scale;
			ymin = ymin * scale; 
			for( i = 0; i <= i_alphabet_ptr->num_directional_codes; i++ )
			{
				io_interpolated_character_ptr->coordinates[i].x =
				io_interpolated_character_ptr->coordinates[i].x * scale;
				io_interpolated_character_ptr->coordinates[i].y =
				io_interpolated_character_ptr->coordinates[i].y * scale;
			}
		}
		
		scale = ( i_rectangle_width - ( 2 * i_rectangle_border ) );
		div = ( xmax - xmin );
		
		xoff = i_rectangle_border - AUCR_Rounding_Divide( xmin * scale, div );
		yoff = i_rectangle_border - AUCR_Rounding_Divide( ymin * scale, div )
		+ AUCR_Rounding_Divide( i_rectangle_height - ( 2 * 
													  i_rectangle_border ) - AUCR_Rounding_Divide( 
																								  ( ymax - ymin ) * scale, div ), 2 );
	}
	
	//loop through all points, scaling, then offsetting them
	for( i = 0; i <= i_alphabet_ptr->num_directional_codes; i++ )
	{
		io_interpolated_character_ptr->coordinates[i].x = xoff +
		AUCR_Rounding_Divide( 
							 io_interpolated_character_ptr->coordinates[i].x * scale, div );
		io_interpolated_character_ptr->coordinates[i].y = yoff +
		AUCR_Rounding_Divide( 
							 io_interpolated_character_ptr->coordinates[i].y * scale, div );
	}
	
	//return success
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_DIRECTIONAL_CODE_MAP_Init()
 *	Description:
 *		Initializes a new AUCR_Directional_Code_Map instance by allocating appropriate
 *		memories and setting member values.
 *	Parameters:
 *		i_mappable_directional_codes -> i_number of unique directional codes to allocate memory
 *							for their mappings.
 *		o_directional_code_map_ptr_ptr <- memory address of the AUCR_Directional_Code_Map_Ptr
 *							to be populated with the new AUCR_Directional_Code_Map.
 *	Preconditions:
 *		i_mappable_directional_codes must be a positive, non-zero values.
 *		The AUCR_Directional_Code_Map_Ptr at o_directional_code_map_ptr_ptr must be NULL.
 *	Postconditions:
 *		o_directional_code_map_ptr_ptr is the address of a newly allocated and initialized
 *		AUCR_Directional_Code_Map_Ptr.  Its "x" and "y" variables are newly allocated arrays
 *		of (mappable_directional_codes) elements... each element set to zero.
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_NO_MEMORY if the structure or array memories could not be allocated
 */
AUCR_Error AUCR_DIRECTIONAL_CODE_MAP_Init(
										  int i_mappable_directional_codes,
										  AUCR_Directional_Code_Map_Ptr * o_directional_code_map_ptr_ptr )
{
	//declare local variables
	int i;
	
	//check for valid input
	if( ( i_mappable_directional_codes < 1 ) || ( o_directional_code_map_ptr_ptr == NULL )
	   || ( ( * o_directional_code_map_ptr_ptr ) != NULL ) )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//malloc space for Directional_Code_Map
	( * o_directional_code_map_ptr_ptr ) = 
	( AUCR_Directional_Code_Map_Ptr )malloc( sizeof( AUCR_Directional_Code_Map ) );
	if( ( * o_directional_code_map_ptr_ptr ) == NULL )
	{
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//malloc space for x array
	( * o_directional_code_map_ptr_ptr )->x = 
	(int *)malloc( sizeof( int ) * ( i_mappable_directional_codes ) );
	if( ( * o_directional_code_map_ptr_ptr )->x == NULL )
	{
		free( * o_directional_code_map_ptr_ptr );
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//malloc space for y array
	( * o_directional_code_map_ptr_ptr )->y = 
	(int *)malloc( sizeof( int ) * ( i_mappable_directional_codes ) );
	if( ( * o_directional_code_map_ptr_ptr )->y == NULL )
	{
		free( * o_directional_code_map_ptr_ptr );
		free( ( * o_directional_code_map_ptr_ptr )->x );
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//memory allocation all good
	//fill in structure
	( * o_directional_code_map_ptr_ptr )->mappable_directional_codes = 
	i_mappable_directional_codes;
	//zero out array entries
	for( i = 0; i < ( i_mappable_directional_codes ); i++ )
	{
		( * o_directional_code_map_ptr_ptr )->x[i] = 0;
		( * o_directional_code_map_ptr_ptr )->y[i] = 0;
	}
	
	//return success
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_DIRECTIONAL_CODE_MAP_Release()
 *	Description:
 *		Frees all dynamically allocated memories for the AUCR_Directional_Code_Map
 *		and NULLs the AUCR_Directional_Code_Map_Ptr.
 *	Parameters:
 *		io_directional_code_map_ptr_ptr <-> address of the AUCR_Directional_Code_Map memory to be freed.
 *	Preconditions:
 *		The AUCR_Directional_Code_Map_Ptr at io_directional_code_map_ptr_ptr
 *		is a valid address of an AUCR_Directional_Code_Map.
 *	Postconditions:
 *		The AUCR_Directional_Code_Map's "x" and "y" arrays are freed
 *		followed by the memory for the AUCR_Directional_Code_Map as well.
 *		The AUCR_Directional_Code_Map_Ptr at io_directional_code_map_ptr_ptr
 *		is NULL.
 *	Returns:
 *		none
 */
void AUCR_DIRECTIONAL_CODE_MAP_Release(
									   AUCR_Directional_Code_Map_Ptr * io_directional_code_map_ptr_ptr )
{
	//check for valid input
	if( ( io_directional_code_map_ptr_ptr == NULL ) ||
	   ( ( * io_directional_code_map_ptr_ptr ) == NULL ) )
	{
		return;
	}
	
	//free y array
	free( ( * io_directional_code_map_ptr_ptr )->y );
	
	//free x array
	free( ( * io_directional_code_map_ptr_ptr )->x );
	
	//free Directional_Code_Map structure
	free( * io_directional_code_map_ptr_ptr );
	
	//NULL the structure pointer
	( * io_directional_code_map_ptr_ptr ) = NULL;
	
	return;
}


/*
 *	Name:
 *		AUCR_DIRECTIONAL_CODE_MAP_Finalize()
 *	Description:
 *		Prepares a directional code map instance for use in converting from interpolated
 *		characters to characters in the alphabet by setting up data structures required for
 *		arbitrary directional code lookup from arbitrary vectors.
 *	Parameters:
 *		io_directional_code_map_ptr <- pointer to AUCR_Directional_Code_Map that has been
 *							filled in completely with mappable_direction_codes (x, y) pairs.
 *	Preconditions:
 *		io_directional_code_map_ptr is non NULL and points to a valid directional code map
 *		that has mappable_directional_codes (x,y) pairs that each describe the vector that
 *		the begginning of a directional region, and all directional vectors, are in
 *		counterclockwise order, starting from the zeroeth direcional code
 *	Postconditions:
 *		positive_begin, positive_end, negative_begin, and negative_end have all been set
 *		to their appropriate values.
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 */
AUCR_Error AUCR_DIRECTIONAL_CODE_MAP_Finalize(
											  AUCR_Directional_Code_Map_Ptr io_directional_code_map_ptr )
{
	//declare local variables
	int i;
	int posBeginX = 0;
	int posBeginY = 1;
	int posEndX = 0;
	int posEndY = -1;
	int negBeginX = 0;
	int negBeginY = -1;
	int negEndX = 0;
	int negEndY = 1;
	
	
	//check for valid input
	if( io_directional_code_map_ptr == NULL )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//intitialize structure variables to their not found state
	io_directional_code_map_ptr->positive_begin = -1;
	io_directional_code_map_ptr->positive_end = -1;
	io_directional_code_map_ptr->negative_begin = -1;
	io_directional_code_map_ptr->negative_end = -1;
	
	//find correct values for structure variables, if they exist
	for( i = 0; i < io_directional_code_map_ptr->mappable_directional_codes; i++ )
	{
		if( io_directional_code_map_ptr->x[i] > 0 )
		{
			if( ( io_directional_code_map_ptr->x[i] * posBeginY ) >= ( io_directional_code_map_ptr->y[i] * posBeginX ) )
			{
				io_directional_code_map_ptr->positive_begin = i;
				posBeginX = io_directional_code_map_ptr->x[i];
				posBeginY = io_directional_code_map_ptr->y[i];
			}
			if( ( io_directional_code_map_ptr->x[i] * posEndY ) <= ( io_directional_code_map_ptr->y[i] * posEndX ) )
			{
				io_directional_code_map_ptr->positive_end = i;
				posEndX = io_directional_code_map_ptr->x[i];
				posEndY = io_directional_code_map_ptr->y[i];
			}
		}
		else
		{
			if( ( io_directional_code_map_ptr->x[i] * negBeginY ) >= ( io_directional_code_map_ptr->y[i] * negBeginX ) )
			{
				io_directional_code_map_ptr->negative_begin = i;
				negBeginX = io_directional_code_map_ptr->x[i];
				negBeginY = io_directional_code_map_ptr->y[i];
			}
			if( ( io_directional_code_map_ptr->x[i] * negEndY ) <= ( io_directional_code_map_ptr->y[i] * negEndX ) )
			{
				io_directional_code_map_ptr->negative_end = i;
				negEndX = io_directional_code_map_ptr->x[i];
				negEndY = io_directional_code_map_ptr->y[i];
			}
		}
	}
	
	//return success
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_ALPHABET_Init()
 *	Description:
 *		Initializes a new AUCR_ALPHABET instance by allocating appropriate
 *		memories and setting member values.
 *	Parameters:
 *		i_directional_code_map_ptr -> the memory address of a directional code
 *							map structure that is used to fill in the directional
 *							code map used by the alphabet.
 *		i_num_activity_regions -> number of activity regions for to allocate
 *							activity measure memories for.
 *		i_num_directional_codes -> number of directional codes to allocate memory
 *							for.
 *		o_alphabet_ptr_ptr <- memory address of the AUCR_Alphabet_Ptr to be
 *							populated with the new AUCR_Alphabet_Ptr which points to
 *							new AUCR_Alphabet.
 *	Preconditions:
 *		The *AUCR_Alphabet_Ptr at o_alphabet_ptr_ptr
 *		is a valid address of a AUCR_Alphabet_Prt that
 *		is currently set to NULL. i_directional_code_map_ptr
 *		is a valide pointer. i_num_activity_regions,
 *		and i_num_directional_codes, are positive, nonzero numbers.
 *	Postconditions:
 *		the structure pointed to by i_directional_code_map_ptr has been copied,
 *		the values held by i_num_activity_regions and i_num_directional_codes
 *		have been stored, bias is new array of biases(length i_num_activity_regions),
 *		activity_regions is a new array of activity regions (length i_num_activity_regions),
 *		characters_ptr_ptr is set to NULL (to prepare it for realloc), num_characters
 *		is set to zero, and o_alphabet_ptr_ptr points to an AUCR_Alphabet_Ptr which in turn points
 *		to a newly created AUCR_Alphabet structure which contains all the previous information.
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_NO_MEMORY if the structure or array memories could not be allocated
 */
AUCR_Error AUCR_ALPHABET_Init(
							  AUCR_Directional_Code_Map_Ptr i_directional_code_map_ptr,
							  int i_num_activity_regions,
							  int i_num_directional_codes,
							  AUCR_Alphabet_Ptr * o_alphabet_ptr_ptr )
{
	//local variables
	int i, tempbias;
	AUCR_Error temperror;
	
	//verify input
	if( i_num_activity_regions <=0 || i_num_directional_codes <= 0 ||
	   o_alphabet_ptr_ptr == NULL || *o_alphabet_ptr_ptr != NULL ||
	   i_directional_code_map_ptr == NULL )
	{
		//bad input, return
		return( AUCR_ERR_FAILED );
	}
	
	//allocate memory for alphabet structure
	( *o_alphabet_ptr_ptr ) = ( AUCR_Alphabet_Ptr ) malloc( sizeof( AUCR_Alphabet ) );
	if( ( *o_alphabet_ptr_ptr ) == NULL )
	{
		//unable to get memory, return
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//allocate memory for new directional code map stucture
	( ( *o_alphabet_ptr_ptr )->directional_code_map_ptr ) = NULL;
	temperror = AUCR_DIRECTIONAL_CODE_MAP_Init( 
											   i_directional_code_map_ptr->mappable_directional_codes,
											   &( ( *o_alphabet_ptr_ptr )->directional_code_map_ptr ) );
	if( temperror != AUCR_ERR_SUCCESS )
	{
		//if there is an error, throw it up
		free( *o_alphabet_ptr_ptr );
		return( temperror );
	}
	
	//allocate memory for activity region arrays
	( ( *o_alphabet_ptr_ptr )->activity_regions ) = 
	( AUCR_Activity_Region_Ptr )malloc( i_num_activity_regions * sizeof( AUCR_Activity_Region ) );
	if( ( ( *o_alphabet_ptr_ptr )->activity_regions ) == NULL )
	{
		free( ( *o_alphabet_ptr_ptr ) );
		AUCR_DIRECTIONAL_CODE_MAP_Release( &( ( * o_alphabet_ptr_ptr )->directional_code_map_ptr ) );
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//allocate memory for activity region biases
	( ( *o_alphabet_ptr_ptr )->bias ) = (int *)malloc( i_num_activity_regions * sizeof( int ) );
	if( ( ( *o_alphabet_ptr_ptr )->bias ) == NULL )
	{
		free( ( *o_alphabet_ptr_ptr ) );
		AUCR_DIRECTIONAL_CODE_MAP_Release( &( ( * o_alphabet_ptr_ptr )->directional_code_map_ptr ) );
		free( ( *o_alphabet_ptr_ptr )->activity_regions );
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//memory allocated, errors avoided, go ahead and fill in values
	( ( *o_alphabet_ptr_ptr )->num_directional_codes ) = i_num_directional_codes;
	( ( *o_alphabet_ptr_ptr )->num_activity_regions ) = i_num_activity_regions;
	( ( ( *o_alphabet_ptr_ptr )->directional_code_map_ptr )->mappable_directional_codes ) =
    i_directional_code_map_ptr->mappable_directional_codes;      
	( ( *o_alphabet_ptr_ptr )->num_characters ) = 0;
	( ( *o_alphabet_ptr_ptr )->characters_ptr_ptr ) = NULL;
	
	//copy x and y arrays from i_directional code map into
	//directional code map in the alphabet
	for( i = 0; i < i_directional_code_map_ptr->mappable_directional_codes; i++ )
	{
		( ( *o_alphabet_ptr_ptr )->directional_code_map_ptr )->x[i] = i_directional_code_map_ptr->x[i];
		( ( *o_alphabet_ptr_ptr )->directional_code_map_ptr )->y[i] = i_directional_code_map_ptr->y[i];
	}
	
	//finalize internal dircode map
	temperror = AUCR_DIRECTIONAL_CODE_MAP_Finalize( 
												   ( *o_alphabet_ptr_ptr )->directional_code_map_ptr );
	if( temperror != AUCR_ERR_SUCCESS )
	{
		free( ( *o_alphabet_ptr_ptr ) );
		AUCR_DIRECTIONAL_CODE_MAP_Release( 
										  &( ( * o_alphabet_ptr_ptr )->directional_code_map_ptr ) );
		free( ( *o_alphabet_ptr_ptr )->activity_regions );
		free( ( *o_alphabet_ptr_ptr )->bias );
		return( temperror );
	}
	
	//fill in bias array
	tempbias = AUCR_SCALE;
	//  tempbias = AUCR_Rounding_Divide( AUCR_SCALE * i_num_directional_codes *
	//      ( i_directional_code_map_ptr->mappable_directional_codes / 2 ) *
	//      ( i_directional_code_map_ptr->mappable_directional_codes / 2 ),
	//      i_num_activity_regions 
	//      * ( i_directional_code_map_ptr->mappable_directional_codes + 1 )
	//      * ( i_directional_code_map_ptr->mappable_directional_codes + 1 ) );
	for( i = 0; i < i_num_activity_regions; i++ )
	{
		( *o_alphabet_ptr_ptr )->bias[i] = tempbias;
	}
	
	//all done, return success
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_ALPHABET_Release()
 *	Description:
 *		Frees all dynamically allocated memories for the AUCR_ALPHABET_Release
 *		and NULLs the AUCR_Alphabet_Ptr.
 *	Parameters:
 *		io_alphabet_ptr_ptr <-> address of the AUCR_Alphabet memory to be freed.
 *	Preconditions:
 *		The AUCR_Alphabet_Ptr at io_alphabet_ptr_ptr
 *		is a valid address of an AUCR_Alphabet.
 *	Postconditions:
 *		num_characters AUCR_Characters have been freed, the characters_ptr_ptr array
 *		has been freed, the biases array has been freed, the activity_regions array
 *		has been freed, the AUCR_Directional_Code_Map has been freed, the AUCR_Alphabet
 *		structure has been freed and the AUCR_Alphabet_Ptr at io_alphabet_ptr_ptr has
 *		been set to NULL.
 *	Returns:
 *		none
 */
void AUCR_ALPHABET_Release(
						   AUCR_Alphabet_Ptr * io_alphabet_ptr_ptr )
{
	//local variables
	int i;
	
	//check for valid input
	if( ( io_alphabet_ptr_ptr == NULL ) && ( ( *io_alphabet_ptr_ptr ) == NULL ) )
	{
		return;
	}
	
	//free directional code map
	AUCR_DIRECTIONAL_CODE_MAP_Release( &( *io_alphabet_ptr_ptr )->directional_code_map_ptr );
	
	//free characters in character array
	for( i = 0; i < ( *io_alphabet_ptr_ptr )->num_characters; i++ )
	{
		free( ( *io_alphabet_ptr_ptr )->characters_ptr_ptr[i] );
	}
	
	//free character array itself
	free( ( *io_alphabet_ptr_ptr )->characters_ptr_ptr );
	
	//free activity regions array
	free( ( *io_alphabet_ptr_ptr )->activity_regions );
	
	//free bias array
	free( ( *io_alphabet_ptr_ptr )->bias );
	
	//free alphabet structure
	free( ( *io_alphabet_ptr_ptr ) );
	
	//NULL alphabet structure pointer
	( *io_alphabet_ptr_ptr ) = NULL;
	
	return;
}


/*
 *	Name:
 *		AUCR_ALPHABET_Add_Interpolated_Character()
 *	Description:
 *		Adds the specified character to the specified alphabet
 *	Parameters:
 *		i_interpolated_character_ptr -> address of an interpolated character
 *		io_alphabet_ptr -> address of the alphabet to add the character to
 *	Preconditions:
 *		i_interpolated_character_ptr is the address of a valid AUCR_Interpolated_Character,
 *		io_alphabet_ptr is the address of a valid AUCR_Alphabet, and the num_directional_codes
 *		for the alphabet is in agreement with the num_directional_codes for the character
 *	Postconditions:
 *		interpolated character has been converted to character, characters_ptr_ptr is
 *		reallocated to have room for new character, which is stored in character array
 *		of the alphabet, num_characters has been incremented by 1,
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_NO_MEMORY if the structure or array memories could not be allocated
 *		AUCR_ERR_CONFLICTING_PARAMETERS if all preconditions except the agreement of
 *													num_directional_codes are met
 */
AUCR_Error AUCR_ALPHABET_Add_Interpolated_Character(
													AUCR_Interpolated_Character_Ptr i_interpolated_character_ptr,
													AUCR_Alphabet_Ptr io_alphabet_ptr )
{
	//local variables
	AUCR_Character_Ptr tempchar = NULL;
	AUCR_Error temperror;
	
	//check for valid input
	if( i_interpolated_character_ptr == NULL || io_alphabet_ptr == NULL )
	{
		return( AUCR_ERR_FAILED );
	}
	else if( i_interpolated_character_ptr->num_directional_codes !=
			io_alphabet_ptr->num_directional_codes )
	{
		return( AUCR_ERR_CONFLICTING_PARAMETERS );
	}
	
	//create character
	temperror = AUCR_CHARACTER_Init(
									i_interpolated_character_ptr->unicode, io_alphabet_ptr->num_directional_codes,
									io_alphabet_ptr->num_activity_regions, &tempchar );
	if( temperror != AUCR_ERR_SUCCESS )
	{
		//if creation returns an error, throw it up
		return( temperror );
	}
	
	//convert interpolated character into character
	temperror = AUCR_CHARACTER_From_Interpolated(
												 i_interpolated_character_ptr, io_alphabet_ptr, tempchar );
	if( temperror != AUCR_ERR_SUCCESS )
	{
		//if conversion returns an error, throw it up
		AUCR_CHARACTER_Release( &tempchar );
		return( temperror );
	}
	
	//increment number of characters in the alphabet
	( io_alphabet_ptr->num_characters )++;
	
	//make room in character array for just one more
	io_alphabet_ptr->characters_ptr_ptr = 
	(AUCR_Character_Ptr *)realloc( ( io_alphabet_ptr->characters_ptr_ptr ),
								  ( ( io_alphabet_ptr->num_characters ) * sizeof( AUCR_Character_Ptr ) ) );
	
	if( io_alphabet_ptr->characters_ptr_ptr == NULL )
	{
		//if realloc fails, return out of memory
		AUCR_CHARACTER_Release( &tempchar );
		( io_alphabet_ptr->num_characters )--;
		return( AUCR_ERR_NO_MEMORY );
	}
	
	//add new character
	io_alphabet_ptr->characters_ptr_ptr[io_alphabet_ptr->num_characters - 1] = tempchar;
	
	//everything worked, return success
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_ALPHABET_Add_Raw()
 *	Description:
 *		Converts the supplied raw coordinates to an interpolated character
 *		and adds the interpolated character to the specified alphabet
 *	Parameters:
 *		i_unicode -> the unicode value the character represents
 *		i_num_coordinates -> the number of raw coordinates given
 *		i_coordinates -> array of i_num_coordinates coordinates
 *		io_alphabet_ptr -> address of the alphabet to add the character to
 *	Preconditions:
 *		i_num_coordinates is a positive integer
 *		i_coordinates is non NULL and is the address of a valid array of
 *							coordinates of length i_num_coordinates
 *		io_alphabet_ptr is the address of a valid AUCR_Alphabet
 *	Postconditions:
 *		raw coordinates have been converted to an interpolated character, which is then
 *		converted to character, stored in character array of the alphabet, num_characters
 *		has been incremented by 1, characters_ptr_ptr is reallocated to have room for new character
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_NO_MEMORY if the structure or array memories could not be allocated
 */
AUCR_Error AUCR_ALPHABET_Add_Raw(
								 wchar_t i_unicode,
								 int i_num_coordinates,
								 AUCR_Coordinate * i_coordinates,
								 AUCR_Alphabet_Ptr io_alphabet_ptr )
{
	//local variables
	AUCR_Interpolated_Character_Ptr tempinterpchar = NULL;
	AUCR_Error temperror;
	
	//check for valid input
	if( i_num_coordinates < 1 || i_coordinates == NULL || io_alphabet_ptr == NULL )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//create interpolated character
	temperror = AUCR_INTERPOLATED_CHARACTER_Init( i_unicode,
												 io_alphabet_ptr->num_directional_codes, &tempinterpchar );
	if( temperror != AUCR_ERR_SUCCESS )
	{
		//if creation returns an error, throw it up
		return( temperror );
	}
	
	//convert raw coordinates to interpolated character
	temperror = AUCR_INTERPOLATED_CHARACTER_From_Raw(
													 i_num_coordinates, i_coordinates, tempinterpchar );
	if( temperror != AUCR_ERR_SUCCESS )
	{
		//if conversion returns an error, throw it up
		AUCR_INTERPOLATED_CHARACTER_Release( &tempinterpchar );
		return( temperror );
	}
	
	//add interpolated character to the alphabet
	temperror = AUCR_ALPHABET_Add_Interpolated_Character( tempinterpchar, io_alphabet_ptr );
	
	//free tempinterpchar memory
	AUCR_INTERPOLATED_CHARACTER_Release( &tempinterpchar );
	
	//return the value returned by add_interpolated
	return( temperror );
}


/*
 *	Name:
 *		AUCR_ALPHABET_Remove_Character()
 *	Description:
 *		Removes the specified character from the specified alphabet
 *	Parameters:
 *			i_character_number -> index of the character in the alphabets
 *											character array
 *			io_alphabet_ptr -> address of the alphabet to remove the character from
 *	Preconditions:
 *		i_character_number is a valid array index, greater than or equal to zero, and
 *		less than num_characters, io_alphabet_ptr is a valid pointer to a valid
 *		AUCR_Alphabet structure
 *	Postconditions:
 *		the character at characters_ptr_ptr[i_character_number] is removed from this
 *		array, and the memory reserved for the character structure is freed, the memory
 *		reserved for the the characters_ptr_ptr array is reallocated and num_characters
 *		is decremented by one
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_NO_MEMORY if the structure or array memories could not be allocated
 */
AUCR_Error AUCR_ALPHABET_Remove_Character(
										  int i_character_number, /* which character in the character array to remove */
										  AUCR_Alphabet_Ptr io_alphabet_ptr )
{
	//check for valid input
	if( io_alphabet_ptr == NULL || i_character_number < 0 ||
	   i_character_number >= io_alphabet_ptr->num_characters )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//free character
	AUCR_CHARACTER_Release( &( io_alphabet_ptr->characters_ptr_ptr[i_character_number] ) );
	
	//if not getting ride of the last charcter, then move last character
	//into the spot formerly held by character that has been deleted
	if( i_character_number < ( io_alphabet_ptr->num_characters - 1 ) )
	{
		io_alphabet_ptr->characters_ptr_ptr[i_character_number] = 
		io_alphabet_ptr->characters_ptr_ptr[io_alphabet_ptr->num_characters - 1];
	}
	
	//decrement number of characters in the alphabet
	( io_alphabet_ptr->num_characters )--;
	
	//resize character array if there are still characters in it
	//avoid resizeing character if there are no characters, as this
	//will free array memory and complicate adding a character later
	//or releasing the alphabet later.
	if( io_alphabet_ptr->num_characters > 0 )
	{
		( io_alphabet_ptr->characters_ptr_ptr ) = 
		(AUCR_Character_Ptr *)realloc( ( io_alphabet_ptr->characters_ptr_ptr ),
									  ( ( io_alphabet_ptr->num_characters ) * sizeof( AUCR_Character_Ptr ) ) );
	}
	
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_ALPHABET_Recognize_From_Interpolated()
 *	Description:
 *		finds the character in the alphabet that is closest to the
 *		interpolated character given as an argument and return the
 *		unicode of this found character
 *	Parameters:
 *			i_interpolated_character_ptr -> interpolated character that we will look
 *														for a match for
 *			io_alphabet_ptr <-> address of the alphabet to find the character in
 *			o_unicode_ptr <- address of wchar_t variable to store unicode in
 *	Preconditions:
 *		i_interpolated_character_ptr is a valid pointer to a valid interpolated character,
 *		io_alphabet_ptr is a valid pointer to a valid pointer to a valid alphabet,
 *		num_directional_codes between the alphabet and the character is in agreement
 *	Postconditions:
 *		the closest character in the alphabet has been found, and its unicode value
 *		is stored in the variable pointed to by o_unicode_ptr
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_NO_MEMORY if the structure or array memories could not be allocated
 *		AUCR_ERR_CONFLICTING_PARAMETERS if all preconditions except the agreement of
 *													num_directional_codes are met
 */
AUCR_Error AUCR_ALPHABET_Recognize_From_Interpolated(
													 AUCR_Interpolated_Character_Ptr i_interpolated_character_ptr,
													 AUCR_Alphabet_Ptr i_alphabet_ptr,
													 wchar_t * o_unicode_ptr )
{
	//local variables
	AUCR_Character_Ptr tempchar = NULL;
	AUCR_Error temperror;
	int i, j;
	unsigned int minindex = 0, tempmin, tempmax;
	unsigned long int difference, mindifference = 0;
	
	//check for valid input
	if( i_interpolated_character_ptr == NULL || i_alphabet_ptr == NULL )
	{
		return( AUCR_ERR_FAILED );
	}
	else if( i_interpolated_character_ptr->num_directional_codes !=
			i_alphabet_ptr->num_directional_codes )
	{
		return( AUCR_ERR_CONFLICTING_PARAMETERS );
	}
	
	//create character
	temperror = AUCR_CHARACTER_Init(
									i_interpolated_character_ptr->unicode, i_alphabet_ptr->num_directional_codes,
									i_alphabet_ptr->num_activity_regions, &tempchar );
	if( temperror != AUCR_ERR_SUCCESS )
	{
		//if creation returns an error, throw it up
		return( temperror );
	}
	
	//convert interpolated character into character
	temperror = AUCR_CHARACTER_From_Interpolated(
												 i_interpolated_character_ptr, i_alphabet_ptr, tempchar );
	if( temperror != AUCR_ERR_SUCCESS )
	{
		//if conversion returns an error, throw it up
		AUCR_CHARACTER_Release( &tempchar );
		return( temperror );
	}
	
	//check new character against each character in the alphabet
	for( i = 0; i < i_alphabet_ptr->num_characters; i++ )
	{
		//zero out difference for newest character
		difference = 0;
		
		//check all activity measure differences
		for( j = 0; j < i_alphabet_ptr->num_activity_regions; j++ )
		{
			if( tempchar->activity_measures[j] <
			   i_alphabet_ptr->characters_ptr_ptr[i]->activity_measures[j] )
			{
				difference += AUCR_Rounding_Divide( 
												   ( i_alphabet_ptr->characters_ptr_ptr[i]->activity_measures[j]
													- tempchar->activity_measures[j] )
												   * ( i_alphabet_ptr->characters_ptr_ptr[i]->activity_measures[j]
													  - tempchar->activity_measures[j] ) * i_alphabet_ptr->bias[j],
												   AUCR_SCALE * AUCR_SCALE );
			}
			else
			{
				difference += AUCR_Rounding_Divide( 
												   i_alphabet_ptr->bias[j] * ( tempchar->activity_measures[j] - 
																			  i_alphabet_ptr->characters_ptr_ptr[i]->activity_measures[j] )
												   * ( tempchar->activity_measures[j] -
													  i_alphabet_ptr->characters_ptr_ptr[i]->activity_measures[j] ),
												   AUCR_SCALE * AUCR_SCALE );
			}
		}
		
		//skip remainder of this checking this character if
		//we are already sure that this character is not the
		//closest
		if( ( difference > mindifference ) && ( i != 0 ) )
		{
			continue;
		}
		
		//check all directional code differences
		for( j = 0; j < i_alphabet_ptr->num_directional_codes; j++ )
		{
			if( tempchar->directional_codes[j] <
			   i_alphabet_ptr->characters_ptr_ptr[i]->directional_codes[j] )
			{
				tempmax = i_alphabet_ptr->characters_ptr_ptr[i]->directional_codes[j];
				tempmin = tempchar->directional_codes[j];
			}
			else
			{
				tempmin = i_alphabet_ptr->characters_ptr_ptr[i]->directional_codes[j];
				tempmax = tempchar->directional_codes[j];
			}
			if( ( tempmax - tempmin ) <
			   ( i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes
				+ tempmin - tempmax ) )
			{
				difference += ( ( tempmax - tempmin ) 
							   * ( tempmax - tempmin ) * AUCR_SCALE );
			}
			else
			{
				difference +=
				( ( i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes
				   + tempmin - tempmax )
				 * ( i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes
					+ tempmin - tempmax ) * AUCR_SCALE );
			}
		}
		
		//check to see if this character is closer than previous
		//closest character
		if( ( difference < mindifference ) || ( i == 0 ) )
		{
			mindifference = difference;
			minindex = i;
		}
		
	}
	//index of closest character now stored in minindex
	
	//retrieve unicode value of closest character and place in o_unicode_ptr
	( *o_unicode_ptr ) = i_alphabet_ptr->characters_ptr_ptr[minindex]->unicode;
	
	//free character memory
	AUCR_CHARACTER_Release( &tempchar );
	
	//return success
	return( AUCR_ERR_SUCCESS );
	
}


/*
 *	Name:
 *		AUCR_ALPHABET_Recognize_From_Raw()
 *	Description:
 *		finds the character in the alphabet that is closest to the
 *		interpolated character that is created from the raw data that is given,
 *		then return the unicode of this found character
 *	Parameters:
 *			i_num_coordinates -> number of raw coordinates supplied
 *			i_coordinates -> array of raw coordinates of length i_num_coordinate
 *			io_alphabet_ptr <-> pointer to alphabet to search against
 *			o_unicode_ptr <- address of wchar_t variable to store unicode in
 *	Preconditions:
 *		i_num coordinates is positive integer
 *		i_coordinates is non NULL and points to a valid array of
 *							of i_num_coordinates coordinates
 *		io_alphabet_ptr is a valid pointer to a valid pointer to a valid alphabet,
 *	Postconditions:
 *		the closest character in the alphabet has been found, and its unicode value
 *		is stored in the variable pointed to by o_unicode_ptr
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_NO_MEMORY if the structure or array memories could not be allocated
 */
AUCR_Error AUCR_ALPHABET_Recognize_From_Raw(
											int i_num_coordinates,
											AUCR_Coordinate * i_coordinates,
											AUCR_Alphabet_Ptr i_alphabet_ptr,
											wchar_t * o_unicode_ptr )
{
	//local variables
	AUCR_Interpolated_Character_Ptr tempinterpchar = NULL;
	AUCR_Error temperror;
	
	//check for valid input
	if( i_num_coordinates < 1 || i_coordinates == NULL || i_alphabet_ptr == NULL )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//create interpolated character
	temperror = AUCR_INTERPOLATED_CHARACTER_Init( 0x0001,
												 i_alphabet_ptr->num_directional_codes, &tempinterpchar );
	if( temperror != AUCR_ERR_SUCCESS )
	{
		//if creation returns an error, throw it up
		return( temperror );
	}
	
	//convert raw coordinates to interpolated character
	temperror = AUCR_INTERPOLATED_CHARACTER_From_Raw(
													 i_num_coordinates, i_coordinates, tempinterpchar );
	if( temperror != AUCR_ERR_SUCCESS )
	{
		//if conversion returns an error, throw it up
		AUCR_INTERPOLATED_CHARACTER_Release( &tempinterpchar );
		return( temperror );
	}
	
	//call interpolated_recognize
	temperror = AUCR_ALPHABET_Recognize_From_Interpolated(
														  tempinterpchar, i_alphabet_ptr, o_unicode_ptr ); 
	
	//free interpolated character memory
	AUCR_INTERPOLATED_CHARACTER_Release( &tempinterpchar );
	
	//return value returned by interpolated_recognize
	return( temperror );
}


/*
 *	Name:
 *		AUCR_ALPHABET_Save_To_File()
 *	Description:
 *		takes the alphabet given as a parameter, and saves all the data contained
 *		in the alphabet as an ascii file that is given by the file pointer given as
 *		an argument.  
 *	Parameters:
 *		i_alphabet_ptr -> pointer to alphabet that will be saved
 *		io_file_path -> pointer to a string that gives the full path of the file
 *	Preconditions:
 *		i_alphabet_ptr is a non-NULL, valid pointer to a valid alphabet structure,
 *		and i_file_ptr is a valid pointer to a valid string that gives the full pathname
 *		for a file that can be written without causing an error.
 *	Postconditions:
 *		the alphabet has been written to the file, and the alphabet itself is
 *		unchanged.
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 */
AUCR_Error AUCR_ALPHABET_Save_To_File( 
									  const char * io_file_path,
									  AUCR_Alphabet_Ptr i_alphabet_ptr )
{
	//local variables
	FILE * tempfp;
	int i;
	
	//check for valid input
	if( io_file_path == NULL || i_alphabet_ptr == NULL )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//open file
	tempfp = fopen( io_file_path, "wb" );
	if( tempfp == NULL )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//write number of characters 
	fwrite( &( i_alphabet_ptr->num_characters ), sizeof( int ), 1, tempfp );
	
	//write number of activity regions
	fwrite( &( i_alphabet_ptr->num_activity_regions ),
		   sizeof( int ), 1, tempfp );
	
	//write number of directional codes
	fwrite( &( i_alphabet_ptr->num_directional_codes ),
		   sizeof( int ), 1, tempfp );
	
	//write mappable directional codes
	fwrite( &( i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes ),
		   
		   sizeof( int ), 1, tempfp );
	
	//write directional code map structure arrays
	fwrite( i_alphabet_ptr->directional_code_map_ptr->x, sizeof( int ), 
		   i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes, tempfp );
	fwrite( i_alphabet_ptr->directional_code_map_ptr->y, sizeof( int ), 
		   i_alphabet_ptr->directional_code_map_ptr->mappable_directional_codes, tempfp );
	
	//write setup info for directional code map 
	fwrite( &( i_alphabet_ptr->directional_code_map_ptr->positive_begin ), 
		   sizeof( int ), 1, tempfp );
	fwrite( &( i_alphabet_ptr->directional_code_map_ptr->negative_begin ), 
		   sizeof( int ), 1, tempfp );
	fwrite( &( i_alphabet_ptr->directional_code_map_ptr->positive_end ), 
		   sizeof( int ), 1, tempfp );
	fwrite( &( i_alphabet_ptr->directional_code_map_ptr->negative_end ), 
		   sizeof( int ), 1, tempfp ); 
	
	//write all character structures
	for( i = 0; i < i_alphabet_ptr->num_characters; i++ )
	{
		fwrite( &( i_alphabet_ptr->characters_ptr_ptr[i]->unicode ), 
			   
			   sizeof( wchar_t ), 1, tempfp );
		fwrite( &( i_alphabet_ptr->characters_ptr_ptr[i]->num_directional_codes ), 
			   sizeof( int ), 1, tempfp );
		fwrite( &( i_alphabet_ptr->characters_ptr_ptr[i]->num_activity_regions ), 
			   sizeof( int ), 1, tempfp );
		
		//write directional code array
		fwrite( i_alphabet_ptr->characters_ptr_ptr[i]->directional_codes, sizeof( int ),
			   i_alphabet_ptr->characters_ptr_ptr[i]->num_directional_codes, tempfp );
		
		//write activity measures array
		fwrite( i_alphabet_ptr->characters_ptr_ptr[i]->activity_measures, sizeof( int ),
			   i_alphabet_ptr->characters_ptr_ptr[i]->num_activity_regions, tempfp );
	}
	
	//write all activity region structures
	fwrite( i_alphabet_ptr->activity_regions, sizeof( AUCR_Activity_Region ),
		   i_alphabet_ptr->num_activity_regions, tempfp );
	
	//write all biases
	fwrite( i_alphabet_ptr->bias, sizeof( int ),
		   i_alphabet_ptr->num_activity_regions, tempfp ); 
	
	if( fclose( tempfp ) != 0 )
	{
		return( AUCR_ERR_FAILED );
	}
	
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_ALPHABET_Open_From_File()
 *	Description:
 *		takes the alphabet given as a pointer to a pointer parameter, and fills it with the
 *		information contained in the file that is given as a parameter.
 *	Parameters:
 *		io_alphabet_ptr_ptr -> pointer to pointer to alphabet that will be filled in
 *		i_file_path -> pointer to a string that gives the full path of
 *							the file
 *	Preconditions:
 *		io_alphabet_ptr_ptr is a a valid pointer to a NULL alphabet pointer
 *		and i_file_path is a valid pointer to a valid string that is the
 *		path name of a valid file that was previously created by a call to the 
 *		AUCR_ALPHABET_Save_To_File() function.
 *	Postconditions:
 *		the alphabet has been filled in with the information from the file, and the
 *		file itself remains unchanged.
 *	Returns:
 *		AUCR_ERR_SUCCESS
 *		AUCR_ERR_FAILED if preconditions are unmet.
 *		AUCR_ERR_NO_MEMORY if the structure or array memories could not be allocated
 */
AUCR_Error AUCR_ALPHABET_Open_From_File(
										const char * i_file_path,
										AUCR_Alphabet_Ptr * io_alphabet_ptr_ptr )
{
	//local variables
	FILE * tempfp;
	int temp_num_characters, temp_num_activity_regions;
	int temp_num_directional_codes, temp_mappable_directional_codes;
	int i;
	wchar_t temp_unicode;
	AUCR_Error temperr;
	AUCR_Directional_Code_Map_Ptr tempdircode = NULL;
	
	//check for valid input
	if( i_file_path == NULL || io_alphabet_ptr_ptr == NULL
	   || ( *io_alphabet_ptr_ptr ) != NULL )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//open file
	tempfp = fopen( i_file_path, "rb" );
	if( tempfp == NULL )
	{
		return( AUCR_ERR_FAILED );
	}
	
	//read number of characters 
	fread( &temp_num_characters, sizeof( int ), 1, tempfp );
	
	//read number of activity regions
	fread( &temp_num_activity_regions,
		  sizeof( int ), 1, tempfp );
	
	//read number of directional codes
	fread( &temp_num_directional_codes,
		  sizeof( int ), 1, tempfp );
	
	//read mappable directional codes
	fread( &temp_mappable_directional_codes,
		  sizeof( int ), 1, tempfp );
	
	//create directional code map structure
	temperr = AUCR_DIRECTIONAL_CODE_MAP_Init(
											 temp_mappable_directional_codes, &tempdircode );
	if( temperr != AUCR_ERR_SUCCESS )
	{
		fclose( tempfp );
		return( temperr );
	}
	
	//fill in directional code map structure
	fread( tempdircode->x, sizeof( int ), 
		  temp_mappable_directional_codes, tempfp );
	fread( tempdircode->y, sizeof( int ), 
		  temp_mappable_directional_codes, tempfp );
	
	//create alphabet
	temperr = AUCR_ALPHABET_Init( tempdircode, temp_num_activity_regions,
								 temp_num_directional_codes, io_alphabet_ptr_ptr );
	if( temperr != AUCR_ERR_SUCCESS )
	{
		fclose( tempfp );
		AUCR_DIRECTIONAL_CODE_MAP_Release( &tempdircode );
		return( temperr );
	}
	
	//release temporary directional code map
	AUCR_DIRECTIONAL_CODE_MAP_Release( &tempdircode );
	
	//fill dir code mapping
	fread( &( ( *io_alphabet_ptr_ptr )->directional_code_map_ptr->positive_begin ), 
		  sizeof( int ), 1, tempfp );
	fread( &( ( *io_alphabet_ptr_ptr )->directional_code_map_ptr->negative_begin ), 
		  sizeof( int ), 1, tempfp );
	fread( &( ( *io_alphabet_ptr_ptr )->directional_code_map_ptr->positive_end ), 
		  sizeof( int ), 1, tempfp );
	fread( &( ( *io_alphabet_ptr_ptr )->directional_code_map_ptr->negative_end ), 
		  sizeof( int ), 1, tempfp );  
	
	//create room in the character array
	( *io_alphabet_ptr_ptr )->num_characters = 0;
	( *io_alphabet_ptr_ptr )->characters_ptr_ptr = ( AUCR_Character_Ptr * )
	malloc( sizeof( AUCR_Character_Ptr ) * temp_num_characters );
	if( ( *io_alphabet_ptr_ptr )->characters_ptr_ptr == NULL )
	{
		fclose( tempfp );
		AUCR_ALPHABET_Release( io_alphabet_ptr_ptr );
		return( temperr );
	}
	//null all pointers
	for( i = 0; i < temp_num_characters; i++ )
	{
		( *io_alphabet_ptr_ptr )->characters_ptr_ptr[i] = NULL;
	}
	
	//create all character structures
	for( i = 0; i < temp_num_characters; i++ )
	{
		fread( &temp_unicode, sizeof( wchar_t ), 1, tempfp );
		fread( &temp_num_directional_codes, sizeof( int ), 1, tempfp );
		fread( &temp_num_activity_regions, sizeof( int ), 1, tempfp );
		
		temperr = AUCR_CHARACTER_Init( temp_unicode, temp_num_directional_codes,
									  temp_num_activity_regions, &( ( *io_alphabet_ptr_ptr )->characters_ptr_ptr[i] ) );
		if( temperr != AUCR_ERR_SUCCESS )
		{
			fclose( tempfp );
			AUCR_ALPHABET_Release( io_alphabet_ptr_ptr );
			return( temperr );
		}
		
		//fill in directional code array
		fread( ( *io_alphabet_ptr_ptr )->characters_ptr_ptr[i]->directional_codes,
			  sizeof( int ), temp_num_directional_codes, tempfp );
		
		//fill in activity measures array
		fread( ( *io_alphabet_ptr_ptr )->characters_ptr_ptr[i]->activity_measures, 
			  sizeof( int ), temp_num_activity_regions, tempfp );
		
		//character added, increment num_characters
		( ( *io_alphabet_ptr_ptr )->num_characters )++;
	}
	
	//fill in all activity region structures
	fread( ( *io_alphabet_ptr_ptr )->activity_regions, sizeof( AUCR_Activity_Region ),
		  ( *io_alphabet_ptr_ptr )->num_activity_regions, tempfp );
	
	//fill in all biases
	fwrite( ( *io_alphabet_ptr_ptr )->bias, sizeof( int ),
		   ( *io_alphabet_ptr_ptr )->num_activity_regions, tempfp ); 
	
	//close file
	if( fclose( tempfp ) != 0 )
	{
		return( AUCR_ERR_FAILED );
	}
	
	return( AUCR_ERR_SUCCESS );
}


/*
 *	Name:
 *		AUCR_Rounding_Divide()
 *	Description:
 *		performs division on two integers, but instead of truncating
 *		the remainder, rounds up or down to the nearest integer
 *	Parameters:
 *		i_dividend -> the integer to be divided
 *		i_divisor -> the integer to divide by
 *	Preconditions:
 *		i_dividend and i_divisor are valid integers
 *	Postconditions:
 *		nothing has changed
 *	Returns:
 *		integer result of the rounded divide
 */
int AUCR_Rounding_Divide( int i_dividend, int i_divisor )
{
	//declare variables
	unsigned long int d1, d2;
	//using "long" int essentially does nothing on 32 bit machines, but
	//on 16 bit processors, this makes sure 32 bit integers are used
	
	if( i_divisor == 0 )
	{
		return( 0 );
	}
	
	d1 = (unsigned long int)( ( i_dividend < 0 ) ? ( i_dividend * -1 ) : i_dividend );
	d2 = (unsigned long int)( ( i_divisor < 0 ) ? ( i_divisor * -1 ) : i_divisor );
	if(( ( ( d1 % d2 ) * AUCR_SCALE ) ) >= ( ( d2 * AUCR_SCALE ) / 2 ) )
	{
		if( ( ( i_divisor > 0 ) && ( i_dividend > 0 ) ) ||
		   ( ( i_divisor < 0 ) && ( i_dividend < 0 ) ) )
		{
			return( (int) ( ( i_dividend / i_divisor ) + 1 ) );
		}
		else
		{
			return( (int) ( ( i_dividend / i_divisor ) - 1 ) );
		}
	}
	else
	{
		return( (int) ( i_dividend / i_divisor ) );
	}
}


/*
 *	Name:
 *		AUCR_Integer_Square_Root()
 *	Description:
 *		function that takes an integer argument, multiplies it
 *		by (AUCR_SCALE)^2, then finds the integer approximation
 *		of the square root of this number, using only integer
 *		operations and the newton-raphson method. checks for
 *		overflow along the way and calls AUCR_Un_Scaled_Integer_Square_Root()
 *		on the original problem if overflow is detected, and returns
 *		the value it returns multiplied times the scale factor,
 *		such that the function will work numbers of any size
 *		representable by an int type, although larger numbers
 *		will have less accuracy
 *	Parameters:
 *		i_number -> the number to scale and take the square root of
 *	Preconditions:
 *		i_number is a valid (nonnegative), unscaled integer
 *	Postconditions:
 *		nothing has changed
 *	Returns:
 *		integer result of the square root of the scaled input if
 *		no error, -1 on error
 */
int AUCR_Integer_Square_Root( int i_number )
{
	//declare variables
	unsigned long int square, tempguess, tempdiv;
	long int difference = 10, olddifference = 11, guess = AUCR_SCALE;
	signed char sign;
	//using "long" int essentially does nothing on 32 bit machines, but
	//on 16 bit processors, this makes sure 32 bit integers are used
	
	//check for valid input
	if( i_number < 0 )
	{
		return( -1 );
	}
	
	//check for obvious case
	if( i_number == 0 )
	{
		return( 0 );
	}
	
	tempguess = (unsigned long int)( i_number * AUCR_SCALE );
	
	//check for overflow
	if( ( tempguess / AUCR_SCALE ) != (unsigned long int)i_number )
	{
		//call AUCR_Un_Scaled_Integer_Square_Root for smaller value
		return( AUCR_SCALE * AUCR_Un_Scaled_Integer_Square_Root( i_number ) );
	}
	
	square = tempguess * AUCR_SCALE;
	//check for overflow
	if( ( square / AUCR_SCALE ) != tempguess )
	{
		//call AUCR_Un_Scaled_Integer_Square_Root for smaller value
		return( AUCR_SCALE * AUCR_Un_Scaled_Integer_Square_Root( i_number ) );
	}
	
	//integer based, so difference of +/- 1
	//is the best we can do.
	while( ( difference != 0 ) && ( difference != olddifference ) 
		  && ( difference != ( -1 * olddifference ) ) )
	{ 
		tempguess = (unsigned long int)( guess * guess );
		//check for overflow
		if( tempguess / (unsigned long int)( ( guess < 0 ) ? ( guess * -1 ) : guess )
		   != (unsigned long int)( ( guess < 0 ) ? ( guess * -1 ) : guess ) )
		{ 
			//call AUCR_Un_Scaled_Integer_Square_Root for smaller value
			return( AUCR_SCALE * AUCR_Un_Scaled_Integer_Square_Root( i_number ) );
		}
		
		//difference is the error of the current guess
		//tempguess can only hold an unsigned value
		if( tempguess > square )
		{
			tempguess = tempguess - square;
			tempdiv = 2 * ( ( guess < 0 ) ? ( guess * -1 ) : guess );
			sign = ( ( guess < 0 ) ? -1 : 1 );
		}
		//so we put the sign in sign if there is one
		else
		{ 
			tempguess = square - tempguess;
			tempdiv = 2 * ( ( guess < 0 ) ? ( guess * -1 ) : guess );
			sign = ( ( guess < 0 ) ? 1 : -1 );
		}
		
		//save old difference
		olddifference = difference;
		
		//difference = tempguess / tempdiv
		//but make sure and round appropriately
		if( ( ( tempguess % tempdiv ) * AUCR_SCALE ) >= ( ( tempdiv * AUCR_SCALE ) / 2 ) )
		{
			difference = ( ( (long int)( tempguess / tempdiv ) * sign ) + sign );
		}
		else
		{
			difference = ( (long int)( tempguess / tempdiv ) * sign );
		}
		
		//for new guess, subtract difference from old guess
		guess -= difference;    
	}
	return( (int)guess );
}


/*
 *	Name:
 *		AUCR_Un_Scaled_Integer_Square_Root()
 *	Description:
 *		function that takes an integer argument, the finds the
 *		integer approximation of the square root of this number,
 *		using only integer operations and the newton-raphson method.
 *		checks for overflow along the way and recursively calls itself on
 *		a reduced version of the problem if overflow is detected,
 *		such that the function will work numbers of any size
 *		representable by an int type, although larger numbers
 *		will have less accuracy
 *	Parameters:
 *		i_number -> the number to scale and take the square root of
 *	Preconditions:
 *		i_number is a valid (nonnegative), unscaled integer
 *	Postconditions:
 *		nothing has changed
 *	Returns:
 *		integer result of the square root of the scaled input if
 *		no error, -1 on error
 */
int AUCR_Un_Scaled_Integer_Square_Root( int i_number )
{
	//declare variables
	unsigned long int square, tempguess, tempdiv;
	long int difference = 10, olddifference = 11, guess = AUCR_SCALE;
	signed char sign;
	//using "long" int essentially does nothing on 32 bit machines, but
	//on 16 bit processors, this makes sure 32 bit integers are used
	
	//check for valid input
	if( i_number < 0 )
	{
		return( -1 );
	}
	
	//check for obvious case
	if( i_number == 0 )
	{
		return( 0 );
	}
	
	square = (unsigned long int) i_number;
	
	//integer based, so difference of +/- 1 is
	//the best we can do.
	while( ( difference != 0 ) && ( difference != olddifference )
		  && ( difference != ( -1 * olddifference ) ) )
	{ 
		tempguess = (unsigned long int)( guess * guess );
		//check for overflow
		if( tempguess / (unsigned long int)( ( guess < 0 ) ? ( guess * -1 ) : guess )
		   != (unsigned long int)( ( guess < 0 ) ? ( guess * -1 ) : guess ) )
		{
			//call AUCR_Un_Scaled_Integer_Square_Root for smaller value
			return( AUCR_Un_Scaled_Integer_Square_Root( AUCR_SCALE ) * 
				   AUCR_Un_Scaled_Integer_Square_Root( 
													  AUCR_Rounding_Divide( i_number, AUCR_SCALE ) ) );
		}
		
		//difference is the error of the current guess
		//tempguess can only hold an unsigned value
		if( tempguess > square )
		{
			tempguess = tempguess - square;
			tempdiv = 2 * ( ( guess < 0 ) ? ( guess * -1 ) : guess );
			sign = ( ( guess < 0 ) ? -1 : 1 );
		}
		//so we put the sign in guess if there is one
		else
		{
			tempguess = square - tempguess;
			tempdiv = 2 * ( ( guess < 0 ) ? ( guess * -1 ) : guess );
			sign = ( ( guess < 0 ) ? 1 : -1 );
		}
		
		//save old difference
		olddifference = difference;
		
		//difference = tempguess / tempdiv
		//but make sure and round appropriately
		if( ( ( tempguess % tempdiv ) * AUCR_SCALE ) >= ( ( tempdiv * AUCR_SCALE ) / 2 ) )
		{
			difference = ( ( (long int)( tempguess / tempdiv ) * sign ) + sign );
		}
		else
		{
			difference = ( (long int)( tempguess / tempdiv ) * sign );
		}
		
		//for new guess, subtract difference from old guess
		guess -= difference;
		
	}
	return( (int)guess );
}
