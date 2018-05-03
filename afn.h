#ifndef __AFN_H
#define __AFN_H

// -----------------------------------------------------------------------------
int ground_truth(					// find ground truth
	int   n,							// number of data objects
	int   qn,							// number of query objects
	int   d,							// dimensionality
	const char *data_set,				// address of data  set
	const char *query_set,				// address of query set
	const char *truth_set);				// address of truth set
	
// -----------------------------------------------------------------------------
int indexing(						// indexing of QDAFN
	int   n,							// number of data points
	int   d,							// dimension of space
	int   B,							// page size
	int   l,							// number of projections
	int   m,							// number of candidates
	float ratio,						// approximation ratio
	const char *data_set,				// address of data set
	const char *data_folder,			// data folder
	const char *output_folder);			// output folder

// -----------------------------------------------------------------------------
int qdafn(							// c-k-AFN via QDAFN
	int   qn,							// number of query points
	int   d,							// dimensionality
	const char *query_set,				// address of query set
	const char *truth_set,				// address of truth set
	const char *data_folder,			// data folder
	const char *output_folder);			// output folder

#endif // __AFN_H
