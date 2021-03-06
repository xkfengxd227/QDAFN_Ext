#include "headers.h"

// -----------------------------------------------------------------------------
//  Ziggurat Method standard normal pseudorandom number generator code from 
//  George Marsaglia and Wai Wan Tsang (2000).
// 
//  "The Ziggurat Method for Generating Random Variables". Journal of
//  Statistical Software 5 (8).
// -----------------------------------------------------------------------------
unsigned long jsr = 123456789;
unsigned long jz;
long hz;
unsigned long iz, kn[128], ke[256];
float wn[128],fn[128], we[256],fe[256];

// -----------------------------------------------------------------------------
//  nfix() generates variates from the residue when rejection in RNOR occurs
// -----------------------------------------------------------------------------
float nfix()
{
	const float r = 3.442620;		// The start of the right tail
	static float x, y;

	while (1) {
		x = hz * wn[iz];
		if (iz == 0) {				// iz == 0, handles the base strip
			do {
				x = -log(UNI)*0.2904764;
				y = -log(UNI);
			} while (y+y < x*x);

			return (hz > 0) ? r+x : -r-x;
		}
									// iz > 0, handle the wedges of other strips
		if (fn[iz] + UNI*(fn[iz-1]-fn[iz]) < exp(-0.5*x*x)) {
			return x;
		}

		hz = SHR3;					// initiate, try to exit loop
		iz = hz & 127;
		if (abs(hz) < kn[iz]) return hz*wn[iz];
	}
}

// -----------------------------------------------------------------------------
//  efix() generates variates from the residue when rejection in REXP occurs
// -----------------------------------------------------------------------------
float efix()
{
	float x;

	while (1) {
		if (iz == 0) {
			return 7.69711F-log(UNI);	// iz == 0
		}

		x = jz * we[iz];
		if (fe[iz] + UNI*(fe[iz-1]-fe[iz]) < exp(-x)) {
			return x;
		}

		jz = SHR3;					// initiate, try to exit loop
		iz = (jz & 255);
		if (jz < ke[iz]) return jz*we[iz];
	}
}

// -----------------------------------------------------------------------------
void zigset(						// set the seed and create the tables
	unsigned long jsrseed)				// new seed
{
	const double m1 = 2147483648.0;
	const double m2 = 4294967296.0;

	double dn = 3.442619855899;
	double tn = dn;
	double vn = 9.91256303526217e-3;
	double q;

	double de=7.697117470131487;
	double te=de;
	double ve=3.949659822581572e-3;
	int i;

	jsr^=jsrseed;

	// -------------------------------------------------------------------------
	//  set up tables for RNOR
	// -------------------------------------------------------------------------
	q = vn / exp(-0.5 * dn * dn);
	kn[0] = (dn / q) * m1;
	kn[1] = 0;

	wn[0] = q / m1;
	wn[127] = dn / m1;

	fn[0] = 1.0;
	fn[127] = exp(-0.5 * dn * dn);

	for (i = 126; i >= 1; i--) {
		dn = sqrt(-2.0 * log(vn / dn + exp(-0.5 * dn * dn)));
		kn[i+1] = (dn / tn) * m1;
		tn = dn;
		fn[i] = exp(-0.5 * dn * dn);
		wn[i] = dn / m1;
	}

	// -------------------------------------------------------------------------
	//  Set up tables for REXP
	// -------------------------------------------------------------------------
	q = ve / exp(-de);
	ke[0] = (de / q) * m2;
	ke[1] = 0;

	we[0] = q / m2;
	we[255] = de / m2;

	fe[0] = 1.0;
	fe[255] = exp(-de);

	for (i = 254; i >= 1; i--) {
		de = -log(ve / de + exp(-de));
		ke[i+1] = (de / te) * m2;
		te = de;
		fe[i] = exp(-de);
		we[i] = de / m2;
	}
}

