#include "headers.h"

// -----------------------------------------------------------------------------
void usage() 						// display the usage of QDAFN
{
	printf("\n"
		"--------------------------------------------------------------------\n"
		" Usage of the Package of QDAFN for External c-k-AFN Search:         \n"
		"--------------------------------------------------------------------\n"
		"    -alg  (integer)   options of algorithms (0 - 2)\n"
		"    -n    (integer)   cardinality of the dataset\n"
		"    -qn   (integer)   number of queries\n"
		"    -d    (integer)   dimensionality of the dataset\n"
		"    -B    (integer)   page size\n"
		"    -l    (integer)   number of projections\n"
		"    -m    (integer)   number of candidates\n"
		"    -c    (real)      approximation ratio (c > 1)\n"
		"    -ds   (string)    file path of the dataset\n"
		"    -qs   (string)    file path of the query set\n"
		"    -ts   (string)    file path of the ground truth set\n"
		"    -df   (string)    data folder to store new format of dataset\n"
		"    -of   (string)    output folder to store output results\n"
		"\n"
		"--------------------------------------------------------------------\n"
		" The Options of Algorithms (-alg) are:                              \n"
		"--------------------------------------------------------------------\n"
		"    0 - Ground-Truth\n"
		"        Parameters: -alg 0 -n -qn -d -ds -qs -ts\n"
		"\n"
		"    1 - Indexing\n"
		"        Parameters: -alg 1 -n -d -B -l -m -c -ds -df -of\n\n"
		"\n"
		"    2 - QDAFN\n"
		"        Parameters: -alg 2 -qn -d -qs -ts -df -of\n"
		"\n"
		"--------------------------------------------------------------------\n"
		" Adapted Author: Qiang HUANG  (huangq2011@gmail.com)                \n"
		"--------------------------------------------------------------------\n"
		"\n\n\n");
}

// -----------------------------------------------------------------------------
int main(int nargs, char **args)
{
	srand((unsigned) time(NULL));	// set the random seed
	//usage();

	int   alg   = -1;				// option of algorithm
	int   n     = -1;				// cardinality
	int   qn    = -1;				// query number
	int   d     = -1;				// dimensionality
	int   B     = -1;				// page size
	int   l     = -1;				// number of projections
	int   m     = -1;				// number of candidates
	float ratio = -1.0f;			// approximation ratio

	char  data_set[200];			// address of data set
	char  query_set[200];			// address of query set
	char  truth_set[200];			// address of ground truth file
	char  data_folder[200];			// data folder
	char  output_folder[200];		// output folder

	bool  failed = false;
	int   cnt = 1;
	
	while (cnt < nargs && !failed) {
		if (strcmp(args[cnt], "-alg") == 0) {
			alg = atoi(args[++cnt]);
			printf("alg           = %d\n", alg);
			if (alg < 0 || alg > 2) {
				failed = true;
				break;
			}
		}
		else if (strcmp(args[cnt], "-n") == 0) {
			n = atoi(args[++cnt]);
			printf("n             = %d\n", n);
			if (n <= 0) {
				failed = true;
				break;
			}
		}
		else if (strcmp(args[cnt], "-d") == 0) {
			d = atoi(args[++cnt]);
			printf("d             = %d\n", d);
			if (d <= 0) {
				failed = true;
				break;
			}
		}
		else if (strcmp(args[cnt], "-qn") == 0) {
			qn = atoi(args[++cnt]);
			printf("qn            = %d\n", qn);
			if (qn <= 0) {
				failed = true;
				break;
			}
		}
		else if (strcmp(args[cnt], "-B") == 0) {
			B = atoi(args[++cnt]);
			printf("B             = %d\n", B);
			if (B <= 0) {
				failed = true;
				break;
			}
		}
		else if (strcmp(args[cnt], "-l") == 0) {
			l = atoi(args[++cnt]);
			printf("l             = %d\n", l);
			if (l < 0) {
				failed = true;
				break;
			}
		}
		else if (strcmp(args[cnt], "-m") == 0) {
			m = atoi(args[++cnt]);
			printf("m             = %d\n", m);
			if (m < 0) {
				failed = true;
				break;
			}
		}
		else if (strcmp(args[cnt], "-c") == 0) {
			ratio = (float) atof(args[++cnt]);
			printf("c             = %.1f\n", ratio);
			if (ratio <= 1.0f) {
				failed = true;
				break;
			}
		}
		else if (strcmp(args[cnt], "-ds") == 0) {
			strncpy(data_set, args[++cnt], sizeof(data_set));
			printf("data_set      = %s\n", data_set);
		}
		else if (strcmp(args[cnt], "-qs") == 0) {
			strncpy(query_set, args[++cnt], sizeof(query_set));
			printf("query_set     = %s\n", query_set);
		}
		else if (strcmp(args[cnt], "-ts") == 0) {
			strncpy(truth_set, args[++cnt], sizeof(truth_set));
			printf("truth_set     = %s\n", truth_set);
		}
		else if (strcmp(args[cnt], "-df") == 0) {
			strncpy(data_folder, args[++cnt], sizeof(data_folder));
			printf("data_folder   = %s\n", data_folder);

			int len = (int) strlen(data_folder);
			if (data_folder[len - 1] != '/') {
				data_folder[len] = '/';
				data_folder[len + 1] = '\0';
			}
			create_dir(data_folder);
		}
		else if (strcmp(args[cnt], "-of") == 0) {
			strncpy(output_folder, args[++cnt], sizeof(output_folder));
			printf("output_folder = %s\n", output_folder);

			int len = (int) strlen(output_folder);
			if (output_folder[len - 1] != '/') {
				output_folder[len] = '/';
				output_folder[len + 1] = '\0';
			}
			create_dir(output_folder);
		}
		else {
			failed = true;
			usage();
			break;
		}
		cnt++;
	}
	printf("\n");

	switch (alg) {
	case 0:
		ground_truth(n, qn, d, data_set, query_set, truth_set);
		break;
	case 1:
		indexing(n, d, B, l ,m, ratio, data_set, data_folder, output_folder);
		break;
	case 2:
		qdafn(qn, d, query_set, truth_set, data_folder, output_folder);
		break;
	default:
		printf("Parameters error!\n");
		usage();
		break;
	}

	return 0;
}


