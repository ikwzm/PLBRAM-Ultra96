#include        <stdio.h>
#include        <stdlib.h>
#include        <fcntl.h>
#include        <string.h>
#include        <time.h>
#include        <unistd.h>
#include        <sys/types.h>
#include        <sys/time.h>
#include        <sys/mman.h>
#include        <sys/utsname.h>

struct uiomem
{
    char*  name;
    char*  dev_name;
    char*  sys_path;
    size_t size;
    int    sync_for_cpu_file;
    int    sync_for_dev_file;
    char   sync_command[1024];
    int    sync_command_len;
};

const  int  UIOMEM_READ_WRITE  =  0;
const  int  UIOMEM_WRITE_ONLY  =  1;
const  int  UIOMEM_READ_ONLY   =  2;

void uiomem_destroy(struct uiomem* uiomem)
{
    if (uiomem == NULL)
        return;
    
    if (uiomem->sync_for_cpu_file >= 0) close(uiomem->sync_for_cpu_file);
    if (uiomem->sync_for_dev_file >= 0) close(uiomem->sync_for_dev_file);
    if (uiomem->name     != NULL) free(uiomem->name);
    if (uiomem->dev_name != NULL) free(uiomem->dev_name);
    if (uiomem->sys_path != NULL) free(uiomem->sys_path);
    free(uiomem);
}

struct uiomem* uiomem_create(char* name)
{
    struct uiomem*  uiomem;
    char            file_name[1024];
    char            attr[1024];
    int             str_len;
    int             fd;

    if ((uiomem = calloc(1, sizeof(struct uiomem))) == NULL) {
        printf("Can not alloc uiomem\n");
        goto failed;
    }
    uiomem->sync_for_cpu_file = -1;
    uiomem->sync_for_dev_file = -1;
    
    if ((uiomem->name = strdup(name)) == NULL) {
        printf("Can not alloc uiomem->name\n");
        goto failed;
    }
    str_len = sprintf(file_name, "/dev/%s", uiomem->name);
    if ((uiomem->dev_name = strdup(file_name)) == NULL) {
        printf("Can not alloc uiomem->dev_name\n");
        goto failed;
    }
    str_len = sprintf(file_name, "/sys/class/uiomem/%s", uiomem->name);
    if ((uiomem->sys_path = strdup(file_name)) == NULL) {
        printf("Can not alloc uiomem->sys_path\n");
        goto failed;
    }
    str_len = sprintf(file_name, "%s/size", uiomem->sys_path);
    if ((fd = open(file_name, O_RDONLY)) != -1) {
        read(fd, attr, 1024);
        sscanf(attr, "%d", &uiomem->size);
        close(fd);
    } else {
        printf("Can not open %s\n", file_name);
        goto failed;
    } 
    str_len = sprintf(file_name, "%s/sync_for_cpu", uiomem->sys_path);
    if ((fd = open(file_name, O_RDWR)) != -1) {
        uiomem->sync_for_cpu_file = fd;
    } else {
        printf("Can not open %s\n", file_name);
        goto failed;
    } 
    str_len = sprintf(file_name, "%s/sync_for_device", uiomem->sys_path);
    if ((fd = open(file_name, O_RDWR)) != -1) {
        uiomem->sync_for_dev_file = fd;
    } else {
        printf("Can not open %s\n", file_name);
        goto failed;
    }
    return uiomem;
      
  failed:
    uiomem_destroy(uiomem);
    return NULL;
}

int  uiomem_open(struct uiomem* uiomem, int flags)
{
    return open(uiomem->dev_name, flags);
}

void uiomem_set_sync_area(struct uiomem* uiomem, unsigned int offset, unsigned int size, int direction)
{
    uiomem->sync_command_len = 
        sprintf(uiomem->sync_command, "0x%08X%08X\n",
                offset,
               ((size & 0xFFFFFFF0) | (direction << 2) | 1));
}

size_t uiomem_sync_for_cpu(struct uiomem* uiomem)
{
    if (uiomem->sync_command_len > 0)
        return write(uiomem->sync_for_cpu_file,
                     uiomem->sync_command,
                     uiomem->sync_command_len);
    else
        return 0;
}

