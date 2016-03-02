/*  load configure file and check the apis */
//#include "cache.h"
//#include "variant_names.h"
#include "anno.h"
#include "config.h"
#include "kson.h"
//#include "anno_setter.h"
//#include <htslib/vcf.h>

struct configs anno_config_file = { .path_string=NULL, .summary=NULL, .anno=NULL, .n_apis=0};

/* annotate with HGVS* tags */
int has_hgvs = 0;

#define ignore_free(x) do                       \
    {                                           \
        if (x!= NULL) { \
	    free((void*)x);			\
	}\
	x = NULL;				\
    } while(0)

static void config_hgvs_release()
{
    if ( anno_config_file.anno == NULL ) return;
    ignore_free( anno_config_file.anno->refgene_file_path);
    ignore_free( anno_config_file.anno->transcripts_list);
    ignore_free( anno_config_file.anno->genes_list);
    ignore_free( anno_config_file.anno->columns);
    ignore_free( anno_config_file.anno);
}
static void config_summary_release(struct summary *summary)
{
    if ( summary != NULL ) {
        ignore_free(summary->name);
        ignore_free(summary->version);
        ignore_free(summary->author);
        ignore_free(summary->ref_version);
        ignore_free(summary->path);
    }
    ignore_free(summary);
}
static void config_api_release(struct vcf_sql_api *api)
{
    if ( api != NULL ) {
        ignore_free(api->vfile);
        ignore_free(api->dynlib);
        ignore_free(api->columns);
        if ( api->has_summary ) {
           config_summary_release(api->summary);
        }
    }
}
void config_release()
{
    ignore_free( anno_config_file.path_string);
    config_hgvs_release();
    config_summary_release( anno_config_file.summary);
    int i;
    for (i = 0; i< anno_config_file.n_apis; ++i)
        config_api_release(&anno_config_file.apis[i]);
    ignore_free(anno_config_file.apis);
}

#undef ignore_free

static char *skip_comments(const char *json_file)
{
    FILE *fp;
    fp = fopen(json_file, "rb");
    if ( fp == NULL ) {
        fprintf(stderr, "%s\n", strerror(errno));
        return NULL;
    }
    char *json;
    int temp, len=0, max=0;
    int i, j;
    char buf[LINE_CACHE];
    while ((temp=fread(buf, 1, LINE_CACHE, fp)) != 0 )
    {
        if ( len+temp+1>max ) {
            max=len+temp+1;
            kroundup32(max);
            json = (char*)realloc(json, max);
        }
        memcpy(json+len, buf, temp);
        len += temp;
    }
    fclose(fp);
    json[len]='\0';
    for ( i=0; i<len-1; ++i )
    {
        if ( json[i]=='/' && json[i+1]=='/' ) {
	    for (j=i+2; j<len && json[j]!='\n'; ++j);
	    for (;i<j; ++i) json[i] = '\0';
        }
    }

    for ( i=j=0; i<len; ++i,++j )
    {
	for (;json[i] == '\0' || json[i] == '\t'|| json[i] == ' '; i++);
	json[j] = json[i];
    }
    for ( i=j=0; i<len; ++i,++j )
    {
    	for (;json[i] == '\n' && i+1<len && json[i+1] == '\n'; i++);
	json[j] = json[i];
    }
    len = j+1;
    memset(json+len, 0, max-len);
    return json;

}

static enum api_type check_api_type (char *str)
{
    if (!strcmp(str, "vcf")) {
        return api_is_vcf;
    } else if (!strcmp(str, "sql")) {
        return api_is_sql;
    } else {
        return api_is_unknown;
    }
}


