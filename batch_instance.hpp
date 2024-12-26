// Copyright (C) 2022
// Klaus Kraßnitzer

#ifndef INSTANCE_H
#define INSTANCE_H

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <limits>
#include "util.hpp"

using namespace std;

class Job {
    public:
        int id;
        int release_time;
        int req_runtime;
        int act_runtime;
        int machines;

        Job(int _id, int _release_time, int _req_runtime, int _act_runtime, int _machines) : id(_id), release_time(_release_time), req_runtime(_req_runtime), act_runtime(_act_runtime), machines(_machines) {}
};

class Instance {
    public:
        // number of jobs
        int n;

        // number of machines
        int m;

        // jobs
        vector<Job> jobs;

        // constructor
        Instance(int _n, int _m) : n(_n), m(_m) {
            jobs.reserve(_n);
        }

        static Instance parse(istream &is) {
            // read number of machines
            int m;
            is >> m;

            // read number of jobs
            int n;
            is >> n;

            // construct instance
            Instance inst(n, m);

            // parse jobs
            for (int i=0; i<inst.n; i++) {
                int r, j, p_r, p_a, r_m; 
                is >> r >> j >> p_r >> p_a >> r_m;
                Job x(j,r,p_r,p_a,r_m);
                inst.jobs.push_back(x);
                //inst.jobs.emplace_back(j,r,p_r,p_a,r_m);
            }

            return inst;
        }
};

// Instance formatter
ostream& operator<<(ostream &os, const Instance &inst) {
    os << "N = " << inst.n << endl;
    os << "M = " << inst.m << endl;
    os << "Jobs = " << inst.jobs << endl;
    return os;
}

// Job formatter
ostream& operator<<(ostream &os, const Job &job) {
    os << "(";
    os << "j=" << job.id << ", "; 
    os << "r_j=" << job.release_time << ", "; 
    os << "p_j=" << job.req_runtime << ", "; 
    os << "~p_j=" << job.act_runtime << ", "; 
    os << "m_j=" << job.machines;
    os << ")";
    return os;
}

#endif /* INSTANCE_H */