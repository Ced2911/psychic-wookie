/*
 * rational numbers
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of Libav.
 *
 * Libav is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Libav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Libav; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * rational numbers
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#include "avassert.h"
//#include <math.h>
#include <limits.h>

#include "common.h"
#include "mathematics.h"
#include "rational.h"

int av_reduce(int *dst_num, int *dst_den,
              int64_t num, int64_t den, int64_t max)
{
    AVRational a0 = { 0, 1 }, a1 = { 1, 0 };
    int sign = (num < 0) ^ (den < 0);
    int64_t gcd = av_gcd(((num) >= 0 ? (num) : (-(num))), ((den) >= 0 ? (den) : (-(den))));

    if (gcd) {
        num = ((num) >= 0 ? (num) : (-(num))) / gcd;
        den = ((den) >= 0 ? (den) : (-(den))) / gcd;
    }
    if (num <= max && den <= max) {
        { AVRational tmp__0 = { num, den }; a1 = tmp__0; }
        den = 0;
    }

    while (den) {
        uint64_t x        = num / den;
        int64_t next_den  = num - den * x;
        int64_t a2n       = x * a1.num + a0.num;
        int64_t a2d       = x * a1.den + a0.den;

        if (a2n > max || a2d > max) {
            if (a1.num) x =          (max - a0.num) / a1.num;
            if (a1.den) x = ((x) > ((max - a0.den) / a1.den) ? ((max - a0.den) / a1.den) : (x));

            if (den * (2 * x * a1.den + a0.den) > num * a1.den)
                { AVRational tmp__1 = { x * a1.num + a0.num, x * a1.den + a0.den }; a1 = tmp__1; }
            break;
        }

        a0  = a1;
        { AVRational tmp__2 = { a2n, a2d }; a1  = tmp__2; }
        num = den;
        den = next_den;
    }
    ((void)0);

    *dst_num = sign ? -a1.num : a1.num;
    *dst_den = a1.den;

    return den == 0;
}

AVRational av_mul_q(AVRational b, AVRational c)
{
    av_reduce(&b.num, &b.den,
               b.num * (int64_t) c.num,
               b.den * (int64_t) c.den, INT_MAX);
    return b;
}

AVRational av_div_q(AVRational b, AVRational c)
{
    { AVRational tmp__3 = { c.den, c.num }; return av_mul_q(b, tmp__3); }
}

AVRational av_add_q(AVRational b, AVRational c) {
    av_reduce(&b.num, &b.den,
               b.num * (int64_t) c.den +
               c.num * (int64_t) b.den,
               b.den * (int64_t) c.den, INT_MAX);
    return b;
}

AVRational av_sub_q(AVRational b, AVRational c)
{
    { AVRational tmp__4 = { -c.num, c.den }; return av_add_q(b, tmp__4); }
}

AVRational av_d2q(double d, int max)
{
    AVRational a;

    int exponent;
    int64_t den;
    if (isnan(d))
        { AVRational tmp__5 = { 0,0 }; return tmp__5; }
    if (isinf(d))
        { AVRational tmp__6 = { d < 0 ? -1 : 1, 0 }; return tmp__6; }
    exponent = (((int)(log(fabs(d) + 1e-20)/0.69314718055994530941723212145817656807550013436025)) > (0) ? ((int)(log(fabs(d) + 1e-20)/0.69314718055994530941723212145817656807550013436025)) : (0));
    den = 1LL << (61 - exponent);
    av_reduce(&a.num, &a.den, (int64_t)(d * den + 0.5), den, max);

    return a;
}

int av_nearer_q(AVRational q, AVRational q1, AVRational q2)
{
    /* n/d is q, a/b is the median between q1 and q2 */
    int64_t a = q1.num * (int64_t)q2.den + q2.num * (int64_t)q1.den;
    int64_t b = 2 * (int64_t)q1.den * q2.den;

    /* rnd_up(a*d/b) > n => a*d/b > n */
    int64_t x_up = av_rescale_rnd(a, q.den, b, AV_ROUND_UP);

    /* rnd_down(a*d/b) < n => a*d/b < n */
    int64_t x_down = av_rescale_rnd(a, q.den, b, AV_ROUND_DOWN);

    return ((x_up > q.num) - (x_down < q.num)) * av_cmp_q(q2, q1);
}

int av_find_nearest_q_idx(AVRational q, const AVRational* q_list)
{
    int i, nearest_q_idx = 0;
    for (i = 0; q_list[i].den; i++)
        if (av_nearer_q(q, q_list[i], q_list[nearest_q_idx]) > 0)
            nearest_q_idx = i;

    return nearest_q_idx;
}
