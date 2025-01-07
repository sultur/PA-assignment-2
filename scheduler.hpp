
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
   * Main backfilling algorithm. Variant on the EASY algorithm.
   */
  void backfill(u32 curr_timestamp) {
    // Repeatedly check if the first job in the queue can be started,
    // start it if so
    while (job_queue.size() > 0 && job_queue[0].machines <= curr_m) {
      start_job(job_queue[0], curr_timestamp);
      job_queue.pop_front();
    }
    // Sort currently_running by the expected runtime
    sort(currently_running.begin(), currently_running.end(),
         Job::compare_expected_endtimes);
  }

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
};

#endif /* SCHEDULER_H */
