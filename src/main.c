#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static int current_guess;
static char message[50];
static int range[2];
static int temp_range[2];
static enum game_state state;

enum game_state { initial, guess_started, guessed };

static void generate_range() {
  range[0] = rand() % 30;
  range[1] = rand() % 51 + 50;
  temp_range[0] = range[0];
  temp_range[1] = range[1];
}

static void show_central_message(char *message) {
  text_layer_set_text(text_layer, message);
}

static void init_the_game() {
  generate_range();
  state = initial;
  
  snprintf(message, sizeof(message), "Make up a number\n\r between %d and %d", range[0], range[1]);
  show_central_message(message);
  
}

static void suggest_guess(bool random) {
  if (temp_range[1] - temp_range[0] < 2) {
    temp_range[0] = range[0];
    temp_range[1] = range[1];
  }
  
  current_guess = random 
    ? rand() % (temp_range[1] - temp_range[0] - 1) + temp_range[0] + 1 
    : (temp_range[1] - temp_range[0]) / 2 + temp_range[0];
  
  snprintf(message, sizeof(message), "Is it %d?", current_guess);
  show_central_message(message);
}

static void start_to_guess() {
  suggest_guess(true);
  state = guess_started;
}

static void set_guessed() {
  snprintf(message, sizeof(message), "Hurray!\n\rIt is %d", current_guess);
  show_central_message(message);
  state = guessed;
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  switch (state) {
    case initial:
      start_to_guess();
      break;
    case guess_started:
      set_guessed();
      break;
    case guessed:
      init_the_game();
      break;
  }
}

static void adjust_search_range(bool is_less) {
    temp_range[is_less ? 1 : 0] = current_guess;
    suggest_guess(false);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (state == guess_started) {
    adjust_search_range(false);
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
if (state == guess_started) {
    adjust_search_range(true);
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  text_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorWhite);
  //text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
  srand((unsigned) time(NULL));
  
  init_the_game();
}

static void main_window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

void init () {
  window = window_create();
  
  window_set_click_config_provider(window, click_config_provider);
  
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_set_background_color(window, GColorBlack);
  
  window_stack_push(window, true);
}

void deinit() {
  window_destroy(window);
}


int main() {
  init();
  app_event_loop();
  deinit();
  return 0;
}