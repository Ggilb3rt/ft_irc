#include <stdio.h>
#include "bircd.h"

int	main(int ac, char **av)
{
  t_env	e;

  init_env(&e);
  get_opt(&e, ac, av);
  printf("\tEnv\nPort : %d\nMaxfd : %d\nMax : %d\nr : %d\n",
          e.port, e.maxfd, e.max, e.r);
  srv_create(&e, e.port);
  main_loop(&e);
  return (0);
}
