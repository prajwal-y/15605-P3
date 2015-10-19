/**
 * The 15-410 kernel project.
 * @name loader.c
 *
 * Functions for the loading
 * of user programs from binary 
 * files should be written in
 * this file. The function 
 * elf_load_helper() is provided
 * for your use.
 */

/* --- Includes --- */
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <simics.h>
#include <malloc_internal.h>
#include <exec2obj.h>
#include <loader/loader.h>
#include <elf_410.h>
#include <vm/vm.h>

#define MAX_SECTION_NAME_LEN 10 /* longer than any we care about */
#define FAILURE -1

/** @brief load a program into memory
 *
 *  memcpy the program regions into memory. It is assumed that paging
 *  information has been setup so copying here should not PAGE FAULT
 *
 *  @pre paging is enabled and page table information has been setup
 *  @param se_hdr the struct containing ELF data about program
 *  @return void
 */
void load_program(simple_elf_t *se_hdr) {
    char *buf;

    /* Load text section into memory */
	if(se_hdr->e_txtlen > 0) {
    	buf = (char *)lmm_alloc(&malloc_lmm, se_hdr->e_txtlen, LMM_ANY_REGION_FLAG);
    	getbytes(se_hdr->e_fname, se_hdr->e_txtoff, se_hdr->e_txtlen, buf);
		memcpy((void *)se_hdr->e_txtstart, buf, se_hdr->e_txtlen);
    	lmm_free(&malloc_lmm, buf, se_hdr->e_txtlen);
	}

    /* Load data section into memory */
	if(se_hdr->e_datlen > 0) {
    	buf = (char *)lmm_alloc(&malloc_lmm, se_hdr->e_datlen, LMM_ANY_REGION_FLAG);
    	getbytes(se_hdr->e_fname, se_hdr->e_datoff, se_hdr->e_datlen, buf);
    	memcpy((void *)se_hdr->e_datstart, buf, se_hdr->e_datlen);
    	lmm_free(&malloc_lmm, buf, se_hdr->e_datlen);
	}

    /* Load rodata section into memory */
	if(se_hdr->e_rodatlen > 0) {
    	buf = (char *)lmm_alloc(&malloc_lmm, se_hdr->e_rodatlen, LMM_ANY_REGION_FLAG);
    	getbytes(se_hdr->e_fname, se_hdr->e_rodatoff, se_hdr->e_rodatlen, buf);
    	memcpy((void *)se_hdr->e_rodatstart, buf, se_hdr->e_rodatlen);
    	lmm_free(&malloc_lmm, buf, se_hdr->e_rodatlen);
	}

    /* Load bss section into memory */
    memset((void *)se_hdr->e_bssstart, 0, se_hdr->e_bsslen);
}

/**
 * Copies data from a file into a buffer.
 *
 * @param filename   the name of the file to copy data from
 * @param offset     the location in the file to begin copying from
 * @param size       the number of bytes to be copied
 * @param buf        the buffer to copy the data into
 *
 * @return returns the number of bytes copied on succes; -1 on failure
 */
int getbytes(const char *filename, int offset, int size, char *buf) {
    if (filename == NULL || size < 0 || buf == NULL || offset < 0
        || size < sizeof(buf)) {
        return FAILURE;
    }
    int i;
    for (i = 0; i < exec2obj_userapp_count; i++) {
        if (!strncmp(filename, exec2obj_userapp_TOC[i].execname, 
                    MAX_EXECNAME_LEN)) {
            size = ((offset + size) <= exec2obj_userapp_TOC[i].execlen) ? size
                   : (exec2obj_userapp_TOC[i].execlen - offset);
            if (size <= 0) {
                break;
            }
            memcpy(buf, exec2obj_userapp_TOC[i].execbytes + offset, size);
            return size;
        }
    }
    return FAILURE;
}

/**
 * Checks to see if file with name fname is an elf
 * executable binary. If so, fills in se_hdr struct
 * and returns ELF_SUCCESS. If not, returns ELF_NOTELF.
 * Assumes that the magic numbers in the header have been
 * verified.
 *
 * @param se_hdr   pointer to simple_elf_t struct to be
 *                 filled in. Memory for it has been
 *                 allocated.
 *
 * @param fname    name of file to use.
 *
 * @return   ELF_SUCCESS if se_hdr was successfully filled in.
 *           ELF_NOTELF if the file is not the kind of binary
 *           we are using in 15-410.
 */
