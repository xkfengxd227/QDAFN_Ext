#ifndef __B_TREE_H
#define __B_TREE_H

class BlockFile;
class BNode;
struct PDist_Pair;

// -----------------------------------------------------------------------------
//  BTree: structure to index the projection produced by QDAFN
// -----------------------------------------------------------------------------
class BTree {
public:
									// disk params
	int root_;						// address of disk for root

									// other params
	BlockFile* file_;				// file in disk to store
	BNode* root_ptr_;				// pointer of root

	// -------------------------------------------------------------------------
	BTree();						// constructor
	~BTree();						// destructor

	// -------------------------------------------------------------------------
	void init(						// init a new b-tree
		char* fname,					// file name
		int b_length);					// block length

	// -------------------------------------------------------------------------
	void init_restore(				// load an exist b-tree
		char* fname);					// file name

	// -------------------------------------------------------------------------
	int bulkload(					// bulkload b-tree
		PDist_Pair* pdp,				// projection distance pairs
		int n);							// number of entries

private:
	// -------------------------------------------------------------------------
	int read_header(				// read <root> from buffer
		char* buf);						// the buffer

	// -------------------------------------------------------------------------
	int write_header(				// write <root> into buffer
		char* buf);						// the buffer (return)

	// -------------------------------------------------------------------------
	void load_root();				// load root of b-tree
	void delete_root();				// delete root of b-tree
};

#endif
