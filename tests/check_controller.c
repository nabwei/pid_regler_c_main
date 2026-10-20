#define main pid_simulation_main
#include "../pid_min_project/pid_min.c"
#undef main

static int near(float actual, float expected){
    return fabsf(actual - expected) < 1e-5f;
}

int main(void){
    PID controller = {.kp=2.0f, .ki=1.0f, .kd=0.1f, .ts=0.01f,
                      .u_min=0.0f, .u_max=5.0f};
    float output = pid_update(&controller, 1.0f, 0.0f);
    if (!near(output, 2.005f) || !near(controller.integrator, 0.005f)) return 1;
    controller = (PID){.kp=2.0f, .ki=1.0f, .ts=0.01f, .u_min=0.0f, .u_max=5.0f};
    output = pid_update(&controller, 100.0f, 0.0f);
    if (!near(output, 5.0f) || !near(controller.integrator, 0.0f)) return 2;
    controller.prev_error = 0.0f;
    output = pid_update(&controller, -100.0f, 0.0f);
    if (!near(output, 0.0f) || !near(controller.integrator, 0.0f)) return 3;
    controller.integrator = 8.0f;
    controller.prev_error = -1.0f;
    output = pid_update(&controller, -1.0f, 0.0f);
    if (!near(output, 5.0f) || !near(controller.integrator, 7.99f)) return 4;
    puts("PASS controller: no derivative kick, both clamps and saturated unwinding");
    return 0;
}
