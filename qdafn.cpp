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
// nfix() generates variates from the residue when rejection in RNOR occurs.
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
// efix() generates variates from the residue when rejection in REXP occurs.
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
// This procedure sets the seed and creates the tables
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
	//  Set up tables for RNOR
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
//  Ascending order for <pdist>
// -----------------------------------------------------------------------------
int pdist_cmp(
	const void* xv,
	const void* yv)
{
	float x,y;

	x = ((PDist_Pair*) xv)->pdist_;
	y = ((PDist_Pair*) yv)->pdist_;

	if (x < y) return -1;
	if (x > y) return 1;
	return 0;
}

// -----------------------------------------------------------------------------
//  Furthest_Index: structure of QDAFN indexed by B+tree. QDAFN is used to 
//  solve the problem of approximate furthest neighbor search.
// -----------------------------------------------------------------------------
Furthest_Index::Furthest_Index()	// constructor
{
	num_points_= -1;
	num_dimensions_ = -1;
	B_ = -1;

	num_projections_ = -1;
	num_candidates_ = -1;
	page_io_ -1;
	dist_io_ = -1;

	projections_ = NULL;
	pdp_ = NULL;
	trees_ = NULL;
}

// -----------------------------------------------------------------------------
Furthest_Index::~Furthest_Index()	// destructor
{
	if (projections_ != NULL) {
		delete[] projections_; projections_ = NULL;
	}

	if (pdp_ != NULL) {
		for (int i = 0; i < num_projections_; i++) {
			delete pdp_[i]; pdp_[i] = NULL;
		}
		delete[] pdp_; pdp_ = NULL;
	}

	if (trees_ != NULL) {
		for (int i = 0; i < num_projections_; i++) {
			delete trees_[i]; trees_[i] = NULL;
		}
		delete[] trees_; trees_ = NULL;
	}
}

// -----------------------------------------------------------------------------
void Furthest_Index::build(			// build index of QDAFN
	int n,								// number of data points
	int d,								// dimension of space
	int B,								// page size
	int l,								// number of projections
	int m,								// number of candidates
	float ratio,						// approximation ratio
	char* output_folder,				// folder to store info of QDAFN
	float** data)						// dataset
{
	// -------------------------------------------------------------------------
	//  set up parameters of QDAFN
	// -------------------------------------------------------------------------
	num_points_ = n;
	num_dimensions_ = d;
	B_ = B;

	strcpy(index_path_, output_folder);
	strcat(index_path_, "indices/");

	if (l == 0 || m ==0) {
		num_projections_ = 2 * (int) ceil(pow((float) n, 1.0F/(ratio*ratio)));
		if (num_projections_ < 1) {
			fprintf(stderr, "bad number of projection <l> %d\n", 
				num_projections_);
			exit(1);
		}

		float x = pow(log((float) n), (ratio*ratio/2.0F-1.0F/3.0F));
		num_candidates_ = 1 + (int) ceil(E*E*num_projections_*x);
		if (num_candidates_ < 1) {
			fprintf(stderr, "bad number of candidates <m> %d\n", 
				num_candidates_);
			exit(1);
		}
	}
	else {
		num_projections_ = l;
		num_candidates_ = m;
	}

	printf("Generating Index of QDAFN (SISAP2015 paper):\n");
	printf("    Data Objects: n = %d\n", num_points_);
	printf("    Dimension:    d = %d\n", num_dimensions_);
	printf("    Page Size:    B = %d\n", B_);
	printf("    Projections:  l = %d\n", num_projections_);
	printf("    Candidates:   m = %d\n", num_candidates_);
	printf("    Algorithm:    %s\n",     algoname[2]);
	printf("    Index Folder: %s\n\n",   index_path_);

	// -------------------------------------------------------------------------
	//  generate random projection directions
	//  TODO: allow adjustable seed, here we use random seed
	// -------------------------------------------------------------------------
	zigset(MAGIC + time(NULL));
	int size = num_projections_ * num_dimensions_;

	projections_ = new float[size];
	for (int i = 0; i < size; i++) {
		projections_[i] = RNOR / sqrt((float) num_dimensions_);
	}

	// -------------------------------------------------------------------------
	//  write the "para" file
	// -------------------------------------------------------------------------
	create_dir(index_path_);
	char fname[200];
	strcpy(fname, index_path_);
	strcat(fname, "para");

	FILE* fp = fopen(fname, "wb");
	if (!fp) {
		printf("Could not create %s.\n", fname);
		printf("Perhaps no such folder %s?\n", index_path_);
		exit(1);
	}

	fwrite(&num_points_, SIZEINT, 1, fp);
	fwrite(&num_dimensions_, SIZEINT, 1, fp);
	fwrite(&B_, SIZEINT, 1, fp);
	fwrite(&num_projections_, SIZEINT, 1, fp);
	fwrite(&num_candidates_, SIZEINT, 1, fp);

	fwrite(projections_, SIZEFLOAT, num_projections_ * num_dimensions_, fp);

	// -------------------------------------------------------------------------
	//  construct the projected distance arrays
	// -------------------------------------------------------------------------
	pdp_ = new PDist_Pair*[num_projections_];
	for (int i = 0; i < num_projections_; i++) {
		pdp_[i] = new PDist_Pair[num_points_];
		for (int j = 0; j < num_points_; j++) {
			pdp_[i][j].obj_ = j;
			pdp_[i][j].pdist_ = calc_proj(i, data[j]);
		}

		qsort(pdp_[i], num_points_, sizeof(PDist_Pair), pdist_cmp);

		if (num_candidates_ > CANDIDATES) {
			// -----------------------------------------------------------------
			//  index with B+ trees
			// -----------------------------------------------------------------
			get_tree_filename(i, fname);
			BTree *bt = new BTree();
			bt->init(fname, B_);
			if (bt->bulkload(pdp_[i], num_candidates_)) {
				return;
			}
			delete bt; bt = NULL;
		}
		else {
			// -----------------------------------------------------------------
			//  store in the 'para' file
			// -----------------------------------------------------------------
			fwrite(pdp_[i], sizeof(PDist_Pair), num_candidates_, fp);
		}
	}

	fclose(fp);
}

