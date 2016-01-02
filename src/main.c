#include <pebble.h>

#include "config.h"
#include "windows/main_window.h"


typedef struct event_type {

  int sticker;
  int day;
  int from_minute;
  int to_minute;

} event_type;

static int s_hour, s_minute, s_day;
static int s_sticker = 0;

static int event_count = 0;
struct event_type events[MAX_EVENTS];

static int b64(char ch) {
  return (int) ch - 65;
}

static void parse_events(char* es) {
  event_count = strlen(es) / EVENT_FEATURE_NUM;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Event count %d", event_count);

  for (int i = 0; i < event_count * EVENT_FEATURE_NUM; i = i + 6) {
      event_type event = {b64(es[i]), b64(es[i + 1]), b64(es[i + 2]) * 60 + b64(es[i + 3]) * 5, b64(es[i + 4]) * 60 + b64(es[i + 5]) * 5};

      if ((event.to_minute < event.from_minute) || (event.to_minute == (23 * 60 + 55))) {
        event.to_minute = 60 * 24;
      }
      events[i / 6] = event;


      APP_LOG(APP_LOG_LEVEL_DEBUG, "Event %s", es);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Event %d %d %d %d", event.sticker, event.day, event.from_minute, event.to_minute);
  }
}

static void update() {

  int sticker = 0;

  int day_minutes = s_hour *  60 + s_minute;

  event_type event;

  for (int i = 0; i < event_count; i++) {
    event = events[i];

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Find event %d", event.sticker);

    if (((event.day == 0) || (event.day == s_day + 1) || (event.day == 8 && s_day > 0 && s_day < 6) || (event.day == 9 && (s_day == 0 || s_day == 6))) && day_minutes >= event.from_minute && day_minutes <= event.to_minute) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Event found %d", event.sticker);
      sticker = event.sticker;
    }
  }

  s_sticker = sticker;

  main_window_update(s_hour, s_minute, s_sticker);

}

static void in_recv_handler(DictionaryIterator *iterator, void *context) {
  //Get Tuple
  Tuple *t = dict_read_first(iterator);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message key %lu", t->key);

  char* es;
  if(t) {
    switch(t->key) {
    case KEY_EVENTS:

      es = t->value->cstring;

      APP_LOG(APP_LOG_LEVEL_DEBUG, "Storage write %s", es);
      persist_write_string(KEY_EVENTS, es);

      parse_events(es);

      update();

      break;
    default:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "No match");
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "No tuple");
  }
}


static void tick_handler(struct tm *time_now, TimeUnits changed) {
  s_hour = time_now->tm_hour;
  s_minute = time_now->tm_min;
  s_day = time_now->tm_wday;

  update();

}

static void init() {
  main_window_push();

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  char* es = "FIJASAGJAAXLGISAXAJAXAXLJAAAIABAIAJABAMANABAUAVAHGSAUAKEWAXACDSGTGCFSGTG";

  persist_read_string(KEY_EVENTS, es, MAX_EVENTS * EVENT_FEATURE_NUM);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Storage read %s", es);

  parse_events(es);
}

static void deinit() {
  tick_timer_service_unsubscribe();
}

int main() {
  init();
  app_event_loop();
  deinit();
}

