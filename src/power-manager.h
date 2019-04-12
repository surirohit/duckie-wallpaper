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

#ifndef _LW_POWER_MANAGER_H_
#define _LW_POWER_MANAGER_H_

#include <libupower-glib/upower.h>

G_BEGIN_DECLS

#define LW_TYPE_POWER_MANAGER            (lw_power_manager_get_type())
#define LW_POWER_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj),   LW_TYPE_POWER_MANAGER, LwPowerManager))
#define LW_IS_POWER_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj),   LW_TYPE_POWER_MANAGER))
#define LW_POWER_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST    ((klass), LW_TYPE_POWER_MANAGER, LwPowerManagerClass))
#define LW_IS_POWER_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE    ((klass), LW_TYPE_POWER_MANAGER))
#define LW_POWER_MANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS  ((obj),   LW_TYPE_POWER_MANAGER, LwPowerManagerClass))

typedef struct _LwPowerManager LwPowerManager;
typedef struct _LwPowerManagerClass LwPowerManagerClass;

typedef struct _LwPowerManagerPrivate LwPowerManagerPrivate;

struct _LwPowerManager
{
	GObject parent_instance;

	/*< private >*/
	LwPowerManagerPrivate *priv;
};

struct _LwPowerManagerClass
{
	GObjectClass parent_class;
};

GType lw_power_manager_get_type(void);

G_END_DECLS

#endif /* _LW_POWER_MANAGER_H_ */
