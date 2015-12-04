#include "pebble.h"


static Window *main_window;
static Layer *time_layer;
TextLayer *title_text_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
int16_t hand_length;
bool shake = true;
int count;


static void set_time(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);  
//   int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;  
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  
//   GPoint second_hand = {
//     .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length_1 / TRIG_MAX_RATIO) + center.x,
//     .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length_1 / TRIG_MAX_RATIO) + center.y,
//   };
  
//   GPoint minute_hand = {
//     .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.x,
//     .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.y,
//   };        

  GPoint hour_hand = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hand_length / TRIG_MAX_RATIO) + center.y,
  };
  
  int i;  
  for (i=1; i < 60; i++){ 
   if (i % 5 == 0){
     // draw plate
          
     // 5 points
     graphics_context_set_stroke_color(ctx, GColorWhite);
     graphics_draw_line(ctx, 
                        GPoint(
                               (int16_t)(sin_lookup(TRIG_MAX_ANGLE * i / 60) * 
                                         (int32_t)hand_length / TRIG_MAX_RATIO) + center.x,
                               (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * i / 60) * 
                                         (int32_t)hand_length / TRIG_MAX_RATIO) + center.y
                        ), 
                         GPoint(
                                (int16_t)(sin_lookup(TRIG_MAX_ANGLE * i / 60) * 
                                          (int32_t)(hand_length - 4) / TRIG_MAX_RATIO) + center.x,
                                 (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * i / 60) * 
                                           (int32_t)(hand_length - 4)/ TRIG_MAX_RATIO) + center.y
                        )
      );
     
    if ((t->tm_min % 60) < i ) {    
      graphics_context_set_fill_color(ctx, GColorWhite);  
      graphics_fill_circle(ctx, 
                           GPoint(
                                  (int16_t)(sin_lookup(TRIG_MAX_ANGLE * i / 60) * 
                                            (int32_t)hand_length / TRIG_MAX_RATIO) + center.x,
                                  (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * i / 60) * 
                                            (int32_t)hand_length / TRIG_MAX_RATIO) + center.y
                            ),
                           1
      );  
   }
    else{
      graphics_context_set_fill_color(ctx, GColorWhite);  
      graphics_fill_circle(ctx, 
                           GPoint(
                                  (int16_t)(sin_lookup(TRIG_MAX_ANGLE * i / 60) * 
                                            (int32_t)hand_length / TRIG_MAX_RATIO) + center.x,
                                  (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * i / 60) * 
                                            (int32_t)hand_length / TRIG_MAX_RATIO) + center.y
                            ), 
                            4
      );        
    }
  }
    if ((t->tm_min % 60) >= i ) {    
      graphics_context_set_fill_color(ctx, GColorWhite);  
      graphics_fill_circle(ctx, 
                           GPoint(
                                  (int16_t)(sin_lookup(TRIG_MAX_ANGLE * i / 60) * 
                                            (int32_t)hand_length / TRIG_MAX_RATIO) + center.x,
                                  (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * i / 60) * 
                                            (int32_t)hand_length / TRIG_MAX_RATIO) + center.y
                           ),
                           1
      );  
    };    
  };
  
  
  //12 point
  graphics_context_set_fill_color(ctx, GColorWhite);  
  graphics_fill_circle(ctx, GPoint(
     (int16_t)(sin_lookup(TRIG_MAX_ANGLE ) * (int32_t)hand_length / TRIG_MAX_RATIO) + center.x,
     (int16_t)(-cos_lookup(TRIG_MAX_ANGLE ) * (int32_t)hand_length / TRIG_MAX_RATIO) + center.y
    ), 
    4);        
  
  //hour
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_circle(ctx, hour_hand, 9);      
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_circle(ctx, hour_hand, 8);      
  graphics_draw_circle(ctx, hour_hand, 7);      
  graphics_draw_circle(ctx, hour_hand, 6);      

  
  //sec
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(
    
    ((int16_t)(sin_lookup(second_angle) * (int32_t)(hand_length -8)/ TRIG_MAX_RATIO) + center.x),
    ((int16_t)(-cos_lookup(second_angle) * (int32_t)(hand_length -8)/ TRIG_MAX_RATIO) + center.y)
     
    ), 3);
  
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(
    
    ((int16_t)(sin_lookup(second_angle) * (int32_t)(hand_length -8)/ TRIG_MAX_RATIO) + center.x),
    ((int16_t)(-cos_lookup(second_angle) * (int32_t)(hand_length -8)/ TRIG_MAX_RATIO) + center.y)
     
    ), 2);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  
  if (shake){ 
    shake = false;
    count = 30;
  }  
}

static void time_update_proc(Layer *layer, GContext *ctx) {
  
  if (count > 0){    
    set_time(layer, ctx);
    count = count - 1;
  }
  else {
    shake = true;        
  }
  
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(main_window));
}

static void window_load(Window *window) {
 
  Layer *window_layer = window_get_root_layer(main_window);
  GRect bounds = layer_get_bounds(window_layer);
       
  time_layer = layer_create(bounds);
  layer_set_update_proc(time_layer, time_update_proc);
  layer_add_child(window_layer, time_layer);      
  
}

static void window_unload(Window *window) {    
  layer_destroy(time_layer);
  
  // Destroy BitmapLayer
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
  
}

static void init() {
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_MAIN_PIC);
  s_background_layer = bitmap_layer_create(layer_get_bounds(window_get_root_layer(main_window)));  
  
  #ifdef PBL_PLATFORM_CHALK  
    hand_length = 79;
  #else 
    hand_length = 65;
  #endif
  
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);  

  window_set_background_color(main_window, GColorBlack);
  layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(s_background_layer));
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  window_stack_push(main_window, true);    
  accel_tap_service_subscribe(tap_handler);
  shake = true;
  count = 30;
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_destroy(main_window);
  animation_unschedule_all();
}

int main() {
  init();
  app_event_loop();
  deinit();
}
