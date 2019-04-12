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

#include <stdio.h>
#include <math.h>

#define STEPS 1024
#define _PI (float) 3.141593

int main(void)
{
	int i;
	float step = (_PI) / (2*STEPS);

	printf("/* This file is automatically generated by tools/sine-table-generator - do not edit manually */\n\n");
	printf("#define STEPS %i\n\n", STEPS);
	printf("const float step = %ff;\n", step);
	printf("const float sin_values[%i] =\n{\n", STEPS);
	for(i = 0; i < STEPS; i++)
	{
		printf("\t%f", sin(i*step));
		if(i != STEPS - 1)
			printf(",\n");
	}
	printf("\n};\n\n");

	return 0;
}

