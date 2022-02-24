__kernel void calc_fractal_point(__global const float* xInputs, __global const float* yInputs , __global int *result) {
    


    /*
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
    */
    int globalId = get_global_id(0);


    float x = xInputs[globalId];
    float y = yInputs[globalId];
    printf("%d: %f, %f\n",globalId, x, y);
    float xn = 0;
    float yn = 0;
    if ((globalId % 10) < 5) {
        result[globalId] = -1;
    }
    return;
    for(int i = 0; i < 10; i++){
        float _xn = xn;
        xn = pow(xn, 2) - pow(yn, 2) + x;
        yn = 2 * _xn * yn + y;
        if ((xn * xn + yn * yn) > 25){
            result[globalId] = 0;
            return;
        }
    }
    result[globalId] = - 1;
    return;
}
