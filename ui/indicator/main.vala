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

[Compact]
public class LwIndicator {
    static int main (string[] args) {
        Gtk.init (ref args);
        Intl.setlocale (LocaleCategory.ALL, "");
        Intl.textdomain (Config.GETTEXT_PACKAGE);
        Intl.bindtextdomain (Config.GETTEXT_PACKAGE, Config.DATADIR + "locale");

        var settings = new GLib.Settings (Config.LW_SETTINGS);
        var menu = new Gtk.Menu();
            var lw = new CheckMenuItem.with_label (_("LiveWallpaper"));
                lw.notify["active"].connect (() => {
                    if (lw.active)
                        launch_app ("pgrep ^livewallpaper$ > /dev/null || livewallpaper");
                });
                settings.bind ("active", lw, "active", SettingsBindFlags.DEFAULT);
                Bus.watch_name (BusType.SESSION, Config.LW_BUS, BusNameWatcherFlags.NONE,
                                () => {lw.active = true;}, () => {lw.active = false;});
            menu.append (lw);

            menu.append (new SeparatorMenuItem());
            load_plugins(menu, settings);
            menu.append (new SeparatorMenuItem());

            var lwc = new Gtk.MenuItem.with_label (_("LiveWallpaper Settings..."));
                lwc.activate.connect (() => {launch_app ("livewallpaper-config");});
            menu.append (lwc);
        menu.show_all ();

        const string icon_name = "livewallpaper-indicator";
#if USE_APPINDICATOR
        var ind = new AppIndicator.Indicator(icon_name, icon_name,
                      AppIndicator.IndicatorCategory.APPLICATION_STATUS);
        ind.set_status(AppIndicator.IndicatorStatus.ACTIVE);
        ind.set_menu(menu);
#else
        var si = new StatusIcon.from_icon_name (icon_name);
        si.activate.connect (() => {
            menu.popup (null, null, si.position_menu, 0, Gtk.get_current_event_time());
        });
#endif
        Gtk.main ();
        return 0;
    }

    private static void load_plugins (Gtk.Menu menu, GLib.Settings s) {
        unowned SList<RadioMenuItem> group = null;
        SList<Plugin> l = Plugin.find_plugins();
        l.sort ((p1, p2) => {return strcmp (p1.name, p2.name);});
        l.foreach ((p) => {
            var ctl = new RadioMenuItem.with_label (group, p._(p.name));
                ctl.set_tooltip_text (p._(p.description));
                ctl.sensitive = p.supported;
                ctl.set_data ("plugin", p.module);
                ctl.toggled.connect (() => {
                    if (ctl.active)
                        s.set_string ("active-plugin", p.module);
                });
            menu.append (ctl);
            group = ctl.get_group ();
        });
        s.changed["active-plugin"].connect (() => {
            string ap = s.get_string ("active-plugin");
            group.foreach ((item) => {
                if (ap == item.get_data<string> ("plugin"))
                    item.active = true;
            });
        });
        Signal.emit_by_name (s, "changed::active-plugin");
    }
}
