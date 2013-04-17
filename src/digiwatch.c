#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x2C, 0x2E, 0x88, 0x40, 0x71, 0x08, 0x4A, 0x9A, 0x96, 0xB0, 0xF8, 0x5D, 0x35, 0x93, 0x9E, 0xF6 }
PBL_APP_INFO(MY_UUID,
             "DigiWatch", "Plastik Apps",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);


Window window;

TextLayer text_time_layer;

int dx1 = 0;
int dy1 = 0;

int shiftDown = 1;
int shiftRight = 1;


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Simplicity");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(dx1, dy1, 144-7, 168-92));
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(&window.layer, &text_time_layer.layer);

}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)ctx;

  //calculate next position
  if (dy1 == 115) {
    shiftDown = 0;
  }
  if (dy1 == 0) {
    shiftDown = 1;
  }
  int dx2 = 144-7;
  if ((dx1+dx2) == 151) {
     shiftRight = 0;
  }
  if (dx1 == 0) {
     shiftRight = 1;
  }

  //move the time
  if (shiftDown == 1) {
     dy1 = dy1 + 1;
  } else {
     dy1 = dy1 - 1;
  }

  if (shiftRight == 1) {
    dx1 = dx1 + 1;
  } else {
    dx1 = dx1 - 1;
  }

  layer_set_frame(&text_time_layer.layer, GRect(dx1, dy1, 144-7, 168-92));

  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";

  char *time_format;
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(&text_time_layer, time_text);

}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
