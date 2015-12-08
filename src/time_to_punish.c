#include "pebble.h"


static Window *main_window;
static Layer *main_layer, *hour_layer, *minute_layer, *seconds_layer;
TextLayer *title_text_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
int16_t hand_length, s_hand_length;

static void seconds_update_proc(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  
  //sec
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(
    
    ((int16_t)(sin_lookup(second_angle) * (int32_t)(s_hand_length)/ TRIG_MAX_RATIO) + center.x),
    ((int16_t)(-cos_lookup(second_angle) * (int32_t)(s_hand_length)/ TRIG_MAX_RATIO) + center.y)
     
    ), 
		3);
  
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(
    
    ((int16_t)(sin_lookup(second_angle) * (int32_t)(s_hand_length)/ TRIG_MAX_RATIO) + center.x),
    ((int16_t)(-cos_lookup(second_angle) * (int32_t)(s_hand_length)/ TRIG_MAX_RATIO) + center.y)
     
    ), 2);
}

static void hour_update_proc(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);  

  GPoint hour_hand = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hand_length / TRIG_MAX_RATIO) + center.y,
  };
  
  //hour
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_circle(ctx, hour_hand, 9);      
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_circle(ctx, hour_hand, 8);      
  graphics_draw_circle(ctx, hour_hand, 7);      
  graphics_draw_circle(ctx, hour_hand, 6);      
    
}

static void minute_update_proc(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
    
  int i;  
	for (i=0; i < t->tm_min + 1; i++){ 
     
   if ((i % 5) > 0) {    
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
                            3
      );        
    }
  }        
}

static void main_update_proc(Layer *layer, GContext *ctx) {
  
	GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);  
	
	int i; 
	
	for (i=1; i <= 12 ; i++){ 
   
     // draw plate            
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorWhite);  
    graphics_fill_circle(ctx, 
    	GPoint(
      	(int16_t)(sin_lookup(TRIG_MAX_ANGLE * i / 12) * 
        	(int32_t)hand_length / TRIG_MAX_RATIO) + center.x,
        (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * i / 12) * 
        	(int32_t)hand_length / TRIG_MAX_RATIO) + center.y
      ),
      1
    );  
		
		graphics_draw_line(ctx, 
    	GPoint(
      	(int16_t)(sin_lookup(TRIG_MAX_ANGLE * i / 12) * 
        	(int32_t)hand_length / TRIG_MAX_RATIO) + center.x,
        (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * i / 12) * 
          (int32_t)hand_length / TRIG_MAX_RATIO) + center.y
      ), 
      GPoint(
      	(int16_t)(sin_lookup(TRIG_MAX_ANGLE * i / 12) * 
        	(int32_t)(hand_length - 4) / TRIG_MAX_RATIO) + center.x,
       	(int16_t)(-cos_lookup(TRIG_MAX_ANGLE * i / 12) * 
          (int32_t)(hand_length - 4)/ TRIG_MAX_RATIO) + center.y
      )
    );
	};
};

static void handle_hour_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(hour_layer);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(minute_layer);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(seconds_layer);
}

static void window_load(Window *window) {
 
  Layer *window_layer = window_get_root_layer(main_window);
  GRect bounds = layer_get_bounds(window_layer);
       
  // plate
	main_layer = layer_create(bounds);
	layer_set_update_proc(main_layer, main_update_proc);
	layer_add_child(window_layer, main_layer);      
	layer_mark_dirty(main_layer);		
	
	//minute
	minute_layer = layer_create(bounds);
	layer_set_update_proc(minute_layer, minute_update_proc);
	layer_add_child(main_layer, minute_layer);      
	
	// hour
	hour_layer = layer_create(bounds);
	layer_set_update_proc(hour_layer, hour_update_proc);
	layer_add_child(minute_layer, hour_layer);      
	
	// seconds
	seconds_layer = layer_create(bounds);
	layer_set_update_proc(seconds_layer, seconds_update_proc);
	layer_add_child(hour_layer, seconds_layer);      
  
  
}

static void window_unload(Window *window) {    
  layer_destroy(main_layer);
	layer_destroy(hour_layer);
	layer_destroy(minute_layer);
	layer_destroy(seconds_layer);
  
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
    hand_length = 71;
		s_hand_length = 84;
  #else 
    hand_length = 60;
		s_hand_length = 68;
  #endif
  
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);  

  window_set_background_color(main_window, GColorBlack);
  layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(s_background_layer));
  tick_timer_service_subscribe(HOUR_UNIT, handle_hour_tick);
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  window_stack_push(main_window, true);    
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_destroy(main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
