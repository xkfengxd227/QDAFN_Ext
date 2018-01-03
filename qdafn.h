#ifndef __QDAFN_H
#define __QDAFN_H

class BNode;
class BTree;

// -----------------------------------------------------------------------------
//  PDist_Pair: structure to store the object id and its projection
// -----------------------------------------------------------------------------
struct PDist_Pair {
	int obj_;						// object id
	float pdist_;					// projection of the object
};

// -----------------------------------------------------------------------------
//  Queue_Item: structure of item for priority
// -----------------------------------------------------------------------------
struct Queue_Item {
	float dist_;					// dist between obj and query after proj
	int tree_id_;					// tree id
};

struct Cmp {						// comparison function
	bool operator()(Queue_Item a, Queue_Item b) {
		if (compfloats(a.dist_, b.dist_) == 0) {
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
	BNode* node_;					// leaf node (level = 0)
	int pos_;						// cur pos of leaf node
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
float nfix();
float efix();
void  zigset(unsigned long jsrseed);

// -----------------------------------------------------------------------------
//  Comparison Function for qsort
// -----------------------------------------------------------------------------
int pdist_cmp(
	const void* xv, 
	const void* yv);

// -----------------------------------------------------------------------------
//  Furthest_Index: structure of QDAFN indexed by B+tree. QDAFN is used to 
//  solve the problem of approximate furthest neighbor search.
// -----------------------------------------------------------------------------
class Furthest_Index {
public:
	Furthest_Index();				// constructor
	~Furthest_Index();				// destructor

	// -------------------------------------------------------------------------
	void build(						// build index of QDAFN
		int n,							// number of data points
		int d,							// dimension of space
		int B,							// page size
		int l,							// number of projections
		int m,							// number of candidates
		float ratio,					// approximation ratio
		char* output_folder,			// folder to store info of QDAFN
		float** data);					// dataset

	// -------------------------------------------------------------------------
	int load(						// load index of QDAFN
		char* output_folder);			// folder to store info of QDAFN

	// -------------------------------------------------------------------------
	int search(						// c-k-afn search via QDAFN
		int top_k,						// top-k value
		float* query,					// query point
		char* data_folder,				// folder to store new format of data
		MaxK_List* list);				// top-k results (return)

protected:
	int num_points_;				// number of data objects <n>
	int num_dimensions_;			// dimensionality <d>
	int B_;							// page size in words
	char index_path_[200];			// folder to store index

	int num_projections_;			// number of random projections <l>
	int num_candidates_;			// number of candidates <m>
	int page_io_;					// page I/O for search
	int dist_io_;					// random I/O to cmpt real distance

	float* projections_;			// random projection vectors
	PDist_Pair** pdp_;				// projected distance arrays
	BTree** trees_;					// B+ trees

	// -------------------------------------------------------------------------
	float calc_proj(				// calc projection of a point
		int id,							// projection vector id
		float* point);					// input point

	// -------------------------------------------------------------------------
	void get_tree_filename(			// get file name of tree
		int tree_id,					// tree id
		char* fname);					// file name of tree (return)

	// -------------------------------------------------------------------------
	int int_search(					// internal search
		int top_k,						// top-k value
		float* query,					// query point
		char* data_folder,				// folder to store new format of data
		MaxK_List* list);				// top-k results (return)

	// -------------------------------------------------------------------------
	int ext_search(					// external search
		int top_k,						// top-k value
		float* query,					// query point
		char* data_folder,				// folder to store new format of data
		MaxK_List* list);				// top-k results (return)

	// -------------------------------------------------------------------------
	void init_buffer(				// init page buffer
		int num_projections,			// num of projections used for search
		float* query,					// query point
		Page_Buffer* page,				// buffer page (return)
		float* proj_q);					// projection of query (return)

	// -------------------------------------------------------------------------
	void update_page(				// update page
		Page_Buffer* page);				// page buffer

	// -------------------------------------------------------------------------
	float calc_proj_dist(			// calc proj_dist
		const Page_Buffer* page,		// page buffer
		float q_dist);					// hash value of query
};

#endif
