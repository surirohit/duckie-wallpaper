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
namespace LwUI {
public void launch_app (string app) {
    try {
        Process.spawn_command_line_async("/bin/bash -c '" + app + "'");
    } catch (Error e) {
        warning (e.message);
    }
}
public bool check_version_str (string version) {
    string[] strv = version.split(".");
    assert (strv.length == 3);
    return check_version_int (int.parse(strv[0]), int.parse(strv[1]), int.parse(strv[2]));
}
public bool check_version_int (uint major, uint minor = 0, uint micro = 0) {
    return (Config.MAJOR  > major ||
           (Config.MAJOR == major && Config.MINOR  > minor) ||
           (Config.MAJOR == major && Config.MINOR == minor && Config.MICRO >= micro));
}
}