// -----------------------------------------------------------------------------
float Furthest_Index::calc_proj(	// calc projection of a point
	int id,								// projection vector id
	float* point)						// input point
{
	float result = 0.0f;
	for (int i = 0; i < num_dimensions_; i++) {
		result += projections_[id * num_dimensions_ + i] * point[i];
	}
	return result;
}

// -----------------------------------------------------------------------------
void Furthest_Index::get_tree_filename(	// get file name of b-tree
	int tree_id,						// tree id, from 0 to m-1
	char* fname)						// file name (return)
{
	char c[10];
	strcpy(fname, index_path_);
	sprintf(c, "%d", tree_id);
	strcat(fname, c);
	strcat(fname, ".qdafn");
}

// -----------------------------------------------------------------------------
int Furthest_Index::load(			// load index of QDAFN
	char* output_folder)				// folder to store info of QDAFN
{
	// -------------------------------------------------------------------------
	//  init <index_path_>
	// -------------------------------------------------------------------------
	strcpy(index_path_, output_folder);
	strcat(index_path_, "indices/");

	// -------------------------------------------------------------------------
	//  read the "para" file
	// -------------------------------------------------------------------------
	char fname[200];
	strcpy(fname, index_path_);
	strcat(fname, "para");

	FILE* fp = fopen(fname, "rb");
	if (!fp) {
		fprintf(stderr, "Could not open %s.\n", fname);
		exit(1);
	}

	fread(&num_points_, SIZEINT, 1, fp);
	fread(&num_dimensions_, SIZEINT, 1, fp);
	fread(&B_, SIZEINT, 1, fp);
	fread(&num_projections_, SIZEINT, 1, fp);
	fread(&num_candidates_, SIZEINT, 1, fp);

	projections_ = new float[num_projections_ * num_dimensions_];
	fread(projections_, SIZEFLOAT, num_projections_ * num_dimensions_, fp);

	printf("Loading Index of QDAFN (SISAP2015 paper):\n");
	printf("    Data Objects: n = %d\n", num_points_);
	printf("    Dimension:    d = %d\n", num_dimensions_);
	printf("    Page Size:    B = %d\n", B_);
	printf("    Projections:  l = %d\n", num_projections_);
	printf("    Candidates:   m = %d\n", num_candidates_);
	printf("    Algorithm:    %s\n",     algoname[2]);
	printf("    Index Folder: %s\n\n",   index_path_);

	if (num_candidates_ > CANDIDATES) {
		// ---------------------------------------------------------------------
		//  load <l> B+ trees
		// ---------------------------------------------------------------------
		trees_ = new BTree*[num_projections_];
		for (int i = 0; i < num_projections_; i++) {
			get_tree_filename(i, fname);

			trees_[i] = new BTree();
			trees_[i]->init_restore(fname);
		}
	}
	else {
		// ---------------------------------------------------------------------
		//  read from 'para' file
		// ---------------------------------------------------------------------
		pdp_ = new PDist_Pair*[num_projections_];
		for (int i = 0; i < num_projections_; i++) {
			pdp_[i] = new PDist_Pair[num_candidates_];
			fread(pdp_[i], sizeof(PDist_Pair), num_candidates_, fp);
		}
	}
	fclose(fp);

	return 0;
}

