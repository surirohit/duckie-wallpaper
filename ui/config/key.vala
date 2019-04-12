/*
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
 * Copyright (C) 2014-2016 RIVIÈRE Aurélien <aurelien.riv@gmail.com>
 *
 */
using Gtk;

namespace Lwc {
public class Key : Box {
    public Key (GLib.Settings settings, string name, string summary, string description, Widget w, bool responsive=false) {
        Object (orientation: Orientation.HORIZONTAL, spacing: 0);
            set ("margin", 3, null);
            tooltip_text = description;

        pack_start (new Label (summary), false, false, 0);

        var ctl = new Button.from_icon_name ("edit-clear", IconSize.BUTTON);
            ctl.set_data ("key", name);
            ctl.tooltip_text = _("Reset key to the default value");
            ctl.focus_on_click = false;
            ctl.clicked.connect (() => {
                settings.reset (ctl.get_data ("key"));
            });
        pack_end (ctl, false, false);

#if GTK_3_14_OR_HIGHER
        if (responsive)
            w.margin_start=100;
#endif
        pack_end (w, responsive, responsive, 3);
    }
}
}
