/*
Name 1: Michael Stecklein
Name 2: 
UTEID 1: mrs4239
UTEID 2: 

- What is "adequate documentation"?
- Do we exit(2) for an invalid pseudo-op?
*/




#include <stdio.h>	/* standard input/output library */
#include <stdlib.h>	/* Standard C Library */
#include <string.h>	/* String operations library */
#include <ctype.h>	/* Library for useful character operations */
#include <limits.h>	/* Library for definitions of common variable type characteristics */








/*
	Op-Code Definitions:

	ADD, AND, BR (all 8 variations), HALT, JMP, JSR, JSRR, LDB, LDW, 
	LEA, NOP, NOT, RET, LSHF, RSHFL, RSHFA, RTI, STB, STW, TRAP, XOR
*/
#define MAX_OPNAME_LEN		5
typedef struct {
	char opName[MAX_OPNAME_LEN+1];
	int opcode;
} OpCode;

const int opcodeLookupTableSize = 28;
const OpCode opcodeLookupTable[] = {
		{	"add",		1	},
		{	"and",		5	},
		{	"br",		0	},
		{	"brn",		0	},
		{	"brz",		0	},
		{	"brp",		0	},
		{	"brzp",		0	},
		{	"brnp",		0	},
		{	"brnz",		0	},
		{	"brnzp",	0	},
		{	"halt",		15	},
		{	"jmp",		12	},
		{	"jsr",		4	},
		{	"jsrr",		4	},
		{	"ldb",		2	},
		{	"ldw",		6	},
		{	"lea",		14	},
		{	"nop",		0	},
		{	"not",		9	},
		{	"ret",		12	},
		{	"lshf",		13	},
		{	"rshfl",	13	},
		{	"rshfa",	13	},
		{	"rti",		8	},
		{	"stb",		3	},
		{	"stw",		7	},
		{	"trap",		15	},
		{	"xor",		9	}
};








/*
	Symbol Table definition:
*/
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
#define MAX_ADDRESS 65536 /* 16bit max number */

typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];
} Label;

int symbolTableSize = 0;
Label symbolTable[MAX_SYMBOLS];








/*
	Function declarations:
*/


/*
	Input: String to determine if it is a name of an opcode in LC-3b assembly and to return the respective opcode value.
	Output: The numerical opcode, or -1 if not an opcode name
*/
int getOpcode(const char* opcodeName);


/*
	Input: String to determine if it is a name of a valid pseodo-op
	Output: 1 if is a pseodo-op name, 0 otherwise
*/
int isValidPseudoOp(const char* pseudoopName);


/*
	Input: Lowercase string to determine if it is a name of a valid label name
	Output: 0 if not a valid label name, 1 if is valid
*/
int isValidLabel(const char* labelName);


/*
	Input: Label to look up in symbol table
	Output: the address of that label, or -1 if the label doesn't exist in the table
*/
int getLabelAddress(const char* labelName);


/*
	Input: Character string representation of an assembly integer.
	Output: Integer equivalent.
*/
int strToNum(char * pStr);


/*
	Input: Opcode string, and up to 4 arguments as strings.
	Output: The integer representation of the command in binary.
*/
int encodeOpcode(char** lOpcode, char** lArg1, char** lArg2, char** lArg3, char** lArg4);


/* Reads a line and parses it into pLabel, pOpcode, pArg1, pArg2, pArg3, and pArg4. All input streams are converted
	to lower case, so all comparisons in the entire assembler are done in lower case, as everything is case
	insensitive. */
#define MAX_LINE_LENGTH 255
enum
{
	DONE, /* end of file */
	OK,
	EMPTY_LINE
};
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode,
							char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4);







