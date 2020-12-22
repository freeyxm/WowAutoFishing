#include "DTW.h"
#include <cstring>


template <typename Numeric>
DTW<Numeric>::DTW(int m, int n)
    : M(m)
    , N(n)
{
    m_dist = new Numeric * [m];
    m_cost = new Numeric * [m];
    for (size_t i = 0; i < m; i++)
    {
        m_dist[i] = new Numeric[n];
        m_cost[i] = new Numeric[n];
    }
}

template <typename Numeric>
DTW<Numeric>::~DTW()
{
    for (size_t i = 0; i < M; i++)
    {
        delete[] m_dist[i];
        delete[] m_cost[i];
    }
    delete[] m_dist;
    delete[] m_cost;
}

template <typename Numeric>
Numeric DTW<Numeric>::Calculate(const Numeric* v1, size_t len1, const Numeric* v2, size_t len2)
{
    if (len1 > M)
        len1 = M;
    if (len2 > N)
        len2 = N;

    // Çå0
    for (size_t i = 0; i < M; ++i)
    {
        for (size_t j = 0; j < N; ++j)
        {
            m_dist[i][j] = 0;
            m_cost[i][j] = 0;
        }
    }

    // Ö¡Æ¥Åä¾àÀë¾ØÕó
    for (size_t i = 0; i < len1; i++)
    {
        for (size_t j = 0; j < len2; j++)
        {
            Numeric dist = v1[i] - v2[j];
            m_dist[i][j] = abs(dist);
        }
    }

    // ³õÊ¼»¯ÀÛ»ý¾àÀë¾ØÕó
    m_cost[0][0] = m_dist[0][0];
    for (size_t i = 1; i < len1; i++)
    {
        m_cost[i][0] = m_cost[i - 1][0] + m_dist[i][0];
    }
    for (size_t j = 1; j < len2; j++)
    {
        m_cost[0][j] = m_cost[0][j - 1] + m_dist[0][j];
    }
    // ÀÛ»ý¾àÀë¾ØÕó
    Numeric d1, d2, d3;
    for (size_t i = 1; i < len1; i++)
    {
        for (size_t j = 1; j < len2; j++)
        {
            d1 = m_cost[i - 1][j];
            d2 = m_cost[i - 1][j - 1];
            d3 = m_cost[i][j - 1];
            m_cost[i][j] = m_dist[i][j] + min(min(d1, d2), d3);
        }
    }

    Numeric result = m_cost[len1 - 1][len2 - 1];
    return result;
}