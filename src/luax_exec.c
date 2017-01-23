#include <stdio.h>

#include "luax.h"

#include "standard_lib/lio.h"

void usage(char* argv[])
{
    printf("usage: %s [option] [script]\n"
        "Available options are:\n"
        "  -e stat    execute string 'stat'\n"
        "  -i         enter interaction mode after executing 'script'\n"
        "  -v         show version information\n"
        "  -          execute stdin and stop handling options\n"
        "  -h         show help info\n"
        "\n"
        , argv[0]
    );
}

#include <stdlib.h>


int main(int argc, char * argv[])
{
    char * const filepath = TEST_BINARY_DIR ".vm_run_test.luax";
    FILE * fp = NULL;
    fp = fopen(filepath, "r");
    if (!fp) {
        printf("Error: can't open file:%s\n", filepath);
        return -1;
    }
    int ret = -1;
    fseek(fp, 0, SEEK_END);
    int filelength = ftell(fp);
    printf("filelength:%d\n", filelength);

    char* data = (char*)lx_malloc(filelength + 1);
    fseek(fp, 0, SEEK_SET);
    if ((ret = fread(data, 1, filelength, fp)) <= 0) {
        printf("Error: can't read file:%s\n", filepath);
        return -1;
    }
    *(data + ret) = '\0';
    fclose(fp);

    lx_parser * p = lx_genBytecode(data, ret);

    lx_helper_dump_bytecode(p->opcodes);


    lx_vm* vm = lx_create_vm();
    lx_object_function* func_obj = LX_NEW(lx_object_function);
    func_obj->base.type = LX_OBJECT_FUNCTION;
    func_obj->func_opcodes = p->opcodes;
    func_obj->_G = lx_create_object_table();
    lx_object_table_replace_s(func_obj->_G, "io", -1, luax_lio_load());

    lx_vm_run(vm, func_obj);
    lx_dump_vm_stack(vm);

    lx_free(data);
    if (p != NULL) {
        printf("success\n");
    }
    lx_delete_parser(p);

    printf("----- program end -------\n");

#ifdef _WIN32
    system("pause");
#endif
    return 0;
}


#include <string.h>  /* strcpy */
#include <stdlib.h>  /* malloc */
#include <stdio.h>   /* printf */
#include "hash/src/uthash.h"

struct my_struct {
    const char *name;          /* key */
    int id;
    UT_hash_handle hh;         /* makes this structure hashable */
};


int __main(int argc, char *argv[]) {
    const char **n, *names[] = { "joe", "bob", "betty", NULL };
    struct my_struct *s, *tmp, *users = NULL;
    int i = 0;

    for (n = names; *n != NULL; n++) {
        s = (struct my_struct*)malloc(sizeof(struct my_struct));
        s->name = *n;
        s->id = i++;
        HASH_ADD_KEYPTR(hh, users, s->name, strlen(s->name), s);
    }

    char tem[1024];
    strcpy(tem, "betty");
    HASH_FIND_STR(users, tem, s);
    if (s)
        printf("betty's id is %d\n", s->id);
    else
    {
        printf("not found\n");
    }

    /* free the hash table contents */
    HASH_ITER(hh, users, s, tmp) {
        HASH_DEL(users, s);
        free(s);
    }
    system("pause");
    return 0;
}