// -----------------------------------------------------------------------------
int Furthest_Index::search(			// c-k-AFN search
	int top_k,							// top-k value
	float* query,						// query point
	char* data_folder,					// folder to store new format of data
	MaxK_List* list)					// top-k results (return)
{
	if (num_candidates_ > CANDIDATES) {
		return ext_search(top_k, query, data_folder, list);
	}
	else {
		return int_search(top_k, query, data_folder, list);
	}
}

// -----------------------------------------------------------------------------
int Furthest_Index::int_search(		// internal search
	int top_k,							// top-k value
	float* query,						// query point
	char* data_folder,					// folder to store new format of data
	MaxK_List* list)					// top-k results (return)
{
	int num_projections = num_projections_;
	int candidates = num_candidates_ + top_k;
	if (candidates > num_points_) candidates = num_points_;

	// -------------------------------------------------------------------------
	//  allocation and initialize <projected_query>
	// -------------------------------------------------------------------------
	int proj_id, obj_id;
	float dist;

	int* next_to_try = new int[num_projections];
	float* projected_query = new float[num_projections];
	for (int i = 0; i < num_projections; i++) {
		next_to_try[i] = 0;
		projected_query[i] = calc_proj(i, query);;
	}

	bool* checked = new bool[num_points_];
	for (int i = 0; i < num_points_; i++) {
		checked[i] = false;
	}

	float* data = new float[num_dimensions_];
	for (int i = 0; i < num_dimensions_; i++) {
		data[i] = 0.0f;
	}

	// -------------------------------------------------------------------------
	//  c-k-afn search
	// -------------------------------------------------------------------------
	priority_queue<Queue_Item, vector<Queue_Item>, Cmp> pri_queue;
	Queue_Item q_item;
	for (int i = 0; i < num_projections; i++) {
		q_item.dist_ = fabs(pdp_[i][next_to_try[i]].pdist_ 
			- projected_query[i]);
		q_item.tree_id_ = i;

		pri_queue.push(q_item);
	}

	dist_io_ = 0;
	for (int i = 0; i < candidates; i++) {
		// ---------------------------------------------------------------------
		//  get obj with largest proj dist and remove it from the queue
		// ---------------------------------------------------------------------
		if (pri_queue.empty()) break;
		q_item = pri_queue.top();
		pri_queue.pop();

		// ---------------------------------------------------------------------
		//  check the current candidate
		// ---------------------------------------------------------------------
		proj_id = q_item.tree_id_;
		obj_id  = pdp_[proj_id][next_to_try[proj_id]].obj_;
		if (!checked[obj_id]) {
			checked[obj_id] = true;
			read_data(obj_id, num_dimensions_, B_, data, data_folder);

			float dist = calc_l2_dist(data, query, num_dimensions_);
			list->insert(dist, obj_id + 1);
			dist_io_++;
		}

		// ---------------------------------------------------------------------
		//  update the priority queue
		// ---------------------------------------------------------------------
		next_to_try[proj_id]++;
		if (next_to_try[proj_id] < num_candidates_) {
			q_item.dist_ = fabs(pdp_[proj_id][next_to_try[proj_id]].pdist_ 
				- projected_query[proj_id]);
			pri_queue.push(q_item);
		}
	}

	delete[] projected_query; projected_query = NULL;
	delete[] next_to_try; next_to_try = NULL;
	delete[] checked; checked = NULL;
	delete[] data; data = NULL;

	return dist_io_;
}

