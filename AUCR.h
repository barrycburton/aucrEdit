#ifndef AUCR_H
#define AUCR_H

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

/* ==============================================
	MACRO / CONSTANT DEFINITIONS
   ============================================== */

/* Set the scale here, appropriately */
#define AUCR_SCALE 1000

/* ==============================================
	TYPE DEFINITIONS
   ============================================== */

typedef enum _AUCR_Error
{
		/* general success */
	AUCR_ERR_SUCCESS,
		/* gerneral failure */
	AUCR_ERR_FAILED,
		/* an allocation of memory failed */
	AUCR_ERR_NO_MEMORY,
		/* parameters are all valid
			when taken alone, but two
			or more parameters have differing
			values for the same variable */
	AUCR_ERR_CONFLICTING_PARAMETERS
} AUCR_Error;

typedef struct _AUCR_Coordinate
{
	int x;
	int y;
} AUCR_Coordinate;
typedef AUCR_Coordinate * AUCR_Coordinate_Ptr;

typedef struct _AUCR_Interpolated_Character
{
	wchar_t unicode;
	int num_directional_codes;
	AUCR_Coordinate * coordinates;
} AUCR_Interpolated_Character;
typedef AUCR_Interpolated_Character * AUCR_Interpolated_Character_Ptr;

typedef struct _AUCR_Character
{
	wchar_t unicode;
	int num_directional_codes;
	int num_activity_regions;
	int * directional_codes;
	int * activity_measures;
} AUCR_Character;
typedef AUCR_Character * AUCR_Character_Ptr;

typedef struct _AUCR_Activity_Region
{
	// whether these are in range should be managed somewhere else
	int start;
	int stop;
} AUCR_Activity_Region;
typedef AUCR_Activity_Region * AUCR_Activity_Region_Ptr;

typedef struct _AUCR_Directional_Code_Map
{
	// number of unique directional codes
	int mappable_directional_codes;
	int * x;
	int * y;
	int positive_begin;
	int negative_begin;
	int positive_end;
	int negative_end;
} AUCR_Directional_Code_Map;
typedef AUCR_Directional_Code_Map * AUCR_Directional_Code_Map_Ptr;

typedef struct _AUCR_Alphabet
{
	int num_characters;
	AUCR_Character_Ptr * characters_ptr_ptr; /* array of character ptrs */
	AUCR_Directional_Code_Map_Ptr directional_code_map_ptr;
	int num_activity_regions;
	AUCR_Activity_Region * activity_regions;
	int num_directional_codes;
	int * bias; /* array of biases with num_activity_regions elements */
} AUCR_Alphabet;
typedef AUCR_Alphabet * AUCR_Alphabet_Ptr;


/* ==============================================
	FUNCTION DEFINITIONS
   ============================================== */

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
 	AUCR_Alphabet_Ptr * io_alphabet_ptr_ptr 
);

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
 	AUCR_Alphabet_Ptr i_alphabet_ptr
);


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
	AUCR_Character_Ptr * o_character_ptr_ptr
);


/*
 *	Name:
 *		AUCR_CHARACTER_Release()
 *	Description:
 *		Frees all dynamically allocated memories for the AUCR_Character and NULLs
 *		the AUCR_Character_Ptr.
 *	Parameters:
 *		io_character_ptr <-> address of the AUCR_Character memory to be freed.
 *	Preconditions:
 *		io_character_ptr is a valid address of an AUCR_Character.
 *	Postconditions:
 *		The character's directional_codes and activity_measures arrai_coordinates[1] are freed
 *		followed by the memory for the character as well. io_character_ptr is NULL.
 *	Returns:
 *		none
 */
void AUCR_CHARACTER_Release(
	AUCR_Character_Ptr * io_character_ptr_ptr
);


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
	AUCR_Interpolated_Character_Ptr * o_interpolated_character_ptr_ptr
);


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
	AUCR_Interpolated_Character_Ptr * io_interpolated_character_ptr_ptr
);


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
	AUCR_Interpolated_Character_Ptr io_interpolated_character_ptr
);


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
	AUCR_Directional_Code_Map_Ptr * o_directional_code_map_ptr_ptr
);


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
	AUCR_Directional_Code_Map_Ptr io_directional_code_map_ptr
);


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
	AUCR_Directional_Code_Map_Ptr * io_directional_code_map_ptr_ptr
);


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
	AUCR_Alphabet_Ptr * o_alphabet_ptr_ptr
);


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
	AUCR_Alphabet_Ptr * io_alphabet_ptr_ptr
);


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
	AUCR_Alphabet_Ptr io_alphabet_ptr
);


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
	AUCR_Alphabet_Ptr io_alphabet_ptr
);


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
	AUCR_Alphabet_Ptr io_alphabet_ptr
);


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
	wchar_t * o_unicode_ptr
);


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
	wchar_t * o_unicode_ptr
);


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
	AUCR_Character_Ptr io_character_ptr
);


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
	AUCR_Interpolated_Character_Ptr io_interpolated_character_ptr
);


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
int AUCR_Rounding_Divide(
	int i_dividend,
	int i_divisor
);


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
int AUCR_Integer_Square_Root( 
	int i_number
);


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
int AUCR_Un_Scaled_Integer_Square_Root( 
	int i_number
);


#endif // AUCR_H
