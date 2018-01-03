#include "headers.h"


// -----------------------------------------------------------------------------
//  Utility functions
// -----------------------------------------------------------------------------
int compfloats(						// compare two real values 
	float v1,							// 1st value (type float)
	float v2)							// 2nd value (type float)
{
	if (v1 - v2 < -FLOATZERO) return -1;
	else if (v1 - v2 > FLOATZERO) return 1;
	else return 0;
}

// -----------------------------------------------------------------------------
void error(							// display an error message
	char* msg,							// message
	bool is_exit)						// whether exit
{
	printf(msg);
	if (is_exit) {
		exit(1);
	}
}

// -----------------------------------------------------------------------------
void create_dir(					// create dir if the path exists
	char* data_path)					// input path
{
#ifdef LINUX_						// create directory under Linux
	int len = (int) strlen(data_path);
	for (int i = 0; i < len; i++) {
		if (data_path[i] == '/') {
			char ch = data_path[i + 1];
			data_path[i + 1] = '\0';
									// check whether the directory exists
			int ret = access(data_path, F_OK);
			if (ret != 0) {			// create the directory
				ret = mkdir(data_path, 0755);
				if (ret != 0) {
					printf("Could not create directory %s\n", data_path);
					error("", true);
				}
			}
			data_path[i + 1] = ch;
		}
	}
#else								// create directory under Windows
	int len = (int) strlen(data_path);
	for (int i = 0; i < len; i++) {
		if (data_path[i] == '/') {
			char ch = data_path[i + 1];
			data_path[i + 1] = '\0';
									// check whether the directory exists
			int ret = _access(data_path, 0);
			if (ret != 0) {			// create the directory
				ret = _mkdir(data_path);
				if (ret != 0) {
					printf("Could not create directory %s\n", data_path);
					error("", true);
				}
			}
			data_path[i + 1] = ch;
		}
	}
#endif
}

// -----------------------------------------------------------------------------
float calc_l2_dist(					// calculate l2 distance of 2 vectors
	float* p1,							// 1st point
	float* p2,							// 2nd point
	int dim)							// dimension
{
	float diff = 0.0f;
	float ret  = 0.0f;
	for (int i = 0; i < dim; i++) {
		diff = p1[i] - p2[i];
		ret += diff * diff;
	}
	return sqrt(ret);
}

// -----------------------------------------------------------------------------
float calc_recall(					// calc recall between two ID list
	int n,								// length of ID list
	MaxK_List* list,					// ID list of k-nn method
	int* ID)							// ground truth ID list
{
	int count = 0;
	for (int i = 0; i < n; i++) {
		int obj_id = list->ith_largest_id(i);

		for (int j = 0; j < n; j++) {
			if (obj_id == ID[j]) {
				count++;
				break;
			}
		}
	}
	float recall = (float) count / (float) n;
	return recall;
}


// -----------------------------------------------------------------------------
//  Generate random variables (r.v.).
// -----------------------------------------------------------------------------
float uniform(						// r.v. from Uniform(min, max)
	float min,							// min value
	float max)							// max value
{
	if (min > max) error("uniform() parameters error.\n", true);

	float x = min + (max - min) * (float) rand() / (float) RAND_MAX;
	if (x < min || x > max) error("uniform() func error.\n", true);

	return x;
}

// -----------------------------------------------------------------------------
//  Use Box-Muller transform to generate a r.v. from Gaussian(mean, sigma)
//
//  Standard Gaussian distribution is Gaussian(0, 1), where mean = 0 and 
//  sigma = 1.
// -----------------------------------------------------------------------------
float gaussian(						// r.v. from Gaussian(mean, sigma)
	float mu,							// mean (location)
	float sigma)						// stanard deviation (scale > 0)
{
	if (sigma <= 0.0f) error("gaussian() parameters error.\n", true);

	float u1, u2;
	do {
		u1 = uniform(0.0f, 1.0f);
	} while (u1 < FLOATZERO);
	u2 = uniform(0.0f, 1.0f);
	
	float x = mu + sigma * sqrt(-2.0f * log(u1)) * cos(2.0f * PI * u2);
	//float x = mu + sigma * sqrt(-2.0f * log(u1)) * sin(2.0f * PI * u2);
	return x;
}


// -----------------------------------------------------------------------------
//  Read the original dataset (or query set) from disk
// -----------------------------------------------------------------------------
int read_set(						// read (data or query) set from disk
	int n,								// number of data points
	int d,								// dimensionality
	char* set,							// address of dataset
	float** points)						// data or queries (return)
{
	int i = 0;
	int j = 0;
	FILE* fp = NULL;

	fp = fopen(set, "r");			// open data file
	if (!fp) {
		printf("I could not open %s.\n", set);
		return 1;
	}

	i = 0;
	while (!feof(fp) && i < n) {	// read data file
		fscanf(fp, "%d", &j);
		for (j = 0; j < d; j++) {
			fscanf(fp, " %f", &points[i][j]);
		}
		fscanf(fp, "\n");

		i++;
	}
	if (!feof(fp) && i == n) {		// check the size of set
		error("The size of set is larger than you input\n", false);
	}
	else if (feof(fp) && i < n) {
		printf("Set the size of dataset to be %d. ", i);
		error("And try again\n", true);
	}
	fclose(fp);						// close data file
	return 0;
}

