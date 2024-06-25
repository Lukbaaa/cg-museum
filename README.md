# cg-museum

## Gruppenmitglieder:
- Lukas Baule 1715212
- Maximilian Kotsarev 1714569
- Florin Morari 1714857
- Sun-Min Yook 1714637

## Ausführung
Zum Ausführen des Programms kann die Makedatei wie folgt verwendet werden:

Linux:
make linux && ./museum

Mac OS:
make apple && ./museum

## Steuerung

- W A S D zum Bewegen
- Umgucken mit der Maus, ansonsten Pfeiltasten
- B de- und aktiviert Bounding Boxes

## Szenenbeschreibung:
Diese Szene stellt ein sich in den Wolken befindentes Mini-Museum dar. Dabei werden Techniken, die während des Semesters, als auch privat erlernt wurden, zur Schau gestellt: Mount Rushmore aber wir, ein Partikelsimulator, eine Wellensimulation und eine Raymarching Darstellung. Dafür wurden folgende Techniken angewandt:

- Laden von externen Objekten 
- Texturieren von Objekten, auch mehrere Texturen
- Verschieben, Rotieren und Skalieren von Objekten
- Animieren von Objekten
- Berechnen von (benötigten) Normalen
- Aufbau einer Szene mithilfe eines Szenengraphen mit einem Modelstack
- Vertex-Displacement
- Framebuffer
- Raymarching
- Skybox
- Belichtung, auch mehrere Lichtquellen
- Freie Bewegung der Kamera
- Bounding Boxes (AABB)

Alle Objekte der Szene wurden selber in Blender erstellt.
Die verwendeten Texturen und das Boot Model sind hier zu finden:

https://www.creativefabrica.com/product/black-marble-texture-graphic-4/
https://www.rawpixel.com/image/6030383/brick-wall-texture-free-public-domain-cc0-photo
https://www.rawpixel.com/image/6164422/photo-image-background-texture-abstract
https://de.freepik.com/fotos-kostenlos/zusammensetzung-der-seilstruktur-von-oben_15618948.htm#query=texture%20rope&position=4&from_view=keyword&track=ais_user&uuid=67119498-09b5-4bad-b931-98732face696
https://sketchfab.com/3d-models/toy-boat-3c96a2a3f207411987c6151e680b72d2
https://www.vecteezy.com/vector-art/1844212-water-texture-top-view-background-vector-design-illustration
https://learnopengl.com/img/advanced/blending_transparent_window.png (Blau selber kolloriert)

Andere Texturen wurden entweder selbst erstellt oder befanden sich im Moodle-Kurs.
