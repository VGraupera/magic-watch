#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

// source http://en.wikipedia.org/wiki/Magic_8-Ball
static char *fortunes[20] = {
  "It is certain",
  "It is decidedly so",
  "Without a doubt",
  "Yes definitely",
  "You may rely on it",
  "As I see it, yes",
  "Most likely",
  "Outlook good",
  "Yes",
  "Signs point to yes",
  "Reply hazy try again",
  "Ask again later",
  "Better not tell you now",
  "Cannot predict now",
  "Concentrate and ask again",
  "Don't count on it",
  "My reply is no",
  "My sources say no",
  "Outlook not so good",
  "Very doubtful",
};


static void animation_started(Animation *animation, void *data) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", __PRETTY_FUNCTION__);
}

static void animation_stopped(Animation *animation, bool finished, void *data) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", __PRETTY_FUNCTION__);
  int index = rand() % 20;
  text_layer_set_text(text_layer, fortunes[index]);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", __PRETTY_FUNCTION__);
  vibes_short_pulse();

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GRect screen_bottom_rect = GRect(0, bounds.size.h, bounds.size.w, bounds.size.h);
  PropertyAnimation *flyout_animation = property_animation_create_layer_frame(text_layer_get_layer(text_layer), NULL, &screen_bottom_rect);

  // Duration defaults to 250 ms
  animation_set_duration(&flyout_animation->animation, 1000);
  GRect starting_rect = layer_get_frame(text_layer_get_layer(text_layer));

  GRect screen_top_rect = GRect(0, 0, bounds.size.w, bounds.size.h);
  PropertyAnimation *flyin_animation = property_animation_create_layer_frame(text_layer_get_layer(text_layer), &screen_top_rect, &starting_rect);

  animation_set_duration(&flyin_animation->animation, 1000);
  // Delay defaults to 0 ms
  animation_set_delay(&flyin_animation->animation, 1000);

  animation_set_handlers((Animation*) flyout_animation, (AnimationHandlers) {
      .started = (AnimationStartedHandler) animation_started,
      .stopped = (AnimationStoppedHandler) animation_stopped,
      }, NULL /* callback data */);

  animation_schedule((Animation*) flyout_animation);
  animation_schedule((Animation*) flyin_animation);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 52 }, .size = { bounds.size.w, 70 } });
  text_layer_set_text(text_layer, "Shake watch");
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  accel_tap_service_subscribe(accel_tap_handler);
}

static void window_unload(Window *window) {
  accel_tap_service_unsubscribe();

  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();

  window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
      });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