// -----------------------------------------------------------------------------
//  Write the data set in new format to the disk
// -----------------------------------------------------------------------------
int write_data_new_form(			// write dataset with new format
	int n,								// cardinality
	int d,								// dimensionality
	int B,								// page size
	float** data,						// data set
	char* output_path)					// output path
{
									// number of data in one data file
	int num = (int) floor((float) B / (d * SIZEFLOAT));
									// total number of data file
	int total_file = (int) ceil((float) n / num);
	if (total_file == 0) {
		return 1;					// fail to return
	}

	// -------------------------------------------------------------------------
	//  Check whether the directory exists. If the directory does not exist, we 
	//  create the directory for each folder.
	// -------------------------------------------------------------------------
	char* data_path = new char[200];

	strcpy(data_path, output_path);
	strcat(data_path, "data/");
	create_dir(data_path);

	// -------------------------------------------------------------------------
	//  Write new format of data
	// -------------------------------------------------------------------------
	char* fname = new char[200];
	char* buffer = new char[B];		// allocate one page size
	for (int i = 0; i < B; i++) {
		buffer[i] = 0;
	}

	int left  = 0;
	int right = 0;
	for (int i = 0; i < total_file; i++) {
									// get file name of data
		get_data_filename(i, data_path, fname);

		left = i * num;
		right = left + num;
		if (right > n) right = n;	// write data to buffer
		write_data_to_buffer(d, left, right, data, buffer);

									// write one page of data to disk
		if (write_buffer_to_page(B, fname, buffer) == 1) {
			error("write_data_new_form error to write a page", true);
		}
	}

	// -------------------------------------------------------------------------
	//  Release space
	// -------------------------------------------------------------------------
	if (buffer != NULL) {
		delete[] buffer; buffer = NULL;
	}
	if (data_path != NULL || fname != NULL) {
		delete[] data_path; data_path = NULL;
		delete[] fname; fname = NULL;
	}
	return 0;
}

// -----------------------------------------------------------------------------
void get_data_filename(				// get file name of data
	int file_id,						// data file id
	char* data_path,					// path to store data in new format
	char* fname)						// file name of data (return)
{
	char c[20];

	strcpy(fname, data_path);
	sprintf(c, "%d", file_id);
	strcat(fname, c);
	strcat(fname, ".data");
}

// -----------------------------------------------------------------------------
void write_data_to_buffer(			// write data to buffer
	int d,								// dimensionality
	int left,							// left data id
	int right,							// right data id
	float** data,						// data set
	char* buffer)						// buffer to store data
{
	int c = 0;
	for (int i = left; i < right; i++) {
		for (int j = 0; j < d; j++) {
			memcpy(&buffer[c], &data[i][j], SIZEFLOAT);
			c += SIZEFLOAT;
		}
	}
}

// -----------------------------------------------------------------------------
int write_buffer_to_page(			// write buffer to one page
	int B,								// page size
	char* fname,						// file name of data
	char* buffer)						// buffer to store data
{
	if (fname == NULL || buffer == NULL) return 1;

	FILE* fp = NULL;
	fp = fopen(fname, "wb");		// open data file to write
	if (!fp) {
		printf("I could not create %s.\n", fname);
		return 1;					// fail to return
	}

	fwrite(buffer, B, 1, fp);
	fclose(fp);
	return 0;
}

// -----------------------------------------------------------------------------
//  Read data in new format from disk
// -----------------------------------------------------------------------------
int read_data(						// read data from page
	int id,								// index of data
	int d,								// dimensionality
	int B,								// page size
	float* data,						// real data (return)
	char* output_path)					// output path
{
	// -------------------------------------------------------------------------
	//  Get file name of data
	// -------------------------------------------------------------------------
	char* fname     = new char[200];
	char* data_path = new char[200];

	strcpy(data_path, output_path);
	strcat(data_path, "data/");
									// number of data in one data file
	int num = (int) floor((float) B / (d * SIZEFLOAT));
									// data file id
	int file_id = (int) floor((float) id / num);

	get_data_filename(file_id, data_path, fname);

	// -------------------------------------------------------------------------
	//  Read buffer (one page of data) in new format from disk
	// -------------------------------------------------------------------------
	char* buffer = new char[B];		// allocate one page size
	for (int i = 0; i < B; i++) {
		buffer[i] = 0;
	}
	if (read_buffer_from_page(B, fname, buffer) == 1) {
		error("read_data() error to read a page", true);
	}

	// -------------------------------------------------------------------------
	//  Read data from buffer
	// -------------------------------------------------------------------------
	int index = id % num;
	read_data_from_buffer(index, d, data, buffer);

	if (buffer != NULL) {
		delete[] buffer; buffer = NULL;
	}
	if (data_path != NULL || fname != NULL) {
		delete[] data_path; data_path = NULL;
		delete[] fname; fname = NULL;
	}
	return 0;
}

// -----------------------------------------------------------------------------
int read_buffer_from_page(			// read buffer from page
	int B,								// page size
	char* fname,						// file name of data
	char* buffer)						// buffer to store data
{
	if (fname == NULL || buffer == NULL) return 1;

	FILE* fp = NULL;
	fp = fopen(fname, "rb");
	if (!fp) {
		printf("read_buffer_from_page could not open %s.\n", fname);
		return 1;					// fail to return
	}

	fread(buffer, B, 1, fp);
	fclose(fp);
	return 0;
}

// -----------------------------------------------------------------------------
void read_data_from_buffer(			// read data from buffer
	int index,							// index of data in buffer
	int d,								// dimensionality
	float* data,						// data set
	char* buffer)						// buffer to store data
{
	int c = index * d * SIZEFLOAT;
	for (int i = 0; i < d; i++) {
		memcpy(&data[i], &buffer[c], SIZEFLOAT);
		c += SIZEFLOAT;
	}
}


