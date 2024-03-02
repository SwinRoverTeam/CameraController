#ifndef DRAW_FRAME_H_
#define DRAW_FRAME_H_

void init_framebuffer();
void draw_framebuffer(unsigned char* src, int width, int height);
void free_framebuffer();

#endif // DRAW_FRAME_H_
