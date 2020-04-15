#include "array.h"
#include "job_priority_queue.h"

TEST_START(jobPriorityQueueCreateTest)
    JobPriorityQueue q = jobPriorityQueueCreate();
TEST_END

TEST_START(jobPriorityQueuePopTest)
    JobPriorityQueue q = jobPriorityQueueCreate();
    jobPriorityQueueSetJobPriority(&q, JOB_DIGGING, 4);
    ASSERT_INT_EQUAL(4, q.jobPriorities[JOB_DIGGING]);

    for (int i = 0 ; i < JOB_NUMBER ; i++) {
        if (i != JOB_DIGGING) {
            ASSERT_INT_EQUAL(0, q.jobPriorities[i]);
        }
    }
    ASSERT_INT_EQUAL(JOB_NUMBER - 1, q.jobsByPriority[0].length);
    ASSERT_INT_EQUAL(1, q.jobsByPriority[4].length);
TEST_END

