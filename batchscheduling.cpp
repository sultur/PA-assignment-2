#include "batch_instance.hpp"
#include "scheduler.hpp"
#include <iostream>

int main(int argc, char **argv) {
  // --- READ INPUT ---
  auto inst = Instance::parse(std::cin);
  // NOTE: The jobs are already sorted by release time, earliest first
  std::cerr << inst;

  // --- SIMULATE ONLINE SCHEDULER ---
  Scheduler scheduler(inst);
  int next_job_idx = 0;
  std::optional<Job> next_job_to_finish;

  while (next_job_idx < inst.jobs.size()) {
    Job next_job_to_release = inst.jobs[next_job_idx];
    next_job_to_finish =
        scheduler.next_job_to_finish(); // This is an optional value

    // Check which event happens first:
    // either a job finishes or the next job gets released

    if (next_job_to_finish.has_value() &&
        scheduler.job_ends_at(next_job_to_finish.value()) <=
            next_job_to_release.release_time) {

      // Some currently running job finishes before the next job gets released
      scheduler.finish_job(next_job_to_finish.value());

    } else {

      // New job gets released before any currently running job finishes
      scheduler.queue_job(next_job_to_release);
      ++next_job_idx;
    }
  }
  // Keep running until job queue is finished, meaning all jobs have
  // received a start time
  while (scheduler.has_jobs_in_queue()) {
    next_job_to_finish = scheduler.next_job_to_finish();
    scheduler.finish_job(next_job_to_finish.value());
  }

  // --- OUTPUT SOLUTION ---

  // Print n
  std::cout << inst.n;
  // Print ID's and start times
  for (u32 id = 1; id <= inst.n; id++) {
    std::cout << '\n' << id << ' ' << scheduler.start_times[id - 1];
  }
  // Flush stdout (don't use endl in loop, it's slower)
  std::cout << std::endl;
}

void queue_job(Job job, vector<Job> &currently_running,
               vector<u32> &start_times) {}
void finish_job(Job job, vector<Job> &currently_running,
                vector<u32> &start_times) {}
