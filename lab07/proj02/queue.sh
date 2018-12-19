#!/bin/sh

#PBS -N CS61C
#PBS -V

# Specify email address to use for notification.
#PBS -m bea
#PBS -M [email]@berkeley.edu

# Specify the number of virtual processors by changing ppn.
#PBS -l nodes=1:ppn=1
#PBS -q batch

# change to the directory qsub was called from
cd $PBS_O_WORKDIR

# name of the file to execute
./benchmark > result.out
