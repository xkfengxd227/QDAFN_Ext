#ifndef __DEF_H
#define __DEF_H

// -----------------------------------------------------------------------------
//  Typedefs
// -----------------------------------------------------------------------------
typedef char Block[];

// -----------------------------------------------------------------------------
//  Macros
// -----------------------------------------------------------------------------
#define MIN(a, b)	(((a) < (b)) ? (a) : (b))
#define MAX(a, b)	(((a) > (b)) ? (a) : (b))

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define MAGIC 36553368

// -----------------------------------------------------------------------------
//  Constants
// -----------------------------------------------------------------------------
const float MAX_FLT = 3.40282e+038F;// max float value
const float MIN_FLT = -MAX_FLT;		// min float value

const int MAX_INT =  2147483647;	// max integer value
const int MIN_INT = -2147483648;		// min integer value

const float E  = 2.7182818F;		// math constants
const float PI = 3.141592654F;
const float FLOATZERO = 1e-6F;		// accuracy

const int SIZEBOOL   = (int) sizeof(bool);
const int SIZECHAR   = (int) sizeof(char);
const int SIZEINT    = (int) sizeof(int);
const int SIZEFLOAT  = (int) sizeof(float);
const int SIZEDOUBLE = (int) sizeof(double);

const int BFHEAD_LENGTH = SIZEINT*2;// file header size
const int CANDIDATES = 200;
const int MAXK = 10;				// max top-k value

#endif
