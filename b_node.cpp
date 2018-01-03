#include "headers.h"


// -----------------------------------------------------------------------------
//  BNode: basic structure of node in b-tree
// -----------------------------------------------------------------------------
BNode::BNode()						// constructor
{
	level_ = -1;
	num_entries_ = -1;
	left_sibling_ = right_sibling_ = -1;

	block_ = capacity_ = -1;
	dirty_ = false;
	btree_ = NULL;

	key_ = NULL;
	son_ = NULL;
}

// -----------------------------------------------------------------------------
BNode::~BNode()						// destructor
{
	char* buf = NULL;
	if (dirty_) {					// if dirty, rewrite to disk
		int block_length = btree_->file_->get_blocklength();
		buf = new char[block_length];
		write_to_buffer(buf);
		btree_->file_->write_block(buf, block_);

		delete[] buf;buf = NULL;
	}

	if (key_ != NULL) {				// release <key_>
		delete[] key_; key_ = NULL;
	}
	if (son_ != NULL) {				// release <son_>
		delete[] son_; son_ = NULL;
	}
}

// -----------------------------------------------------------------------------
void BNode::init(					// init a new node, which not exist
	int level,							// level (depth) in b-tree
	BTree* btree)						// b-tree of this node
{
	btree_ = btree;					// init <btree_>
	level_ = (char) level;			// init <level_>

	num_entries_ = 0;				// init <num_entries_>
	left_sibling_ = -1;				// init <left_sibling_>
	right_sibling_ = -1;			// init <right_sibling_>
	dirty_ = true;					// init <dirty_>

									// init <capacity_>
	int b_length = btree_->file_->get_blocklength();
	capacity_ = (b_length - get_header_size()) / get_entry_size();
	if (capacity_ < 50) {			// at least 50 entries
		printf("capacity = %d\n", capacity_);
		error("BNode::init() capacity too small.\n", true);
	}

	key_ = new float[capacity_];	// init <key_>
	for (int i = 0; i < capacity_; i++) {
		key_[i] = MIN_FLT;
	}
	son_ = new int[capacity_];		// init <son_>
	for (int i = 0; i < capacity_; i++) {
		son_[i] = -1;
	}

	char* blk = new char[b_length];	// init <block_>, get new addr
	block_ = btree_->file_->append_block(blk);
	delete[] blk; blk = NULL;
}

// -----------------------------------------------------------------------------
void BNode::init_restore(			// load an exist node from disk to init
	BTree* btree,						// b-tree of this node
	int block)							// addr of disk for this node
{
	btree_ = btree;					// init <btree_>
	block_ = block;					// init <block_>
	dirty_ = false;					// init <dirty_>

									// get block length
	int b_len = btree_->file_->get_blocklength();

									// init <capacity_>
	capacity_ = (b_len - get_header_size()) / get_entry_size();
	if (capacity_ < 50) {			// at least 50 entries
		printf("capacity = %d\n", capacity_);
		error("BIndexNode::init_restore capacity too small.\n", true);
	}

	key_ = new float[capacity_];	// init <key_>
	for (int i = 0; i < capacity_; i++) {
		key_[i] = MIN_FLT;
	}
	son_ = new int[capacity_];		// init <son_>
	for (int i = 0; i < capacity_; i++) {
		son_[i] = -1;
	}

	// -------------------------------------------------------------------------
	//  Read the buffer <blk> to init <level_>, <num_entries_>, 
	//  <left_sibling_>, <right_sibling_>, <key_> and <son_>.
	// -------------------------------------------------------------------------
	char* blk = new char[b_len];
	btree_->file_->read_block(blk, block);
	read_from_buffer(blk);

	delete[] blk; blk = NULL;
}

// -----------------------------------------------------------------------------
void BNode::read_from_buffer(		// read a b-node from buffer
	char* buf)							// store info of a b-index node
{
	int i = 0;
	// -------------------------------------------------------------------------
	//  read header: <level_> <num_entries_> <left_sibling_> <right_sibling_>
	// -------------------------------------------------------------------------
	memcpy(&level_, &buf[i], SIZECHAR);
	i += SIZECHAR;

	memcpy(&num_entries_, &buf[i], SIZEINT);
	i += SIZEINT;

	memcpy(&left_sibling_, &buf[i], SIZEINT);
	i += SIZEINT;

	memcpy(&right_sibling_, &buf[i], SIZEINT);
	i += SIZEINT;

	// -------------------------------------------------------------------------
	//  Read entries: <key_> and <son_>
	// -------------------------------------------------------------------------
	for (int j = 0; j < num_entries_; j++) {
		memcpy(&key_[j], &buf[i], SIZEFLOAT);
		i += SIZEFLOAT;

		memcpy(&son_[j], &buf[i], SIZEINT);
		i += SIZEINT;
	}
}

