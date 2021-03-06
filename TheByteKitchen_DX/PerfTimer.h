//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once
#include <windows.h>

struct PerfTimer
{
    void Start() 
    {
        QueryPerformanceCounter(&m_start);
    }

    void Stop() 
    {
        QueryPerformanceCounter(&m_stop);
    }
    
    // Returns elapsed time in milliseconds (ms)
    double Elapsed()
    {
        return (m_stop.QuadPart - m_start.QuadPart - m_overhead) \
                                          * 1000.0 / m_freq.QuadPart;
    }

private:

    // Returns the overhead of the timer in ticks
    static LONGLONG GetOverhead()
    {
        PerfTimer t;
        t.Start();
        t.Stop();
        return t.m_stop.QuadPart - t.m_start.QuadPart;
    }

    LARGE_INTEGER m_start;
    LARGE_INTEGER m_stop;
    static LARGE_INTEGER m_freq;
    static LONGLONG m_overhead;
};