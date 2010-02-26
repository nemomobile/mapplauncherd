/*
 * Copyright © 2005, 2007, 2008, 2009 Nokia Corporation
 *
 * Author: Guillem Jover <guillem.jover@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <gtk/gtk.h>
#include <hildon/hildon-program.h>
#include <hildon/hildon-window.h>

void
cleanup(void)
{
  g_print("called at exit time...\n");
}

int
main(int argc, char *argv[])
{
  GTimer *timer;
  HildonProgram *program;
  HildonWindow *window;
  int r;

  timer = g_timer_new();

  gtk_init(&argc, &argv);

  g_print("gtk_init() took %f seconds\n", g_timer_elapsed(timer, NULL));

  program = hildon_program_get_instance();
  g_set_application_name("app-hildon");

  window = HILDON_WINDOW(hildon_window_new());
  hildon_program_add_window(program, window);

  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(gtk_main_quit), NULL);

  g_print("creating widgets took %f seconds\n", g_timer_elapsed(timer, NULL));

  gtk_widget_show_all(GTK_WIDGET(window));
  g_print("showing widgets took %f seconds\n", g_timer_elapsed(timer, NULL));

  g_timer_destroy(timer);

  g_print("installing exit function...\n");
  r = atexit(cleanup);
  if (r)
    g_print("error while setting exit function (%d)\n", r);

  gtk_main();

  return 0;
}

