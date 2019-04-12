/*
 *
 * LiveWallpaper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2012-2016 Maximilian Schnarr <Maximilian.Schnarr@googlemail.com>
 *
 */

#ifndef _LW_RANDOM_H_
#define _LW_RANDOM_H_
/**
 * SECTION: random
 * @Short_description: Various functions returning random numbers
 * @Title: Random Numbers
 *
 * The following functions provide an easy way to get random floating point
 * numbers. All functions using the internal <function>rand()</function> function
 * to generate random numbers.
 */


 /**
 * randf:
 *
 * Returns: A random floating point number between 0 and 1
 */
static inline gfloat
randf(void)
{
	return rand() / (gfloat) RAND_MAX;
}

/**
 * rand1f:
 * @max: A number to use as maximum
 *
 * Returns: A random floating point number between 0 and @max
 */
static inline gfloat
rand1f(gfloat max)
{
	return max * randf();
}

/**
 * rand2f:
 * @min: A number to use as minimum
 * @max: A number to use as maximum
 *
 * Returns: A random floating point number between @min and @max
 */
static inline gfloat
rand2f(gfloat min, gfloat max)
{
	return min + (max - min) * randf();
}

/**
 * clampf:
 * @num: A number
 * @min: A number to use as minimum
 * @max: A number to use as maximum
 *
 * Returns: @min if @num < @min, @max if @num > @max and @num in all other cases
 */
static inline gfloat
clampf(gfloat num, gfloat min, gfloat max)
{
	if(num > max) return max;
	else if(num < min) return min;
	return num;
}

#endif /* _LW_RANDOM_H_ */