// -----------------------------------------------------------------------------
Furthest_Index::Furthest_Index()	// default constructor
{
	n_pts_       = -1;
	dim_         = -1;
	B_           = -1;
	l_           = -1;
	m_           = -1;
	page_io_     = -1;
	dist_io_     = -1;
	projections_ = NULL;
	table_       = NULL;
	trees_       = NULL;
}

// -----------------------------------------------------------------------------
Furthest_Index::~Furthest_Index()	// destructor
{
	if (projections_ != NULL) {
		delete[] projections_; projections_ = NULL;
	}

	if (table_ != NULL) {
		for (int i = 0; i < l_; ++i) {
			delete table_[i]; table_[i] = NULL;
		}
		delete[] table_; table_ = NULL;
	}

	if (trees_ != NULL) {
		for (int i = 0; i < l_; ++i) {
			delete trees_[i]; trees_[i] = NULL;
		}
		delete[] trees_; trees_ = NULL;
	}
}

// -----------------------------------------------------------------------------
void Furthest_Index::build(			// build index
	int   n,							// number of data objects
	int   d,							// dimension of space
	int   B,							// page size
	int   l,							// number of projections
	int   m,							// number of candidates
	float ratio,						// approximation ratio
	const char *index_path,				// index path
	const float **data)					// data objects
{
	// -------------------------------------------------------------------------
	//  set up parameters of QDAFN
	// -------------------------------------------------------------------------
	n_pts_ = n;
	dim_   = d;
	B_     = B;

	strcpy(index_path_, index_path);
	create_dir(index_path_);

	if (l == 0 || m ==0) {
		l_ = 2 * (int) ceil(pow((float) n, 1.0F/(ratio*ratio)));
		if (l_ < 1) {
			fprintf(stderr, "bad number of projection <l> %d\n", 
				l_);
			exit(1);
		}

		float x = pow(log((float) n), (ratio*ratio/2.0F - 1.0F/3.0F));
		m_ = 1 + (int) ceil(E * E * l_ * x);
		if (m_ < 1) {
			fprintf(stderr, "bad number of candidates <m> %d\n", m_);
			exit(1);
		}
	}
	else {
		l_ = l;
		m_ = m;
	}

	printf("Build Index of QDAFN (SISAP2015 paper):\n");
	printf("    n         = %d\n", n_pts_);
	printf("    d         = %d\n", dim_);
	printf("    B         = %d\n", B_);
	printf("    l         = %d\n", l_);
	printf("    m         = %d\n", m_);
	printf("    algorithm = %s\n", algoname[2]);
	printf("    path      = %s\n", index_path_);
	printf("\n");

	// -------------------------------------------------------------------------
	//  generate random projection directions
	//  TODO: allow adjustable seed, here we use random seed
	// -------------------------------------------------------------------------
	zigset(MAGIC + time(NULL));

	int size = l_ * dim_;
	projections_ = new float[size];
	for (int i = 0; i < size; ++i) {
		projections_[i] = RNOR / sqrt((float) dim_);
	}

	// -------------------------------------------------------------------------
	//  write the "para" file
	// -------------------------------------------------------------------------
	char fname[200];
	strcpy(fname, index_path_);
	strcat(fname, "para");

	FILE* fp = fopen(fname, "wb");
	if (!fp) {
		printf("Could not create %s\n", fname);
		printf("Perhaps no such folder %s?\n", index_path_);
		exit(1);
	}

	fwrite(&n_pts_, SIZEINT, 1, fp);
	fwrite(&dim_, SIZEINT, 1, fp);
	fwrite(&B_, SIZEINT, 1, fp);
	fwrite(&l_, SIZEINT, 1, fp);
	fwrite(&m_, SIZEINT, 1, fp);
	fwrite(projections_, SIZEFLOAT, l_ * dim_, fp);

	// -------------------------------------------------------------------------
	//  construct the projected distance arrays
	// -------------------------------------------------------------------------
	table_ = new Result*[l_];
	for (int i = 0; i < l_; ++i) {
		table_[i] = new Result[n_pts_];
		for (int j = 0; j < n_pts_; ++j) {
			table_[i][j].id_  = j;
			table_[i][j].key_ = calc_proj(i, data[j]);
		}

		qsort(table_[i], n_pts_, sizeof(Result), ResultComp);

		if (m_ > CANDIDATES) {
			// -----------------------------------------------------------------
			//  index with B+ trees
			// -----------------------------------------------------------------
			get_tree_filename(i, fname);
			BTree *bt = new BTree();
			bt->init(B_, fname);
			if (bt->bulkload(m_, table_[i])) return;
			delete bt; bt = NULL;
		}
		else {
			// -----------------------------------------------------------------
			//  store in the 'para' file
			// -----------------------------------------------------------------
			fwrite(table_[i], sizeof(Result), m_, fp);
		}
	}

	fclose(fp);
}

