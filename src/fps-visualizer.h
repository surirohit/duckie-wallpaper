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

#ifndef _LW_FPS_VISUALIZER_H_
#define _LW_FPS_VISUALIZER_H_

G_BEGIN_DECLS

#define LW_TYPE_FPS_VISUALIZER            (lw_fps_visualizer_get_type())
#define LW_FPS_VISUALIZER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj),   LW_TYPE_FPS_VISUALIZER, LwFPSVisualizer))
#define LW_IS_FPS_VISUALIZER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj),   LW_TYPE_FPS_VISUALIZER))
#define LW_FPS_VISUALIZER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST    ((klass), LW_TYPE_FPS_VISUALIZER, LwFPSVisualizerClass))
#define LW_IS_FPS_VISUALIZER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE    ((klass), LW_TYPE_FPS_VISUALIZER))
#define LW_FPS_VISUALIZER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS  ((obj),   LW_TYPE_FPS_VISUALIZER, LwFPSVisualizerClass))

typedef struct _LwFPSVisualizer LwFPSVisualizer;
typedef struct _LwFPSVisualizerClass LwFPSVisualizerClass;

typedef struct _LwFPSVisualizerPrivate LwFPSVisualizerPrivate;

struct _LwFPSVisualizer
{
	GObject parent_instance;

	/*< private >*/
	LwFPSVisualizerPrivate *priv;
};

struct _LwFPSVisualizerClass
{
	GObjectClass parent_class;
};

GType lw_fps_visualizer_get_type(void);

LwFPSVisualizer *lw_fps_visualizer_new(LwClock *clock);
void lw_fps_visualizer_paint(LwFPSVisualizer *self, LwOutput *output);

G_END_DECLS

#endif /* _LW_FPS_VISUALIZER_H_ */

