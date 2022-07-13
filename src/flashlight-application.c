/* flashlight-application.c
 *
 * Copyright 2022 Daniel Svitan
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
 */

#include "flashlight-application.h"
#include "flashlight-window.h"

struct _FlashlightApplication {
  GtkApplication parent_instance;
};

G_DEFINE_TYPE(FlashlightApplication, flashlight_application, GTK_TYPE_APPLICATION)

FlashlightApplication *flashlight_application_new(gchar *application_id, GApplicationFlags flags) {
  return g_object_new(FLASHLIGHT_TYPE_APPLICATION, "application-id", application_id, "flags", flags, NULL);
}

static void flashlight_application_finalize(GObject *object) {
  FlashlightApplication *self = (FlashlightApplication *)object;

  G_OBJECT_CLASS(flashlight_application_parent_class)->finalize(object);
}

static void flashlight_application_activate(GApplication *app) {
  GtkWindow *window;
  GtkCssProvider *provider;
  GdkDisplay *display;

  /* It's good practice to check your parameters at the beginning of the
   * function. It helps catch errors early and in development instead of
   * by your users.
   */
  g_assert(GTK_IS_APPLICATION(app));

  /* Get the current window or create one if necessary. */
  window = gtk_application_get_active_window(GTK_APPLICATION(app));
  if (window == NULL)
    window = g_object_new(FLASHLIGHT_TYPE_WINDOW, "application", app, NULL);

  provider = gtk_css_provider_new();
  display = gdk_display_get_default();
  gtk_css_provider_load_from_resource(provider, "/com/streamer272/Flashlight/flashlight-window.css");
  //gtk_css_provider_load_from_data(provider, style, -1);
  gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  /* Ask the window manager/compositor to present the window. */
  gtk_window_present(window);
}

static void flashlight_application_class_init(FlashlightApplicationClass *klass) {
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  GApplicationClass *app_class = G_APPLICATION_CLASS(klass);

  object_class->finalize = flashlight_application_finalize;

  /*
   * We connect to the activate callback to create a window when the application
   * has been launched. Additionally, this callback notifies us when the user
   * tries to launch a "second instance" of the application. When they try
   * to do that, we'll just present any existing window.
   */
  app_class->activate = flashlight_application_activate;
}

static void flashlight_application_show_about(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  FlashlightApplication *self = FLASHLIGHT_APPLICATION(user_data);
  GtkWindow *window = NULL;
  const gchar *authors[] = {"Daniel Svitan", NULL};

  g_return_if_fail(FLASHLIGHT_IS_APPLICATION(self));

  window = gtk_application_get_active_window(GTK_APPLICATION(self));

  gtk_show_about_dialog(window, "program-name", "Flashlight", "authors", authors, "version", "0.2.0", NULL);
}

static void flashlight_application_maximize(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  FlashlightApplication *self = FLASHLIGHT_APPLICATION(user_data);
  GtkWindow *window = NULL;
  
  g_return_if_fail(FLASHLIGHT_IS_APPLICATION(self));
  
  window = gtk_application_get_active_window(GTK_APPLICATION(self));

  gboolean is_maximized = gtk_window_is_fullscreen(window);
  if (is_maximized == TRUE) {
    gtk_window_unfullscreen(window);
  }
  else {
    gtk_window_fullscreen(window);
  }
}

static void flashlight_application_init(FlashlightApplication *self) {
  g_autoptr(GSimpleAction) quit_action = g_simple_action_new("quit", NULL);
  g_signal_connect_swapped(quit_action, "activate", G_CALLBACK(g_application_quit), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(quit_action));

  g_autoptr(GSimpleAction) about_action = g_simple_action_new("about", NULL);
  g_signal_connect(about_action, "activate", G_CALLBACK(flashlight_application_show_about), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(about_action));

  g_autoptr(GSimpleAction) maximize_action = g_simple_action_new("maximize", NULL);
  g_signal_connect(maximize_action, "activate", G_CALLBACK(flashlight_application_maximize), self);
  g_action_map_add_action(G_ACTION_MAP(self), G_ACTION(maximize_action));

  gtk_application_set_accels_for_action(GTK_APPLICATION(self), "app.quit", (const char *[]) { "<primary>q", NULL, });
  gtk_application_set_accels_for_action(GTK_APPLICATION(self), "app.about", (const char *[]) { "<primary>a", NULL, });
  gtk_application_set_accels_for_action(GTK_APPLICATION(self), "app.maximize", (const char *[]) { "<primary>m", NULL, });
}
