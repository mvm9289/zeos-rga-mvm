#define OPEN 0
#define WRITE 2
#define LSEEK 3
#define CLOSE 1
#define CLONE 4
#define TOTES -1

struct t_info{
  int num_entrades;
  int num_sortides_ok;
  int num_sortides_error;
  unsigned long long durada_total;
};
