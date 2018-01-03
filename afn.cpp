#include "headers.h"

// -----------------------------------------------------------------------------
int ground_truth(					// output the ground truth results
	int   n,							// number of data points
	int   qn,							// number of query points
	int   d,							// dimension of space
	char* data_set,						// address of data set
	char* query_set,					// address of query set
	char* truth_set)					// address of ground truth file
{
	clock_t start_time = (clock_t) -1;
	clock_t end_time   = (clock_t) -1;

	int i, j;
	FILE* fp = NULL;
	
	int maxk = MAXK;
	MaxK_List* list = new MaxK_List(MAXK);

	// -------------------------------------------------------------------------
	//  Read data set and query set
	// -------------------------------------------------------------------------
	start_time = clock();
	float** data = new float*[n];
	for (i = 0; i < n; i++) data[i] = new float[d];
	if (read_set(n, d, data_set, data)) {
		error("Reading Dataset Error!\n", true);
	}

	float** query = new float*[qn];
	for (i = 0; i < qn; i++) query[i] = new float[d];
	if (read_set(qn, d, query_set, query) == 1) {
		error("Reading Query Set Error!\n", true);
	}
	end_time = clock();
	printf("Read Dataset and Query Set: %.6f Seconds\n\n", 
		((float) end_time - start_time) / CLOCKS_PER_SEC);

	// -------------------------------------------------------------------------
	//  output ground truth results (using linear scan method)
	// -------------------------------------------------------------------------
	start_time = clock();
	fp = fopen(truth_set, "w");		// open output file
	if (!fp) {
		printf("Could not create %s.\n", truth_set);
		return 1;
	}

	fprintf(fp, "%d %d\n", qn, maxk);
	for (i = 0; i < qn; i++) {
		list->reset();
		for (j = 0; j < n; j++) {	// find k-fn points of query
			float dist = calc_l2_dist(data[j], query[i], d);
			int id = j + 1;
			list->insert(dist, id);
		}

		fprintf(fp, "%d", i + 1);	// output Lp dist of k-fn points
		for (j = 0; j < maxk; j++) {
			fprintf(fp, " %f", list->ith_largest_key(j));
		}
		for (j = 0; j < maxk; j++) {
			fprintf(fp, " %d", list->ith_largest_id(j));
		}
		fprintf(fp, "\n");
	}
	fclose(fp);						// close output file
	end_time = clock();
	printf("Generate Ground Truth: %.6f Seconds\n\n", 
		((float) end_time - start_time) / CLOCKS_PER_SEC);

	// -------------------------------------------------------------------------
	//  Release space
	// -------------------------------------------------------------------------
	for (i = 0; i < n; i++) {
		delete[] data[i]; data[i] = NULL;
	}
	delete[] data; data = NULL;

	for (i = 0; i < qn; i++) {
		delete[] query[i]; query[i] = NULL;
	}
	delete[] query; query = NULL;
	delete list; list = NULL;

	return 0;
}

// -----------------------------------------------------------------------------
int indexing(						// build hash tables for the dataset
	int   n,							// number of data points
	int   d,							// dimension of space
	int   B,							// page size
	int   l,							// number of projections
	int   m,							// number of candidates
	float ratio,						// approximation ratio
	char* data_set,						// address of data set
	char* data_folder,					// folder to store new format of data
	char* output_folder)				// folder to store info of qdafn
{
	clock_t start_time = (clock_t) -1;
	clock_t end_time   = (clock_t) -1;

	// -------------------------------------------------------------------------
	//  Read data set
	// -------------------------------------------------------------------------
	start_time = clock();
	float** data = new float*[n];
	for (int i = 0; i < n; i++) data[i] = new float[d];
	if (read_set(n, d, data_set, data) == 1) {
		error("Reading Dataset Error!\n", true);
	}
	end_time = clock();
	printf("Read Dataset: %.6f Seconds\n\n", 
		((float) end_time - start_time) / CLOCKS_PER_SEC);

	// -------------------------------------------------------------------------
	//  Write the data set in new format to disk
	// -------------------------------------------------------------------------
	start_time = clock();
	write_data_new_form(n, d, B, data, data_folder);
	end_time = clock();
	printf("Write Dataset in New Format: %.6f Seconds\n\n", 
		((float) end_time - start_time) / CLOCKS_PER_SEC);

	// -------------------------------------------------------------------------
	//  Indexing
	// -------------------------------------------------------------------------
	char fname[200];
	strcpy(fname, output_folder);
	strcat(fname, "index.out");

	FILE* fp = fopen(fname, "w");
	if (!fp) {
		printf("Could not create %s.\n", fname);
		return 1;					// fail to return
	}

	start_time = clock();
	Furthest_Index* fi = new Furthest_Index();
	fi->build(n, d, B, l, m, ratio, output_folder, data);
	end_time = clock();

	float indexing_time = ((float) end_time - start_time) / CLOCKS_PER_SEC;
	printf("\nIndexing Time: %.6f seconds\n\n", indexing_time);
	fprintf(fp, "Indexing Time: %.6f seconds\n", indexing_time);
	fclose(fp);

	// -------------------------------------------------------------------------
	//  Release space
	// -------------------------------------------------------------------------
	for (int i = 0; i < n; i++) {
		delete[] data[i]; data[i] = NULL;
	}
	delete[] data; data = NULL;
	delete fi; fi = NULL;

	return 0;
}

