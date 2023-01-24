## Reverse-Engineering: Aufbau und Funktion

Die Fernbedienung ist nur zusammengeclipst und lässt sich mit etwas vorsichtigem Hebeln einfach außeinanderbauen. Die Konstruktion ist solide und lässt sich auch mehrfach ohne Zerstörung außeinanderbauen.
Darin befindet sich eine einfache Platine mit einem Mikrocontroller zur Auswertung der Tastendrücke sowie einem @@@(welcher ESP?).

Wird eine Taste gedrückt, aktiviert der Mikrocontroller das WLAN-Modul und sendet einen Schwall an Daten auf UART mit 9600 Baud. Das WLAN-Modul muss sich dann mit dem WLAN-Netzwerk verbinden, die Daten entsprechend absenden und antwortet dann auf UART mit entsprechenden Nachrichten zur Bestätigung. Kurz darauf @@@Zeit wird dann das WLAN-Modul wieder abgeschaltet.

## Reverse-Engineering: Kommunikation

Damit später das WLAN-Modul mit der eigenen Firmware richtig mit dem Mikrocontroller spricht, mussten erst sämtliche Befehle ausgelesen werden. Hierfür wurden zwei USB-Seriell-Adapter eingesetzt, die jeweils mit ihrem RX-Anschluss auf eine der Datenleitungen verbunden wurden.

Mit geöffnetem Terminal für beide Leitungen wurden dann alle Tasten gedrückt und die Befehle ausgelesen.
Das Ergebnis steht in der nachfolgenden Tabelle:

Key					| Data from MCU									| Response from WiFi-Module
--------------------|-----------------------------------------------|------------------------
Moon				| 55 AA 01 02 00 06 01 21 02 00 00 00 2C		| 55 AA 01 02 00 00 02	
Monitor				| 55 AA 01 02 00 06 01 21 02 02 00 00 2E		| 55 AA 01 02 00 00 02
Book				| 55 AA 01 02 00 06 01 21 02 01 00 00 2D		| 55 AA 01 02 00 00 02
Palm				| 55 AA 01 02 00 06 01 21 02 03 00 00 2F		| 55 AA 01 02 00 00 02
A					| 55 AA 01 06 00 01 00 07						| 55 AA 01 06 00 01 01 08
B					| 55 AA 01 06 00 01 01 08						| 55 AA 01 06 00 01 01 08
Slider left (down)	| 55 AA 01 02 00 06 01 25 01 00 14 00 43		| 55 AA 01 02 00 00 02
			(up)	| 55 AA 01 02 00 06 01 25 02 00 00 00 30		| 55 AA 01 02 00 00 02
Slider right(down)	| 55 AA 01 02 00 06 01 25 00 00 14 00 42		| 55 AA 01 02 00 00 02
			(up)	| 55 AA 01 02 00 06 01 25 02 00 00 00 30		| 55 AA 01 02 00 00 02
Square left	(down)	| 55 AA 01 02 00 06 01 27 01 00 14 00 45		| 55 AA 01 02 00 00 02
			(up)	| 55 AA 01 02 00 06 01 27 02 00 00 00 32		| 55 AA 01 02 00 00 02
Square right(down)	| 55 AA 01 02 00 06 01 27 00 00 14 00 44		| 55 AA 01 02 00 00 02
			(up)	| 55 AA 01 02 00 06 01 27 02 00 00 00 32		| 55 AA 01 02 00 00 02
Square top	(down)	| 55 AA 01 02 00 06 01 2A 00 14 00 00 47		| 55 AA 01 02 00 00 02
			(up)	| 55 AA 01 02 00 06 01 2A 02 00 00 00 35		| 55 AA 01 02 00 00 02
Square bottom(down)	| 55 AA 01 02 00 06 01 2A 01 14 00 00 48		| 55 AA 01 02 00 00 02
			(up)	| 55 AA 01 02 00 06 01 2A 02 00 00 00 35		| 55 AA 01 02 00 00 02
On					| 55 AA 01 02 00 03 FF 01 01 06					| 55 AA 01 02 00 00 02
Off					| 55 AA 01 02 00 03 FF 01 00 05					| 55 AA 01 02 00 00 02

Um es einfach zu halten habe ich nicht versucht, das Protokoll weiter nachzuvollziehen. Mir geht es nur um Funktion und nicht um 100%iges Verständnis.

## Hardware-Modifikation

Das verbaute [https://developer.tuya.com/en/docs/iot/wb3s-module-datasheet?id=K9dx20n6hz5n4](WLAN-Modul) WB3S stammt von Tuya und basiert auf dem BK7231T. Da ich mit diesem Chip nicht sehr vertraut bin, habe ich entschieden eine alternative zu suchen.
Das Pinout dieses Moduls ist kompatibel mit den gängigen ESP-Modulen wie ESP-08 oder ESP-12, jedoch werden die Pull-Widerstände auf den GPIO-Leitungen 0, 2 und 15 nicht benötigt. Entsprechend habe ich das Modul durch einen ESP-12 ersetzt und die Wiederstände hinzugefügt.

@@Bild innenleben

Zum Austauschen des WLAN-Moduls habe ich meine Heißluftstation eingesetzt. Die Kunststoff-Folie mit den Metall-Domen der Taster ist nicht Hitze-Resistent und muss vorsichtig abgezogen werden, bevor die Platine erhitzt wird.
Nach dem Umbau kann die Folie wieder aufgeklebt werden. Hierbei ist natürlich darauf zu achten, dass kein Dreck unter die Metall-Dome kommt, da sonst kein sauberer Kontakt bei Tastendruck mehr gegeben ist.

## Firmware-Modifikation

Ich habe dieses Projekt nicht sehr sauber umgesetzt, entsprechend ist die Firmware auf Arduino-Basis und nicht sonderlich sauber geschrieben - sorry.
Wird eine Taste gedrückt, wird nun der ESP gestartet, woraufhin dieser eine Verbindung zu meinem WLAN-Netzwerk und meinem lokalen MQTT-Broker aufbaut.
Abhängig von den erhaltenen Befehlen via UART werden dann entsprechend vorgegebene Befehle an den Broker gesendet.

## OTA-Update

Durch Arduino-OTA kann jederzeit die Firmware auf der Fernbedienung geändert werden.
Damit das WLAN-Modul dauerhaft aktiv bleibt für das Update muss hierzu einfach eine Taste dauerhaft gedrückt werden.

## Downloads & Links

[https://github.com/Techspiring/wifi-remote](Repository des Projekts auf github)