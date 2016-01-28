/*
Name 1: Michael Stecklein
Name 2: 
UTEID 1: mrs4239
UTEID 2: 
*/


#include <stdio.h>	/* standard input/output library */
#include <stdlib.h>	/* Standard C Library */
#include <string.h>	/* String operations library */
#include <ctype.h>	/* Library for useful character operations */
#include <limits.h>	/* Library for definitions of common variable type characteristics */







/*
	Input: String to determine if it is a name of an opcode in LC-3b assembly and to return the respective opcode value.
	Output: The numerical opcode, or -1 if not an opcode name
*/
int getOpcode(const char* opcodeName) {
	printf("WARNING: getOpcode() is not yet implemented\n");
	return -1;
}



/*
	Input: String to determine if it is a name of a valid pseodo-op
	Output: 0 if not a pseodo-op name, 1 otherwise
*/
int isValidPseudoOp(const char* pseudoopName) {
	printf("WARNING: isValidPseudoOp() is not yet implemented\n");
	return 0;
}


/*
	Input: String to determine if it is a name of a valid label name
	Output: 0 if not a valid label name, 1 if is valid
*/
int isValidLabel(const char* labelName) {
	printf("WARNING: isValidLabel() is not yet implemented\n");
	return 0;
}


/*
	Input: Label to look up in symbol table
	Output: the address of that label, or -1 if the label doesn't exist in the table
*/
int getLabelAddress(const char* labelName) {
	printf("WARNING: getLabelAddress() is not yet implemented\n");
	return -1;
}






int strToNum( char * pStr )
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
     pStr++;
     if( *pStr == '-' )			/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	 printf("Error: invalid decimal operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )	/* hex */
   {
     pStr++;
     if( *pStr == '-' )			/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4); 
	/*This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}







	#define MAX_LINE_LENGTH 255
	enum
	{
		DONE, /* end of file */
		OK,
		EMPTY_LINE
	};


	int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode,
							char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
	{
	   char * lRet, * lPtr;
	   int i;

	   /* if empty, DONE */
	   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
			return( DONE );

       /* convert entire line to lowercase */
	   for( i = 0; i < strlen( pLine ); i++ )
			pLine[i] = tolower( pLine[i] );
	   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	   /* ignore the comments */
	   lPtr = pLine;
	   while( *lPtr != ';' && *lPtr != '\0'  &&  *lPtr != '\n' ) 
			lPtr++;
	   *lPtr = '\0'; /* set beginning of comment to delimiter of string */

		/* check for empty line */
	   if( !(lPtr = strtok( pLine, "\t\n ," )) ) 
			return( EMPTY_LINE );

		/* check for label */
	   if( getOpcode( lPtr ) && lPtr[0] != '.' ) /* found a label */
	   {
			*pLabel = lPtr;
			if( !( lPtr = strtok( NULL, "\t\n ," ) ) )
				return( OK );
	   }
	   
	   /* set opcode */
       *pOpcode = lPtr;

       /* set parameters */
	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) )
	   		return( OK );
	   	*pArg1 = lPtr;
       if( !( lPtr = strtok( NULL, "\t\n ," ) ) )
       		return( OK );
       	*pArg2 = lPtr;
	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) )
	   		return( OK );
		*pArg3 = lPtr;
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) )
			return( OK );
		*pArg4 = lPtr;

	   return( OK );
	}


































#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255

typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];
} Label;

int symbolTableSize = 0;
Label symbolTable[MAX_SYMBOLS];
 



FILE* infile = NULL;
FILE* outfile = NULL;




int main(int argc, char* argv[]) {
	
     /* Open Source Files */
     infile = fopen(argv[1], "r");
     outfile = fopen(argv[2], "w");

    if (!infile) {
       printf("Error: Cannot open file %s\n", argv[1]);
       exit(4);
	}

    if (!outfile) {
       printf("Error: Cannot open file %s\n", argv[2]);
       exit(4);
    }









	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
	int lRet;
	int startAddress;



	/*
		Handle .ORIG Pseudo-Op
			- do this first since it will be needed in both passes
			- set "startAddress" value or handle any errors
	*/
	/*lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );*/


	/*
		First Pass to Build Symbol Table
	*/
	int lineCount = 1;
	do
	{
		lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
		if( lRet != DONE && lRet != EMPTY_LINE )
		{
			if (strcmp(lLabel,"")) { /* if label is not blank */
				if (isValidLabel(lLabel)) {
					if (getLabelAddress(lLabel) == -1) {
						strcpy(symbolTable[symbolTableSize].label, lLabel);
						symbolTable[symbolTableSize].address = startAddress + 2*lineCount;
						symbolTableSize++;
					} else {
						exit(4);
					}
				} else {
					exit(4);
				}
			}

			if (getOpcode(lOpcode) != -1) { /* lineCount only increments for each operations */
				lineCount++;
			}
		}
	} while( lRet != DONE );



	/*
		Second Pass to Encode Operations
	*/

	rewind(infile); /* sets file position to beginning of file */

	do
	{
		lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
		if( lRet != DONE && lRet != EMPTY_LINE )
		{
			/* encode operation here */
			/* fprintf() */
		}
	} while( lRet != DONE );









	/* Clean Up? */
	/* do we need to clean up here? all those char* (strings) we used? symbolTable? */








     /* Close Source Files */
     fclose(infile);
     fclose(outfile);
}