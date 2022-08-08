#!/bin/sh -x

#PJM -L "node=1024"
#PJM -L "rscunit=rscunit_ft01"
#PJM -L "rscgrp=large"
#PJM -L "elapse=24:00:00"
#PJM -L "freq=2200"
#PJM -L "throttling_state=0"
#PJM -L "issue_state=0"
#PJM -L "ex_pipe_state=0"
#PJM -L "eco_state=0"
#PJM --mpi "proc=4096"
#PJM --mpi "max-proc-per-node=4"
#PJM -s

export OMP_PLACES=cores
export OMP_DISPLAY_AFFINITY=TRUE
export OMP_PROC_BIND=close
export OMP_BIND=close
export OMP_NUM_THREADS=12
export XOS_MMM_L_PAGING_POLICY="demand:demand:demand"

# mpiexec -stdout job_stdout.log -stderr job_stderr.log ./build/test/test_STRUMPACK_starsh \
    #        $HOME/57114x64.dat 3655280 --hss_rel_tol 1e-9

mpiexec -stdout out_1024.log -stderr err_1024.log ./build/test/test_STRUMPACK_starsh 1048576 --hss_rel_tol 1e-9
