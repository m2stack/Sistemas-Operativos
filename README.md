# 📘 Operating Systems

This repository contains the projects, exercises and materials developed relating to the study of operating systems, ubuntu software and functionalities, etc.

It contains the programs developed for practical and theorical research and knowledge reinforcement. All different projects have been developed and coded using **VSCode** with **C**. Base support and development guides were supplied by official documentation from the URJC.

<div align="center">
  <a>
    <img src="media/operating_syst.png" alt="Logo" width="45%">
    <img src="media/ubuntuos.png" alt="Logo" width="45%">
  </a>
</div>

---

## 🛠️ About development

- **Language(s) used:** [C]
- **Tools and environments:** [VSCode, Nano, Vi]
- **Version control:** Git (individual private repositories for each project migrated to the current repository).
- **Reinforced skills:**
  - Shell scripting remembering
  - Communication between processes
  - Signaling, memory handling, error treatment, etc.
  - Handling of external libraries and analysis of official documentation
  - Modelling of specific algorithmic systems
  - Basic documentation and good style practices

---

## 📂 Relevant content and structure

All programs below are source .c files which can be directly compiled using the correspondant makefile inside their own folders. To run the code you can follow the next steps:

1. Clone the repository into your workspace
```
git clone https://github.com/m2stack/Sistemas-Operativos.git
```

2. Navigate to the folder where the code you want to run is (e.g. p1-argsort/)
```
cd Sistemas-Operativos/projects/p1-argsort/
```

3. Compilate with the make command and then you can execute the program as it should
```
make
```

---

```bash
📁 /media/
├── /operating_syst.png
├── /ubuntuos.png
📁 /projects/
├── 📁 p1-argsort/
│   └── argsort.c
│   └── makefile
│   └── theory.md
│   └── ...
├── 📁p2-pinger/
│   └── pinger.c
│   └── makefile
│   └── theory.md
│   └── ...
├── 📁 p3-copybytes/
│   └── copybytes.c
│   └── makefile
│   └── theory.md
│   └── ...
├── 📁 p4-ngrams/
│   └── ngrams.c
│   └── makefile
│   └── ...
├── 📁 p5-readpaths/
│   └── readpaths.c
│   └── sourcefiles.c
│   └── makefile
│   └── paths_doc.txt
└── 📁 p6-pspids/
│   └── pspids.c
│   └── makefile
└── indent.sh
```