#include "main_window.h"

static Window *s_window;
static Layer *s_canvas;

static int s_hours, s_minutes;
static int s_sticker = -1;

static BitmapLayer *sticker_layer;

static GBitmap *sticker_bitmap;

//static GDrawCommandImage *s_command_image;

static int32_t get_angle_for_hour(int hour) {
  // Progress through 12 hours, out of 360 degrees
  return (hour * 360) / 12;
}

static int32_t get_angle_for_minute(int minute) {
  // Progress through 60 minutes, out of 360 degrees
  return (minute * 360) / 60;
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // 12 hours only, with a minimum size
  s_hours -= (s_hours > 12) ? 12 : 0;

  // Minutes are expanding circle arc
  int minute_angle = get_angle_for_minute(s_minutes);
  GRect frame = grect_inset(bounds, GEdgeInsets(4 * INSET));
  graphics_context_set_fill_color(ctx, MINUTES_COLOR);
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 20, 0, DEG_TO_TRIGANGLE(minute_angle));

  // Adjust geometry variables for inner ring
  frame = grect_inset(frame, GEdgeInsets(3 * HOURS_RADIUS));

  // Hours are dots
  for(int i = 1; i < 13; i++) {
    int hour_angle = get_angle_for_hour(i);
    GPoint pos = gpoint_from_polar(frame, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(hour_angle));

    graphics_context_set_fill_color(ctx, i <= s_hours ? HOURS_COLOR : HOURS_COLOR_INACTIVE);
    graphics_fill_circle(ctx, pos, HOURS_RADIUS);
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, layer_update_proc);
  layer_add_child(window_layer, s_canvas);

  sticker_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_compositing_mode(sticker_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(sticker_layer));
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  window_destroy(s_window);
  bitmap_layer_destroy(sticker_layer);
}

void main_window_push() {
  s_window = window_create();
  window_set_background_color(s_window, BG_COLOR);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}

void main_window_update(int hours, int minutes, int sticker) {
  s_hours = hours;
  s_minutes = minutes;

  if (s_sticker != sticker) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Sticker changed %d", sticker);
    vibes_short_pulse();
    gbitmap_destroy(sticker_bitmap);

    int res = RESOURCE_ID_DEFAULT;
    GColor color = GColorDukeBlue;
    switch(sticker) {
    case 1:
      res = RESOURCE_ID_FOOD;
      color = GColorFolly;
      break;
    case 2:
      res = RESOURCE_ID_SPORT;
      color = GColorGreen;
      break;
    case 3:
      res = RESOURCE_ID_DRINK;
      color = GColorPictonBlue;
      break;
    case 4:
      res = RESOURCE_ID_LEARN;
      color = GColorImperialPurple;
      break;
    case 5:
      res = RESOURCE_ID_WORK;
      color = GColorRed;
      break;
    case 6:
      res = RESOURCE_ID_REST;
      color = GColorTiffanyBlue;
      break;
    case 7:
      res = RESOURCE_ID_BEER;
      color = GColorChromeYellow;
      break;
    case 8:
      res = RESOURCE_ID_FRIENDS;
      color = GColorWindsorTan;
      break;
    case 9:
      res = RESOURCE_ID_BEDTIME;
      color = GColorOxfordBlue;
      break;
    case 10:
      res = RESOURCE_ID_SEX;
      color = GColorBrilliantRose;
      break;
    case 11:
      res = RESOURCE_ID_REMINDER;
      color = GColorRed;
      break;
    default:
      color = GColorDukeBlue;
      res = RESOURCE_ID_DEFAULT;
    }
    sticker_bitmap = gbitmap_create_with_resource(res);
    window_set_background_color(s_window, color);

    bitmap_layer_set_bitmap(sticker_layer, sticker_bitmap);
  }

  s_sticker = sticker;

  layer_mark_dirty(s_canvas);
}
