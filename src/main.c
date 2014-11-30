#include <pebble.h>
  
#define KEY_WIND_SPEED 0
#define KEY_WIND_DIR   1
#define KEY_TIME  2
#define KEY_WIND_SPEED1 3
#define KEY_WIND_DIR1   4
#define KEY_TIME1  5
#define KEY_WIND_SPEED2 6
#define KEY_WIND_DIR2   7
#define KEY_TIME2  8
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_weather_layer[3];
static TextLayer *s_weather_title[3];

//static GFont s_time_font;
static GFont s_weather_font, s_weather_title_font;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
  //Use 2h hour format
    strftime(buffer, sizeof("00:00:00"), "%H:%M:%S", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00:00"), "%I:%M:%S", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
  
  
  // Set up the weather areas
  // Use system font, apply it and add to Window
  s_weather_font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_weather_title_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  
  //Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 138, 144, 30));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  
  //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_weather_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

  // Set some intial string
  text_layer_set_text(s_time_layer, "00:00:00");  

  
  // Create temperature Layer
  s_weather_layer[0] = text_layer_create(GRect(0, 0, 144, 28));
  s_weather_title[0] = text_layer_create(GRect(0, 28, 144, 18));
  s_weather_layer[1] = text_layer_create(GRect(0, 46, 144, 28));
  s_weather_title[1] = text_layer_create(GRect(0, 74, 144, 18));
  s_weather_layer[2] = text_layer_create(GRect(0, 92, 144, 28));
  s_weather_title[2] = text_layer_create(GRect(0, 120, 144, 18));
  

  int i;
  for (i =0; i < 3; i++)
    {
    text_layer_set_background_color(s_weather_layer[i], GColorClear);
    text_layer_set_text_color(s_weather_layer[i], GColorWhite);
    text_layer_set_text_alignment(s_weather_layer[i], GTextAlignmentCenter);
    text_layer_set_text(s_weather_layer[i], "00-00 NNN");
    text_layer_set_font(s_weather_layer[i], s_weather_font);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer[i]));

    text_layer_set_background_color(s_weather_title[i], GColorClear);
    text_layer_set_text_color(s_weather_title[i], GColorWhite);
    text_layer_set_text_alignment(s_weather_title[i], GTextAlignmentCenter);
    text_layer_set_font(s_weather_title[i], s_weather_title_font);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_title[i]));

  }
  
  text_layer_set_text(s_weather_title[0], "North Head");
  text_layer_set_text(s_weather_title[1], "Sydney Harbour");
  text_layer_set_text(s_weather_title[2], "Fort Denison");
  
  // Make sure the time is displayed from the start
  update_time();
}



static void main_window_unload(Window *window) {
  //Unload GFont
  //fonts_unload_custom_font(s_time_font);
  
  //Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  //Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  // Destroy weather elements
  text_layer_destroy(s_weather_layer[0]);
  text_layer_destroy(s_weather_layer[1]);
  text_layer_destroy(s_weather_layer[2]);
  text_layer_destroy(s_weather_title[0]);
  text_layer_destroy(s_weather_title[1]);
  text_layer_destroy(s_weather_title[2]);

  //fonts_unload_custom_font(s_weather_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every 10 minutes
  if(tick_time->tm_min % 10 == 6) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char wind_speed_buffer[8];
  static char time_buffer[32];
  static char wind_dir_buffer[32];
  static char weather_layer_buffer[32];
  static char weather_layer_buffer1[32];
  static char weather_layer_buffer2[32];

  static char wind_speed_buffer1[8];
  static char time_buffer1[32];
  static char wind_dir_buffer1[32];

  static char wind_speed_buffer2[8];
  static char time_buffer2[32];
  static char wind_dir_buffer2[32];

  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TIME:
      snprintf(time_buffer, sizeof(time_buffer), "North Head %s", t->value->cstring);
      text_layer_set_text(s_weather_title[0], time_buffer);
      break;
    case KEY_WIND_SPEED:
      snprintf(wind_speed_buffer, sizeof(wind_speed_buffer), "%s", t->value->cstring);
      break;
    case KEY_WIND_DIR:
      snprintf(wind_dir_buffer, sizeof(wind_dir_buffer), "%s", t->value->cstring);
    case KEY_TIME1:
      snprintf(time_buffer1, sizeof(time_buffer1), "Sydney Hbr %s", t->value->cstring);
      text_layer_set_text(s_weather_title[1], time_buffer1);
      break;
    case KEY_WIND_SPEED1:
      snprintf(wind_speed_buffer1, sizeof(wind_speed_buffer1), "%s", t->value->cstring);
      break;
    case KEY_WIND_DIR1:
      snprintf(wind_dir_buffer1, sizeof(wind_dir_buffer1), "%s", t->value->cstring);
    case KEY_TIME2:
      snprintf(time_buffer2, sizeof(time_buffer2), "Ft Denison %s", t->value->cstring);
      text_layer_set_text(s_weather_title[2], time_buffer2);
      break;
    case KEY_WIND_SPEED2:
      snprintf(wind_speed_buffer2, sizeof(wind_speed_buffer2), "%s", t->value->cstring);
      break;
    case KEY_WIND_DIR2:
      snprintf(wind_dir_buffer2, sizeof(wind_dir_buffer2), "%s", t->value->cstring);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  
  
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s %s", wind_speed_buffer, wind_dir_buffer);
 // APP_LOG(APP_LOG_LEVEL_INFO, "wind_speed_buffer=%s", wind_speed_buffer);
//  APP_LOG(APP_LOG_LEVEL_INFO, "wind_dir_buffer=%s",wind_dir_buffer);
//  APP_LOG(APP_LOG_LEVEL_INFO, "weather_layer_buffer=%s",weather_layer_buffer);
  text_layer_set_text(s_weather_layer[0], weather_layer_buffer);
  snprintf(weather_layer_buffer1, sizeof(weather_layer_buffer), "%s %s", wind_speed_buffer1, wind_dir_buffer1);
//  APP_LOG(APP_LOG_LEVEL_INFO, "wind_speed_buffer=%s", wind_speed_buffer1);
//  APP_LOG(APP_LOG_LEVEL_INFO, "wind_dir_buffer=%s",wind_dir_buffer1);
//  APP_LOG(APP_LOG_LEVEL_INFO, "weather_layer_buffer1=%s",weather_layer_buffer1);
  text_layer_set_text(s_weather_layer[1], weather_layer_buffer1);
  snprintf(weather_layer_buffer2, sizeof(weather_layer_buffer2), "%s %s", wind_speed_buffer2, wind_dir_buffer2);
//  APP_LOG(APP_LOG_LEVEL_INFO, "wind_speed_buffer=%s", wind_speed_buffer2);
//  APP_LOG(APP_LOG_LEVEL_INFO, "wind_dir_buffer=%s",wind_dir_buffer2);
//  APP_LOG(APP_LOG_LEVEL_INFO, "weather_layer_buffer2=%s",weather_layer_buffer2);
  text_layer_set_text(s_weather_layer[2], weather_layer_buffer2);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
