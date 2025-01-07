
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "batch_instance.hpp"
#include <algorithm>
#include <deque>
#include <optional>

using namespace std;

class Scheduler {
public:
  vector<u32> start_times;
  vector<Job> currently_running;
  deque<Job> job_queue;
  int curr_m;

  Scheduler(Instance inst) {
    start_times.resize(inst.n);
    curr_m = inst.m;
  }

  /*
   * Return the currently running job that finishes first, if any.
   */
  optional<Job> next_job_to_finish() {
    if (currently_running.size() == 0) {
      return {};
    }
    // Finds a currently running job with the earliest actual end time
    return *min_element(currently_running.begin(), currently_running.end(),
                        Job::compare_actual_endtimes);
  }

  /*
   * Run when a job finishes. Removes job from currently running and
   * executes backfill algorithm.
   */
  void finish_job(Job job) {
    // The next few lines simply remove job from currently_running
    auto job_idx =
        find(currently_running.begin(), currently_running.end(), job);
    iter_swap(job_idx, currently_running.end() - 1);
    currently_running.pop_back();

    // Free up the machines that the job required
    curr_m += job.machines;

    // TODO: Check if profile can be compressed instead of calling backfill

    // Run backfill algorithm
    backfill(job.actual_end());
  }

  /*
   * Run when job is released. Appends job to job queue and executes
   * backfill algorithm.
   */
  void queue_job(Job job) {
    // Add to job queue
    job_queue.push_back(job);

    // Run backfill algorithm
    backfill(job.release_time);
  }

  /*
   * Return true if there are still jobs either running or queued.
   */
  bool still_running() {
    return job_queue.size() > 0 || currently_running.size() > 0;
  }

private:
  /*
   * Start a job. Sets the start time of the job and subtracts the needed
   * machines.
   */
  void start_job(Job job, u32 timestamp) {
    // Set start time
    start_times[job.id - 1] = timestamp;
    job.set_start_time(timestamp);
    // Add to currently running
    currently_running.push_back(job);
    // Allocate resources
    curr_m -= job.machines;
  }

  /*
   * Main backfilling algorithm.
   */
  void backfill(u32 curr_timestamp) {
    u32 old_size = 0;
    // While the job queue is non-empty and alg_EASY modifies it,
    // run alg_EASY.
    while (job_queue.size() != old_size) {
      old_size = job_queue.size();
      alg_EASY(curr_timestamp);
    }
  }

  /* EASY backfilling algorithm */
  void alg_EASY(u32 curr_timestamp) {
    if (job_queue.size() == 0)
      return;

    if (job_queue.front().machines <= curr_m) { // First job in queue can run
      start_job(job_queue.front(), curr_timestamp);
      job_queue.pop_front();
      return;
    }

    if (job_queue.size() == 1) { // Can't backfill with only one job in queue
      return;
    }

    // Sort currently_running by the expected runtime
    sort(currently_running.begin(), currently_running.end(),
         Job::compare_expected_endtimes);

    Job job = job_queue.front();
    // Find first point where sufficient machines are available
    // for first job in queue
    u32 nodes = curr_m;
    u32 shadow_time = curr_timestamp;
    for (const auto &cr : currently_running) {
      nodes += cr.machines;
      shadow_time = cr.expected_end();
      if (nodes >= job.machines) {
        break;
      }
    }
    // How many extra machines will be available when the job can start
    int extra_nodes = nodes - job.machines;
    // Expected time when enough machines free up to start the job
    shadow_time -= curr_timestamp;

    // cerr << "Backfilling for job " << job.id << " at " << curr_timestamp
    //      << " with curr_m: " << curr_m << " extra-nodes: " << extra_nodes
    //      << " shadow-time: " << shadow_time << endl;
    // Find a backfill job (starting at index 1)
    for (auto it = job_queue.begin() + 1; it != job_queue.end(); ++it) {
      Job other = *it;

      if (other.machines <= curr_m) {
        // We would be able to start the other job
        if ((other.req_runtime <= shadow_time) ||
            (other.machines <= extra_nodes)) {
          // The other job finishes before the shadow time, or
          // requires few enough machines to not delay the first job
          start_job(other, curr_timestamp);
          job_queue.erase(it);
          return;
        }
      }
    }
  }
};

#endif /* SCHEDULER_H */
