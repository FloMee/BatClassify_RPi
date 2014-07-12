#ifndef RUNNING_STAT_H__
#define RUNNING_STAT_H__

class RunningStat {
    public:
        RunningStat();

        void Clear();
        void Push(double x);
        int NumDataValues() const;
        double Mean() const;
        double Variance() const;
        double StandardDeviation() const;

    private:
        int m_n;
        double m_oldM, m_newM, m_oldS, m_newS;
};

#endif  // RUNNING_STAT_H__
