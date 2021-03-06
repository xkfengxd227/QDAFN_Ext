#ifndef __QDAFN_H
#define __QDAFN_H

class  BNode;
class  BTree;
struct Result;

// -----------------------------------------------------------------------------
//  Queue_Item: structure of item for priority
// -----------------------------------------------------------------------------
struct Queue_Item {
	float dist_;					// dist between obj and query after proj
	int   tree_id_;					// tree id
};

struct Cmp {						// comparison function
	bool operator()(Queue_Item a, Queue_Item b) {
		if (fabs(a.dist_ - b.dist_) < FLOATZERO) {
			return (a.tree_id_ > b.tree_id_);
		}
		if (a.dist_ > b.dist_) return false;
		else return true;
	}
};

// -----------------------------------------------------------------------------
//  Page_Buffer: buffer pages of b-tree for search of QDAFN
// -----------------------------------------------------------------------------
struct Page_Buffer {
	BNode *node_;					// leaf node (level = 0)
	int   pos_;						// cur pos of leaf node
};

// -----------------------------------------------------------------------------
//  Ziggurat Method standard normal pseudorandom number generator code from 
//  George Marsaglia and Wai Wan Tsang (2000).
// 
//  "The Ziggurat Method for Generating Random Variables". Journal of
//  Statistical Software 5 (8).
// -----------------------------------------------------------------------------
extern unsigned long jz,jsr;

extern long hz;
extern unsigned long iz, kn[128], ke[256];
extern float wn[128],fn[128], we[256],fe[256];

static char* algoname[3]={"By Value","By Rank","Query Dependent"};

#define SHR3 (jz=jsr, jsr^=(jsr<<13), jsr^=(jsr>>17), jsr^=(jsr<<5), jz+jsr)
#define UNI  (0.5 + (signed) SHR3 * 0.2328306e-9)
#define IUNI SHR3

#define RNOR (hz=SHR3, iz=hz&127, (abs(hz)<kn[iz])? hz*wn[iz] : nfix())
#define REXP (jz=SHR3, iz=jz&255, (jz <ke[iz])? jz*we[iz] : efix())

// -----------------------------------------------------------------------------
//  nfix() generates variates from the residue when rejection in RNOR occurs
// -----------------------------------------------------------------------------
float nfix();

// -----------------------------------------------------------------------------
//  efix() generates variates from the residue when rejection in REXP occurs
// -----------------------------------------------------------------------------
float efix();

// -----------------------------------------------------------------------------
void zigset(						// set the seed and create the tables
	unsigned long jsrseed);				// new seed

// -----------------------------------------------------------------------------
//  Furthest_Index: data structure of QDAFN for c-k-AFN search
// -----------------------------------------------------------------------------
class Furthest_Index {
public:
	Furthest_Index();				// default constructor
	~Furthest_Index();				// destructor

	// -------------------------------------------------------------------------
	void build(						// build index
		int   n,						// number of data objects
		int   d,						// dimension of space
		int   B,						// page size
		int   l,						// number of projections
		int   m,						// number of candidates
		float ratio,					// approximation ratio
		const char *index_path,			// index path
		const float **data);			// data objects

	// -------------------------------------------------------------------------
	int load(						// load index
		const char *index_path);		// index path

	// -------------------------------------------------------------------------
	int search(						// c-k-afn search
		int   top_k,					// top-k value
		const float *query,				// query object
		const char *data_folder,		// new format data folder
		MaxK_List *list);				// top-k results (return)

protected:
	int    n_pts_;					// number of data objects <n>
	int    dim_;					// dimensionality <d>
	int    B_;						// page size in words
	char   index_path_[200];		// folder to store index

	int    l_;						// number of random projections <l>
	int    m_;						// number of candidates <m>
	int    page_io_;				// page I/O for search
	int    dist_io_;				// random I/O to compute Euclidean dist
	float  *projections_;			// random projection vectors
	Result **table_;				// projected distance arrays
	BTree  **trees_;				// B+ trees

	// -------------------------------------------------------------------------
	float calc_proj(				// calc projection of input data object
		int   id,						// projection vector id
		const float *data);				// input data object 

	// -------------------------------------------------------------------------
	void get_tree_filename(			// get file name of tree
		int  tree_id,					// tree id
		char *fname);					// file name of tree (return)

	// -------------------------------------------------------------------------
	int int_search(					// internal search
		int   top_k,					// top-k value
		const float *query,				// query object
		const char *data_folder,		// new format data folder
		MaxK_List *list);				// top-k results (return)

	// -------------------------------------------------------------------------
	int ext_search(					// external search
		int   top_k,					// top-k value
		const float *query,				// query object
		const char *data_folder,		// new format data folder
		MaxK_List *list);				// top-k results (return)

	// -------------------------------------------------------------------------
	void init_buffer(				// init page buffer
		const float *query,				// query point
		Page_Buffer *page,				// buffer page (return)
		float *proj_q);					// projection of query (return)

	// -------------------------------------------------------------------------
	void update_page(				// update page
		Page_Buffer *page);				// page buffer (return)

	// -------------------------------------------------------------------------
	float calc_dist(				// calc projected distance
		float proj_q,					// projection of query
		const Page_Buffer *page);		// page buffer
};

#endif // __QDAFN_H
