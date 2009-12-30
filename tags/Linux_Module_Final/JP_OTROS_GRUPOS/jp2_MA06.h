#define OPEN 0
#define WRITE 1
#define LSEEK 2
#define CLOSE 3
#define CLONE 4
#define TOTES -1

struct t_info{
  int num_entrades;
  int num_sortides_ok;
  int num_sortides_error;
  unsigned long long durada_total;
};
