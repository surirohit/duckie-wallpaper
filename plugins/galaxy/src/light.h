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

#ifndef _GALAXY_LIGHT_H_
#define _GALAXY_LIGHT_H_

G_BEGIN_DECLS

#define GALAXY_TYPE_LIGHT_PROGRAM            (galaxy_light_program_get_type())
#define GALAXY_LIGHT_PROGRAM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GALAXY_TYPE_LIGHT_PROGRAM, GalaxyLightProgram))
#define GALAXY_IS_LIGHT_PROGRAM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GALAXY_TYPE_LIGHT_PROGRAM))
#define GALAXY_LIGHT_PROGRAM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GALAXY_TYPE_LIGHT_PROGRAM, GalaxyLightProgramClass))
#define GALAXY_IS_LIGHT_PROGRAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GALAXY_TYPE_LIGHT_PROGRAM))
#define GALAXY_LIGHT_PROGRAM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GALAXY_TYPE_LIGHT_PROGRAM, GalaxyLightProgramClass))

typedef struct _GalaxyLightProgram GalaxyLightProgram;
typedef struct _GalaxyLightProgramClass GalaxyLightProgramClass;

typedef struct _GalaxyLightProgramPrivate GalaxyLightProgramPrivate;

struct _GalaxyLightProgram
{
	LwProgram parent_instance;
	
	/*< private >*/
	GalaxyLightProgramPrivate *priv;
};

struct _GalaxyLightProgramClass
{
	LwProgramClass parent_class;
};

GType galaxy_light_program_get_type(void);

GalaxyLightProgram *galaxy_light_program_new();

void galaxy_light_program_set_uniform(GalaxyLightProgram *self);

G_END_DECLS

#endif /* _GALAXY_LIGHT_H_ */

