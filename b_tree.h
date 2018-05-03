#ifndef __B_TREE_H
#define __B_TREE_H

class  BlockFile;
class  BNode;
struct Result;

// -----------------------------------------------------------------------------
//  BTree: structure to index the projection produced by QDAFN
// -----------------------------------------------------------------------------
class BTree {
public:
	int root_;						// address of disk for root
	BNode *root_ptr_;				// pointer of root
	BlockFile *file_;				// file in disk to store

	// -------------------------------------------------------------------------
	BTree();						// default constructor
	~BTree();						// destructor

	// -------------------------------------------------------------------------
	void init(						// init a new b-tree
		int   b_length,					// block length
		const char *fname);				// file name

	// -------------------------------------------------------------------------
	void init_restore(				// load an exist b-tree
		const char *fname);				// file name

	// -------------------------------------------------------------------------
	int bulkload(					// bulkload b-tree
		int   n,						// number of entries
		const Result *table);			// table of projected distance

protected:
	// -------------------------------------------------------------------------
	int read_header(				// read <root> from buffer
		const char* buf);				// the buffer

	// -------------------------------------------------------------------------
	int write_header(				// write <root> into buffer
		char* buf);						// the buffer (return)

	// -------------------------------------------------------------------------
	void load_root();				// load root of b-tree

	// -------------------------------------------------------------------------
	void delete_root();				// delete root of b-tree
};

#endif // __B_TREE_H
