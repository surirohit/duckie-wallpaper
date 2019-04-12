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

#ifndef _LW_PLUGINS_ENGINE_H_
#define _LW_PLUGINS_ENGINE_H_

G_BEGIN_DECLS

#define LW_TYPE_PLUGINS_ENGINE            (lw_plugins_engine_get_type())
#define LW_PLUGINS_ENGINE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj),   LW_TYPE_PLUGINS_ENGINE, LwPluginsEngine))
#define LW_IS_PLUGINS_ENGINE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj),   LW_TYPE_PLUGINS_ENGINE))
#define LW_PLUGINS_ENGINE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST    ((klass), LW_TYPE_PLUGINS_ENGINE, LwPluginsEngineClass))
#define LW_IS_PLUGINS_ENGINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE    ((klass), LW_TYPE_PLUGINS_ENGINE))
#define LW_PLUGINS_ENGINE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS  ((obj),   LW_TYPE_PLUGINS_ENGINE, LwPluginsEngineClass))

typedef struct _LwPluginsEngine LwPluginsEngine;
typedef struct _LwPluginsEngineClass LwPluginsEngineClass;

typedef struct _LwPluginsEnginePrivate LwPluginsEnginePrivate;

struct _LwPluginsEngine
{
	PeasEngine parent_instance;

	/*< private >*/
	LwPluginsEnginePrivate *priv;
};

struct _LwPluginsEngineClass
{
	PeasEngineClass parent_class;
};

GType lw_plugins_engine_get_type(void);

LwPluginsEngine *lw_plugins_engine_new();

G_END_DECLS

#endif /* _LW_PLUGINS_ENGINE_H_ */