size_t uiomem_sync_for_dev(struct uiomem* uiomem)
{
    if (uiomem->sync_command_len > 0)
        return write(uiomem->sync_for_dev_file,
                     uiomem->sync_command,
                     uiomem->sync_command_len);
    else
        return 0;
}

static void diff_time(struct timeval* run_time, struct timeval* start_time, struct timeval* end_time)
{
    if (end_time->tv_usec < start_time->tv_usec) {
        run_time->tv_sec  = end_time->tv_sec  - start_time->tv_sec  - 1;
        run_time->tv_usec = end_time->tv_usec - start_time->tv_usec + 1000*1000;
    } else {
        run_time->tv_sec  = end_time->tv_sec  - start_time->tv_sec ;
        run_time->tv_usec = end_time->tv_usec - start_time->tv_usec;
    }
}

void uiomem_mmap_write_test(struct uiomem* uiomem, void* buf, unsigned int size, int sync, struct timeval* run_time)
{
    int            fd;
    void*          iomem;
    struct timeval start_time, end_time;

    if (sync == 0)
        uiomem_set_sync_area(uiomem, 0, size, UIOMEM_WRITE_ONLY);
      
    if ((fd  = uiomem_open(uiomem, O_RDWR | ((sync)?O_SYNC:0))) != -1) {
        iomem = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        gettimeofday(&start_time, NULL);
        if (sync == 0)
            uiomem_sync_for_cpu(uiomem);
        memcpy(iomem, buf, size);
        if (sync == 0)
            uiomem_sync_for_dev(uiomem);
        gettimeofday(&end_time  , NULL);
        diff_time(run_time, &start_time, &end_time);
        (void)close(fd);
    }
}

void uiomem_mmap_read_test(struct uiomem* uiomem, void* buf, unsigned int size, int sync, struct timeval* run_time)
{
    int            fd;
    void*          iomem;
    struct timeval start_time, end_time;

    if (sync == 0)
        uiomem_set_sync_area(uiomem, 0, size, UIOMEM_READ_ONLY);
      
    if ((fd  = uiomem_open(uiomem, O_RDWR | ((sync)?O_SYNC:0))) != -1) {
        iomem = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        gettimeofday(&start_time, NULL);
        if (sync == 0)
            uiomem_sync_for_cpu(uiomem);
        memcpy(buf, iomem, size);
        if (sync == 0)
            uiomem_sync_for_dev(uiomem);
        gettimeofday(&end_time  , NULL);
        if (run_time != NULL)
            diff_time(run_time, &start_time, &end_time);
        close(fd);
    }
}

void uiomem_file_write_test(struct uiomem* uiomem, void* buf, unsigned int size, int sync, struct timeval* run_time)
{
    int            fd;
    int            len;
    void*          ptr;
    struct timeval start_time, end_time;

    if (sync == 0)
        uiomem_set_sync_area(uiomem, 0, size, UIOMEM_WRITE_ONLY);
      
    if ((fd  = uiomem_open(uiomem, O_RDWR | ((sync)?O_SYNC:0))) != -1) {
        gettimeofday(&start_time, NULL);
        if (sync == 0)
            uiomem_sync_for_cpu(uiomem);
        len = size;
        ptr = buf;
        while(len > 0) {
            int count = write(fd, ptr, len);
            if (count < 0) {
                break;
            }
            ptr += count;
            len -= count;
        }
        if (sync == 0)
            uiomem_sync_for_dev(uiomem);
        gettimeofday(&end_time  , NULL);
        diff_time(run_time, &start_time, &end_time);
        (void)close(fd);
    }
}