// -----------------------------------------------------------------------------
int qdafn(							// c-k-AFN via qdafn (data in disk)
	int   qn,							// number of query points
	int   d,							// dimensionality
	char* query_set,					// path of query set
	char* truth_set,					// groundtrue file
	char* data_folder,					// folder to store new format of data
	char* output_folder)				// output folder
{
	clock_t start_time = (clock_t) -1;
	clock_t end_time   = (clock_t) -1;

	int maxk = MAXK;
	int i, j;
	FILE* fp = NULL;				// file pointer

	// -------------------------------------------------------------------------
	//  Read query set
	// -------------------------------------------------------------------------
	float** query = new float*[qn];
	for (i = 0; i < qn; i++) {
		query[i] = new float[d];
	}

	if (read_set(qn, d, query_set, query)) {
		error("Reading Query Set Error!\n", true);
	}

	// -------------------------------------------------------------------------
	//  Read the ground truth file
	// -------------------------------------------------------------------------
	float** R = new float*[qn];
	int**   ID = new int*[qn];
	for (i = 0; i < qn; i++) {
		R[i]  = new float[maxk];
		ID[i] = new int[maxk];
		for (j = 0; j < maxk; j++) {
			R[i][j]  = -1.0;
			ID[i][j] = -1;
		}
	}

	fp = fopen(truth_set, "r");		// open ground truth file
	if (!fp) {
		printf("Could not open %s.\n", truth_set);
		return 1;
	}

	fscanf(fp, "%d %d\n", &qn, &maxk);
	for (int i = 0; i < qn; i++) {
		fscanf(fp, "%d", &j);
		for (j = 0; j < maxk; j++) {
			fscanf(fp, "%f", &R[i][j]);
		}
		for (j = 0; j < maxk; j++) {
			fscanf(fp, "%d", &ID[i][j]);
		}
	}
	fclose(fp);						// close groundtrue file

	// -------------------------------------------------------------------------
	//  c-k-Approximate Furthest Neighbor (c-k-AFN) search via QDAFN
	// -------------------------------------------------------------------------
	int kFNs[] = {1, 2, 5, 10};
	int max_round = 4;
	int top_k = 0;

	float all_time    = -1.0f;		// average running time
	float all_ratio   = -1.0f;		// average ratio
	float this_ratio  = -1.0f;

	float all_recall  = -1.0f;		// average recall
	float this_recall = -1.0f;
	int   this_io = 0;				// average page IOs
	int   all_io  = 0;

									// load index of QDAFN
	Furthest_Index* fi = new Furthest_Index();
	fi->load(output_folder);

	char output_set[200];
	strcpy(output_set, output_folder);
	strcat(output_set, "qdafn.out");

	fp = fopen(output_set, "w");	// open output file
	if (!fp) {
		printf("Could not create the output file.\n");
		return 1;
	}

	printf("QDAFN for c-k-AFN Search: \n");
	printf("    Top-k\tRatio\t\tI/O\t\tTime (ms)\tRecall\n");
	for (int num = 0; num < max_round; num++) {
		start_time = clock();
		top_k = kFNs[num];
		MaxK_List* list = new MaxK_List(top_k);

		all_ratio  = 0.0f;
		all_recall = 0.0f;
		all_io     = 0;
		for (i = 0; i < qn; i++) {
			list->reset();
			this_io = fi->search(top_k, query[i], data_folder, list);
			all_io += this_io;

			this_ratio = 0.0f;
			for (j = 0; j < top_k; j++) {
				this_ratio += (R[i][j] / list->ith_largest_key(j));
			}
			this_ratio /= top_k;
			all_ratio += this_ratio;

			this_recall = calc_recall(top_k, list, ID[i]);
			all_recall += this_recall;
			/*
			printf("        No = %2d, Top-k = %d, IO = %4d, Ratio = %0.4f, "
				"Recall = %.2f%%\n", i + 1, top_k, this_io, this_ratio, 
				this_recall * 100.0f);
			*/
		}
		delete list; list = NULL;
		end_time = clock();
		all_time = ((float) end_time - start_time) / CLOCKS_PER_SEC;

		all_ratio  = all_ratio / qn;
		all_time   = (all_time * 1000.0f) / qn;
		all_recall = (all_recall * 100.0f) / qn;
		all_io     = (int) ceil((float) all_io / (float) qn);

		printf("    %3d\t\t%.4f\t\t%d\t\t%.2f\t\t%.2f%%\n", 
			top_k, all_ratio, all_io, all_time, all_recall);
		fprintf(fp, "%d\t%f\t%d\t%f\t%f\n", 
			top_k, all_ratio, all_io, all_time, all_recall);
	}
	printf("\n");
	fclose(fp);						// close output file

	// -------------------------------------------------------------------------
	//  Release space
	// -------------------------------------------------------------------------
	for (i = 0; i < qn; i++) {
		delete[] query[i]; query[i] = NULL;
	}
	delete[] query; query = NULL;
	delete fi; fi = NULL;

	for (i = 0; i < qn; i++) {
		delete[] R[i]; R[i] = NULL;
		delete[] ID[i]; ID[i] = NULL;
	}
	delete[] R; R = NULL;
	delete[] ID; ID = NULL;

	return 0;
}