// -----------------------------------------------------------------------------
float Furthest_Index::calc_proj(	// calc projection of input data object
	int   id,							// projection vector id
	const float *data)					// input data object
{
	int   base   = id * dim_;
	float result = 0.0f;
	for (int i = 0; i < dim_; ++i) {
		result += projections_[base + i] * data[i];
	}
	return result;
}

// -----------------------------------------------------------------------------
void Furthest_Index::get_tree_filename(	// get file name of b-tree
	int tree_id,						// tree id, from 0 to m-1
	char* fname)						// file name (return)
{
	sprintf(fname, "%s%d.qdafn", index_path_, tree_id);
}

// -----------------------------------------------------------------------------
int Furthest_Index::load(			// load index
	const char *index_path)				// index path
{
	strcpy(index_path_, index_path);

	// -------------------------------------------------------------------------
	//  read the "para" file
	// -------------------------------------------------------------------------
	char fname[200];
	strcpy(fname, index_path_);
	strcat(fname, "para");

	FILE* fp = fopen(fname, "rb");
	if (!fp) {
		fprintf(stderr, "Could not open %s\n", fname);
		exit(1);
	}

	fread(&n_pts_, SIZEINT, 1, fp);
	fread(&dim_, SIZEINT, 1, fp);
	fread(&B_, SIZEINT, 1, fp);
	fread(&l_, SIZEINT, 1, fp);
	fread(&m_, SIZEINT, 1, fp);

	int size = l_ * dim_;
	projections_ = new float[size];
	fread(projections_, SIZEFLOAT, size, fp);

	printf("Load Index of QDAFN (SISAP2015 paper):\n");
	printf("    n         = %d\n", n_pts_);
	printf("    d         = %d\n", dim_);
	printf("    B         = %d\n", B_);
	printf("    l         = %d\n", l_);
	printf("    m         = %d\n", m_);
	printf("    algorithm = %s\n", algoname[2]);
	printf("    path      = %s\n", index_path_);
	printf("\n");

	if (m_ > CANDIDATES) {
		// ---------------------------------------------------------------------
		//  load <l> B+ trees
		// ---------------------------------------------------------------------
		trees_ = new BTree*[l_];
		for (int i = 0; i < l_; ++i) {
			get_tree_filename(i, fname);

			trees_[i] = new BTree();
			trees_[i]->init_restore(fname);
		}
	}
	else {
		// ---------------------------------------------------------------------
		//  read from 'para' file
		// ---------------------------------------------------------------------
		table_ = new Result*[l_];
		for (int i = 0; i < l_; ++i) {
			table_[i] = new Result[m_];
			fread(table_[i], sizeof(Result), m_, fp);
		}
	}
	fclose(fp);

	return 0;
}

// -----------------------------------------------------------------------------
int Furthest_Index::search(			// c-k-AFN search
	int   top_k,						// top-k value
	const float *query,					// query object
	const char *data_folder,			// new format data folder
	MaxK_List *list)					// top-k results (return)
{
	if (m_ > CANDIDATES) return ext_search(top_k, query, data_folder, list);
	else return int_search(top_k, query, data_folder, list);
}