/*
	MAIN
*/

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
			- set "startAddress" value or handle any errors (code 3)
	*/
	int foundOrigPseudOp = 0;

	do {
		lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );

		if ( strcmp(lOpcode,".orig") == 0 ) {
			foundOrigPseudOp = 1;
		}

		if ( !foundOrigPseudOp  &&  (strcmp(lLabel,"") != 0  ||  strcmp(lOpcode,"") != 0) ) {
			/* there is a label or opcode before .ORIG pseudo-op */
			printf("ERROR: Label or opcode found before \".ORIG\" %s %s\n",lLabel,lOpcode);
			exit(4);
		}

	} while (lRet != DONE  &&  !foundOrigPseudOp);

	if (!foundOrigPseudOp) {
		printf("ERROR: No \".ORIG\" pseudo-op found\n");
		exit(4);
	}

	/* handle orig start address */
	startAddress = strToNum(lArg1);
	if (startAddress % 2 == 1  ||  startAddress > MAX_ADDRESS) {
		printf("ERROR: Invalid start address %s\n", lArg1);
		exit(3);
	}




	/*
		First Pass to Build Symbol Table
			- will check for .ORIG and .END pseudo-op errors here to
	*/
	int opCount = 0;

	/* vars for checking pseudo-op errors */
	int origPseudoOpCount = 0;
	int foundEnd = 0;

	do {
		lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
		
		if( lRet != DONE && lRet != EMPTY_LINE ) {

			/* build label symbol table */
			if (strcmp(lLabel,"")) { /* if label is blank, there is no label on this line of the sourcecode */
				if (isValidLabel(lLabel)  &&  getLabelAddress(lLabel) == -1) {

					strcpy(symbolTable[symbolTableSize].label, lLabel);
					symbolTable[symbolTableSize].address = startAddress + 2*opCount;
					symbolTableSize++;

				} else {
					printf("ERROR: Invalid label %s\n",lLabel);
					exit(4);
				}
			}

			if (getOpcode(lOpcode) != -1) { /* lineCount only increments for each operation */
				opCount++;
			}



			/* check for multipe .orig error */
			if (strcmp(lOpcode,".orig") == 0) {
				if (origPseudoOpCount > 0) {
					printf("ERROR: Multiple \".ORIG\" pseudo-ops found\n");
					exit(4);
				} else {
					origPseudoOpCount++;
				}
			}

			/* check for ".END" pseudo-op */
			if (strcmp(lOpcode,".end") == 0) {
				foundEnd = 1;
			}

		}
	} while( lRet != DONE  &&  !foundEnd);

	if (!foundEnd) { /* make sure an ".END" pseudo-op was found before end of file */
		printf("ERROR: Need a terminating pseud-op \".END\"\n");
		exit(4);
	}




	/*
		Second Pass to Encode Operations
			- all output file writing will be done here
	*/

	rewind(infile); /* sets file position back to beginning of file */

	/* write start address first */
	fprintf( outfile, "0x%.4X\n", startAddress );

	do
	{
		lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );

		if( lRet != DONE && lRet != EMPTY_LINE ) {

			if (isValidPseudoOp(lOpcode)  ||  getOpcode(lOpcode) != -1) {

				/* handle opcodes */
				if (getOpcode(lOpcode) != -1) {
					int hexToWrite = encodeOpcode(&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
					fprintf( outfile, "0x%.4X\n", hexToWrite );
				}

				/* handle .fill pseudo-op */
				if (strcmp(lOpcode,".fill") == 0) {
					fprintf( outfile, "0x%.4X\n", strToNum(lArg1) );
				}

				/* check for .END */
				if (strcmp(lOpcode,".end") == 0) {
					lRet = DONE;
				}

			} else {
				printf("ERROR: Invalid opcode %s\n",lOpcode);
				exit(2);
			}
		}
	} while( lRet != DONE );






	/* Clean Up */
	/* do we need to clean up the symbolTable? */








    /* Close Source Files */
    fclose(infile);
    fclose(outfile);
}





















/*
	Method Implementations
*/




/* debugging method */
void printSymbolTable() {
	printf("+++Symbol Table+++\n");
	int i;
	for (i = 0; i < symbolTableSize; i++) {
		printf(	"0x%.4X\t%s\n",
				symbolTable[i].address,
				symbolTable[i].label);
	}
	printf("++++++++++++++++++\n");
}




/*
	Input: String to determine if it is a name of an opcode in LC-3b assembly and to return the respective opcode value.
	Output: The numerical opcode, or -1 if not an opcode name
*/
int getOpcode(const char* opcodeName) {
	int i = 0;
	while (i < opcodeLookupTableSize) {
		if (strcmp(opcodeName,opcodeLookupTable[i].opName) == 0) {
			return opcodeLookupTable[i].opcode;
		}
		i++;
	}

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
	int i = 0;
	while (i < symbolTableSize) {
		if (strcmp(labelName, symbolTable[i].label) == 0) {
			return symbolTable[i].address;
		}
		i++;
	}

	return -1;
}



/*
	Input: Character string representation of an assembly integer.
	Output: Integer equivalent.
*/
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







	/* Reads a line and parses it into pLabel, pOpcode, pArg1, pArg2, pArg3, and pArg4. All input streams are converted
		to lower case, so all comparisons in the entire assembler are done in lower case, as everything is case
		insensitive. */
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






	/*
		Input: Opcode string, and up to 4 arguments as strings.
		Output: The integer representation of the command in binary.
	*/
	int encodeOpcode(char** lOpcode, char** lArg1, char** lArg2, char** lArg3, char** lArg4) {
		int opcodeInt = getOpcode(*lOpcode);

		switch (opcodeInt) {

			case 0: /* BR(nzp) or NOP */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 1: /* ADD */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 2: /* LDB */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;
				
			case 3: /* STB */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 4: /* JSR or JSRR */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 5: /* AND */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 6: /* LDW */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 7: /* STW */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 8: /* RTI */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 9: /* NOT or XOR */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 12: /* JMP or RET */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 13: /* LSHF or RSHFL or RSHFA */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 14: /* LEA */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			case 15: /* TRAP or HALT */
				printf("WARNING: encoding of %s not yet written\n",*lOpcode);
				return 0;
			break;

			default:
				printf("ERROR: Opcode %s not found in Opcode Lookup Table\n",*lOpcode);
				exit(4);
				return 0;
			break;

		}
	}























