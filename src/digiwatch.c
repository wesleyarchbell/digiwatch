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
TextLayer text_date_layer;

int dx1 = 0;
int dy1 = 0;

int dx2 = 22;
int dy2 = 50;

int shiftDown = 1;
int shiftRight = 1;

//set the speed of the bouncing text
int speed = 2;

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "DigiWatch");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  //init time layer
  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(dx1, dy1, 144-9, 168-92));
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OFFSIDE_49)));
  layer_add_child(&window.layer, &text_time_layer.layer);

  //init date layer
  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(dx2, dy2, 144-1, 168-68));
  text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OFFSIDE_21)));
  layer_add_child(&window.layer, &text_date_layer.layer);

}

void animateDateTime() {
  //check position
  if (dy1 >= 91) {
    shiftDown = 0;
  }
  if (dy1 <= 0) {
    shiftDown = 1;
  }
  int xx = 144-9;
  if ((dx1+xx) >= 155) {
     shiftRight = 0;
  }
  if (dx1 <= -1) {
     shiftRight = 1;
  }

  //calculate next x,y
  if (shiftDown == 1) {
     dy1 += speed;
     dy2 += speed;
  } else {
     dy1 -= speed;
     dy2 -= speed;
  }
  if (shiftRight == 1) {
    dx1 += speed;
    dx2 += speed;
  } else {
    dx1 -= speed;
    dx2 -= speed;
  }

  layer_set_frame(&text_time_layer.layer, GRect(dx1, dy1, 144-9, 168-92));
  layer_set_frame(&text_date_layer.layer, GRect(dx2, dy2, 144-9, 168-68));
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;

  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";

  string_format_time(date_text, sizeof(date_text), "%B %e", t->tick_time);
  text_layer_set_text(&text_date_layer, date_text);

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

  animateDateTime();
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
