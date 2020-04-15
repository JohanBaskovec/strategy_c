#include "job_priority_queue.h"
#include "job_type.h"

JobPriorityQueue
jobPriorityQueueCreate(){
    JobPriorityQueue q;
    for (int i = 0 ; i < JOB_PRIORITY_NUMBER ; i++) {
        JobTypeArray a = arrayCreate();
        q.jobsByPriority[i] = a;
    }
    // set all the jobs to maximum priority
    for (int i = 0 ; i < JOB_NUMBER ; i++) {
        arrayAdd(&q.jobsByPriority[0], i);
        q.jobPriorities[0] = 0;
    }
    return q;
}

void
jobPriorityQueueSetJobPriority(
        JobPriorityQueue *q
        , JobType jobType
        , int nextPriority
) {
    int currentPriority = q->jobPriorities[jobType];
    JobTypeArray *a = &q->jobsByPriority[currentPriority];
    for (int i = 0 ; i < a->length ; i++) {
        if (a->data[i] == jobType) {
            arrayRemove(a, i);
            break;
        }
    }
    arrayAdd(&q->jobsByPriority[nextPriority], jobType);
    q->jobPriorities[jobType] = nextPriority;
}
void
jobPriorityQueueFree(JobPriorityQueue *q) {
    for (int i = 0 ; i < JOB_PRIORITY_NUMBER ; i++) {
        arrayFree(q->jobsByPriority[i]);
    }
}

float
jobPriorityQueueGetJobPriority(
        JobPriorityQueue *q
        , JobType jobType
) {
    return q->jobPriorities[jobType];
}
