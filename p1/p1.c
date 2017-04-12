/*   Name:          Anas Benothmane 
*    Instructor:    John Carroll
*    Class :        cs570
*    Due Date:     September 21, 2016
*/

#include <stdio.h>
#include "getword.h"

int DSFLAG = 0;
int getword( char *w )
{
    int iochar ; 
    int count = 0;
    char *a = w; 
    int tracker = -1;    /*to track $ usage*/                                                 
    int tracker2 = 0;    /*to track \ usage*/
    DSFLAG = 0;
                                             

 while ( ( iochar = getchar() ) != EOF )
    { 	 
    		if(count == 254)
		{ 
		   ungetc(iochar,stdin);
		   return count;
	        }
	 	switch(iochar)
		{
		case '<':                                                                    /*The cases of word delimeters*/
		case '>':
		case '|':
		case '&':
				    
			 if(count !=0)                                                      /*if we have word counting in process*/
			 {
			 	
				if  (a[0] == '$')                                          /*if the first character is $ multiply the count by -1 */
				{
				    ungetc(iochar, stdin);
				    a[count]=0;
				    return count*tracker;
					
				}
				
				if(a[count-1] == '\\' && tracker2 == 0)                 /*Check if we didnt encounter | before, if we did not we ignore it.*/
				{
				    
				    a[count-1]=iochar;
				    break;
				}
				ungetc(iochar, stdin);
				a[count]=0;
				return count;
			 }
				     
			 	      a[count]=iochar;
				      a[count+1]=0;
				      return 1;         
		
		case '\\' :     if(count!=0)                                         /*if we encounter \ twice we ignore one and increase the tracker for future reference*/
		
				{
				     if(a[count-1] == '\\')
				         {    
				             a[count-1]=iochar;
				             tracker2++;
				             break;	
				         } 
				}
				
				
		case '$':                                                           /*if $ is in the middle of a word, we count it as a regular character*/
		                if(count!=0)
				     {if(a[count-1] == '\\')                        /*if $ is preceeded by \ set the tracker to 1 so the count is porsitive*/
				        {     
				    
				           a[count-1]=iochar;
				           tracker =1;
				            break;
					
				         } 
				   
				      }
				 a[count] = iochar;
				 count+=1;
				 break;			             
		case ' ':
			   if(count !=0)                                         /*if space is in the middle of cord counting*/  
				  {				     
				       if(a[0] == '$')                          /*check if the first character is $ so we can multiply the count by -1*/
				
				           {
				               ungetc(iochar, stdin);
				               a[count]=0;
				               return count*tracker;	
				           }
					   
				       if(a[count-1] == '\\')                   /*if it is preceeded by \ we put space in the array and count it*/
				           {	    
				               a[count-1]=iochar;
				               break;
					
				           }
				      ungetc(iochar, stdin);
				      a[count] =0;     
		                      return count;
				  } 
				  
		           if(count ==0)                                      /*if the count is 0 then it's a leading space and must be ignored*/
			         {
			              break;
			         }
			   a[count]=iochar;
		           a[count+1]=0;
		           return 1;  
		         
		case '\n':                                                   /*if it's a new line*/
			   if(count !=0)
			        {                                           /*if it's in the middle of word counting and preceeded by \, de decerement the count */
				
				     if(a[count-1] == '\\')                 /*so the \ char doesnt count in the word*/  
				         {
				              a[--count]=0;
				         } 
					 
				     if(a[0] == '$')                       /*check if the first char is $ so we can negate the count*/
				         {
				              ungetc(iochar, stdin);
				              a[count]=0;
				              return count*tracker;
					
				         } 
			             ungetc('\n', stdin);
			             a[count]=0;	
		                     return count;
			        }
			  else
			        {
				     a[count]=0;
				     return 0;
			        }
		default:
		
		         if(a[count-1] == '\\' && tracker2 == 0)              /*if we encounter anyother char that it is not a delimiter*/
				{
				    a[count-1]=iochar;                        /*if it preceeded by \ and \ was not ignored before*/
				    break;                                    /*we ignore it */
				}
		         a[count] = iochar; 
		         tracker2 =0;                                         /*we set the tracker of char \ to zero*/                         
		         count+=1;
		         break;
			
		}
		 		 			
   }
   
   if(count !=0)                                                             /*if it is the end of file following a word */
               {       
                   if(a[0] == '$')                                            /*check if the first character is $ so we can multiply the count by -1*/                                           
		       {
		           ungetc(iochar, stdin);
		           a[count]=0;
			   return count*tracker;					
		       }
		   a[count]=0;
		   return count;
	       }

  count =0;                                                                /*set the length to 0 and return -1 to exit the loop in p1*/
  a[count] = 0;
  return -1;   
}
