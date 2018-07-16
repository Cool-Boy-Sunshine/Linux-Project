#ifndef __MY_REQ_H__
#define __MY_REQ_H__
#include <stdint.h>
#define SIZE 32
#define MD5_SIZE 32
#define FILE_SIZE 256

enum __req_type_t
{
    REQ_LOGIN = 0,
    RSP_LOGIN,
    REQ_UPLOAD,
    RSP_UPLOAD,
    REQ_FLIST,
    RSP_FLIST,
    REQ_DLOAD,
    RSP_DLOAD,
};
typedef struct __com_req_t
{
    uint8_t type;
    uint64_t len;
}com_req_t;

typedef struct __login_info_t
{
    char username[SIZE];
    char password[SIZE];
}login_info_t;

typedef struct __file_hdr_t
{
    uint64_t    flen;
    char        fname[FILE_SIZE];
    char        fmd5[MD5_SIZE];
}file_hdr_t;

#endif