// -----------------------------------------------------------------------------
int Furthest_Index::ext_search(		// external search
	int top_k,							// top-k value
	float* query,						// query point
	char* data_folder,					// folder to store new format of data
	MaxK_List* list)					// top-k results (return)
{
	int num_projections = num_projections_;
	int candidates = num_candidates_ + top_k;
	if (candidates > num_points_) candidates = num_points_;

	// -------------------------------------------------------------------------
	//  Space allocation and initialization
	// -------------------------------------------------------------------------
	bool* checked = new bool[num_points_];
	for (int i = 0; i < num_points_; i++) {
		checked[i] = false;
	}

	float* data = new float[num_dimensions_];
	for (int i = 0; i < num_dimensions_; i++) {
		data[i] = 0.0f;
	}

	float* proj_q = new float[num_projections];
	for (int i = 0; i < num_projections; i++) {
		proj_q[i] = -1.0f;
	}

	Page_Buffer* page = new Page_Buffer[num_projections];
	for (int i = 0; i < num_projections; i++) {
		page[i].node_ = NULL;
		page[i].pos_  = -1;
	}

	// -------------------------------------------------------------------------
	//  Compute hash value <q_dist> of query and init page buffers
	//  <lptr> and <rptr>.
	// -------------------------------------------------------------------------
	page_io_ = 0;					// page i/os for search
	dist_io_ = 0;					// i/os for distance computation
	init_buffer(num_projections, query, page, proj_q);

	// -------------------------------------------------------------------------
	//  c-k-afn search via QDAFN
	// -------------------------------------------------------------------------
	priority_queue<Queue_Item, vector<Queue_Item>, Cmp> pri_queue;
	Queue_Item q_item;
	for (int i = 0; i < num_projections; i++) {
		if (page[i].node_) {
			q_item.dist_ = calc_proj_dist(&page[i], proj_q[i]);
			q_item.tree_id_ = i;

			pri_queue.push(q_item);
		}
	}

	for (int i = 0; i < candidates; i++) {
		if (pri_queue.empty()) break;
		q_item = pri_queue.top();	// get the object with largest proj dist
		pri_queue.pop();			// delete the object from the queue

		int j = q_item.tree_id_;
		int obj_id = page[j].node_->get_son(page[j].pos_);
		if (!checked[obj_id]) {
			checked[obj_id] = true;
			read_data(obj_id, num_dimensions_, B_, data, data_folder);

			float dist = calc_l2_dist(data, query, num_dimensions_);
			list->insert(dist, obj_id + 1);
			dist_io_++;
		}

		update_page(&page[j]);		// update page
		if (page[j].node_) {		// update priority queue
			q_item.dist_ = calc_proj_dist(&page[j], proj_q[j]);
			pri_queue.push(q_item);
		}
	}

	// -------------------------------------------------------------------------
	//  release space
	// -------------------------------------------------------------------------
	while (!pri_queue.empty()) pri_queue.pop();

	delete[] proj_q; proj_q = NULL;
	delete[] data; data = NULL;
	delete[] checked; checked = NULL;

	for (int i = 0; i < num_projections; i++) {
		if (page[i].node_) {
			delete page[i].node_; page[i].node_ = NULL;
		}
	}
	delete[] page; page = NULL;

	return page_io_ + dist_io_;
}

// -----------------------------------------------------------------------------
void Furthest_Index::init_buffer(	// init page buffer (loc pos of b-treee)
	int num_projections,				// num of projections used for search
	float* query,						// query point
	Page_Buffer* page,					// buffer page (return)
	float* proj_q)						// projection of query (return)
{
	int block = -1;
	BNode* node = NULL;

	for (int i = 0; i < num_projections; i++) {
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
	Page_Buffer* page)					// page buffer
{
	BNode* node = NULL;
	BNode* old_node = NULL;

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
float Furthest_Index::calc_proj_dist(// calc proj_dist
	const Page_Buffer* page,			// page buffer
	float proj_q)						// projection of query
{
	float key = page->node_->get_key(page->pos_);
	return fabs(key - proj_q);
}
