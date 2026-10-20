# Diskreter PID-Regler in C

[![Tests](https://github.com/nabwei/pid_regler_c_main/actions/workflows/tests.yml/badge.svg)](https://github.com/nabwei/pid_regler_c_main/actions/workflows/tests.yml)

C11-Lehrbeispiel: ein begrenzter PID-Regler steuert eine synthetische Strecke
erster Ordnung. `simulation.csv` enthält den Verlauf. Keine Hardwarevalidierung.
Die Überarbeitung, Tests und Dokumentation entstanden mit KI-Unterstützung.

## Bauen und prüfen

Im Repository-Stamm, mit GCC und Python 3.12:

```sh
mkdir -p build
gcc -O2 -Wall -Wextra -Werror -std=c11 pid_min_project/pid_min.c -o build/pid_min -lm
gcc -O2 -Wall -Wextra -Werror -std=c11 tests/check_controller.c -o build/check_controller -lm
./build/check_controller
./build/pid_min
python tests/check_step.py simulation.csv
```

Unter Windows mit MinGW GCC dieselben Befehle verwenden; die Binärdateien haben
die Endung `.exe`. In PowerShell den Ordner mit `mkdir build` anlegen.
Die CI führt diese Prüfungen auf Ubuntu aus.

## Regelgesetz und Fehlerkorrektur

[`pid_update()`](pid_min_project/pid_min.c) verwendet P auf dem Fehler,
trapezförmige Integration und eine Rückwärtsdifferenz der Messgröße für D.
Ein Sollwertsprung erzeugt deshalb keinen D-Kick.

Conditional Integration verwirft nur den aktuellen I-Zuwachs, wenn er eine
überschrittene Stellgrenze weiter belasten würde. Bei entgegengesetztem Vorzeichen
darf sich der Integrator abbauen. P oder D werden nicht in den Integrator geschrieben.
Dies ersetzt die vorherige unskalierte Rückrechnung des gesamten Ausgangsfehlers.

Die Parameter bleiben `kp=2`, `ki=1`, `kd=0.1`, `ts=0.01 s`, Grenzen `[0,5]`.
Der Sollwert springt bei 1 s von 0 auf 1. Die Strecke hat Verstärkung 1 und
Zeitkonstante 0.5 s: `y_next = exp(-ts/0.5)*y + (1-exp(-ts/0.5))*u`.
Die Simulation läuft jetzt bis 12 s; die Parameter wurden nicht passend zum Test verändert.

## Automatisierte Regression

Der C-Test prüft fehlenden D-Kick, beide Integrationssperren und Abbau eines
bereits hohen Integrators trotz Ausgangssättigung. Der Python-Test prüft die CSV:

- 1201 endliche Datenzeilen, Zeitraster 0.01 s und korrekten Sollwertsprung;
- Ausgangsgrenzen `[0,5]`, keine Bewegung vor dem Sprung;
- Integrator beim Sprung mindestens -1, Überschwingen höchstens 10 Prozent;
- Ausgang ab 10 s bis zum Simulationsende dauerhaft innerhalb von 2 Prozent des Sollwerts.

Im ausgeführten synthetischen GCC-Lauf am 20.10.2026: `y(6s)=0.949675`,
`y(10s)=0.988045`, `y(12s)=0.994173`. Das sind Ergebnisse dieses Modells,
keine allgemeine Regler-Leistungsspezifikation. Der frühere Lauf lieferte bei
6 s etwa 0.895665; die frühere Prüfung auf lediglich positiven Ausgang war unzureichend.

CSV-Spalten: `t,r,y,u,integrator`. Jede Zeile enthält die Stellgröße zur Zeit `t`,
aber bereits den anschließend aktualisierten Streckenausgang. Dieser Versatz
um einen Schritt bleibt beim Vergleich mit anderen Simulationen zu beachten.
Der Lauf überschreibt `simulation.csv` im aktuellen Verzeichnis.

## Reglerentwurf und MATLAB/Simulink

Der ursprüngliche Entwurf wurde im Rahmen eines Laborprojekts mit MATLAB/Simulink
bearbeitet. Modelle, Parametersynthese und ein C/Simulink-Abgleich sind hier nicht
enthalten; ein solcher Abgleich wird nicht als geprüft dargestellt.

## Limitations

- Nur eine deterministische Modellstrecke, keine Stabilitäts- oder Robustheitsgarantie.
- Kein D-Filter; Messrauschen kann den Differentialanteil verstärken.
- Positive sinnvolle Abtastzeit und passende, endliche Parameter werden vorausgesetzt.
- Keine Echtzeit-, Embedded- oder Hardwareintegration.
- Der Schrittversuch allein validiert keinen allgemeinen Reglerentwurf.

## English summary

A synthetic C11 PID example with output limits, conditional integration and
derivative on measurement. Controller checks and CSV regression run in CI.
With unchanged gains, the declared test requires a 2-percent band from 10 s
through 12 s, not 98 percent at 6 s. The revision, tests and documentation used
AI assistance. No hardware or Simulink-equivalence claim. [MIT](LICENSE).
