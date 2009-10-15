#ifndef STATS_H
#define STATS_H

/* Structure used by 'get_stats' function */
struct stats
{
    int tics;
    int ts; /* Number of times the process has got the CPU: READY->RUN transitions */
};
#endif /* !STATS_H */
