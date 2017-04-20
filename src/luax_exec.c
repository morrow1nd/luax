#include <stdio.h>
#include <stdlib.h>

#include "luax.h"

#include "standard_lib/lio.h"

void usage(char* argv[])
{
    printf("usage: %s [option] [script]\n"
        "Available options are:\n"
        "  -o path        specify output file path\n"
        "  -c             complie luax code to opcode\n"
        "  -r             run opcode\n"
        //"  -e stat    execute string 'stat'\n"
        //"  -i         enter interaction mode after executing 'script'\n"
        //"  -          execute stdin and stop handling options\n"
        "  --show_opcode  print opcode to standard output\n"
        "  -h --help      show help info\n"
        "  --version      version info\n"
        "\n"
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
    bool show_opcode = false;

    // process arguments
    for (int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "-c") == 0)
            mode = M_COMPLIE;
        else if(strcmp(argv[i], "-r") == 0)
            mode = M_RUN_OPCODE;
        else if (strcmp(argv[i], "-o") == 0) {
            output_name = argv[i + 1];
            ++i;
        } else if(strcmp(argv[i], "--show_opcode") == 0){
            show_opcode = true;
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
#if false
    printf("mode: %d\n", mode);
    printf("output_path: %s\n", output_name);
    printf("script_file_number: %d\n", script_file_number);
    for (int i = 0; i < script_file_number; ++i) {
        printf("\t%d: %s\n", i + 1, script_files[i]);
    }
#endif

    switch (mode) {
    case M_NORMAL: {
        if (script_file_number == 0) {
            usage(argv);
            break;
        }
        for (int i = 0; i < script_file_number; ++i) {
#if LX_MALLOC_STATISTICS
            lx_dump_memory_usage();
#endif
            int file_len = 0;
            char * data = _read_file(script_files[i], &file_len);
            if (data == NULL)
                continue;
            lx_parser * p = lx_gen_opcodes(data, file_len);
            lx_free(data); // lx_genBytecode has copied data
            if (p == NULL) {
                printf("Error: syntax error\n");
                continue;
            }
#if LX_VM_OPCODE_SHOW
            lx_helper_dump_opcode(p->opcodes, stdout);
#else
            if(show_opcode)
                lx_helper_dump_opcode(p->opcodes, stdout);
#endif

            lx_vm* vm = lx_create_vm();

#if LX_VM_DEBUG
            lx_dump_vm_gc_status(vm);
#endif

            // we use base_env_table now, it doesn't load any Standard lib.
            lx_object_function* func_obj = lx_create_function_ops(vm, p->opcodes, /* env_creator */ lx_create_env_table_with_inside_function(vm));

            lx_object* exception;
            int ret = lx_vm_run(vm, func_obj, &exception);
            if (exception) {
                lx_dump_object(exception, stderr);
                delete_object_by_type(exception);
            }
#if LX_VM_DEBUG
            lx_dump_vm_gc_status(vm);
#endif
            lx_delete_vm(vm);
            lx_delete_parser(p);
#if LX_MALLOC_STATISTICS
            lx_dump_memory_usage();
#endif
        }
        break;
    }
    case M_COMPLIE: {
        for (int i = 0; i < script_file_number; ++i) {
            int file_len = 0;
            char * data = _read_file(script_files[i], &file_len);
            if(data == NULL)
                continue;

            lx_parser * p = lx_gen_opcodes(data, file_len);
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
