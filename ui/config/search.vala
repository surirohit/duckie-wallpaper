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
using LwUI;

namespace Lwc {
public class Search {
    bool   dry_run;
    string needle;
    Plugin p;

    public Search (Plugin p, string needle, Widget w, bool dry_run=false, out bool? ret=null) {
        this.dry_run = dry_run;
        this.needle = needle;
        this.p = p;
        ret = emit_signal (w) || needle in (p.module + p.name + p.description).down();
    }

   private bool emit_signal (Widget w) {
        bool ret = false;
        for (Type t = w.get_type(); t != 0; t = t.parent())
            if (Signal.lookup (t.name().down(), typeof(Search)) > 0) {
                Signal.emit_by_name (this, t.name().down(), out ret, w);
                break;
            }
        return ret;
    }

    public virtual signal void gtkstack     (out bool ret, Stack w) {
        bool stack_visible = false;
        w.foreach((item) => {
            bool visible = false;
            (item as Container).get_children().foreach ((child) => {
                visible |= emit_signal (child);
            });
            if (! visible) {
                unowned string title;
                w.child_get(item, "title", out title, null);
                visible = needle in title.down ();
            }
            if (!dry_run)
                item.visible = visible;
            stack_visible |= visible;
        });
        if (!dry_run)
            w.visible = stack_visible;
        ret = stack_visible;
    }

    private extern bool needle_in_ssk(string schema_id, string key, string needle);

    public virtual signal void lwckey       (out bool ret, Key w) {
        if (! (ret = needle in w.tooltip_text.down())) {
            unowned string key = w.get_children().nth_data(2).get_data ("key");
            ret = needle in key ||
                  needle in p.settings.get_value(key).print(false).down();
            #if GTK_3_14_OR_HIGHER
                var ssk = SettingsSchemaSource.get_default().lookup(p.schema_id, true).get_key (key);
                ret |= needle in ssk.get_summary()     ||
                       needle in ssk.get_description() ||
                       needle in ssk.get_range().print(false).down();
            #else
                ret |= needle_in_ssk(p.schema_id, key, needle);
            #endif
        }
    }

    public virtual signal void gtkcontainer (out bool ret, Container w) {
        bool visible = false;
        w.get_children().foreach ((child) => {
            visible |= emit_signal (child);
        });
        if (! visible && (w is Frame || w is Expander)) {
            Value label = Value(typeof(string));
            w.get_property ("label", ref label);
            visible = needle in label.get_string().down();
        }
        if (!dry_run)
            w.visible = visible;
        ret = visible;
    }

    public virtual signal void gtklabel     (out bool ret, Label w) {
        ret = needle in w.label.down();
    }
}
}
