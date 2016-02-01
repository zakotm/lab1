/*
Name 1: Michael Stecklein
Name 2: Zakaria Alrmaih
UTEID 1: mrs4239
UTEID 2: za3488
*/

#include <stdio.h>	/* standard input/output library */
#include <stdlib.h>	/* Standard C Library */
#include <string.h>	/* String operations library */
#include <ctype.h>	/* Library for useful character operations */
#include <limits.h>	/* Library for definitions of common variable type characteristics */

#define ASCII_VALUE_0	48 /* 0 is 48 is ASCII */
#define MAX_5_BIT_NUM	15
#define MIN_5_BIT_NUM	-16

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
#define MAX_ADDRESS 65535 /* largest 16 bit number */

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
	Input: String to determine if is a formatted constant, either hex (x3000) or constant (#9)
	Output: 1 for true, 0 for false
*/
int isConstant(const char* constStr);


/*
	Input: String representation of register
	Output: Integer of register number
*/
int getRegisterNumber(const char* regStr);


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
int encodeOpcode(int address, char** lOpcode, char** lArg1, char** lArg2, char** lArg3, char** lArg4);


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

			if (getOpcode(lOpcode) != -1  ||  strcmp(lOpcode,".fill") == 0) { /* lineCount only increments for each operation or .FILL */
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
	opCount = 0; /* reuse and reset opCount */

	/* write start address first */
	fprintf( outfile, "0x%.4X\n", startAddress );

	do
	{
		lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );

		if( lRet != DONE && lRet != EMPTY_LINE ) {

			if (isValidPseudoOp(lOpcode)  ||  getOpcode(lOpcode) != -1) {

				/* handle opcodes */
				if (getOpcode(lOpcode) != -1) {
					int currentAddress = startAddress + 2 * opCount;
					int hexToWrite = encodeOpcode(currentAddress, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
					fprintf( outfile, "0x%.4X\n", hexToWrite );
					opCount++;
				}

				/* handle .orig pseudo-op */
				if (strcmp(lOpcode,".orig") == 0) {
					continue;
				}

				/* handle .fill pseudo-op */
				if (strcmp(lOpcode,".fill") == 0) {
					const int MAXFILLVAL = 65535;
					const int MINFILLVAL = -32768;
					if (!isConstant(lArg1)) {
						printf("ERROR: Invalid constant for .FILL %s\n",lArg1);
						exit(4);
					}
					int fillVal = strToNum(lArg1);
					if (fillVal > MAXFILLVAL  ||  fillVal < MINFILLVAL) {
						printf("ERROR: Invalid constant for fill %s\n",lArg1);
						exit(3);
					}
					int BIT_MASK_16 = 0x0000FFFF;
					fprintf( outfile, "0x%.4X\n", fillVal & BIT_MASK_16 );
					opCount++;
				}

				/* check for .END */
				if (strcmp(lOpcode,".end") == 0) {
					lRet = DONE;
				}

			} else {
				printf("ERROR: Invalid opcode %s   %s   %i\n",lOpcode,lLabel,opCount);
				exit(2);
			}
		}
	} while( lRet != DONE );

    /* Close Source Files */
    fclose(infile);
    fclose(outfile);


    exit(0);
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

/* debugging method */
void printInBinary16(int word) {
	int i;
	int bitMask = 1 << 15;
	for (i = 0; i < 16; i++) {
		if (i != 0  &&  i % 4 == 0) {
			printf(" ");
		}

		if (word & bitMask) {
			printf("%i",1);
		} else {
			printf("%i",0);
		}

		bitMask >>= 1;
	}
	printf("\n");
}

/* helper method */
int power(int base, int exponent) {
	int i, result = 1;
	for (i = 0; i < exponent; i++) {
		result *= base;
	}
	return result;
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

	if (strcmp(labelName,".orig")==0 || strcmp(labelName,".fill")==0 ||
			strcmp(labelName,".end")==0) { return 0; } /* cannot be an pseudo-ops name */

	if (getOpcode(labelName) != -1) { return 0; } /* cannot be opcode name*/

	if (isdigit(labelName[0])) { return 0; } /* cannot start with a digit */

	if (strcmp(labelName,"in")==0 || strcmp(labelName,"out")==0 || strcmp(labelName,"getc")==0
			|| strcmp(labelName,"puts")==0) { return 0; } /* cannot be IN, OUT, GETC, or PUTS */

	/* cannot be a register name */
	int i;
	for (i = 0; i < 8; i++) {
		char reg[] = "r ";
		reg[1] = i + ASCII_VALUE_0;
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
	Input: String to determine if is a formatted constant, either hex (x3000) or constant (#9)
	Output: 1 for true, 0 for false
*/
int isConstant(const char* constStr) {
	
	if (!constStr  ||  strlen(constStr) < 2) {
		return 0;
	}

	if (constStr[0] != 'x'  &&  constStr[0] != '#') {
		return 0;
	}

	/* hex */
	if (constStr[0] == 'x') {
		int i;
		for (i = 1; i < strlen(constStr); i++) {
			if (i == 1  && constStr[i] == '-') {
				/* nop */
			} else if (!isdigit(constStr[i]) && constStr[i] != 'a' &&
										constStr[i] != 'b' &&
										constStr[i] != 'c' &&
										constStr[i] != 'd' &&
										constStr[i] != 'e' &&
										constStr[i] != 'f') {
				return 0;
			}
		}
	} else { /* decimal */
		int i;
		for (i = 1; i < strlen(constStr); i++) {
			if (i == 1 && constStr[i] == '-') {
				/* nop */
			} else if (!isdigit(constStr[i])) {
				return 0;
			}
		}
	}

	return 1;
}


/*
	Input: String representation of register
	Output: Integer of register number
*/
int getRegisterNumber(const char* regStr) {

	/* all valid register strings should be of length 2, where the first char is 'r' and the second is a digit */
	if (!regStr  ||  strlen(regStr) != 2  ||  regStr[0] != 'r'  ||  !isdigit(regStr[1])) {
		printf("ERROR: Invalid register %s\n",regStr);
		exit(4);
	}

	/* get register number */
	int regNum = regStr[1] - ASCII_VALUE_0;

	/* check for register in correct range */
	if (regNum > 7  ||  regNum < 0) {
		printf("ERROR: Invalid register %s\n",regStr);
		exit(4);
	}

	return regNum;
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
   if( getOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
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
	Encode 3 argrument arithmetic logic
	- ADD, AND, XOR, and NOT all have the same opcode pattern. It is assumed that lArg3 is changed to "#-1"
		for NOT, as to fill the last five bits with 1's.
*/
#define NOT_OP_ARG3 "#-1"
int encode3ArgumentAL(int address, int opcodeInt, char* lArg1, char* lArg2, char* lArg3) {
	const int MODE_BIT_MASK = 0x0020; /* bit 5 */

	int encoded = 0;
	int dr = getRegisterNumber(lArg1);
	int sr1 = getRegisterNumber(lArg2);

	/* add in opcode, dr, and sr1 to encoded */
	encoded += opcodeInt << 12;
	encoded += dr << 9;
	encoded += sr1 << 6;

	/* set flag and remaining bits, depending on mode */
	if (isConstant(lArg3)) { /* adding constant */

		const CONST_BIT_MASK = 0x001F; /* last 5 bits */
		encoded |= MODE_BIT_MASK;
		int constant = strToNum(lArg3);
		if (constant > MAX_5_BIT_NUM  ||  constant < MIN_5_BIT_NUM) {
			printf("ERROR: Invalid constant, out of 5 bit range %i\n",constant);
			exit(3);
		}
		constant &= CONST_BIT_MASK;
		encoded += constant;;

	} else { /* using source register */
		encoded += getRegisterNumber(lArg3); /* sr2 */
	}

	return encoded;
}


/*
	Encode offset for given label
	-will add encoded offset to the last numBits bits of opcodeToEncode
*/
int encodeLabelOffset(int* opcodeToEncode, int address, char* label, int numBits) {
	/* calculate mask */
	int BIT_MASK = -1;
	BIT_MASK <<= numBits;

	/* calculate limits */
	int maxOffset = power(2,numBits-1) - 1;
	int minOffset = -1 * power(2,numBits-1);

	/* calculate offset */
	int labelAddress = getLabelAddress(label);
	if (labelAddress == -1) {
		printf("ERROR: Undefined label %s\n",label);
		exit(1);
	}
	int offset = ( labelAddress - (address + 2) ) / 2; /* plus one for incremented PC */

	/* check limits */
	if (offset > maxOffset  ||  offset < minOffset) {
		printf("ERROR: Invalid label constant, out of %i bit bounds\n",numBits);
		exit(4);
	}

	/* adjust original encoded op */
	*opcodeToEncode &= BIT_MASK;
	*opcodeToEncode += offset & ~BIT_MASK;


	return *opcodeToEncode;
}

/*
	Parse and encode Branch operation
*/
int encodeBR(int opcodeInt, int address, char* opcodeStr, char* labelStr) {
	const int N_MASK = 1 << 11;
	const int Z_MASK = 1 << 10;
	const int P_MASK = 1 << 9;

	int encoded = opcodeInt << 12;

	/* set nzp bits */
	int i;
	if (strlen(opcodeStr) == 2) { /* unconditional branch */
		encoded |= ( N_MASK +  Z_MASK + P_MASK );
	}
	for (i = 2; i < strlen(opcodeStr); i++) {
		if (opcodeStr[i] == 'n') {
			encoded |= N_MASK;
		}
		if (opcodeStr[i] == 'z') {
			encoded |= Z_MASK;
		}
		if (opcodeStr[i] == 'p') {
			encoded |= P_MASK;
		} 
	}

	/* encode offset */
	encodeLabelOffset(&encoded, address, labelStr, 9);

	return encoded;
}

/*
	Parse and encode TRAP operation
*/
int encodeTRAP(int opcodeInt, char* arg) {
	int encoded = 0;
	encoded += opcodeInt << 12;

	int vector = strToNum(arg);
	int maxVectorVal = power(2,8) - 1;

	if (vector < 0 || vector > maxVectorVal) {
		printf("ERROR: TRAP vector out of range  %s\n",arg);
		exit(3);
	}

	encoded += vector;

	return encoded;
}

/*
	Parse and encode JMP operation
*/
int encodeJMP(int opcodeInt, char* reg) {
	int encoded = opcodeInt << 12;
	encoded += getRegisterNumber(reg) << 6;
	return encoded;
}

/*
	Parse and encode JSR and JSRR operations
*/
int encodeJSRr(char* opcodeStr, int opcodeInt, char* arg, int address) {
	const MODE_BIT_MASK = 1 << 11;

	int encoded = opcodeInt << 12;

	if (strcmp(opcodeStr,"jsr") == 0) {
		encoded |= MODE_BIT_MASK;
		encodeLabelOffset(&encoded,address,arg,11);
	} else {
		encoded += getRegisterNumber(arg) << 6;
	}

	return encoded;
}

/*
	Parse and encode LSHF, RSHFL, and RSHFA operations
*/
int encodeShift(int opcodeInt, char* opcodeStr, char* arg1, char* arg2, char* arg3) {
	const int DIRECTION_BIT_MASK = 1 << 4;
	const int ARITH_LOGIC_BIT_MASK = 1 << 5;

	int encoded = opcodeInt << 12;

	/* evaluate registers and constant */
	int dr = getRegisterNumber(arg1);
	int sr = getRegisterNumber(arg2);
	int amount = strToNum(arg3);

	/* check if amount fits in 4 bits */
	if (amount < 0  ||  amount > 15) {
		printf("ERROR: Invalid constant %s for %s\n",arg3,opcodeStr);
		exit(3);
	}

	/* encode dr, sr, and amount */
	encoded += dr << 9;
	encoded += sr << 6;
	encoded += amount;

	/* flip bits 4 and 5 for direction and type of shift */
	if (opcodeStr[0] == 'r') {
		encoded |= DIRECTION_BIT_MASK;

		if (opcodeStr[4] == 'a') {
			encoded |= ARITH_LOGIC_BIT_MASK;
		}
	}

	return encoded;
}

/*
	Parse and encode STB and STW operations
*/
int encodeStore(int opcodeInt, char* lArg1, char* lArg2, char* lArg3) {
	/* mask for front bits of offset */
	const int OFFSET_MASK = 0x003F; /* first 6 bits */

	int encoded = opcodeInt << 12;

	int sr = getRegisterNumber(lArg1);
	int baser = getRegisterNumber(lArg2);
	int offset = strToNum(lArg3);

	/* check bounds of offset */
	if (offset > 31  ||  offset < -32) {
		printf("ERROR: Invalid constant, offset %i for store is out of bounds\n", offset);
		exit(3);
	}

	encoded += sr << 9;
	encoded += baser << 6;
	encoded += offset & OFFSET_MASK;

	return encoded;
}

/*
	Parse and encode LDB, LDW, and LEA operations.
*/
int encodeLoad(int opcodeInt, char* opcodeStr, int address, char* arg1, char* arg2, char* arg3) {
	int encoded = opcodeInt << 12;

	int dr = getRegisterNumber(arg1);
	encoded += dr << 9;

	if (strcmp(opcodeStr, "lea") == 0) { /* LEA */

		encodeLabelOffset(&encoded, address, arg2, 9);

	} else { /* LDB or LDW */
		const int OFFSET_MASK = 0x003F; /* 6 bits */

		int baser = getRegisterNumber(arg2);
		encoded += baser << 6;

		int offset = strToNum(arg3);
		if (offset < -32  ||  offset > 31) {
			printf("ERROR: Invalid constant, %i is out of bounds of %s\n",offset,opcodeStr);
			exit(3);
		}
		encoded += offset & OFFSET_MASK;

	}

	return encoded;
}


/*
	Input: Opcode string, and up to 4 arguments as strings.
	Output: The integer representation of the command in binary.
*/
int encodeOpcode(int address, char** lOpcode, char** lArg1, char** lArg2, char** lArg3, char** lArg4) {

	int opcodeInt = getOpcode(*lOpcode);

	switch (opcodeInt) {

		case 9: /* NOT or XOR */
			if (strcmp(*lOpcode,"not") == 0) { /* constant for NOT should be "11111" */
				return encode3ArgumentAL(address,opcodeInt,*lArg1,*lArg2,NOT_OP_ARG3);
			}
		case 1: /* ADD */
		case 5: /* AND */
			return encode3ArgumentAL(address,opcodeInt,*lArg1,*lArg2,*lArg3);
		break;

		case 13: /* LSHF or RSHFL or RSHFA */
			return encodeShift(opcodeInt, *lOpcode, *lArg1, *lArg2, *lArg3);
		break;



		case 2: /* LDB */
		case 6: /* LDW */
			return encodeLoad(opcodeInt, *lOpcode, address, *lArg1, *lArg2, *lArg3);
		case 14: /* LEA */
			return encodeLoad(opcodeInt, *lOpcode, address, *lArg1, *lArg2, *lArg3);
		break;

		case 3: /* STB */
		case 7: /* STW */
			return encodeStore(opcodeInt, *lArg1, *lArg2, *lArg3);
		break;



		case 0: /* BR(nzp) or NOP */
			if (strcmp(*lOpcode,"nop") == 0) {
				return 0;
			}
			return encodeBR(opcodeInt, address,*lOpcode,*lArg1);
		break;

		case 4: /* JSR or JSRR */
			return encodeJSRr(*lOpcode, opcodeInt, *lArg1, address);
		break;

		case 8: /* RTI */
			return opcodeInt << 12;
		break;

		case 12: /* JMP or RET */
			if (strcmp(*lOpcode, "ret") == 0) {
				return encodeJMP(opcodeInt,"r7");
			}
			return encodeJMP(opcodeInt,*lArg1);
		break;

		case 15: /* TRAP or HALT */
			if (strcmp(*lOpcode, "halt") == 0) {
				return encodeTRAP(opcodeInt, "x25");
			}
			return encodeTRAP(opcodeInt, *lArg1);
		break;



		default:
			printf("ERROR: Opcode %s not found in Opcode Lookup Table\n",*lOpcode);
			exit(4);
			return 0;
		break;

	}
}























