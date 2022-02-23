__kernel void calc_fractal_point(__global const float* pos, __global const int* screen , __global char *result) {

    float vx = pos[0];
    float vy = pos[1];
    float vWidth = pos[2];
    float vHeight = pos[3];

    int width = screen[0];
    int height = screen[1];
    int max_iteration = screen[2];

    int globalId = get_global_id(0);
    int localID = get_local_id(0);
    char *p = result + (globalId * 4);
    int px = globalId % width;
    int py = globalId / width;
    if (localID < 50) {
         printf("%d_%d_%d\n", globalId, globalId * 4, get_local_id(0));
    }

    float x = px * vWidth / (float)width + vx;
    float y = py * vHeight / (float)height + vy;

    float xn = 0;
    float yn = 0;
    *p ++= 0xff;
    *p = 0x55;

    if (px < 20) {
        *p = 0xff;
    }
    return;
    for(int i = 0; i < max_iteration; i++){
        float _xn = xn;
        xn = pow(xn, 2) - pow(yn, 2) + x;
        yn = 2 * _xn * yn + y;
        if (isinf(xn) != 0 || isinf(yn) != 0 || isnan(xn) != 0 || isnan(yn) != 0 || i > 10){
            *p = 0x00;
            return;
        }
    }

    *p = 0xff;
    return;
}
