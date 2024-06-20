# TFG Virtualització del RSF Kobol Expander: envelope ADS

# Mostres i codis

Aquest repositori conté els enregistraments utilitzats en el proecte i el codi necessari per analitzar-los. 
Les mostres estàn a les carpetetes "/Anàlisis amb Python/ENREGISTRAMENTS", "/Anàlisis amb Python/ENREGISTRAMENTS ADS1 vs ADS2" i "/Anàlisis amb Python/ENREGISTRAMENTS KOBOL vs PLUGIN". 
Els codis estan dins la carpeta /Analisis. Trobem 
Este repositorio contiene muestras de datos y el código necesario para analizarlas. El proyecto está estructurado de manera que las muestras se encuentran en la carpeta MUESTRAS y los códigos en la carpeta codigos.

A part, hi ha el codi en C++ del plugin creat en JUCE en la carpeta Plugin/ADS_Kobol.

# Estructura del Repositori
El repositori està organitzat de la siguent manera:

Copiar código
/
├── MUESTRAS/
│   ├── ENREGISTRAMENTS/
│   ├── ENREGISTRAMENTS ADS1 vs ADS2/
│   └── ENREGISTRAMENTS KOBOL vs plugin/
├── codigos/
│   ├── ADS1_vs_ADS2,_ADS1_vs_Sound.ipynb.ipynb
│   ├── Anàlisis amb Python/Estudi_ADS_Kobol.ipynb.ipynb
│   └── Kobol_vs_Plugin.ipynb
├── Plugin/ADS_Kobol
└── README.md


# Descargar Muestras
Per treballar amb les ostres simpleent clona el repositori i cambia el path d'on es trobi la carpeta dels enregistraments a analitar.

# Utilitzar el PLugin. 
Per utilitzar el Plugin cal tenir instalat el JUCE i el llenguatge C++ 17. 
També cal instalar els mòduls necessaris de JUCE.
Per fer-lo serv s'ha de compilar en un IDE amb C++.