static int load_readers(const kson_t *s)
{
    assert(anno_config_file.summary == NULL);
    anno_config_file.summary = (struct summary*)malloc(sizeof(struct summary));
    anno_config_file.summary->author = NULL;
    anno_config_file.summary->name = NULL;
    anno_config_file.summary->version = NULL;
    anno_config_file.summary->ref_version = NULL;
    anno_config_file.summary->path = NULL;
    const kson_node_t *root = s->root;

    int i;

#define BRANCH(__node, __key, __hand, __func)	do {	\
	if ( __node ) {					\
	    if (!strcmp((__node)->key, __key)) {	\
		if ((__node)->v.str == NULL) {		\
		    __hand = 0;				\
		} else {				\
		    __hand = __func((__node)->v.str);	\
		}					\
	    }						\
	}						\
    } while(0)

    for (i = 0; i<root->n; ++i) {

	const kson_node_t *node = kson_by_index(root, i);
        if ( node == NULL) continue;
        struct summary * const summary = anno_config_file.summary;

        BRANCH(node, "ANNO_THREAD", anno_config_file.n_theads, atoi);
        BRANCH(node, "AUTHOR", summary->author, strdup);
        BRANCH(node, "ID", summary->name, strdup);
        BRANCH(node, "REF", summary->ref_version, strdup);

        if (!strcmp(node->key, "HGVS")) {
	    assert(has_hgvs == 0);
            has_hgvs = 1;
            anno_config_file.anno = (struct anno_data_file*)malloc(sizeof(struct anno_data_file));
            struct anno_data_file * const anno =  anno_config_file.anno;
            anno->refgene_file_path = NULL;
            anno->transcripts_list = NULL;
            anno->genes_list = NULL;
            anno->intron_edge = DEFAULT_INTRON_EDGE;
            anno->columns = NULL;

            int i;
            for (i = 0; i < node->n; ++i) {
                const kson_node_t *node1 = kson_by_index(node, i);
		if ( node1==NULL)
		    error("empty HGVS configure !!\n");
	
                BRANCH(node, "refgene", anno->refgene_file_path, strdup);
                BRANCH(node, "trans_file", anno->transcripts_list, strdup);
                BRANCH(node, "genes_file", anno->genes_list, strdup);
                BRANCH(node, "intron_edge", anno->intron_edge, atoi);
                BRANCH(node, "columns", anno->columns, strdup);
                //warnings("[%s] %s is not a pre-defined element. skip it ..", __FUNCTION__, node->key);
            }
            if (anno->refgene_file_path == NULL) {
                config_release();
                error("Cannot find refgene file in the HGVS configure!\n");
            }
            if (anno->columns == NULL) {
                warnings("Do you use a right configure file ?? No tags columns in HGVS configure, HGVS variants name will not be annotated. \n");
                config_hgvs_release();
            }
            //continue; // only accept one node
        }

        if (!strcmp(node->key, "API")) {
	    assert(node->type == KSON_TYPE_BRACKET);
            anno_config_file.apis = (struct vcf_sql_api*)calloc(node->n, sizeof(struct vcf_sql_api));
	    anno_config_file.n_apis = 0;
	    
            long i, j;
            for (i = 0; i < (long)node->n; ++i) {
		//const kson_node_t *node1 = kson_by_index(node, i);
		const kson_node_t *node1 = node->v.child[i];
                if ( node1==NULL) continue;
		struct vcf_sql_api * const api = &anno_config_file.apis[anno_config_file.n_apis++];
		for (j = 0; j < (long)node1->n; ++j) {
		    fprintf(stderr, "n: %llu\ti:%ld\n", node1->n, j);
		    const kson_node_t *node2 = kson_by_index(node1, j);
		    BRANCH(node2, "type", api->type, check_api_type);
		    BRANCH(node2, "file", api->vfile, strdup);
		    BRANCH(node2, "dynlib", api->dynlib, strdup);
		    BRANCH(node2, "columns", api->columns, strdup);
		    // summarys
		    api->has_summary = 0;
		    //warnings("[%s] %s is not a pre-defined element. skip it ..\n", __FUNCTION__, node2->key);
		}
            }
            //continue;
        }
        // filter fields

    }


#undef BRANCH

    // check all the api or file reachable

    return 0;    
}

static void debug_configure_summary(struct summary *summary)
{
    if ( summary->name != NULL )
        printf ("[summary] name : %s\n", summary->name);
    if ( summary->version != NULL)
        printf ("[summary] version: %s\n", summary->version);
    if ( summary->author != NULL)
        printf ("[summary] author: %s\n", summary->author);
    if ( summary->ref_version != NULL)
        printf ("[summary] ref version: %s\n", summary->ref_version);
}
void debug_configure_file()
{
    int i;
    
    if ( anno_config_file.path_string )
        printf("path string : %s\n", anno_config_file.path_string);
    if ( anno_config_file.summary )
	debug_configure_summary(anno_config_file.summary);
    if ( anno_config_file.anno ) {
	struct anno_data_file *anno = anno_config_file.anno;
	printf("[hgvs] intron edge: %u\n", anno->intron_edge);
	if ( anno->refgene_file_path )
	    printf("[hgvs] file path: %s\n", anno->refgene_file_path);
	if ( anno->transcripts_list )
	    printf("[hgvs] transcript list: %s\n", anno->transcripts_list);
	if ( anno->genes_list )
	    printf("[hgvs] genes list: %s\n", anno->genes_list);
	if ( anno->columns )
	    printf("[hgvs] columns: %s\n", anno->columns);
    }

    for (i=0; i<anno_config_file.n_apis; i++) {
	struct vcf_sql_api *api = &anno_config_file.apis[i];
	if (api->type == api_is_vcf && api->vfile) {
	    printf( "[api] api is VCF\n");
	    printf( "[api] %s\n", api->vfile);
	    printf( "[api] columns: %s\n", api->columns);
	} else if ( api->type == api_is_sql && api->dynlib) {
	    printf( "[api] api is dynlib\n");
	    printf( "[api] %s\n", api->dynlib);
	    printf( "[api] columns: %s\n", api->columns);
	} 
    }    
}

int load_config(const char *json_file)
{

    kson_t *kson = 0;
    const char *json = skip_comments(json_file);
    if (json==NULL) return 1;
    anno_config_file.path_string = strdup(json_file);
    fprintf(stderr, "[debug] %s\n", json);
    kson = kson_parse(json);
    safe_free(json);
    if ( !kson ) {
        fprintf(stderr, "[%s] can not parse %s\n", __FUNCTION__, json_file);
        exit(-1);
    }
    int n;
    n= load_readers(kson);
    if (n < 0) error("Failed to load file from %s\n", json_file);
    debug_configure_file();
    config_release();
    kson_destroy(kson);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        fprintf(stderr, "%s input.json\n", argv[0]);
        exit(-1);
    }
    load_config(argv[1]);
    return 0;
}
