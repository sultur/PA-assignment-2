#! /usr/bin/env python

import sys
import argparse
from parse_batch_instance import read_instance, Instance

class InvalidSchedule(Exception):
    pass


def read_schedule(infile=None):
    if infile is None:
        f = sys.stdin
    else:
        f = open(infile, "r")
    
    jobs = []
    num_jobs = None
    
    for i,l in enumerate(f):
        if i == 0:
            num_jobs = int(l.strip())
        else:
            job_id, start_time = l.split()
            jobs.append((int(job_id), int(start_time)))
    
    if infile is None:
        f.close()
    
    return {"jobs": jobs, "num_jobs": num_jobs }


def verify_schedule(instance: Instance, schedule: list):
    schedule["jobs"].sort(key=lambda x: x[1])
    job_dict = {j.id: j for j in instance.jobs}
    events = []
    for job_id, start_time in schedule["jobs"]:
        events.append((start_time, job_id, True))
        events.append((start_time + job_dict[job_id].runtime_act, job_id, False)) 
    
    if schedule["num_jobs"] != len(instance.jobs):
        raise InvalidSchedule(f"number of jobs in schedule ({schedule['num_jobs']}) does not match number of jobs in instance ({len(instance.jobs)})")
    
    if schedule["num_jobs"] != len(schedule["jobs"]):
        raise InvalidSchedule(f"number of jobs in schedule ({len(schedule['jobs'])}) does not match number of job entries ({schedule['num_jobs']})")
    
    events.sort(key=lambda x: x[0])
    p = instance.m
    for ts,ji,is_start in events:
        if is_start:
            p -= job_dict[ji].machines
            if ts < job_dict[ji].release_time:
                raise InvalidSchedule(f"job {ji} starts before arrival at {ts}")
        else:
            p += job_dict[ji].machines
        if p < 0:
            raise InvalidSchedule(f"p < 0 at {ts} for job {ji}")


def main():
    parser = argparse.ArgumentParser(description='Verify a schedule')
    parser.add_argument('instance', help='The instance file')
    parser.add_argument('-s', '--schedule', help='Generated schedule file (read from stdin if not provided)', default=None)
    args = parser.parse_args()
    instance = read_instance(args.instance)
    schedule = read_schedule(args.schedule)
    try:
        verify_schedule(instance, schedule)
        print("Schedule is VALID")
        exit(0)
    except InvalidSchedule as e:
        print(f"INVALID schedule: {e}")
        exit(1)


if __name__ == '__main__':
    main()