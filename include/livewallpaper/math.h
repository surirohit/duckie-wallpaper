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
 * Copyright (C) 2013-2016 Maximilian Schnarr <Maximilian.Schnarr@googlemail.com>
 *
 */

 /**
 * SECTION: math
 * @Short_description: Some common mathematical functions
 * @Title: Common Functions
 *
 * The LiveWallpaper Core Library provides faster implementations of some common
 * mathematical functions.
 */

#ifndef _LW_MATH_H_
#define _LW_MATH_H_

/**
 * LW_PI:
 *
 * Value of pi as used by LiveWallpaper.
 *
 * Since: 0.5
 */

/**
 * LW_2PI:
 *
 * Value of 2 * pi as used by LiveWallpaper.
 *
 * Since: 0.5
 */

/**
 * LW_PI2:
 *
 * Value of pi / 2 as used by LiveWallpaper.
 *
 * Since: 0.5
 */

#define LW_PI  (gfloat) 3.141593
#define LW_2PI (gfloat) 6.283185
#define LW_PI2 (gfloat) 1.570796

extern const float step;
extern const float sin_values[1024];

/**
 * lw_sin:
 * @angle: The angle in the range -%LW_2PI <= @angle <= 0
 *
 * Calculates the sine value of an angle by using a lookup table. Using this function
 * is usually faster than computing the sine value with the sin() function of the standard
 * library.
 *
 * <warning>
 *   <para>
 *     Make sure to use this function only with angles in the allowed range. This function
 *     does not check if the angle is out of range and so the application will crash.
 *   </para>
 * </warning>
 *
 * Returns: The sine value of the @angle
 *
 * Since: 0.5
 */
static inline gfloat
lw_sin(gfloat angle)
{
	gint sign = -1;

	angle = -angle;
	/* 0 <= angle <= 2PI */

	if(angle > LW_PI)
	{
		sign = 1;
		angle = angle - LW_PI;
	}
	/* 0 <= angle <= PI */

	if(angle > LW_PI2)
		angle = LW_PI - angle;
	/* 0 <= angle <= PI/2 */

	return sign * sin_values[ ((gint) (angle / step)) ];
}

/**
 * lw_cos:
 * @angle: The angle in the range -%LW_2PI <= @angle <= 0
 *
 * Calculates the cosine value of an angle by using a lookup table. Using this function
 * is usually faster than computing the cosine value with the cos() function of the standard
 * library.
 *
 * <warning>
 *   <para>
 *     Make sure to use this function only with angles in the allowed range. This function
 *     does not check if the angle is out of range and so the application will crash.
 *   </para>
 * </warning>
 *
 * Returns: The cosine value of the @angle
 *
 * Since: 0.5
 */
static inline gfloat
lw_cos(gfloat angle)
{
	gint sign = 1;

	angle = -angle;
	/* 0 <= angle <= 2PI */

	/* cos(angle) = sin(angle + PI/2) */
	angle += LW_PI2;
	/* PI/2 <= angle <= 2PI + PI/2 */

	if(angle > LW_2PI)
		angle -= LW_2PI;
	/* 0 <= angle <= 2PI */

	if(angle > LW_PI)
	{
		sign = -1;
		angle = angle - LW_PI;
	}
	/* 0 <= angle <= PI */

	if(angle > LW_PI2)
		angle = LW_PI - angle;
	/* 0 <= angle <= PI/2 */

	return sign * sin_values[ ((gint) (angle / step)) ];
}

#endif /* _LW_MATH_H_ */
