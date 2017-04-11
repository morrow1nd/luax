#include <stdio.h>
#include <stdlib.h>

#include "luax.h"

#include "standard_lib/lio.h"

void usage(char* argv[])
{
    printf("usage: %s [option] [script]\n"
        "Available options are:\n"
        "  -o path    specify output file path\n"
        "  -c         complie luax code to opcode\n"
        "  -r         run opcode\n"
        //"  -e stat    execute string 'stat'\n"
        //"  -i         enter interaction mode after executing 'script'\n"
        //"  -          execute stdin and stop handling options\n"
        //"  -v         show version information\n"
        "  -h --help  show help info\n"
        "  --version  version info\n"
        "\n"
        "  https://github.com/morrow1nd/luax\n"
        , argv[0]
    );
}

// remember to lx_free this pointer
char * _read_file(const char* filepath, int* file_len) {
    FILE * fp = NULL;
    fp = fopen(filepath, "r");
    if (!fp) {
        printf("Error: can't open file:%s\n", filepath);
        return NULL;
    }
    int ret = -1;
    fseek(fp, 0, SEEK_END);
    int filelength = ftell(fp);
    printf("filelength:%d\n", filelength);

    char* data = (char*)lx_malloc(filelength + 1);
    fseek(fp, 0, SEEK_SET);
    if ((ret = fread(data, 1, filelength, fp)) <= 0) {
        printf("Error: can't read file:%s\n", filepath);
        fclose(fp);
        return NULL;
    }
    *(data + ret) = '\0';
    fclose(fp);
    *file_len = ret;
    return data;
}

enum luax_exec_run_mode {
    M_NORMAL = 0, // parse and run luax source code
    M_COMPLIE,
    M_RUN_OPCODE,
};

int main(int argc, char * argv[])
{
    int mode = M_NORMAL;
    const char * output_name = "a.luaxo";
    char * script_files[1024];
    int script_file_number = 0;

    // process arguments
    for (int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "-c") == 0)
            mode = M_COMPLIE;
        else if(strcmp(argv[i], "-r") == 0)
            mode = M_RUN_OPCODE;
        else if (strcmp(argv[i], "-o") == 0) {
            output_name = argv[i + 1];
            ++i;
        } else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
            usage(argv);
            exit(0);
        } else if(strcmp(argv[i], "--version") == 0){
            version();
            exit(0);
        } else {
            script_files[script_file_number++] = argv[i];
        }
    }
#if LX_DEBUG
    printf("mode: %d\n", mode);
    printf("output_path: %s\n", output_name);
    printf("script_file_number: %d\n", script_file_number);
    for (int i = 0; i < script_file_number; ++i) {
        printf("    %d: %s\n", i + 1, script_files[i]);
    }
#endif

    switch (mode) {
    case M_NORMAL: {
        if (script_file_number == 0) {
            usage(argv);
            break;
        }
        for (int i = 0; i < script_file_number; ++i) {
            int file_len = 0;
            char * data = _read_file(script_files[i], &file_len);
            if (data == NULL)
                continue;
            lx_parser * p = lx_genBytecode(data, file_len);
            lx_free(data); // lx_genBytecode has copied data
            if (p == NULL) {
                printf("Error: syntax error\n");
                continue;
            }
            
            lx_vm* vm = lx_create_vm();
            // we use base_env_table now, it doesn't load any Standard lib.
            lx_object_function* func_obj = lx_create_object_function_ops(p->opcodes, /* env_creator */ lx_create_env_table_with_inside_function());

            int ret = lx_vm_run(vm, func_obj);

            lx_delete_parser(p);
            lx_delete_vm(vm);
        }
        break;
    }
    case M_COMPLIE: {
        for (int i = 0; i < script_file_number; ++i) {
            int file_len = 0;
            char * data = _read_file(script_files[i], &file_len);
            if(data == NULL)
                continue;

            lx_parser * p = lx_genBytecode(data, file_len);
            lx_free(data); // lx_genBytecode has copied data
            if (p == NULL) {
                printf("Error: syntax error\n");
                continue;
            }

            FILE* output_fp = NULL;
            if (i > 0) {
                output_fp = fopen(output_name, "a");
            } else {
                output_fp = fopen(output_name, "w");
            }
            if (output_fp == NULL) {
                printf("Error: can't open output file: %s\n", output_name);
                return -1;
            }
            fprintf(output_fp, "; opcode generated from: %s\n", script_files[i]);
            lx_helper_dump_opcode(p->opcodes, output_fp);
            fclose(output_fp);

            lx_delete_parser(p);
        }
        break;
    }
    case M_RUN_OPCODE: {
        // opcode parser has not been written

        break;
    }
    default: {
        usage(argv);
        break;
    }
    }

    return 0;
}


