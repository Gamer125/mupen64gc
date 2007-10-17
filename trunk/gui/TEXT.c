/* TEXT.c - Helper functions for reformatting text
   by Mike Slegeir for Mupen64-GC
 */

#include "TEXT.h"

char TEXT_split_lines[TEXT_MAX_SPLIT][TEXT_WIDTH];

int TEXT_split(char* string){
	int current_line = 0, i = 0;
	while( *string != 0 ){
		if( *string == '\n' ){
			if(++current_line >= TEXT_MAX_SPLIT) return current_line;
			i = 0;
		} else TEXT_split_lines[current_line][i++] = *string;
		
		++string;
	}
	
	// Return the number of lines created
	//   the boolean checks whether the last line
	//   contains any characters or not, hence
	//   whether or not to count it
	return current_line + (i == 0) ? 0 : 1;
}

void TEXT_expand(char* string){
	// Although it'd be nice to get this in one sweep, it'd probably not be a good idea
	
	// First determine if every tab is expandable without overflow
	int i, spaces = 0;
	for(i=0; string[i] != 0; ++i)
		if(string[i] == '\t') spaces += TEXT_SPACE_PER_TAB - 1;
	
	if( TEXT_WIDTH - i > spaces ){ // We have the space to expand
		while(i >= 0){
			if(string[i] == '\t'){ // Expand this tab
				int j;
				for(j=0; j<TEXT_SPACE_PER_TAB; ++j)
					string[i+spaces-j] = ' ';
				spaces -= TEXT_SPACE_PER_TAB - 1;
				
			} else // Otherwise just move this character forward
				string[i+spaces] = string[i];
			
			--i;
		}
		
	} else { // Just translate every tab into a single space
		for(i=0; string[i] != 0; ++i)
			if(string[i] == '\t') string[i] = ' ';
	}
}
