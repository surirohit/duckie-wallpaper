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
 * Copyright (C) 2012-2016 Aurélien   Rivière <aurelien.riv@gmail.com>
 *
 */

#ifndef _DUCKIEGALAXY_LIGHT_H_
#define _DUCKIEGALAXY_LIGHT_H_

G_BEGIN_DECLS

#define DUCKIEGALAXY_TYPE_LIGHT_PROGRAM            (duckiegalaxy_light_program_get_type())
#define DUCKIEGALAXY_LIGHT_PROGRAM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DUCKIEGALAXY_TYPE_LIGHT_PROGRAM, DuckieGalaxyLightProgram))
#define DUCKIEGALAXY_IS_LIGHT_PROGRAM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DUCKIEGALAXY_TYPE_LIGHT_PROGRAM))
#define DUCKIEGALAXY_LIGHT_PROGRAM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DUCKIEGALAXY_TYPE_LIGHT_PROGRAM, DuckieGalaxyLightProgramClass))
#define DUCKIEGALAXY_IS_LIGHT_PROGRAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DUCKIEGALAXY_TYPE_LIGHT_PROGRAM))
#define DUCKIEGALAXY_LIGHT_PROGRAM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DUCKIEGALAXY_TYPE_LIGHT_PROGRAM, DuckieGalaxyLightProgramClass))

typedef struct _DuckieGalaxyLightProgram DuckieGalaxyLightProgram;
typedef struct _DuckieGalaxyLightProgramClass DuckieGalaxyLightProgramClass;

typedef struct _DuckieGalaxyLightProgramPrivate DuckieGalaxyLightProgramPrivate;

struct _DuckieGalaxyLightProgram
{
	LwProgram parent_instance;

	/*< private >*/
	DuckieGalaxyLightProgramPrivate *priv;
};

struct _DuckieGalaxyLightProgramClass
{
	LwProgramClass parent_class;
};

GType duckiegalaxy_light_program_get_type(void);

DuckieGalaxyLightProgram *duckiegalaxy_light_program_new();

void duckiegalaxy_light_program_set_uniform(DuckieGalaxyLightProgram *self);

G_END_DECLS

#endif /* _GALAXY_LIGHT_H_ */