//int main(int argc, char * argv[])
//{
//    char * const filepath = TEST_BINARY_DIR ".vm_run_test.luax";
//    FILE * fp = NULL;
//    fp = fopen(filepath, "r");
//    if (!fp) {
//        printf("Error: can't open file:%s\n", filepath);
//        return -1;
//    }
//    int ret = -1;
//    fseek(fp, 0, SEEK_END);
//    int filelength = ftell(fp);
//    printf("filelength:%d\n", filelength);
//
//    char* data = (char*)lx_malloc(filelength + 1);
//    fseek(fp, 0, SEEK_SET);
//    if ((ret = fread(data, 1, filelength, fp)) <= 0) {
//        printf("Error: can't read file:%s\n", filepath);
//        return -1;
//    }
//    *(data + ret) = '\0';
//    fclose(fp);
//
//    lx_parser * p = lx_genBytecode(data, ret);
//
//    lx_helper_dump_bytecode(p->opcodes);
//
//
//    lx_vm* vm = lx_create_vm();
//    lx_object_function* func_obj = LX_NEW(lx_object_function);
//    func_obj->base.type = LX_OBJECT_FUNCTION;
//    func_obj->func_opcodes = p->opcodes;
//    func_obj->_E = lx_create_object_table();
//    lx_object_table_replace_s(func_obj->_E, "io", -1, luax_lio_load());
//    func_obj->_G = lx_create_object_table();
//
//    lx_vm_run(vm, func_obj);
//    lx_dump_vm_stack(vm);
//
//    lx_free(data);
//    if (p != NULL) {
//        printf("success\n");
//    }
//    lx_delete_parser(p);
//
//    printf("----- program end -------\n");
//
//#ifdef _WIN32
//    system("pause");
//#endif
//    return 0;
//}


//#include <string.h>  /* strcpy */
//#include <stdlib.h>  /* malloc */
//#include <stdio.h>   /* printf */
//#include "hash/src/uthash.h"
//
//struct my_struct {
//    const char *name;          /* key */
//    int id;
//    UT_hash_handle hh;         /* makes this structure hashable */
//};
//
//
//int __main(int argc, char *argv[]) {
//    const char **n, *names[] = { "joe", "bob", "betty", NULL };
//    struct my_struct *s, *tmp, *users = NULL;
//    int i = 0;
//
//    for (n = names; *n != NULL; n++) {
//        s = (struct my_struct*)malloc(sizeof(struct my_struct));
//        s->name = *n;
//        s->id = i++;
//        HASH_ADD_KEYPTR(hh, users, s->name, strlen(s->name), s);
//    }
//
//    char tem[1024];
//    strcpy(tem, "betty");
//    HASH_FIND_STR(users, tem, s);
//    if (s)
//        printf("betty's id is %d\n", s->id);
//    else
//    {
//        printf("not found\n");
//    }
//
//    /* free the hash table contents */
//    HASH_ITER(hh, users, s, tmp) {
//        HASH_DEL(users, s);
//        free(s);
//    }
//    system("pause");
//    return 0;
//}