// -----------------------------------------------------------------------------
int Furthest_Index::int_search(		// internal search
	int   top_k,						// top-k value
	const float *query,					// query object
	const char *data_folder,			// new format data folder
	MaxK_List *list)					// top-k results (return)
{
	int candidates = m_ + top_k;
	if (candidates > n_pts_) candidates = n_pts_;

	// -------------------------------------------------------------------------
	//  allocation and initialize <proj_q>
	// -------------------------------------------------------------------------
	int   *next   = new int[l_];
	float *proj_q = new float[l_];
	for (int i = 0; i < l_; ++i) {
		next[i]   = 0;
		proj_q[i] = calc_proj(i, query);;
	}

	bool *checked = new bool[n_pts_];
	for (int i = 0; i < n_pts_; ++i) checked[i] = false;

	float *data = new float[dim_];
	for (int i = 0; i < dim_; ++i) data[i] = -1.0f;

	// -------------------------------------------------------------------------
	//  c-k-AFN search
	// -------------------------------------------------------------------------
	priority_queue<Queue_Item, vector<Queue_Item>, Cmp> pri_queue;
	Queue_Item q_item;
	for (int i = 0; i < l_; ++i) {
		q_item.dist_ = fabs(table_[i][next[i]].key_ - proj_q[i]);
		q_item.tree_id_ = i;

		pri_queue.push(q_item);
	}

	dist_io_ = 0;
	for (int i = 0; i < candidates; ++i) {
		// ---------------------------------------------------------------------
		//  get obj with largest proj dist and remove it from the queue
		// ---------------------------------------------------------------------
		if (pri_queue.empty()) break;
		q_item = pri_queue.top();
		pri_queue.pop();

		// ---------------------------------------------------------------------
		//  check the current candidate
		// ---------------------------------------------------------------------
		int pid = q_item.tree_id_;
		int id  = table_[pid][next[pid]].id_;
		if (!checked[id]) {
			checked[id] = true;
			read_data_new_format(id, dim_, B_, data_folder, data);

			float dist = calc_l2_dist(dim_, (const float *) data, query);
			list->insert(dist, id + 1);
			++dist_io_;
		}

		// ---------------------------------------------------------------------
		//  update the priority queue
		// ---------------------------------------------------------------------
		if (++next[pid] < m_) {
			q_item.dist_ = fabs(table_[pid][next[pid]].key_ - proj_q[pid]);
			pri_queue.push(q_item);
		}
	}

	// -------------------------------------------------------------------------
	//  release space
	// -------------------------------------------------------------------------
	delete[] proj_q; proj_q = NULL;
	delete[] next; next = NULL;
	delete[] checked; checked = NULL;
	delete[] data; data = NULL;

	return dist_io_;
}

