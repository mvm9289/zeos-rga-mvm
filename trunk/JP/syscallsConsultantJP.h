#define OPEN_CALL 0
#define CLOSE_CALL 1
#define WRITE_CALL 2
#define LSEEK_CALL 3
#define CLONE_CALL 4

#define ALL -1


struct t_info {
    int num_entrades;
    int num_sortides_ok;
    int num_sortides_error;
    unsigned long long durada_total;
};

