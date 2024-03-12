#ifndef PANELS_H_
#define PANELS_H_

typedef struct panel {
	int x, y, w, h;
} panel_t;

void panel_debug(panel_t* p);

void stack_top(panel_t* base, panel_t* new_panel);
void stack_bottom(panel_t* base, panel_t* new_panel);
void stack_left(panel_t* base, panel_t* new_panel);
void stack_right(panel_t* base, panel_t* new_panel);

#endif // PANELS_H_
