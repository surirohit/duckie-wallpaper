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
public class Application : Gtk.Application {
    PluginStore _store;
    Grid        _content;
    SearchEntry _search;
    Plugin      _selected;

    public string active_plugin {get; set;}

    public Application () {
        Object (application_id: Config.LW_SETTINGS + ".livewallpaper-config",
                flags:          ApplicationFlags.FLAGS_NONE);
    }

    public override void activate () {
        unowned List<Gtk.Window> l = this.get_windows ();
        if (l != null)
            l.data.present();
        else
            create_window ();
    }

//------------------------------------ GModules --------------------------------
    public static void lw_switch_active (Switch w) {
        if (w.active)
            LwUI.launch_app ("pgrep ^livewallpaper$ > /dev/null || livewallpaper");
    }
    public static void opacity_changed (Scale  w, Window p) {
        p.opacity = w.get_value();
    }
    [CCode (instance_pos = -1)] public void show_plugin (TreeSelection selection) {
        TreeModel model;
        TreeIter it;
        if (selection.get_selected(out model, out it)) {
            Plugin p;
            Stack w;
            model.get(it, PluginStore.Cols.PLUGIN, out p,
                          PluginStore.Cols.WIDGET, out w, -1);
            if (p == _selected)
                return;

            (_content.get_child_at (0,0) as Image).pixbuf = p.get_icon(48);
            (_content.get_child_at (1,0) as Label).label  = p._(p.name);
            (_content.get_child_at (1,1) as Label).label  = p._(p.description);
             _content.get_child_at (0,2).visible = ! p.supported;

            var switcher = (((_content.get_child_at (0,3) as Bin).get_child() as Bin).get_child() as StackSwitcher);
                switcher.stack = (w is Stack) ? w : null;
                switcher.foreach ((e) => {switcher.child_set(e, "expand", true, null);});

            var swStack = _content.get_child_at (0,4) as ScrolledWindow;
            var vpStack = swStack.get_child() as Viewport;
            if (vpStack is Viewport) {
                vpStack.remove (vpStack.get_child());
                vpStack.destroy();
            }
            swStack.add (w);
            w.show_all ();
            new Search (p, _search.text.strip().down(), w);

            _selected = p;
        }
    }
    [CCode (instance_pos = -1)] public void activate_plugin (CellRenderer cell, string path) {
        active_plugin = _store.activate_row (path).module;
    }
    [CCode (instance_pos = -1)] public void show_plugin_info (Button w) {
        var p = _selected;
        var dialog = new AboutDialog ();
            dialog.set ("program-name", p.name,
                        "comments",     p.description,
                        "logo",         p.get_icon(64),
                        "website",      p.website,
                        "version",      p.version,
                        "copyright",    p.copyright,
                        "license",      p.license,
                        "authors",      p.authors,
                        "artists",      p.artists,
                        "documenters",  p.documenters,
                        "license-type", p.license_type,
                        "wrap-license", true,
                        "translator-credits", p._("translator-credits"),
                        null);
            dialog.transient_for = w.get_toplevel() as Window;
            dialog.response.connect ((rt) => {
                if (rt == ResponseType.CANCEL || rt == ResponseType.DELETE_EVENT)
                    dialog.destroy();
            });
        dialog.run();
    }
    [CCode (instance_pos = -1)] public void search (Entry e) {
        _content.show_all ();
        string needle = e.text.strip().down();
        (_store.store as TreeModel).foreach ((model, path, it) => {
            bool visible = true;
            if (needle.length > 0) {
                Plugin p;
                Widget w;
                model.get (it, PluginStore.Cols.PLUGIN, out p, PluginStore.Cols.WIDGET, out w, -1);
                if (p is Plugin)
                    new Search (p, needle, w, p != _selected, out visible);
            }
            (model as Gtk.ListStore).set(it, PluginStore.Cols.VISIBILITY, visible, -1);
            return false;
        });
    }
//--------------------------------- Private methods ----------------------------
    private void create_window () {
        GLib.Settings settings = new GLib.Settings (Config.LW_SETTINGS);
        Builder b = new Builder.from_resource ("/net/launchpad/livewallpaper/ui/config/livewallpaper-config.ui");
        b.connect_signals (this);

        _store   = new PluginStore (b);
        _content = b.get_object ("grid")   as Grid;
        _search  = b.get_object ("search") as SearchEntry;

        var window = b.get_object ("window") as Window;
            window.application = this;

        var lw = b.get_object ("lw_switch") as Switch;
            settings.bind ("active", lw, "active", SettingsBindFlags.DEFAULT);
            Bus.watch_name (BusType.SESSION, Config.LW_BUS, BusNameWatcherFlags.NONE,
                            () => {lw.active = settings.get_boolean("active");},
                            () => {lw.active = false;});

        settings.bind ("active-plugin", this, "active-plugin", SettingsBindFlags.DEFAULT);
        (b.get_object ("tvcolumn") as CellLayout).set_cell_data_func (
            b.get_object("plugin_cell_radio") as CellRenderer,
            (layout, cell, model, it) => {
                unowned Plugin p;
                model.get(it, PluginStore.Cols.PLUGIN, out p, -1);
                if (p is Plugin)
                    cell.set ("active", (p.module == active_plugin), null);
        });
        (b.get_object ("tvselection") as TreeSelection).set_select_function (
            (selection, model, path) => {
                TreeIter it;
                model.get_iter(out it, path);
                bool is_plugin;
                model.get(it, PluginStore.Cols.PLUGIN_ROW, out is_plugin, -1);
                return is_plugin;
        });

        load_plugins ();

        TreePath first = new TreePath.first ();
        (b.get_object ("treeview") as TreeView).set_cursor(first, null, false);

        window.show_all ();
    }
    private void load_plugins  () {
        SList<Plugin> l = Plugin.find_plugins();
        try {
            l.prepend (new Plugin (Config.LIBDIR + "livewallpaper/livewallpaper.plugin"));
        } catch (Error e) {
            critical (Config.LIBDIR + "livewallpaper/livewallpaper.plugin: " + e.message);
        }
        l.foreach ((p) => {
            try {
                _store.add (p, new PluginParser(p).parse());
            } catch (Error e) {
                _store.add (p, new Label (e.message));
            }
        });
    }
}
}
