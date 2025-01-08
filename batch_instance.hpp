// Copyright (C) 2022
// Klaus Kraßnitzer

#ifndef INSTANCE_H
#define INSTANCE_H

#include "util.hpp"
#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;
typedef uint32_t u32;

class Job {
public:
  int id;
  int release_time;
  int req_runtime;
  int act_runtime;
  int machines;

  u32 start_time = 0;

  Job(int _id, int _release_time, int _req_runtime, int _act_runtime,
      int _machines)
      : id(_id), release_time(_release_time), req_runtime(_req_runtime),
        act_runtime(_act_runtime), machines(_machines) {}

  // Comparison functions

  bool operator==(Job other) { return id == other.id; }

  static bool compare_actual_endtimes(const Job &j1, const Job &j2) {
    return j1.actual_end() < j2.actual_end();
  }
  static bool compare_expected_endtimes(const Job &j1, const Job &j2) {
    return j1.expected_end() < j2.expected_end();
  }

  /*
   * Set start time of job.
   */
  void set_start_time(u32 timestamp) { start_time = timestamp; }

  /*
   * Return the expected end time of the job.
   */
  u32 expected_end() const { return start_time + (u32)req_runtime; }

  /*
   * Return the actual end time of the job (the scheduler isn't
   * allowed to use this for the algorithm).
   */
  u32 actual_end() const { return start_time + (u32)act_runtime; }
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
  Instance(int _n, int _m) : n(_n), m(_m) { jobs.reserve(_n); }

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
    for (int i = 0; i < inst.n; i++) {
      int r, j, p_r, p_a, r_m;
      is >> r >> j >> p_r >> p_a >> r_m;
      Job x(j, r, p_r, p_a, r_m);
      inst.jobs.push_back(x);
      // inst.jobs.emplace_back(j,r,p_r,p_a,r_m);
    }

    return inst;
  }
};

// Instance formatter
inline ostream &operator<<(ostream &os, const Instance &inst) {
  os << "N = " << inst.n << endl;
  os << "M = " << inst.m << endl;
  os << "Jobs = " << inst.jobs << endl;
  return os;
}

// Job formatter
inline ostream &operator<<(ostream &os, const Job &job) {
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
