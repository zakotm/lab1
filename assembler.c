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
	Symbol Table definition:
*/
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255

typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];
} Label;

int symbolTableSize = 0;
Label symbolTable[MAX_SYMBOLS];

/* debugging method */
void printSymbolTable() {
	printf("+++++++++++Symbol Table++++++++++\n");
	int i;
	for (i = 0; i < symbolTableSize; i++) {
		printf(	"0x%.4X\t%s\n",
				symbolTable[i].address,
				symbolTable[i].label);
	}
	printf("+++++++++++++++++++++++++++++++++\n");
}















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
	Output: 1 if is a pseodo-op name, 0 otherwise
*/
int isValidPseudoOp(const char* pseudoopName) {
	if (strcmp(pseudoopName,".orig") == 0) {
		return 1;
	}

	if (strcmp(pseudoopName,".fill") == 0) {
		return 1;
	}

	if (strcmp(pseudoopName,".end") == 0) {
		return 1;
	}

	return 0;
}


/*
	Input: Lowercase string to determine if it is a name of a valid label name
	Output: 0 if not a valid label name, 1 if is valid
*/
int isValidLabel(const char* labelName) {
	if (!labelName) { return 0; } /* null pointer */

	if (strlen(labelName) == 0 || strlen(labelName) > 20) { return 0; } /* must be of length 1 to 20 */

	if (labelName[0] == 'x') { return 0; } /* cannot start with 'x' */

	if (strcmp(labelName,"orig")==0 || strcmp(labelName,"fill")==0 ||
			strcmp(labelName,"end")==0) { return 0; } /* cannot be opcode name*/

	if (getOpcode(labelName) != -1) { return 0; } /* cannot be an operation name */

	if (isdigit(labelName[0])) { return 0; } /* cannot start with a digit */

	if (strcmp(labelName,"in")==0 || strcmp(labelName,"out")==0 || strcmp(labelName,"getc")==0
			|| strcmp(labelName,"puts")==0) { return 0; } /* cannot be IN, OUT, GETC, or PUTS */

	/* cannot be a register name */
	int i;
	for (i = 0; i < 8; i++) {
		char reg[] = "R ";
		reg[1] = i + 48; /* 0 is 48 is ASCII */
		if (strcmp(labelName, reg) == 0) {
			return 0;
		}
	}

	/* must be made of alphanumeric characters */
	i = 0;
	while (labelName[i] != 0) {
		if (!isalnum(labelName[i])) {
			return 0;
		}
		i++;
	}

	/* passes all tests, is valid label */
	return 1;
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
	int opCount = 0;
	do
	{
		lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
		
		if( lRet != DONE && lRet != EMPTY_LINE )
		{
			if (strcmp(lLabel,"")) { /* if label is blank, there is no label on this line of the sourcecode */
				if (isValidLabel(lLabel)  &&  getLabelAddress(lLabel) == -1) {

					strcpy(symbolTable[symbolTableSize].label, lLabel);
					symbolTable[symbolTableSize].address = startAddress + 2*opCount;
					symbolTableSize++;

				} else {
					/* invalid label or label is already in the table */
					printf("EXITING: %s     %i\n",lLabel,isValidLabel(lLabel));
					exit(4);
				}
			}

			if (getOpcode(lOpcode) != -1) { /* lineCount only increments for each operation */
				opCount++;
			}
		}
	} while( lRet != DONE );
	printSymbolTable();


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