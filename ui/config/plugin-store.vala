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
public class PluginStore {
    public enum Cols {
        PLUGIN,
        ICON,
        NAME,
        PLUGIN_ROW,
        TOGGLE_ROW,
        TITLE,
        HEADING_ROW,
        SORT_KEY,
        VISIBILITY,
        WIDGET
    }
    private enum Pos {
        GENERAL,
        PLUGIN = 2
        /* EFFECTS = 4 */
    }

    public Gtk.ListStore store {get; private set;}

    public PluginStore (Builder b) {
        store = b.get_object ("store") as Gtk.ListStore;
        store.set_default_sort_func ((model, a, b) => {
            int    sa, sb;
            string na, nb;
            model.get (a, Cols.SORT_KEY, out sa, Cols.NAME, out na, -1);
            model.get (b, Cols.SORT_KEY, out sb, Cols.NAME, out nb, -1);
            return (sa != sb ? (sa > sb ? 1 : -1) : strcmp (na, nb));
        });
        store.set_sort_column_id (SortColumn.DEFAULT, SortType.ASCENDING);

        var model = new TreeModelFilter (store as TreeModel, null);
            model.set_visible_func ((model, it) => {
                bool visible;
                model.get (it, Cols.VISIBILITY, out visible, -1);
                return visible;
            });
        (b.get_object ("treeview") as TreeView).model = model;
    }

    public void add (Plugin p, Widget w) {
        bool is_general = p.module == "general";
        TreeIter it;
        store.append (out it);
        store.set (it,
                   Cols.PLUGIN,     p,
                   Cols.ICON,       p.get_icon(48),
                   Cols.NAME,       p.name,
                   Cols.PLUGIN_ROW, true,
                   Cols.TOGGLE_ROW, ! is_general && p.supported,
                   Cols.SORT_KEY,     is_general ? Pos.GENERAL : Pos.PLUGIN,
                   Cols.VISIBILITY, true,
                   Cols.WIDGET,     w,
                   -1);
    }

    public unowned Plugin activate_row (string strPath) {
        TreePath treePath = new TreePath.from_string(strPath);

        TreeIter it;
        (store as TreeModel).get_iter(out it, treePath);

        unowned Plugin p;
        (store as TreeModel).get(it, Cols.PLUGIN, out p, -1);
        (store as TreeModel).row_changed (treePath, it);
        return p;
    }
}
}
