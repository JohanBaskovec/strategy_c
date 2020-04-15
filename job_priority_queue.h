#ifndef ST_MIN_HEAP_H
#define ST_MIN_HEAP_H
#include "array.h"
#include "job_type.h"

ARRAY_DECLARE(JobType, JobTypeArray)

typedef struct JobPriorityQueue {
    int jobPriorities[JOB_NUMBER];
    JobTypeArray jobsByPriority[JOB_PRIORITY_NUMBER];
} JobPriorityQueue;

JobPriorityQueue
jobPriorityQueueCreate();

void
jobPriorityQueueSetJobPriority(
        JobPriorityQueue *q
        , JobType jobType
        , int nextPriority
);

void
jobPriorityQueueFree(JobPriorityQueue *q);
#endif
