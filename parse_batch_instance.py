#! /usr/bin/env python

# Copyright (C) 2022 Klaus Kraßnitzer

import sys

class Job:
    def __init__(self, arrival_time: float, job_id: int, requested_time: float, runtime: float, num_cpus: int):
        self.release_time = arrival_time
        self.id = job_id
        self.runtime_req = requested_time
        self.runtime_act = runtime
        self.machines = num_cpus
    
    def __repr__(self):
        return f"Job({self.id}, {self.release_time}, {self.runtime_req}, {self.runtime_act}, {self.machines})"

class Instance:
    def __init__(self, p: int, jobs: list):
        self.m = p
        self.n = len(jobs)
        self.jobs = jobs
    
    def __repr__(self):
        jobs = '[\n\t{}\n]'.format(",\n\t".join(map(str, self.jobs)))
        return f"Instance(m={self.m}, n={self.n}, jobs={jobs})"


def read_instance(infile=None):
    if infile is None:
        f = sys.stdin
    else:
        f = open(infile, "r")
    jobs = []
    p = int(f.readline().strip())
    n = int(f.readline().strip())
    for _ in range(n):
        arrival_time, job_id, requested_time, runtime, num_cpus = f.readline().split()
        jobs.append(Job(int(float(arrival_time)), int(job_id), int(float(requested_time)), int(float(runtime)), int(num_cpus)))
    
    return Instance(p, jobs)


    