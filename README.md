# Autonomes Modellfahrzeug
**Plattform:** ARM Cortex-M3 (LPC1768 auf Landtigerboard)  
**Framework:** Mbed OS 5.15  
**Entwicklungsumgebung:** Visual Studio Code + PlatformIO  
**Sprache:** C++

---

## Aufgabenbeschreibung
Ziel der Laboraufgabe 5 ist die Implementierung eines autonomen Modellfahrzeugs (Maßstab 1:10) auf Basis eines ARM Cortex‑M3 Mikrocontrollers (LPC1768) und MbedOS. Das Fahrzeug soll verschiedene Fahrfunktionen realisieren, Hard‑/Software‑Schnittstellen nutzen und harte Echtzeitanforderungen einhalten.

---

## Hardware‑Übersicht
- Landtigerboard (ARM Cortex‑M3, LPC1768)  
- 4 × Sharp Infrarotsensoren (Vorne, Hinten, Links, Rechts)  
- 2 × Ultraschallsensoren (Front)  
- 1 × Lenkservo (Lenkung)  
- 1 × Motorregler (Antrieb; servo‑ähnliche Steuerung)  
- LCD‑Touchdisplay (Benutzerinteraktion)

---

## Software‑Anforderungen / Funktionale Punkte

### 1. Systemmonitor
- Zykluszeit: **250 ms**  
- Dauerhafte Anzeige in der unteren Bildschirmhälfte  
- Überwachung relevanter Tasks mit:
  - Aktuell gemessener Zykluszeit in **µs**
  - Counter für Verletzungen der Soll‑Zykluszeit

### 2. Grundprogramm Fahrzeugansteuerung
- Sensoreinlesung: **exakt alle 100 ms** (IR‑ und Ultraschall)  
- Aktoransteuerung: **exakt alle 20 ms** (Lenkung und Motor)  
- Parallele Task‑Verwaltung unter Einhaltung der Timing‑Vorgaben

### 3. Testprogramm
- Start per Touchscreen‑Button  
- Sequenzieller Test aller Sensoren und Aktoren  
- Echtzeit‑Anzeige der Sensordaten (250 ms Zyklus)  
- Manueller Abbruch und Neustart möglich  
- Integration der Systemmonitor‑Anzeige während Tests

### 4. Querregelung (simuliert)
- Fahrzeug steht aufgebockt, Anfangszustand:
  - Stillstand
  - Objekt bei 20 cm vorne
  - Räder geradeaus
- Bewegungsstart: Entfernung des vorderen Objekts → langsame Vorwärtsfahrt  
- Seitliche Hinderniserkennung:
  - Hindernis links → Rechtslenkung
  - Hindernis rechts → Linkslenkung
  - Lenkung proportional zum seitlichen Abstand
- Freie Fahrt: Keine seitlichen Objekte → Räder gerade
- Stopp: Bei erneuter Objekterkennung vorne → Räder gerade, anhalten
- Dauerhafte Anzeige: Sensorwerte und Task‑Zykluszeiten auf Display

---

## Technische Umsetzungsempfehlungen

### Architektur
- Modularer Aufbau (z. B. `SensorManager`, `ActuatorController`, `SystemMonitor`, `TestSuite`, `Main`)  
- Verwende MbedOS Tasks/Threads, `EventQueue` oder `Ticker` für zyklische Aufgaben  
- Kommunikationsmechanismen:
  - Message Queues (für Befehle / Telemetrie)
  - Shared Objects mit `Mutex` (für gemeinsame Konfiguration / Displayzugriff)

### Timing & Echtzeit
- Sensoreinlesung: `Ticker` oder periodischer `Thread` mit Absolutzeit‑Wiederholung (nicht relative Delays)  
- Aktuatorzyklus: höher priorisierte Task oder Timer mit 20 ms Periode  
- Monitoring: gering priorisierter, periodischer Task (250 ms) zur Anzeige und Zyklusüberwachung  
- Missverkettungen vermeiden: Prioritätsinversion durch geeignete Mutex‑Strategien verhindern

### Display und UI
- Touch‑Button zum Start/Stop des Testprogramms  
- Untere Bildschirmhälfte reserviert für Systemmonitor (Zykluszeiten, Verletzungszähler)  
- Klar strukturierte Anzeige während der Simulation (Sensorwerte, Aktorzustand, Logs)

---

## Entwicklung & Build
1. Visual Studio Code + PlatformIO konfigurieren (Target: LPC1768, Mbed OS 5.15)  
2. Projektstruktur (Beispiel):
```
ModelCar/
├── src/
│   ├── main.cpp
│   ├── SensorManager.cpp
│   ├── SensorManager.h
│   ├── ActuatorController.cpp
│   ├── ActuatorController.h
│   ├── SystemMonitor.cpp
│   ├── SystemMonitor.h
│   ├── TestSuite.cpp
│   └── TestSuite.h
├── platformio.ini
└── README.md
```
3. Build & Upload: PlatformIO CLI oder VSCode Interface  
4. Logging: Serieller Output (UART) für Debug + optional SD/Flash für Aufzeichnungen

---

## Testprozeduren
- **Unit‑ und Integrationstests** (so weit möglich, Hardware‑abstrahiert)  
- **Manueller Testlauf**:
  1. System einschalten, Monitor prüfen (250 ms Anzeige)  
  2. Touch‑Button: Testprogramm starten  
  3. Prüfe sequentielle Sensor‑/Aktor‑Tests (Erwartungswerte dokumentieren)  
  4. Simuliere seitliche Hindernisse (z. B. Mobiles Objekt) → beobachte Lenkung  
  5. Prüfe Zyklusverletzungen im Monitor und analysiere Ursachen

---

## Bewertungskriterien (Hinweis für Abgabe)
- **Funktionalität (25%)**: Einhaltung der Echtzeitanforderungen (20 ms / 100 ms / 250 ms)  
- **Codequalität (10%)**: Struktur, Lesbarkeit, Nutzung MbedOS‑APIs  
- **Dokumentation (15%)**: UML/SysML Diagramme, Timing‑Diagramme, Entscheidungen begründen  
- **Individuelles Wissen (50%)**: Abnahmegespräch — konzeptionelles Verständnis und Begründen technischer Entscheidungen

---

## Abgabehinweise
- Repositorium: sauber strukturierter Quellcode, README, UML/Timing‑Diagramme (PDF)  
- Kurze Anleitung zur Kompilierung und zum Hochladen (platformio.ini beschreiben)  
- Optional: Video des Testlaufs als ergänzende Evidenz

---

## Nützliche Hinweise & Best Practices
- Verwende für periodische Tasks `Ticker` + `EventQueue` oder `Thread::flags_set` zur deterministischen Steuerung  
- Prioritäten sinnvoll verteilen: höhere Priorität für Aktuatorzyklus (20 ms)  
- Robustheit: Sensorwerte filtern (Median, gleitender Mittelwert) und Plausibilitätschecks durchführen  
- Dokumentiere Annahmen (z. B. Sensorreichweiten, Kalibrierungswerte)

---

Wenn du möchtest, kann ich jetzt:
- eine Beispiel‑Projektstruktur mit Basis‑Quellcode (MbedOS) erzeugen, oder
- ein README in einer anderen Ausprägung (z. B. Kurz‑Fassung oder Prüfungsblatt‑Format) erstellen.

