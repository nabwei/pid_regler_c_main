#include <stdio.h>
#include <math.h>

typedef struct {
    float kp, ki, kd, ts;
    float u_min, u_max;
    float integrator, prev_error;
} PID;

static float clampf(float x, float lo, float hi){
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

float pid_update(PID* pid, float r, float y){
    float e = r - y;

    // P, I (Tustin), D (Vorwärts)
    float P = pid->kp * e;
    float I = pid->integrator + 0.5f * pid->ki * pid->ts * (e + pid->prev_error);
    float D = pid->kd * (e - pid->prev_error) / (pid->ts > 1e-9f ? pid->ts : 1e-9f);

    float u_unsat = P + I + D;
    float u = clampf(u_unsat, pid->u_min, pid->u_max);

    // Einfaches Anti‑Windup (Back‑Calculation, feste Verstärkung = 1 hier)
    pid->integrator = I + (u - u_unsat);

    pid->prev_error = e;
    return u;
}

int main(void){
    // Parameter
    PID pid = {.kp=2.0f, .ki=1.0f, .kd=0.1f, .ts=0.01f, .u_min=0.0f, .u_max=5.0f,
               .integrator=0.0f, .prev_error=0.0f};

    const float Ts = pid.ts;
    const float Tsim = 6.0f;
    const int N = (int)(Tsim / Ts);

    float r = 0.0f, y = 0.0f;

    FILE* f = fopen("simulation.csv", "w");
    if(!f){ perror("fopen"); return 1; }
    fprintf(f, "t,r,y,u\n");

    for(int k=0; k<=N; ++k){
        float t = k * Ts;
        if(t >= 1.0f) r = 1.0f;  // Step

        float u = pid_update(&pid, r, y);

        // einfache PT1-ähnliche Dynamik (diskret): y[k+1] = a*y + b*u
        float tau = 0.5f, a = expf(-Ts/tau), b = (1.0f - a); // K=1
        y = a*y + b*u;

        fprintf(f, "%.6f,%.6f,%.6f,%.6f\n", t, r, y, u);
    }
    fclose(f);
    printf("Fertig. CSV: simulation.csv\n");
    return 0;
}