int elf_load_helper(simple_elf_t *se_hdr, const char *fname) {
    Elf32_Ehdr elf_hdr;           /* elf header */
    Elf32_Shdr elf_sec_hdr;       /* section header */
    int ret;                      /* various return values */
    unsigned int i;               /* loop index */

    memset(se_hdr, 0, sizeof(simple_elf_t));

    /*
     * Grab the elf header
     */
    ret = getbytes(fname, 0, sizeof(Elf32_Ehdr), (char *)&elf_hdr);
    if (ret != sizeof(Elf32_Ehdr)) {
        lprintf("Loader: Couldn't read elf header: %d, %s", ret, fname);
        return ELF_NOTELF;
    }

    /*
     * grab the entry point and file name.
     */
    se_hdr->e_entry = elf_hdr.e_entry;
    se_hdr->e_fname = fname;


    /*
     * I want the section header string table first
     * so I know what other entries are.
     */
    ret = getbytes(fname,
                   elf_hdr.e_shoff + elf_hdr.e_shstrndx*sizeof(Elf32_Shdr),
                   sizeof(Elf32_Shdr),
                   (char *)&elf_sec_hdr);
    if (ret != sizeof(Elf32_Shdr)) {
        lprintf("Loader: could not read section header");
        return ELF_NOTELF;
    }

    unsigned int string_offset = elf_sec_hdr.sh_offset;

    /*
     * loop to find .text .rodata .data .bss section headers.
     */
    for (i = 0; i < elf_hdr.e_shnum; i++) {
        /* Read in the section */
        ret = getbytes(fname,
                       elf_hdr.e_shoff + i*sizeof(Elf32_Shdr),
                       sizeof(Elf32_Shdr),
                       (char *)&elf_sec_hdr);
        if (ret != sizeof(Elf32_Shdr)) {
            lprintf("Loader: could not read section header");
            return ELF_NOTELF;
        }

        if (elf_sec_hdr.sh_name == SHN_UNDEF) {
            continue;
        }

        unsigned int str_idx = elf_sec_hdr.sh_name;

        /* Some annoying contortions to handle the unlikely case that
         * the string table is at the very end of the file and so the
         * read comes up short.
         * We zero the section_name to make sure we don't compare
         * against bogus data when this happens. */
        char section_name[MAX_SECTION_NAME_LEN];
        memset(section_name, 0, sizeof(section_name));
        ret = getbytes(fname, string_offset+str_idx, sizeof(section_name),
                       section_name);
        if (ret < 0) {
            lprintf("Loader: could not read section name");
            return ELF_NOTELF;
        }

        if (strcmp(".text", section_name) == 0) {
            /*
             * This section header is for the text segment
             */
            se_hdr->e_txtoff   = elf_sec_hdr.sh_offset;
            se_hdr->e_txtlen   = elf_sec_hdr.sh_size;
            se_hdr->e_txtstart = elf_sec_hdr.sh_addr;
        }
        else if (strcmp(".rodata", section_name) == 0) {
            /*
             * This section header is for the rodata segment
             */
            se_hdr->e_rodatoff   = elf_sec_hdr.sh_offset;
            se_hdr->e_rodatlen   = elf_sec_hdr.sh_size;
            se_hdr->e_rodatstart = elf_sec_hdr.sh_addr;
        }
        else if (strcmp(".data", section_name) == 0) {
            /*
             * This section header is for the data segment
             */
            se_hdr->e_datoff   = elf_sec_hdr.sh_offset;
            se_hdr->e_datlen   = elf_sec_hdr.sh_size;
            se_hdr->e_datstart = elf_sec_hdr.sh_addr;
        }
        else if (strcmp(".bss", section_name) == 0) {
            /*
             * This section header is for the bss segment
             */
            se_hdr->e_bsslen = elf_sec_hdr.sh_size;
            se_hdr->e_bssstart = elf_sec_hdr.sh_addr;
        }
        else if (strcmp(".symtab", section_name) != 0 &&
                 strcmp(".strtab", section_name) != 0 &&
                 strcmp(".shstrtab", section_name) != 0 &&
                 strcmp(".stab", section_name) != 0 &&
                 strcmp(".stabstr", section_name) != 0 &&
                 strcmp(".comment", section_name) != 0 &&
                 strcmp(".note", section_name) != 0 &&
		         strncmp(".debug", section_name, sizeof(".debug")-1) != 0) {
            lprintf("Loader: unknown header: \"%s\"",
                    section_name);
        }

    }

    return ELF_SUCCESS;
}

/**
 * Checks fields in the potential ELF header to
 * make sure that is actually and ELF header.
 *
 * @param fname  file name of potential ELF binary
 *
 * @return  ELF_SUCCESS if it is an ELF header.
 *          ELF_NOTELF if it isn't.
 */
int elf_check_header(const char *fname) {
    Elf32_Ehdr elf_hdr;
    unsigned int ret;

    /*
     * read the elf header from the file.
     */
    ret = getbytes(fname, 0, sizeof(Elf32_Ehdr), (char *)&elf_hdr);
    if (ret != sizeof(Elf32_Ehdr)) {
        lprintf("Loader: Couldn't read elf header: %d, %s", ret, fname);
        return ELF_NOTELF;
    }

    if (memcmp(elf_hdr.e_ident, ELFMAG, SELFMAG) != 0) {
        return ELF_NOTELF;
    }

    if (elf_hdr.e_type != ET_EXEC) {
        return ELF_NOTELF;
    }

    if (elf_hdr.e_machine != EM_386) {
        return ELF_NOTELF;
    }

    if (elf_hdr.e_version != EV_CURRENT) {
        return ELF_NOTELF;
    }

    return ELF_SUCCESS;
}
