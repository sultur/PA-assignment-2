
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

    // Compress profile
    compress_profile(job.actual_end());
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
    // Add to currently running and keep sorted
    currently_running.push_back(job);
    sort(currently_running.begin(), currently_running.end(),
         Job::compare_expected_endtimes);
    // Allocate resources
    curr_m -= job.machines;
  }

  /*
   * Main backfilling algorithm.
   */
  void backfill(u32 curr_timestamp) {
    Job job = job_queue.back();
    // Find anchor point for job
    u32 anchor = find_anchor(job, curr_timestamp);
    // Anchor found, update profile by setting jobs start time
    job.set_start_time(anchor);

    // Start job immediately if anchor is current timestamp
    if (anchor == curr_timestamp) {
      start_job(job, anchor);
      job_queue.pop_back();
    } else {
      // Otherwise sort the job queue
      sort(job_queue.begin(), job_queue.end(), Job::compare_start_times);
    }
  }

  /*
   * A job finished, possibly earlier than expected, reschedule queued jobs.
   */
  void compress_profile(u32 curr_timestamp) {

    if (job_queue.size() == 0) {
      return;
    }
    if (job_queue.front().machines <= curr_m) {
      start_job(job_queue.front(), curr_timestamp);
      job_queue.pop_front();
      return;
    }

    deque<Job> upcoming_jobs;
    swap(job_queue, upcoming_jobs); // backfill uses job_queue, clear
                                    // it and fill incrementally

    // Re-queue the jobs in the job queue, backfilling for some jobs
    for (auto job : upcoming_jobs) {
      job_queue.push_back(job);
      if (job.machines <= curr_m) {
        backfill(curr_timestamp);
      }
    }
  }

  /*
   * Return vector containing pairs of timestamps and how many
   * machines get reserved/freed. Sorted so the earliest timestamps
   * are last.
   */
  vector<pair<u32, int>> upcoming_events() {
    vector<pair<u32, int>> events;
    // Currently running jobs will end and release machines
    for (auto j : currently_running) {
      events.push_back(pair(j.expected_end(), j.machines));
    }

    // Queued jobs are planned to start at some time, requiring
    // machines, and end at some time, releasing machines
    for (auto it = job_queue.begin(); it != job_queue.end() - 1; ++it) {
      events.push_back(
          pair(it->start_time, -it->machines)); // Machines is negative here

      events.push_back(pair(it->expected_end(), it->machines));
    }
    // Sort events with earliest last
    sort(events.begin(), events.end());

    return events;
  }

  /*
   * Find anchor point for job, searching from curr timestamp.
   */
  u32 find_anchor(Job job, u32 curr_timestamp) {
    // Find anchor point for job
    u32 anchor = curr_timestamp;
    int avail_m = curr_m;

    auto events = upcoming_events();

    // We now have a list of upcoming events, containing a
    // timestamp and the number of machines that get used/freed
    while (events.size() > 0) {
      pair<u32, int> ev = events.back();
      events.pop_back();

      if (ev.first >= anchor + job.req_runtime && avail_m >= job.machines) {
        // Event happens later than job's requested runtime and the
        // currently available machines are enough, anchor is valid
        break;
      }
      // Update available machines from event
      avail_m += ev.second;
      if (avail_m < job.machines ||
          (avail_m >= job.machines && avail_m - ev.second < job.machines)) {
        // Either there aren't enough machines available, or the
        // current event timestamp made enough machines free to be a
        // possible anchor point
        anchor = ev.first;
      }
    }
    return anchor;
  }
};

#endif /* SCHEDULER_H */