// -----------------------------------------------------------------------------
int Furthest_Index::ext_search(		// external search
	int   top_k,						// top-k value
	const float *query,					// query object
	const char *data_folder,			// new format data folder
	MaxK_List *list)					// top-k results (return)
{
	int candidates = m_ + top_k;
	if (candidates > n_pts_) candidates = n_pts_;

	// -------------------------------------------------------------------------
	//  allocation and initialization
	// -------------------------------------------------------------------------
	bool *checked = new bool[n_pts_];
	for (int i = 0; i < n_pts_; ++i) checked[i] = false;

	float *data = new float[dim_];
	for (int i = 0; i < dim_; ++i) data[i] = 0.0f;

	float *proj_q = new float[l_];
	Page_Buffer *page = new Page_Buffer[l_];
	for (int i = 0; i < l_; ++i) {
		proj_q[i]     = -1.0f;
		page[i].node_ = NULL;
		page[i].pos_  = -1;
	}

	// -------------------------------------------------------------------------
	//  compute hash value <q_dist> of query and init page buffers <page> 
	// -------------------------------------------------------------------------
	page_io_ = 0;					// page i/os for search
	dist_io_ = 0;					// i/os for distance computation
	init_buffer(query, page, proj_q);

	// -------------------------------------------------------------------------
	//  c-k-AFN search
	// -------------------------------------------------------------------------
	priority_queue<Queue_Item, vector<Queue_Item>, Cmp> pri_queue;
	Queue_Item q_item;
	for (int i = 0; i < l_; ++i) {
		if (page[i].node_) {
			q_item.dist_    = calc_dist(proj_q[i], &page[i]);
			q_item.tree_id_ = i;

			pri_queue.push(q_item);
		}
	}

	for (int i = 0; i < candidates; ++i) {
		if (pri_queue.empty()) break;
		q_item = pri_queue.top();	// get the object with largest proj dist
		pri_queue.pop();			// delete the object from the queue

		int j = q_item.tree_id_;
		int id = page[j].node_->get_son(page[j].pos_);
		if (!checked[id]) {
			checked[id] = true;
			read_data_new_format(id, dim_, B_, data_folder, data);

			float dist = calc_l2_dist(dim_, (const float *) data, query);
			list->insert(dist, id + 1);
			++dist_io_;
		}

		update_page(&page[j]);		// update page
		if (page[j].node_) {		// update priority queue
			q_item.dist_ = calc_dist(proj_q[j], &page[j]);
			pri_queue.push(q_item);
		}
	}

	// -------------------------------------------------------------------------
	//  release space
	// -------------------------------------------------------------------------
	while (!pri_queue.empty()) pri_queue.pop();

	delete[] proj_q;  proj_q = NULL;
	delete[] data;    data = NULL;
	delete[] checked; checked = NULL;
	for (int i = 0; i < l_; ++i) {
		if (page[i].node_ != NULL) {
			delete page[i].node_; page[i].node_ = NULL;
		}
	}
	delete[] page; page = NULL;

	return page_io_ + dist_io_;
}

// -----------------------------------------------------------------------------
void Furthest_Index::init_buffer(	// init page buffer
	const float *query,					// query point
	Page_Buffer *page,					// buffer page (return)
	float *proj_q)						// projection of query (return)
{
	int block = -1;
	BNode *node = NULL;

	for (int i = 0; i < l_; ++i) {
		proj_q[i] = calc_proj(i, query);

		block = trees_[i]->root_;
		node = new BNode();
		node->init_restore(trees_[i], block);
		page_io_++;

		if (node->get_level() == 0) {
			// -----------------------------------------------------------------
			//  leaf level
			// -----------------------------------------------------------------
			page[i].node_ = node;
			page[i].pos_ = 0;

			node = NULL;
		}
		else {
			// -----------------------------------------------------------------
			//  non-leaf level
			// -----------------------------------------------------------------
			while (node->get_level() > 1) {
				block = node->get_son(0);
				delete node; node = NULL;

				node = new BNode();
				node->init_restore(trees_[i], block);
				page_io_++;
			}

			block = node->get_son(0);
			page[i].node_ = new BNode();
			page[i].node_->init_restore(trees_[i], block);
			page[i].pos_ = 0;
			page_io_++;

			if (node != NULL) {
				delete node; node = NULL;
			}
		}
	}
}

// -----------------------------------------------------------------------------
void Furthest_Index::update_page(	// update right node info
	Page_Buffer *page)					// page buffer
{
	BNode *node = NULL;
	BNode *old_node = NULL;

	page->pos_++;
	if (page->pos_ > page->node_->get_num_entries() - 1) {
		old_node = page->node_;
		node = page->node_->get_right_sibling();

		if (node != NULL) {
			page->node_ = node;
			page->pos_ = 0;
			page_io_++;
		}
		else {
			page->node_ = NULL;
			page->pos_ = -1;
		}
		delete old_node; old_node = NULL;
	}
}

// -----------------------------------------------------------------------------
float Furthest_Index::calc_dist(	// calc proj_dist
	float proj_q,						// projection of query
	const Page_Buffer *page)			// page buffer
{
	float key = page->node_->get_key(page->pos_);
	return fabs(key - proj_q);
}
