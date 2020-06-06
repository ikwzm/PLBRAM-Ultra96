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

static int  _sys_sync_for_cpu_file = -1;
static int  _sys_sync_for_dev_file = -1;
const  int  _SYS_DMA_BIDIRECTIONAL =  0;
const  int  _SYS_DMA_TO_DEVICE     =  1;
const  int  _SYS_DMA_FROM_DEVICE   =  2;

static int  _sys_sync_command(char* buf, unsigned int offset, unsigned int size, int direction)
{
    return sprintf(buf, "0x%08X%08X\n", offset, ((size & 0xFFFFFFF0) |
                                                 (direction << 2) | 1));
}
static void _sys_sync_write(int file, char* attr, size_t len)
{
    size_t status;
    status = write(file, attr, len);
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

void udmabuf_mmap_write_test(void* buf, unsigned int size, int sync, struct timeval* run_time)
{
    int            fd;
    unsigned char  sync_cmd[1024];
    int            sync_len;
    struct timeval start_time, end_time;
    void*          udmabuf;

    if (sync == 0)
        sync_len = _sys_sync_command(sync_cmd, 0, size, _SYS_DMA_TO_DEVICE);
      
    if ((fd  = open("/dev/udmabuf0", O_RDWR | ((sync)?O_SYNC:0))) != -1) {
        udmabuf = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        gettimeofday(&start_time, NULL);
        if (sync == 0)
            _sys_sync_write(_sys_sync_for_cpu_file, sync_cmd, sync_len);
        memcpy(udmabuf, buf, size);
        if (sync == 0)
            _sys_sync_write(_sys_sync_for_dev_file, sync_cmd, sync_len);
        gettimeofday(&end_time  , NULL);
        diff_time(run_time, &start_time, &end_time);
        (void)close(fd);
    }
}

void udmabuf_mmap_read_test(void* buf, unsigned int size, int sync, struct timeval* run_time)
{
    int            fd;
    unsigned char  sync_cmd[1024];
    int            sync_len;
    struct timeval start_time, end_time;
    void*          udmabuf;

    if (sync == 0)
        sync_len = _sys_sync_command(sync_cmd, 0, size, _SYS_DMA_FROM_DEVICE);
      
    if ((fd  = open("/dev/udmabuf0", O_RDWR | ((sync)?O_SYNC:0))) != -1) {
        udmabuf = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        gettimeofday(&start_time, NULL);
        if (sync == 0)
            _sys_sync_write(_sys_sync_for_cpu_file, sync_cmd, sync_len);
        memcpy(buf, udmabuf, size);
        if (sync == 0)
            _sys_sync_write(_sys_sync_for_dev_file, sync_cmd, sync_len);
        gettimeofday(&end_time  , NULL);
        if (run_time != NULL)
            diff_time(run_time, &start_time, &end_time);
        close(fd);
    }
}

void main()
{
    int            fd;
    unsigned char* buf;
    unsigned int   buf_size;
    void*          null_buf = NULL;
    void*          src0_buf = NULL;
    void*          src1_buf = NULL;
    void*          temp_buf = NULL;

    if ((fd  = open("/sys/class/u-dma-buf/udmabuf0/size"     , O_RDONLY)) != -1) {
        char attr[1024];
        read(fd, attr, 1024);
        sscanf(attr, "%d", &buf_size);
        close(fd);
    }
    printf("size=%d\n", buf_size);

    if ((_sys_sync_for_cpu_file = open("/sys/class/u-dma-buf/udmabuf0/sync_for_cpu", O_RDWR)) < 0) {
        printf("Can not open /sys/class/u-dma-buf/udmabuf0/sync_for_cpu\n");
        goto done;
    }
    
    if ((_sys_sync_for_dev_file = open("/sys/class/u-dma-buf/udmabuf0/sync_for_device", O_RDWR)) < 0) {
        printf("Can not open /sys/class/u-dma-buf/udmabuf0/sync_for_device\n");
        goto done;
    }

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

    {
        struct timeval run_time;
        int            w_sync = 1;
        int            r_sync = 1;

        memset(temp_buf, 0, buf_size);

        printf("mmap write test(SIZE=%d, O_SYNC=%d) : ", buf_size, w_sync);
        udmabuf_mmap_write_test(src0_buf, buf_size, w_sync, &run_time);
        printf("time = %ld.%06ld sec\n", run_time.tv_sec, run_time.tv_usec);

        printf("mmap read  test(SIZE=%d, O_SYNC=%d) : ", buf_size, r_sync);
        udmabuf_mmap_read_test (temp_buf, buf_size, r_sync, &run_time);
        printf("time = %ld.%06ld sec\n", run_time.tv_sec, run_time.tv_usec);

        if (memcmp(temp_buf, src0_buf, buf_size) != 0)
            printf("compare = mismatch\n");
        else
            printf("compare = ok\n");
    }

    {
        struct timeval run_time;
        int            w_sync = 1;
        int            r_sync = 0;

        memset(temp_buf, 0, buf_size);
        
        printf("mmap write test(SIZE=%d, O_SYNC=%d) : ", buf_size, w_sync);
        udmabuf_mmap_write_test(src0_buf, buf_size, w_sync, &run_time);
        printf("time = %ld.%06ld sec\n", run_time.tv_sec, run_time.tv_usec);

        printf("mmap read  test(SIZE=%d, O_SYNC=%d) : ", buf_size, r_sync);
        udmabuf_mmap_read_test (temp_buf, buf_size, r_sync, &run_time);
        printf("time = %ld.%06ld sec\n", run_time.tv_sec, run_time.tv_usec);

        if (memcmp(temp_buf, src0_buf, buf_size) != 0)
            printf("compare = mismatch\n");
        else
            printf("compare = ok\n");
    }

    {
        struct timeval run_time;
        int            w_sync = 0;
        int            r_sync = 1;

        memset(temp_buf, 0, buf_size);
        
        printf("mmap write test(SIZE=%d, O_SYNC=%d) : ", buf_size, w_sync);
        udmabuf_mmap_write_test(src0_buf, buf_size, w_sync, &run_time);
        printf("time = %ld.%06ld sec\n", run_time.tv_sec, run_time.tv_usec);

        printf("mmap read  test(SIZE=%d, O_SYNC=%d) : ", buf_size, r_sync);
        udmabuf_mmap_read_test (temp_buf, buf_size, r_sync, &run_time);
        printf("time = %ld.%06ld sec\n", run_time.tv_sec, run_time.tv_usec);

        if (memcmp(temp_buf, src0_buf, buf_size) != 0)
            printf("compare = mismatch\n");
        else
            printf("compare = ok\n");
    }

    {
        struct timeval run_time;
        int            w_sync = 0;
        int            r_sync = 0;

        memset(temp_buf, 0, buf_size);
        
        printf("mmap write test(SIZE=%d, O_SYNC=%d) : ", buf_size, w_sync);
        udmabuf_mmap_write_test(src0_buf, buf_size, w_sync, &run_time);
        printf("time = %ld.%06ld sec\n", run_time.tv_sec, run_time.tv_usec);

        printf("mmap read  test(SIZE=%d, O_SYNC=%d) : ", buf_size, r_sync);
        udmabuf_mmap_read_test (temp_buf, buf_size, r_sync, &run_time);
        printf("time = %ld.%06ld sec\n", run_time.tv_sec, run_time.tv_usec);

        if (memcmp(temp_buf, src0_buf, buf_size) != 0)
            printf("compare = mismatch\n");
        else
            printf("compare = ok\n");
    }


 done:
    if (temp_buf != NULL)
        free(temp_buf);
    if (src1_buf != NULL)
        free(src1_buf);
    if (src0_buf != NULL)
        free(src0_buf);
    if (null_buf != NULL)
        free(null_buf);
    if (_sys_sync_for_cpu_file >= 0)
        close(_sys_sync_for_cpu_file);
    if (_sys_sync_for_dev_file >= 0)
        close(_sys_sync_for_dev_file);
}
