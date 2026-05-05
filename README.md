# Schottky Cable Tester (Arduino Nano)

Acest proiect testează un cablu cu 2 conectori în care există o diodă între capete.
Sketch-ul din `main.ino` simulează testul de diodă al multimetrului în ambele polarități și decide rezultatul cu 2 LED-uri:

- **LED verde (D5)** = cablu bun / valori în intervalul calibrat.
- **LED roșu (D6)** = problemă (diodă lipsă, inversată, altă valoare, contact slab).

## Cum funcționează

Programul aplică pe rând două polarități pe cablu:

1. **D8 = plus**, **D9 = minus** și măsoară căderea de tensiune `v_ab`.
2. **D9 = plus**, **D8 = minus** și măsoară căderea de tensiune `v_ba`.

Valorile măsurate sunt comparate cu țintele calibrate:

- `TARGET_AB = 4.945 V`
- `TARGET_BA = 0.280 V`

Cu toleranțe:

- `TOL_AB = ±0.30 V`
- `TOL_BA = ±0.15 V`

Dacă ambele măsurători sunt în interval, testul este **OK** (verde), altfel **PROBLEMĂ** (roșu).

## Conexiuni hardware

### Test cablu (obligatoriu cu rezistențe serie)

- `D8` → **1kΩ** → Conector cablu 1
- `D9` → **1kΩ** → Conector cablu 2
- `A0` la nodul D8 (după rezistor)
- `A1` la nodul D9 (după rezistor)

### LED-uri stare

- `D5` → rezistor 220Ω → LED verde → GND
- `D6` → rezistor 220Ω → LED roșu → GND

## Monitor serial

La `9600 baud`, sketch-ul afișează continuu:

- tensiunea pentru `D8+,D9-`
- tensiunea pentru `D9+,D8-`
- verdictul `OK` sau `PROBLEMA`

Exemplu:

```text
D8+,D9-: 4.940 V (tinta 4.945) | D9+,D8-: 0.276 V (tinta 0.280) => OK (LED verde)
```

## Ajustare / calibrare

Dacă folosești alt cablu, alt tip de diodă sau altă sursă, ajustează în `main.ino`:

- `TARGET_AB`
- `TARGET_BA`
- `TOL_AB`
- `TOL_BA`

Recomandare: măsoară de câteva ori pe un cablu „bun” și setează țintele după media rezultatelor.

## Fișiere

- `main.ino` – logica de test în ambele polarități + control LED-uri.
- `README.md` – documentația pentru versiunea curentă.