void uiomem_file_read_test(struct uiomem* uiomem, void* buf, unsigned int size, int sync, struct timeval* run_time)
{
    int            fd;
    int            len;
    void*          ptr;
    struct timeval start_time, end_time;

    if (sync == 0)
        uiomem_set_sync_area(uiomem, 0, size, UIOMEM_READ_ONLY);
      
    if ((fd  = uiomem_open(uiomem, O_RDWR | ((sync)?O_SYNC:0))) != -1) {
        gettimeofday(&start_time, NULL);
        if (sync == 0)
            uiomem_sync_for_cpu(uiomem);
        len = size;
        ptr = buf;
        while(len > 0) {
            int count = read(fd, ptr, len);
            if (count < 0) {
                break;
            }
            ptr += count;
            len -= count;
        }
        if (sync == 0)
            uiomem_sync_for_dev(uiomem);
        gettimeofday(&end_time  , NULL);
        if (run_time != NULL)
            diff_time(run_time, &start_time, &end_time);
        close(fd);
    }
}

void main()
{
    struct uiomem* uiomem;
    unsigned char* buf;
    unsigned int   buf_size;
    void*          null_buf = NULL;
    void*          src0_buf = NULL;
    void*          src1_buf = NULL;
    void*          temp_buf = NULL;

    if ((uiomem = uiomem_create("uiomem0")) == NULL) {
        goto done;
    }

    buf_size = uiomem->size;

    printf("size=%d\n", uiomem->size);

    if ((null_buf = malloc(buf_size)) == NULL) {
        printf("Can not malloc null_buf\n");
        goto done;
    } else {
        memset(null_buf, 0, buf_size);
    }

    if ((src0_buf = malloc(buf_size)) == NULL) {
        printf("Can not malloc src0_buf\n");
        goto done;
    } else {
        int*   word  = (int *)src0_buf;
        size_t words = buf_size/sizeof(int);
        for(int i = 0; i < words; i++) {
            word[i] = i;
        }
    }

    if ((src1_buf = malloc(buf_size)) == NULL) {
        printf("Can not malloc src1_buf\n");
        goto done;
    } else {
        int*   word  = (int *)src1_buf;
        size_t words = buf_size/sizeof(int);
        for(int i = 0; i < words; i++) {
            word[i] = ~i;
        }
    }

    if ((temp_buf = malloc(buf_size)) == NULL) {
        printf("Can not malloc temp_buf\n");
        goto done;
    } else {
        memset(temp_buf, 0, buf_size);
    }

#define TEST(w_type,w_sync,r_type,r_sync,src,size) \
    {                                              \
        struct timeval run_time;                   \
        memset(temp_buf, 0, buf_size);             \
        printf(#w_type " write test(SIZE=%d, O_SYNC=%d) : ", size, w_sync);  \
        uiomem_##w_type##_write_test(uiomem, src, size, w_sync, &run_time);  \
        printf("time = %ld.%06ld sec\n", run_time.tv_sec, run_time.tv_usec); \
        printf(#r_type " read  test(SIZE=%d, O_SYNC=%d) : ", size, r_sync);  \
        uiomem_##r_type##_read_test (uiomem, temp_buf, size, r_sync, &run_time); \
        printf("time = %ld.%06ld sec\n", run_time.tv_sec, run_time.tv_usec); \
        if (memcmp(temp_buf, src, size) != 0)      \
            printf("compare = mismatch\n");        \
        else                                       \
            printf("compare = ok\n");              \
    }

    TEST(mmap, 1, mmap, 1, src0_buf, buf_size);
    TEST(mmap, 0, mmap, 1, src1_buf, buf_size);
    TEST(mmap, 1, mmap, 0, src0_buf, buf_size);
    TEST(mmap, 0, mmap, 0, src1_buf, buf_size);
    TEST(file, 1, mmap, 0, src0_buf, buf_size);
    TEST(file, 0, mmap, 0, src1_buf, buf_size);
    TEST(mmap, 0, file, 1, src0_buf, buf_size);
    TEST(mmap, 0, file, 0, src1_buf, buf_size);

 done:
    if (temp_buf != NULL)
        free(temp_buf);
    if (src1_buf != NULL)
        free(src1_buf);
    if (src0_buf != NULL)
        free(src0_buf);
    if (null_buf != NULL)
        free(null_buf);
    if (uiomem   != NULL)
        uiomem_destroy(uiomem);
}
