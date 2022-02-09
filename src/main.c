#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "main.h"

double lerp(double x, double min, double max) {
    return (min * (1 - x)) + (max * x);
}

double invertLerp(double x, double min, double max) {
    return (x - min) / (max - min);
}

double remap(double x, double minX, double maxX, double minTo, double maxTo) {
    return lerp(invertLerp(x, minX, maxX), minTo, maxTo);
}

double getDist(double xA, double yA, double xB, double yB) {
    return pow(pow(xA - xB, 2) + pow(yA - yB, 2), 0.5);
}


fractalPoint* getFractalSuite(double x, double y, int size, int maxEstrangement, int maxEstrangementReset) {
    double xn = x;
    double yn = y;
    int estrangementCount = 0;
    int estrangementReset = 0;
    double lastEstrangement = 0;
    
    fractalPoint *fpoint = malloc(sizeof(fractalPoint));
    double** points = malloc(sizeof(double*) * size);
    fpoint->suite = points;
    fpoint->suiteSize = size;

    for (int i = 0; i < size; i++) {
        points[i] = malloc(sizeof(double) * 2);
        fpoint->suiteAlloc = i + 1;
        points[i][0] = xn;
        points[i][1] = yn;        

        double _xn = xn;

        xn = pow(xn, 2) - pow(yn, 2) + x;
        yn = 2 * _xn * yn + y;
        
        if (i == 0 || (maxEstrangement == -1 && maxEstrangementReset == -1))
            continue;

        double distWithLast = fabs(getDist(xn, yn, points[i - 1][0], points[i - 1][1]));
        //printf("%f; ", distWithLast);
        if (distWithLast > lastEstrangement) {
            estrangementCount += 1;
            lastEstrangement = distWithLast;
        }
        else {
            estrangementCount = 0;
            estrangementReset += 1;
            lastEstrangement = 0;
            //printf("r; ");
        }

        if ((maxEstrangement != -1 && estrangementCount > maxEstrangement) || isinf(distWithLast) || isnan(distWithLast)) {
            fpoint->result = pow(i  / (double)size, 2);
            return fpoint;
        }

        if (maxEstrangementReset != -1 && estrangementReset > maxEstrangementReset) {
            fpoint->result = -1;
            return fpoint;
        }

        
    }
    fpoint->result = -1; 
    return fpoint;
}


void freeFractalPoint(fractalPoint* point) {
    for (int i = 0; i < point->suiteAlloc; i++) {
        free(point->suite[i]);
    }
    free(point->suite);
    free(point);
}

void drawPointSuite(Display *display, Window window, GC gc, fractalPoint *point, double right, double left, double top, double bottom) {
    XWindowAttributes attributes;
    XGetWindowAttributes(display, window, &attributes);
    int width = attributes.width;
    int height = attributes.height;

    
    for (int i = 1; i < point->suiteAlloc; i++) {
        int xa = remap(point->suite[i][0], left, right, 0, width);
        int ya = remap(point->suite[i][1], bottom, top, 0, height);
        int xb = remap(point->suite[i - 1][0], left, right, 0, width);
        int yb = remap(point->suite[i - 1][1], bottom, top, 0, height);

        XDrawLine(display, window, gc, xa, ya, xb, yb);
    }
}

void drawFractal(Display *display, Window window, int screen, GC gc, double posX, double posY, double zoom, Bool fancy) {
    XClearWindow(display, window);

    XWindowAttributes attributes;
    
    XGetWindowAttributes(display, window, &attributes);
    int width = attributes.width;
    int height = attributes.height;

    double right = posX - zoom;
    double left = posX + zoom;
    double zoomY = (zoom * height) / (double)width;
    double top = posY + zoomY;
    double bottom = posY - zoomY;

    printf("\n\n\nwidth: %d\nheight: %d\nright: %f\nleft: %f\ntop: %f\nbottom: %f\nposX: %f\nposY: %f\nzoom: %f\nzoomY: %f\n",
            width, height, right, left, top, bottom, posX, posY, zoom, zoomY);
    
    
    char *data = malloc(width * height * 4);
    int color = 0;
    for (int x = 0; x < width; x ++) {
        for (int y = 0; y < height; y++) {
            color = 0;
            fractalPoint *fPoint = getFractalSuite(remap(x, 0, width, left, right), remap(y, 0, height, bottom, top), fancy ? 100 : 50 , fancy ? 30 : 7, fancy ? 200 : 10);
            
            if (fPoint->result != -1){
                color = (int)(fPoint->result * 255.0) << 16;
                color |= 0xff;
                /*
                if (x == width / 2 && y == height / 2){
                    printf("color: %x\nresult: %f %f\n", color, fPoint->result, fPoint->result * 255.0);
                }
                */
                //XPutPixel(image, x, y, color);
            }
            ((int*)data)[y * width + x] = color;
            freeFractalPoint(fPoint);
        }
    }


    XImage *image = XCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen), ZPixmap, 0, data, width, height, 32, 0);
    XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);
    
    XSetForeground(display, gc, 0x00FFFF0000);
    fractalPoint *point = getFractalSuite(posX, posY, 50, 10, 20);
    drawPointSuite(display, window, gc, point, right, left, top, bottom);
    printf("Fractal Status %f\nFractal Alloc: %d\n", point->result, point->suiteAlloc);
    freeFractalPoint(point);
    XSetForeground(display, gc, 0x00000000);
}

 

int main(void) {
    Display *d;
    Window w;
    XEvent e;
    const char *msg = "Hello, World!";
    int s;
    printf("Hello word\n");
    //printf("%d\n", getFractalAtPos(0, 0));
    printf("%d\n", (int)2.2);
   
    double zoom = 0.25f;
    double posX = -0.8;
    double posY = 0;

    d = XOpenDisplay(NULL);
    if (d == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    s = DefaultScreen(d);
    w = XCreateSimpleWindow(d, RootWindow(d, s), 100, 100, 100, 100, 1,
            BlackPixel(d, s), WhitePixel(d, s));
    XSelectInput(d, w, ExposureMask | KeyPressMask);
    XMapWindow(d, w);

    while (1) {
        XNextEvent(d, &e);
        if (e.type == Expose) {
            //drawFractal(d, w, s, DefaultGC(d, s), posX, posY, zoom);
        }
        if (e.type == KeyPress) {
            printf("Key Pressed  %x\n", e.xkey.keycode);
            Bool fancy = False;
            if (e.xkey.keycode == 0x09)
                break;

            if (e.xkey.keycode == 0x71) {
                posX += zoom * 0.1;
            }

            if (e.xkey.keycode == 0x72) {
                posX -= zoom * 0.1;
            }

            if (e.xkey.keycode == 0x6F) {
                posY -= zoom * 0.1;
            }

            if (e.xkey.keycode == 0x74) {
                posY += zoom * 0.1f;
            }

            if (e.xkey.keycode == 0x56) {
                zoom /= (double)1.25f;
            }

            if (e.xkey.keycode == 0x52) {
                zoom *= 1.25f;
            }
            if (e.xkey.keycode == 0x16) {
                fancy = True;
            }

            drawFractal(d, w, s, DefaultGC(d, s), posX, posY, zoom, fancy);
        }

    }

    XCloseDisplay(d);
    return 0;
}



