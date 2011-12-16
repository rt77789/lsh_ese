
#include <flann/flann.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void
print_now() {
	time_t t = time(0); 
    char tmp[64]; 
    strftime(tmp, sizeof(tmp), "%Y/%m/%d %X %A",localtime(&t)); 
    puts(tmp);
}


float* read_points(const char* filename, int rows, int cols)
{
    int ret;
	float* data;
	float *p;
	FILE* fin;
	int i,j;

    fin = fopen(filename,"r");
    if (!fin) {
        printf("Cannot open input file.\n");
        exit(1);
    }
    
    data = (float*) malloc(rows*cols*sizeof(float));
    if (!data) {
        printf("Cannot allocate memory.\n");
        exit(1);
    }
    p = data;
    
    for (i=0;i<rows;++i) {
        for (j=0;j<cols;++j) {
            ret = fscanf(fin,"%g ",p);
            p++;
        }
    }
    
    fclose(fin);
    
    return data;
}

void write_results(const char* filename, int *data, int rows, int cols)
{
	FILE* fout;
	int* p;
	int i,j;

    fout = fopen(filename,"w");
    if (!fout) {
        printf("Cannot open output file.\n");
        exit(1);
    }
    
    p = data;
    for (i=0;i<rows;++i) {
        for (j=0;j<cols;++j) {
            fprintf(fout,"%d ",*p);
            p++;
        }
        fprintf(fout,"\n");
    }
    fclose(fout);
}



int main(int argc, char** argv)
{
	float* dataset;
	float* testset;
	int nn;
	int* result;
	float* dists;
	struct FLANNParameters p;
	float speedup;
	flann_index_t index_id;

    int rows = 100;//9000;
    int cols = 4096; //128;
    int tcount = 1; //1000;

	if(argc < 3) {
		perror("argc < 3");
		exit(0);
	}
    printf("Reading input data file.\n");
    //dataset = read_points("dataset.dat", rows, cols);
    dataset = read_points(argv[1], rows, cols);
    printf("Reading test data file.\n");
    //testset = read_points("testset.dat", tcount, cols);
    testset = read_points(argv[2], tcount, cols);
    
	/* nn is the number of query-result. */
    nn = 10;
    result = (int*) malloc(tcount*nn*sizeof(int));
    dists = (float*) malloc(tcount*nn*sizeof(float));
    
    p = DEFAULT_FLANN_PARAMETERS;
	//p.algorithm = FLANN_INDEX_AUTOTUNED ;
    //p.algorithm = FLANN_INDEX_KDTREE;
    //p.trees = 16;
	//p.leaf_max_size = 10;
    //p.log_level = FLANN_LOG_INFO;
	//p.checks = 64;
	//p.target_precision = 0.9;
    
    printf("Computing index.\n");
	//Logger::warn("Logger::info test.\n");
	print_now();
	/* build index process. */
    //index_id = flann_build_index(dataset, rows, cols, &speedup, &p);
	index_id = flann_load_index("file.index", dataset, rows, cols);
	print_now();

	//flann_save_index(index_id, "file.index");

	printf("find nearest neighbors index.\n");

    flann_find_nearest_neighbors_index(index_id, testset, tcount, result, dists, nn, &p);
    
	print_now();
    write_results("results.dat",result, tcount, nn);
    
	print_now();
	printf("free index.");
    flann_free_index(index_id, &p);
	print_now();

    free(dataset);
    free(testset);
    free(result);
    free(dists);
    
    return 0;
}
