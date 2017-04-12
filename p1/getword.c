

#include "getword.h"

int DSFLAG = 0;

int getword( char *w )
{
    int iochar; 
    int count = 0; //  charcater counter
    char *word = w; //  
    int backslash = 0; // backlsash counter
    int apostraphe = 0; // apostraphe counter                                    
    DSFLAG = 0;

    while ( ( iochar = getchar() ) != EOF ){
 	if(count == 254){ 
		ungetc(iochar,stdin);
		/**word = '\0';*/
		return count;
	}
	
    	
    	if( iochar == '\\' ){ 
		iochar = getchar();
		if(iochar == '$' && count == 0){
			DSFLAG = 1;
		}
		if( apostraphe == 0)  
			backslash = 1;
		else{
			if( iochar == '\'' ){   
				*word++ = iochar;
				count++;
				iochar = getchar();
			}
			else{			
				*word++ = '\\';
				count++;
			}	
		}
	}

	
		
    	if( iochar == ' '){
		
		if(backslash == 1 || apostraphe == 1){ 
			*word++ = iochar;
			count++;
		}
		else if(count == 0){}
		else{
			*word = '\0'; 
    			return count; 
		}	
	} 
	
	else if( iochar == '\n' || iochar == '<'|| iochar == '>'|| iochar == '\'' || iochar == '|'|| iochar == ';'|| iochar == '&' ){

		if ( iochar == '\n' || iochar == ';'){		
			if(count == 0){
				*word = '\0';
				return count;
			}	
			*word = '\0'; 
			ungetc(iochar,stdin);
			if(apostraphe == 1){ 		
   				return -2;
   			}
    			return count; 
		}
		else if (backslash == 1){ 
			*word++ = iochar;
			count++;
		}
		else if( iochar == '\'' && apostraphe == 0){ 
			apostraphe++;	
		}
		else if( iochar == '\'' && apostraphe == 1){ 
			apostraphe = 0;
			continue;
		}	
		else if( count != 0 && apostraphe == 0){ 
			*word = '\0'; 
			ungetc(iochar,stdin);
    			return count; 
			}	
		else if( apostraphe == 1){ 
			*word++ = iochar;
			count++;
		}	
		else{ 
			*word++ = iochar;
			count++;
			*word = '\0';
			return count;
		}
	}
	
	else{
		*word++ = iochar; 
		count++;	  
	}
	
	backslash = 0; 
		
    }
   if(apostraphe == 1){
   	return -2;
   }
   *word = '\0'; 
   if( count != 0){	
   	return count;
   }
   return -1;  

    	             
}