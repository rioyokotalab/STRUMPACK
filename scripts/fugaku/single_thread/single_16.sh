#!/bin/sh -x

#PJM -L "node=16"
#PJM -L "rscunit=rscunit_ft01"
#PJM -L "rscgrp=small"
#PJM -L "elapse=24:00:00"
#PJM -L "freq=2200"
#PJM -L "throttling_state=0"
#PJM -L "issue_state=0"
#PJM -L "ex_pipe_state=0"
#PJM -L "eco_state=0"
#PJM --mpi "proc=768"
#PJM --mpi "max-proc-per-node=48"
#PJM -s

export OMP_PLACES=cores
export OMP_DISPLAY_AFFINITY=TRUE
export OMP_PROC_BIND=close
export OMP_BIND=close
export OMP_NUM_THREADS=1
export XOS_MMM_L_PAGING_POLICY="demand:demand:demand"

mpiexec -stdout out_single_16.log -stderr err_single_16.log ./build/test/test_STRUMPACK_starsh 524288 1e-11 --hss_rel_tol 1e-9
mpiexec -stdout out_single_16.log -stderr err_single_16.log ./build/test/test_STRUMPACK_starsh 524288 1e-9 --hss_rel_tol 1e-9