// -----------------------------------------------------------------------------
void BNode::write_to_buffer(		// write info of node into buffer
	char* buf)							// store info of this node (return)
{
	int i = 0;
	// -------------------------------------------------------------------------
	//  Write header: <level_> <num_entries_> <left_sibling_> <right_sibling_>
	// -------------------------------------------------------------------------
	memcpy(&buf[i], &level_, SIZECHAR);
	i += SIZECHAR;

	memcpy(&buf[i], &num_entries_, SIZEINT);
	i += SIZEINT;

	memcpy(&buf[i], &left_sibling_, SIZEINT);
	i += SIZEINT;

	memcpy(&buf[i], &right_sibling_, SIZEINT);
	i += SIZEINT;

	// -------------------------------------------------------------------------
	//  Write entries: <key_> and <son_>
	// -------------------------------------------------------------------------
	for (int j = 0; j < num_entries_; j++) {
		memcpy(&buf[i], &key_[j], SIZEFLOAT);
		i += SIZEFLOAT;

		memcpy(&buf[i], &son_[j], SIZEINT);
		i += SIZEINT;
	}
}

// -----------------------------------------------------------------------------
BNode* BNode::get_left_sibling()	// get the left-sibling node
{
	BNode* node = NULL;
	if (left_sibling_ != -1) {		// left sibling node exist
		node = new BNode();			// read left-sibling from disk
		node->init_restore(btree_, left_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
BNode* BNode::get_right_sibling()	// get the right-sibling node
{
	BNode* node = NULL;
	if (right_sibling_ != -1) {		// right sibling node exist
		node = new BNode();			// read right-sibling from disk
		node->init_restore(btree_, right_sibling_);
	}
	return node;
}

// -----------------------------------------------------------------------------
int BNode::get_son(					// get son indexed by <index>
	int index)							// input index
{
	if (index < 0 || index >= num_entries_) {
		error("BIndexNode::get_son out of range.", true);
	}
	return son_[index];
}

// -----------------------------------------------------------------------------
void BNode::add_new_child(			// add a new entry from its child node
	float key,							// input key
	int son)							// input son
{
	if (num_entries_ >= capacity_) {
		error("BIndexNode::add_new_child overflow", true);
	}

	key_[num_entries_] = key;		// add new entry into its pos
	son_[num_entries_] = son;

	num_entries_++;					// update <num_entries_>
	dirty_ = true;					// node modified, <dirty_> is true
}

// -----------------------------------------------------------------------------
//  Finds position of entry that is just less than or equal to input entry.
//  If input entry is smaller than all entry in this node, we'll return -1. 
//  The scan order is from right to left.
// -----------------------------------------------------------------------------
int BNode::find_position_by_key(	// find pos just less than input entry
	float key)							// input key
{
	int pos = -1;
									// linear scan (right to left)
	for (int i = num_entries_ - 1; i >= 0; i--) {
		if (key_[i] <= key) {
			pos = i;
			break;
		}
	}
	return pos;
}

// -----------------------------------------------------------------------------
float BNode::get_key(				// get <key> indexed by <index>
	int index)							// input index
{
	if (index < 0 || index >= num_entries_) {
		error("BIndexNode::get_key out of range.", true);
	}
	return key_[index];
}

// -----------------------------------------------------------------------------
//  entry: <key_> (SIZEFLOAT) and <son_> (SIZEINT)
// -----------------------------------------------------------------------------
int BNode::get_entry_size()			// get entry size of b-node
{
	int entry_size = SIZEFLOAT + SIZEINT;
	return entry_size;
}

// -----------------------------------------------------------------------------
//  <level>: SIZECHAR
//  <num_entries> <left_sibling> and <right_sibling>: SIZEINT
// -----------------------------------------------------------------------------
int BNode::get_header_size()		// get header size of b-node
{
	int header_size = SIZECHAR + SIZEINT * 3;
	return header_size;
}

// -----------------------------------------------------------------------------
int BNode::get_block()				// get <block_> (address of this node)
{
	return block_;
}

// -----------------------------------------------------------------------------
int BNode::get_num_entries()		// get <num_entries_>
{
	return num_entries_;
}

// -----------------------------------------------------------------------------
int BNode::get_level()				// get <level_>
{
	return level_;
}

// -----------------------------------------------------------------------------
float BNode::get_key_of_node()		// get key of this node
{
	if (key_ != NULL) return key_[0];
	else return -1.0f;
}

// -----------------------------------------------------------------------------
bool BNode::isFull()				// whether is full?
{
	if (num_entries_ >= capacity_) return true;
	else return false;
}

// -----------------------------------------------------------------------------
void BNode::set_left_sibling(		// set addr of left sibling node
	int left_sibling)					// addr of left sibling node
{
	left_sibling_ = left_sibling;
}

// -----------------------------------------------------------------------------
void BNode::set_right_sibling(		// set addr of right sibling node
	int right_sibling)					// addr of right sibling node
{
	right_sibling_ = right_sibling;
}
