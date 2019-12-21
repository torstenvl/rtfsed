#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "rtfrep.h"

// REMINDER OF STRUCTURE DECLARATIONS AND CONSTANTS
// typedef struct repobj {
//     FILE *is;             // Input stream
//     FILE *os;             // Output stream
//     byte ib[FB_Z];        // Input file buffer
//     byte ibto[FB_Z];      // Input file buffer token oracle (readahead)
//     byte ob[FB_Z];        // Output file buffer
//     uccp pb[PB_Z];        // Pattern buffer for comparing codepoints
//     size_t ibi;           // Input buffer iterator
//     size_t ibtoi;         // Input buffer token oracle iterator
//     size_t obi;           // Output buffer iterator
//     size_t pbi;           // Pattern buffer iterator
//     size_t ibz;           // Input buffer actual size
//     size_t obz;           // Output buffer actual size
//     size_t pbz;           // Pattern buffer actual size
//     size_t pbmaps[PB_Z];  // Maps to start of rtf sequence for pb[i] in ib[]
//     size_t pbmape[PB_Z];  // Maps to end of rtf sequence for pb[i] in ib[]
// } repobj;
// FB_Z file buffer (ib and ob) size
// PB_Z pattern buffer size


int rtf_process(repobj *r) {
    int nerrs;  // Number of errors
    int err;    // Current error

    nerrs = 0;

    while (!feof(r->is)) {

        memset_s(r->ob, FB_Z, 0, FB_Z);
        r->ibz = fread(r->ib, 1, FB_Z, r->is);

        // Incomplete buffer fill = error or EOF. If an error, log it.
        // Then keep processing. (If EOF, it'll fall through on next while().)
        if (r->ibz < FB_Z) {
            if (ISERROR(err = ferror(r->is))) {
                fprintf(stderr, "File read error %d!\n", err);
                nerrs++;
            }
        }

        for (r->ibi = 0, r->obi = 0; r->ibi < r->ibz; r->ibi++) {
            // if (r->ib[r->ibi] == '\\') { }
            // if (r->ib[r->ibi] == '{') { }
            // if (r->ib[r->ibi] == '}') { }
            r->ob[r->obi++] = r->ib[r->ibi];
        }

        fwrite(r->ob, 1, r->obi, r->os);

    } // end of buffer-read loop

    return nerrs;
}








































/***************************************************************************/
/*                                                                         */
/*  THE FOLLOWING SECTION IMPLEMENTS BASIC OBJECT CREATION / DESTRUCTION   */
/*                                                                         */
/*  repobj *new_repobj(void);                                              */
/*  repobj *new_repobj_from_file(const char *);                            */
/*  repobj *new_repobj_to_file(const char *);                              */
/*  repobj *new_repobj_from_file_to_file(const char *, const char *);      */
/*  void destroy_repobj(repobj *);                                         */
/*                                                                         */
/***************************************************************************/





repobj *new_repobj(void) {
    return new_repobj_from_stream_to_stream(stdin, stdout);
}





repobj *new_repobj_from_file(const char *ifname) {
    FILE *ifptr;
    repobj *r;

    ifptr = fopen(ifname, "r");
    if (!ifptr) return NULL;

    r = new_repobj_from_stream_to_stream(ifptr, stdout);
    if (!r) {
        fclose (ifptr);
        return NULL;
    }

    return r;
}





repobj *new_repobj_to_file(const char *ofname) {
    FILE *ofptr;
    repobj *r;

    ofptr = fopen(ofname, "w");
    if (!ofptr) return NULL;

    r = new_repobj_from_stream_to_stream(stdin, ofptr);
    if (!r) {
        fclose(ofptr);
        return NULL;
    }

    return r;
}





repobj *new_repobj_from_file_to_file(const char *ifname, const char *ofname) {
    FILE *ifptr;
    FILE *ofptr;
    repobj *r;

    ifptr = fopen(ifname, "r");
    if (!ifptr) return NULL;

    ofptr = fopen(ofname, "w");
    if (!ofptr) {
        fclose(ifptr);
        return NULL;
    }

    r = new_repobj_from_stream_to_stream(ifptr, ofptr);
    if (!r) {
        fclose(ifptr);
        fclose(ofptr);
        return NULL;
    }

    return r;
}





repobj *new_repobj_from_stream_to_stream(FILE *ifstream, FILE *ofstream) {
    repobj *r;

    r = malloc(sizeof(*r));
    if (!r) return NULL;

    memset_s(r, sizeof *r, 0, sizeof *r);
    r->is = ifstream;
    r->os = ofstream;

    return r;
}





void destroy_repobj(repobj *r) {
    if (r->is != stdin) fclose(r->is);
    if (r->os != stdout) fclose(r->os);

    memset_s(r, sizeof *r, 0, sizeof *r);

    free(r);
}
