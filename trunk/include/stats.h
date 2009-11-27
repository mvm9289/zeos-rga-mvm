#ifndef STATS_H
#define STATS_H

/* Structure used by 'get_stats' function */
struct stats {
    int total_tics; /* Number of times the process has got the CPU */
    int total_trans; /* READY->RUN transitions */
    int remaining_tics;
};

#endif /* !STATS_H */
