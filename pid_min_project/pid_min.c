#include <stdio.h>
#include <math.h>

typedef struct {
    float kp, ki, kd, ts;
    float u_min, u_max;
    float integrator, prev_error, prev_measurement;
} PID;

static float clampf(float x, float lo, float hi){
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

float pid_update(PID* pid, float setpoint, float measurement){
    float error = setpoint - measurement;
    float proportional = pid->kp * error;
    float integral_delta = 0.5f * pid->ki * pid->ts * (error + pid->prev_error);
    float derivative = -pid->kd * (measurement - pid->prev_measurement) / pid->ts;
    float candidate_output = proportional + pid->integrator + integral_delta + derivative;
    int drives_high = candidate_output > pid->u_max && integral_delta > 0.0f;
    int drives_low = candidate_output < pid->u_min && integral_delta < 0.0f;
    if (!drives_high && !drives_low) pid->integrator += integral_delta;
    pid->prev_error = error;
    pid->prev_measurement = measurement;
    return clampf(proportional + pid->integrator + derivative, pid->u_min, pid->u_max);
}

int main(void){
    // Parameter
    PID pid = {.kp=2.0f, .ki=1.0f, .kd=0.1f, .ts=0.01f, .u_min=0.0f, .u_max=5.0f,
               .integrator=0.0f, .prev_error=0.0f, .prev_measurement=0.0f};

    const float Ts = pid.ts;
    const float Tsim = 12.0f;
    const int N = (int)(Tsim / Ts);

    float r = 0.0f, y = 0.0f;

    FILE* f = fopen("simulation.csv", "w");
    if(!f){ perror("fopen"); return 1; }
    fprintf(f, "t,r,y,u,integrator\n");

    for(int k=0; k<=N; ++k){
        float t = k * Ts;
        if(t >= 1.0f) r = 1.0f;  // Step

        float u = pid_update(&pid, r, y);

        // einfache PT1-ähnliche Dynamik (diskret): y[k+1] = a*y + b*u
        float tau = 0.5f, a = expf(-Ts/tau), b = (1.0f - a); // K=1
        y = a*y + b*u;

        fprintf(f, "%.6f,%.6f,%.6f,%.6f,%.6f\n", t, r, y, u, pid.integrator);
    }
    fclose(f);
    printf("Fertig. CSV: simulation.csv\n");
    return 0;
}
